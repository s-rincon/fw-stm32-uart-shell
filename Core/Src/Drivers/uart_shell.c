/**
 * @file uart_shell.c
 * @brief UART shell driver implementation for STM32.
 *
 * This file contains the implementation of the UART shell driver, which handles
 * UART RX/TX using ring buffers and provides packet-based reception with timeout.
 *
 * @author Santiago Rincon
 * @date 2025
 */

#include "uart_shell.h"

/**
 * @def UART_RX_TIMEOUT_MS
 * @brief Timeout (in milliseconds) for UART receive operations.
 *
 * This defines the maximum time allowed to wait for incoming UART data before
 * considering the reception as timed out.
 */
#define UART_RX_TIMEOUT_MS       100U

void uart_shell_rx_it_callback(uart_shell_t *uart_shell) {
    if ((uart_shell == NULL) || (uart_shell->huart == NULL)) {
        return;
    }

    (void) ring_buffer_push(&uart_shell->ring_buffer_rx, uart_shell->rx_byte);
    HAL_UART_Receive_IT(uart_shell->huart, (uint8_t *) &uart_shell->rx_byte, 1);
}

void uart_shell_tx_it_callback(uart_shell_t *uart_shell) {
    if ((uart_shell == NULL) || (uart_shell->huart == NULL)) {
        return;
    }

    uint8_t next;
    if (ring_buffer_pop(&uart_shell->ring_buffer_tx, &next)) {
        HAL_UART_Transmit_IT(uart_shell->huart, &next, 1);

    } else {
        uart_shell->tx_busy = false;
    }
}

size_t uart_shell_send(uart_shell_t *uart_shell, uint8_t *data, size_t length) {
    if ((data == NULL) || (length == 0)) {
		return 0U;
	}

    for (size_t data_idx = 0; data_idx < length; data_idx++) {
        ring_buffer_push(&uart_shell->ring_buffer_tx, data[data_idx]);
    }

    if (!uart_shell->tx_busy) {
        uint8_t first;
        if (ring_buffer_pop(&uart_shell->ring_buffer_tx, &first)) {
            uart_shell->tx_busy = true;
            HAL_UART_Transmit_IT(uart_shell->huart, &first, 1);
        } else {
            return 0U;
        }
    }

    return length;
}

void uart_shell_poll(uart_shell_t *uart_shell) {
    static uint8_t packet[UART_SHELL_MAX_RX_BUFFER];
    static size_t packet_len = 0;
    static uint32_t last_rx_tick = 0;
    uint8_t byte;

    if ((packet_len > 0U) && ((HAL_GetTick() - last_rx_tick) > UART_RX_TIMEOUT_MS)) {
        /* Rx timeout occurred — reset packet */
        packet_len = 0U;
    }

    while (ring_buffer_pop(&uart_shell->ring_buffer_rx, &byte)) {
        last_rx_tick = HAL_GetTick();

        if (packet_len < UART_SHELL_MAX_RX_BUFFER) {
            packet[packet_len++] = byte;
        } else {
            packet_len = 0U;
            continue;
        }

        if (packet_len > 2U) {
            if ((packet[packet_len - 2] == '\r') && (packet[packet_len - 1] == '\n')) {
                if (uart_shell->rx_callback && packet_len > 0) {
                    uart_shell->rx_callback(packet, packet_len);
                }
                packet_len = 0U;
                continue;
            }
        }
    }
}

bool uart_shell_reconfigure(uart_shell_t *uart_shell, uint32_t baud_rate) {
    if ((uart_shell == NULL) || (uart_shell->huart == NULL) || (baud_rate == 0)) {
        return false;
    }

    HAL_UART_AbortTransmit(uart_shell->huart);
    HAL_UART_AbortReceive(uart_shell->huart);

    if (HAL_UART_DeInit(uart_shell->huart) != HAL_OK) {
        return false;
    }

    uart_shell->huart->Init.BaudRate = baud_rate;

    if (HAL_UART_Init(uart_shell->huart) != HAL_OK) {
        return false;
    }

    HAL_UART_Receive_IT(uart_shell->huart, (uint8_t *) &uart_shell->rx_byte, 1);

    return true;
}

void uart_shell_init(uart_shell_t *uart_shell, UART_HandleTypeDef *huart, uart_shell_rx_callback_t rx_callback) {
    if ((huart == NULL) || (rx_callback == NULL)) {
        return;
    }

    uart_shell->huart = huart;
    uart_shell->rx_callback = rx_callback;
    uart_shell->tx_busy = false;

    ring_buffer_init(&uart_shell->ring_buffer_rx, uart_shell->rx_buffer, UART_SHELL_MAX_RX_BUFFER);
    ring_buffer_init(&uart_shell->ring_buffer_tx, uart_shell->tx_buffer, UART_SHELL_MAX_TX_BUFFER);

    HAL_UART_Receive_IT(uart_shell->huart, (uint8_t *) &uart_shell->rx_byte, 1);
}

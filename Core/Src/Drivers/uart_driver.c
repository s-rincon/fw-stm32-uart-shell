/**
 * @file uart_driver.c
 * @brief UART driver implementation for STM32.
 *
 * This file contains the implementation of the UART driver, which handles
 * UART RX/TX using ring buffers and provides packet-based reception with timeout.
 *
 * @author Santiago Rincon
 * @date 2025
 */

#include "uart_driver.h"

/**
 * @def UART_RX_TIMEOUT_MS
 * @brief Timeout (in milliseconds) for UART receive operations.
 *
 * This defines the maximum time allowed to wait for incoming UART data before
 * considering the reception as timed out.
 */
#define UART_RX_TIMEOUT_MS       100U

/**
 * @brief UART RX interrupt callback.
 *
 * This function should be called from the UART RX interrupt handler.
 * It pushes the received byte into the RX ring buffer and restarts the RX interrupt.
 *
 * @param uart_driver Pointer to uart_driver_t structure.
 */
void uart_driver_rx_it_callback(uart_driver_t *uart_driver) {
    if ((uart_driver == NULL) || (uart_driver->huart == NULL)) {
        return;
    }

    (void) ring_buffer_push(&uart_driver->ring_buffer_rx, uart_driver->rx_byte);
    HAL_UART_Receive_IT(uart_driver->huart, (uint8_t *) &uart_driver->rx_byte, 1);
}

/**
 * @brief UART TX interrupt callback.
 *
 * This function should be called from the UART TX interrupt handler.
 * It pops the next byte from the TX ring buffer and transmits it.
 * If no more data is available, it marks TX as not busy.
 *
 * @param uart_driver Pointer to uart_driver_t structure.
 */
void uart_driver_tx_it_callback(uart_driver_t *uart_driver) {
    if ((uart_driver == NULL) || (uart_driver->huart == NULL)) {
        return;
    }

    uint8_t next;
    if (ring_buffer_pop(&uart_driver->ring_buffer_tx, &next)) {
        HAL_UART_Transmit_IT(uart_driver->huart, &next, 1);

    } else {
        uart_driver->tx_busy = false;
    }
}

/**
 * @brief Send data over UART using the driver.
 *
 * This function pushes the provided data into the TX ring buffer and starts transmission
 * if not already busy.
 *
 * @param uart_driver Pointer to uart_driver_t structure.
 * @param data Pointer to data buffer to send.
 * @param length Number of bytes to send.
 * @return Number of bytes successfully queued for transmission.
 */
size_t uart_driver_send(uart_driver_t *uart_driver, uint8_t *data, size_t length) {
    if ((data == NULL) || (length == 0)) {
        return 0U;
    }

    for (size_t data_idx = 0; data_idx < length; data_idx++) {
        ring_buffer_push(&uart_driver->ring_buffer_tx, data[data_idx]);
    }

    if (!uart_driver->tx_busy) {
        uint8_t first;
        if (ring_buffer_pop(&uart_driver->ring_buffer_tx, &first)) {
            uart_driver->tx_busy = true;
            HAL_UART_Transmit_IT(uart_driver->huart, &first, 1);
        } else {
            return 0U;
        }
    }

    return length;
}

/**
 * @brief Poll for received UART packets.
 *
 * This function should be called periodically. It collects bytes from the RX ring buffer,
 * assembles packets terminated by "\r\n", and calls the RX callback when a packet is complete.
 * It also handles RX timeout to reset incomplete packets.
 *
 * @param uart_driver Pointer to uart_driver_t structure.
 */
void uart_driver_poll(uart_driver_t *uart_driver) {
    static uint8_t packet[UART_DRIVER_MAX_RX_BUFFER];
    static size_t packet_len = 0;
    static uint32_t last_rx_tick = 0;
    uint8_t byte;

    if ((packet_len > 0U) && ((HAL_GetTick() - last_rx_tick) > UART_RX_TIMEOUT_MS)) {
        /* Rx timeout occurred — reset packet */
        packet_len = 0U;
    }

    while (ring_buffer_pop(&uart_driver->ring_buffer_rx, &byte)) {
        last_rx_tick = HAL_GetTick();

        if (packet_len < UART_DRIVER_MAX_RX_BUFFER) {
            packet[packet_len++] = byte;
        } else {
            packet_len = 0U;
            continue;
        }

        if (packet_len > 2U) {
            if ((packet[packet_len - 2] == '\r') && (packet[packet_len - 1] == '\n')) {
                if (uart_driver->rx_callback && packet_len > 0) {
                    uart_driver->rx_callback(packet, packet_len);
                }
                packet_len = 0U;
                continue;
            }
        }
    }
}

/**
 * @brief Reconfigure UART driver with a new baud rate.
 *
 * This function aborts ongoing transmissions, deinitializes and reinitializes the UART
 * peripheral with the new baud rate, and restarts RX interrupt.
 *
 * @param uart_driver Pointer to uart_driver_t structure.
 * @param baud_rate New baud rate to configure.
 * @return true if reconfiguration succeeded, false otherwise.
 */
bool uart_driver_reconfigure(uart_driver_t *uart_driver, uint32_t baud_rate) {
    if ((uart_driver == NULL) || (uart_driver->huart == NULL) || (baud_rate == 0)) {
        return false;
    }

    HAL_UART_AbortTransmit(uart_driver->huart);
    HAL_UART_AbortReceive(uart_driver->huart);

    if (HAL_UART_DeInit(uart_driver->huart) != HAL_OK) {
        return false;
    }

    uart_driver->huart->Init.BaudRate = baud_rate;

    if (HAL_UART_Init(uart_driver->huart) != HAL_OK) {
        return false;
    }

    HAL_UART_Receive_IT(uart_driver->huart, (uint8_t *) &uart_driver->rx_byte, 1);

    return true;
}

/**
 * @brief Initialize the UART driver.
 *
 * This function initializes the UART driver structure, sets up ring buffers,
 * and starts the RX interrupt.
 *
 * @param uart_driver Pointer to uart_driver_t structure to initialize.
 * @param huart Pointer to UART_HandleTypeDef structure.
 * @param rx_callback Callback function to handle received packets.
 */
void uart_driver_init(uart_driver_t *uart_driver, UART_HandleTypeDef *huart, uart_driver_rx_callback_t rx_callback) {
    if ((huart == NULL) || (rx_callback == NULL)) {
        return;
    }

    uart_driver->huart = huart;
    uart_driver->rx_callback = rx_callback;
    uart_driver->tx_busy = false;

    ring_buffer_init(&uart_driver->ring_buffer_rx, uart_driver->rx_buffer, UART_DRIVER_MAX_RX_BUFFER);
    ring_buffer_init(&uart_driver->ring_buffer_tx, uart_driver->tx_buffer, UART_DRIVER_MAX_TX_BUFFER);

    HAL_UART_Receive_IT(uart_driver->huart, (uint8_t *) &uart_driver->rx_byte, 1);
}

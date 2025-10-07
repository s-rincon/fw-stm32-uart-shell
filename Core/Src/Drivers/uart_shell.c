/**
 * @file uart_shell.c
 * @brief UART shell driver implementation for STM32.
 *
 * Handles UART RX/TX using ring buffers and provides packet-based reception with timeout.
 *
 * @author Santiago Rincon
 * @date 2025
 */

#include "uart_shell.h"
#include "ring_buffer.h"

#define UART_SHELL_RX_TIMEOUT_MS 20

/**
 * @brief UART shell context structure.
 */
typedef struct uart_shell_ {
    UART_HandleTypeDef *huart;                /**< Pointer to UART handle */
    ring_buffer_t ring_buffer_rx;             /**< RX ring buffer */
    ring_buffer_t ring_buffer_tx;             /**< TX ring buffer */
    uint8_t tx_buffer[UART_SHELL_MAX_TX_BUFFER]; /**< TX buffer memory */
    uint8_t rx_buffer[UART_SHELL_MAX_RX_BUFFER]; /**< RX buffer memory */
    uint8_t rx_byte;                          /**< Last received byte */
    bool rx_packet_ready;                     /**< Packet ready flag */
    uart_shell_rx_callback_t rx_callback;     /**< RX callback function */
} uart_shell_t;

uart_shell_t uart_shell;

/**
 * @brief HAL UART RX complete callback.
 *
 * Called by HAL when a byte is received. Stores the byte in the RX ring buffer,
 * sets the packet ready flag if newline is received, and restarts reception.
 *
 * @param huart Pointer to UART handle.
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance != uart_shell.huart->Instance) {
		return;
	}

    ring_buffer_push(&uart_shell.ring_buffer_rx, uart_shell.rx_byte);

    if (uart_shell.rx_byte == '\n') {
        uart_shell.rx_packet_ready = true;
    }

    HAL_UART_Receive_IT(uart_shell.huart, &uart_shell.rx_byte, 1);
}

/**
 * @brief HAL UART TX complete callback.
 *
 * Called by HAL when a byte is transmitted. Sends next byte from TX ring buffer if available.
 *
 * @param huart Pointer to UART handle.
 */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance != uart_shell.huart->Instance) {
		return;
	}

    uint8_t data_char;
    if (ring_buffer_pop(&uart_shell.ring_buffer_tx, &data_char)) {
        HAL_UART_Transmit_IT(uart_shell.huart, &data_char, 1);
    }
}

void uart_shell_send(const uint8_t *data, uint16_t length) {
    if ((!data) || (length == 0)) {
		return;
	}

    for (uint16_t i = 0; i < length; i++) {
        ring_buffer_push(&uart_shell.ring_buffer_tx, data[i]);
    }

    if (__HAL_UART_GET_FLAG(uart_shell.huart, UART_FLAG_TXE)) {
        uint8_t data_char;
        if (ring_buffer_pop(&uart_shell.ring_buffer_tx, &data_char)) {
            HAL_UART_Transmit_IT(uart_shell.huart, &data_char, 1);
        }
    }
}

void uart_shell_reconfigure(uint32_t baud_rate) {
    while(uart_shell.ring_buffer_tx.count > 0);

    __disable_irq();

    if(HAL_UART_DeInit(uart_shell.huart) != HAL_OK) {
        __enable_irq();
        return;
    }

    uart_shell.huart->Init.BaudRate = baud_rate;

    if(HAL_UART_Init(uart_shell.huart) != HAL_OK) {
        __enable_irq();
        return;
    }

    HAL_UART_Receive_IT(uart_shell.huart, &uart_shell.rx_byte, 1);

    __enable_irq();
}

void uart_shell_poll(void) {
    static uint8_t packet[UART_SHELL_MAX_RX_BUFFER];
    static uint16_t packet_len = 0;
    uint8_t byte;

    if (!uart_shell.rx_packet_ready) {
		return;
	}

	uart_shell.rx_packet_ready = false;
	while (ring_buffer_pop(&uart_shell.ring_buffer_rx, &byte)) {
		if (packet_len < UART_SHELL_MAX_RX_BUFFER) {
			packet[packet_len++] = byte;
		} else {
			packet_len = 0;
			continue;
		}

		if (byte == '\n') {
			if (uart_shell.rx_callback && packet_len > 0) {
				uart_shell.rx_callback(packet, packet_len);
			}
			packet_len = 0;
			break;
		}
	}
}

void uart_shell_init(UART_HandleTypeDef *huart, uart_shell_rx_callback_t rx_callback) {
    uart_shell.huart = huart;
    uart_shell.rx_callback = rx_callback;
    uart_shell.rx_packet_ready = false;

    ring_buffer_init(&uart_shell.ring_buffer_rx, uart_shell.rx_buffer, UART_SHELL_MAX_RX_BUFFER);
    ring_buffer_init(&uart_shell.ring_buffer_tx, uart_shell.tx_buffer, UART_SHELL_MAX_TX_BUFFER);

    HAL_UART_Receive_IT(uart_shell.huart, &uart_shell.rx_byte, 1);
}

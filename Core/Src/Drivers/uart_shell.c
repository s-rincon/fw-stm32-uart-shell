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

#define UART_RECONFIG_TIMEOUT_MS 200U

/**
 * @brief UART shell context structure.
 *
 * Holds all state and buffers for a UART shell instance.
 */
typedef struct uart_shell_ {
    UART_HandleTypeDef *huart;                      /**< Pointer to UART handle */
    ring_buffer_t ring_buffer_rx;                   /**< RX ring buffer */
    ring_buffer_t ring_buffer_tx;                   /**< TX ring buffer */
    uint8_t tx_buffer[UART_SHELL_MAX_TX_BUFFER];    /**< TX buffer memory */
    uint8_t rx_buffer[UART_SHELL_MAX_RX_BUFFER];    /**< RX buffer memory */
    volatile uint8_t rx_byte;                       /**< Last received byte */
    volatile bool tx_busy;                          /**< TX busy flag */
    uart_shell_rx_callback_t rx_callback;           /**< RX callback function */
} uart_shell_t;

/**
 * @brief UART shell context singleton instance.
 */
static uart_shell_t uart_shell;

/**
 * @brief HAL UART RX complete callback.
 *
 * Called by HAL when a byte is received. Stores the byte in the RX ring buffer,
 * and restarts reception for the next byte.
 *
 * @param huart Pointer to UART handle.
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if ((huart == NULL) || (uart_shell.huart == NULL)) {
        return;
    }

    if (huart->Instance != uart_shell.huart->Instance) {
		return;
	}

    (void) ring_buffer_push(&uart_shell.ring_buffer_rx, uart_shell.rx_byte);

    HAL_UART_Receive_IT(uart_shell.huart, (uint8_t *) &uart_shell.rx_byte, 1);
}

/**
 * @brief HAL UART TX complete callback.
 *
 * Called by HAL when a byte is transmitted. Sends next byte from TX ring buffer if available,
 * otherwise marks TX as not busy.
 *
 * @param huart Pointer to UART handle.
 */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
    if ((huart == NULL) || (uart_shell.huart == NULL)) {
        return;
    }

    if (huart->Instance != uart_shell.huart->Instance) {
		return;
	}

    uint8_t next;
    if (ring_buffer_pop(&uart_shell.ring_buffer_tx, &next)) {
        HAL_UART_Transmit_IT(uart_shell.huart, &next, 1);

    } else {
        uart_shell.tx_busy = false;
    }
}

size_t uart_shell_send(const uint8_t *data, size_t length) {
    if ((data == NULL) || (length == 0)) {
		return 0U;
	}

    for (size_t data_idx = 0; data_idx < length; data_idx++) {
        ring_buffer_push(&uart_shell.ring_buffer_tx, data[data_idx]);
    }

    if (!uart_shell.tx_busy) {
        uint8_t first;
        if (ring_buffer_pop(&uart_shell.ring_buffer_tx, &first)) {
            uart_shell.tx_busy = true;
            HAL_UART_Transmit_IT(uart_shell.huart, &first, 1);
        } else {
            return 0U;
        }
    }

    return length;
}

void uart_shell_poll(void) {
    static uint8_t packet[UART_SHELL_MAX_RX_BUFFER];
    static uint16_t packet_len = 0;

    while (ring_buffer_pop(&uart_shell.ring_buffer_rx, &packet[packet_len])) {
        packet_len++;

        if (packet_len >= UART_SHELL_MAX_RX_BUFFER) {
            packet_len = 0U;
            continue;
        }

        if (packet_len > 2U) {
            if ((packet[packet_len - 2] == '\r') && (packet[packet_len - 1] == '\n')) {
                if (uart_shell.rx_callback && packet_len > 0) {
                    uart_shell.rx_callback(packet, packet_len);
                }
                packet_len = 0U;
                continue;
            }
        }
    }
}

bool uart_shell_reconfigure(uint32_t baud_rate) {
    if ((uart_shell.huart == NULL) || (baud_rate == 0)) {
        return false;
    }

    HAL_UART_AbortTransmit(uart_shell.huart);
    HAL_UART_AbortReceive(uart_shell.huart);

    if (HAL_UART_DeInit(uart_shell.huart) != HAL_OK) {
        return false;
    }

    uart_shell.huart->Init.BaudRate = baud_rate;

    if (HAL_UART_Init(uart_shell.huart) != HAL_OK) {
        return false;
    }

    HAL_UART_Receive_IT(uart_shell.huart, (uint8_t *) &uart_shell.rx_byte, 1);

    return true;
}

void uart_shell_init(UART_HandleTypeDef *huart, uart_shell_rx_callback_t rx_callback) {
    if ((huart == NULL) || (rx_callback == NULL)) {
        return;
    }

    uart_shell.huart = huart;
    uart_shell.rx_callback = rx_callback;
    uart_shell.tx_busy = false;

    ring_buffer_init(&uart_shell.ring_buffer_rx, uart_shell.rx_buffer, UART_SHELL_MAX_RX_BUFFER);
    ring_buffer_init(&uart_shell.ring_buffer_tx, uart_shell.tx_buffer, UART_SHELL_MAX_TX_BUFFER);

    HAL_UART_Receive_IT(uart_shell.huart, (uint8_t *) &uart_shell.rx_byte, 1);
}

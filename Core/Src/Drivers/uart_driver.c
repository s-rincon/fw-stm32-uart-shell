/**
 * @file uart_driver.c
 * @brief UART driver implementation for STM32.
 *
 * Implements UART RX/TX using ring buffers and provides packet-based reception with timeout.
 *
 * @author Santiago Rincon
 * @date 2025
 */

#include "uart_driver.h"

/**
 * @brief UART RX interrupt callback.
 *
 * Call this from the UART RX complete interrupt handler.
 * Pushes the received byte into the RX ring buffer and restarts reception.
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
 * Call this from the UART TX complete interrupt handler.
 * Pops the next byte from the TX ring buffer and transmits it.
 * If no more data is available, marks TX as not busy.
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
 * Pushes the provided data into the TX ring buffer and starts transmission if not busy.
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
 * @brief Get a single received byte from the RX ring buffer.
 *
 * @param uart_driver Pointer to uart_driver_t structure.
 * @param byte Pointer to variable to store the received byte.
 * @return true if a byte was received, false otherwise.
 */
bool uart_driver_get_byte(uart_driver_t *uart_driver, uint8_t *byte) {
    if ((uart_driver == NULL) || (byte == NULL)) {
        return false;
    }

    return ring_buffer_pop(&uart_driver->ring_buffer_rx, byte);
}

/**
 * @brief Reconfigure UART driver with a new baud rate.
 *
 * Aborts ongoing transmissions, deinitializes and reinitializes the UART peripheral with the new baud rate,
 * and restarts RX interrupt.
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
 * Initializes the UART driver structure, sets up ring buffers, and starts the RX interrupt.
 *
 * @param uart_driver Pointer to uart_driver_t structure to initialize.
 * @param huart Pointer to UART_HandleTypeDef structure.
 * @return true if initialization succeeded, false otherwise.
 */
bool uart_driver_init(uart_driver_t *uart_driver, UART_HandleTypeDef *huart) {
    if ((huart == NULL)) {
        return false;
    }

    uart_driver->huart = huart;
    uart_driver->tx_busy = false;

    ring_buffer_init(&uart_driver->ring_buffer_rx, uart_driver->rx_buffer, UART_DRIVER_MAX_RX_BUFFER);
    ring_buffer_init(&uart_driver->ring_buffer_tx, uart_driver->tx_buffer, UART_DRIVER_MAX_TX_BUFFER);

    return (HAL_UART_Receive_IT(uart_driver->huart, (uint8_t *) &uart_driver->rx_byte, 1) == HAL_OK);
}

/**
 * @file uart_shell.h
 * @brief UART shell driver API for STM32.
 *
 * Provides initialization, send, polling, and interrupt callback functions for UART shell communication.
 *
 * @author Santiago Rincon
 * @date 2025
 */

#ifndef __UART_SHELL_INC_
#define __UART_SHELL_INC_

#include "main.h"


#include <stdbool.h>
#include <stdint.h>

#include "ring_buffer.h"

/**
 * @def UART_SHELL_MAX_RX_BUFFER
 * @brief Maximum RX buffer size for UART shell.
 */
#define UART_SHELL_MAX_RX_BUFFER 256

/**
 * @def UART_SHELL_MAX_TX_BUFFER
 * @brief Maximum TX buffer size for UART shell.
 */
#define UART_SHELL_MAX_TX_BUFFER 256

/**
 * @brief RX callback type for UART shell.
 *
 * Called when a packet is received.
 *
 * @param data Pointer to received data buffer.
 * @param length Length of received data.
 */
typedef void (*uart_shell_rx_callback_t)(uint8_t *data, uint16_t length);

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
 * @brief UART RX interrupt callback.
 *
 * Should be called from the UART RX interrupt handler.
 * Pushes the received byte into the RX ring buffer and restarts reception.
 *
 * @param uart_shell Pointer to uart_shell_t structure.
 */
void uart_shell_rx_it_callback(uart_shell_t *uart_shell);

/**
 * @brief UART TX interrupt callback.
 *
 * Should be called from the UART TX interrupt handler.
 * Pops the next byte from the TX ring buffer and transmits it.
 * If no more data is available, marks TX as not busy.
 *
 * @param uart_shell Pointer to uart_shell_t structure.
 */
void uart_shell_tx_it_callback(uart_shell_t *uart_shell);

/**
 * @brief Initializes the UART shell driver.
 *
 * Sets up UART, ring buffers, and starts reception.
 *
 * @param uart_shell Pointer to uart_shell_t structure to initialize.
 * @param huart Pointer to UART handle.
 * @param rx_callback RX packet callback function.
 */
void uart_shell_init(uart_shell_t *uart_shell, UART_HandleTypeDef *huart, uart_shell_rx_callback_t rx_callback);

/**
 * @brief Reconfigures the UART shell baud rate.
 *
 * Aborts ongoing transfers, deinitializes and reinitializes UART with new baud rate.
 *
 * @param uart_shell Pointer to uart_shell_t structure.
 * @param baud_rate New baud rate.
 * @return true if successful, false otherwise.
 */
bool uart_shell_reconfigure(uart_shell_t *uart_shell, uint32_t baud_rate);

/**
 * @brief Sends data over the UART shell.
 *
 * Pushes data into TX ring buffer and starts transmission if not busy.
 *
 * @param uart_shell Pointer to uart_shell_t structure.
 * @param data Pointer to data buffer.
 * @param length Number of bytes to send.
 * @return Number of bytes accepted for transmission.
 */
size_t uart_shell_send(uart_shell_t *uart_shell, uint8_t *data, size_t length);

/**
 * @brief Polls for received packets and calls RX callback.
 *
 * Accumulates bytes until a packet delimiter (\r\n) is found, then calls RX callback.
 * Should be called periodically in main loop.
 *
 * @param uart_shell Pointer to uart_shell_t structure.
 */
void uart_shell_poll(uart_shell_t *uart_shell);

#endif /* __UART_SHELL_INC_ */

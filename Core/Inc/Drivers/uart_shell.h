/**
 * @file uart_shell.h
 * @brief UART shell driver API for STM32.
 *
 * Provides initialization, send, and polling functions for UART shell communication.
 *
 * @author Santiago Rincon
 * @date 2025
 */

#ifndef __UART_SHELL_INC_
#define __UART_SHELL_INC_

#include "main.h"
#include <stdbool.h>
#include <stdint.h>

#define UART_SHELL_MAX_RX_BUFFER 256
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
 * @brief Initializes the UART shell driver.
 *
 * Sets up UART, ring buffers, and starts reception.
 *
 * @param huart Pointer to UART handle.
 * @param rx_callback RX packet callback function.
 */
void uart_shell_init(UART_HandleTypeDef *huart, uart_shell_rx_callback_t rx_callback);

/**
 * @brief Reconfigures the UART shell baud rate.
 *
 * Aborts ongoing transfers, deinitializes and reinitializes UART with new baud rate.
 *
 * @param baud_rate New baud rate.
 * @return true if successful, false otherwise.
 */
bool uart_shell_reconfigure(uint32_t baud_rate);

/**
 * @brief Sends data over the UART shell.
 *
 * Pushes data into TX ring buffer and starts transmission if not busy.
 *
 * @param data Pointer to data buffer.
 * @param length Number of bytes to send.
 * @return Number of bytes accepted for transmission.
 */
size_t uart_shell_send(const uint8_t *data, size_t length);

/**
 * @brief Polls for received packets and calls RX callback.
 *
 * Accumulates bytes until a packet delimiter (\r\n) is found, then calls RX callback.
 * Should be called periodically in main loop.
 */
void uart_shell_poll(void);

#endif /* __UART_SHELL_INC_ */

#ifndef __UART_SHELL_INC_
#define __UART_SHELL_INC_

#include "main.h"
#include <stdbool.h>
#include <stdint.h>

#define UART_SHELL_MAX_RX_BUFFER 256
#define UART_SHELL_MAX_TX_BUFFER 256

/**
 * @brief Callback type for UART shell RX events.
 *
 * This function is called when new data is received via UART.
 *
 * @param data Pointer to received data buffer.
 * @param length Length of received data.
 */
typedef void (*uart_shell_rx_callback_t)(uint8_t *data, uint16_t length);

/**
 * @brief Initializes the UART shell driver.
 *
 * Configures the UART and DMA peripherals for shell communication.
 *
 * @param rx_callback Callback function for received data.
 */
void uart_shell_init(UART_HandleTypeDef *huart, uart_shell_rx_callback_t rx_callback);

/**
 * @brief Reconfigures the UART shell baud rate.
 *
 * Changes the baud rate of the UART peripheral.
 *
 * @param baud_rate New baud rate to set.
 */
void uart_shell_reconfigure(uint32_t baud_rate);

/**
 * @brief Sends data over the UART shell.
 *
 * Transmits the specified data buffer via UART.
 *
 * @param data Pointer to data buffer to send.
 * @param length Length of data to send.
 */
void uart_shell_send(const uint8_t *data, uint16_t length);

/**
 * @brief Polls the UART shell for events.
 *
 * Should be called periodically to handle UART shell tasks.
 */
void uart_shell_poll(void);

#endif /* __UART_SHELL_INC_ */

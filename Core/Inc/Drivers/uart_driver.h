/**
 * @file uart_driver.h
 * @brief UART driver API for STM32.
 *
 * Provides initialization, send, polling, and interrupt callback functions for UART communication.
 *
 * @author Santiago Rincon
 * @date 2025
 */

#ifndef __UART_DRIVER_INC_
#define __UART_DRIVER_INC_

#include "main.h"


#include <stdbool.h>
#include <stdint.h>

#include "ring_buffer.h"

/**
 * @def UART_DRIVER_MAX_RX_BUFFER
 * @brief Maximum RX buffer size for UART driver.
 */
#define UART_DRIVER_MAX_RX_BUFFER 256

/**
 * @def UART_DRIVER_MAX_TX_BUFFER
 * @brief Maximum TX buffer size for UART driver.
 */
#define UART_DRIVER_MAX_TX_BUFFER 256



/**
 * @brief UART driver context structure.
 *
 * Holds all state and buffers for a UART driver instance.
 */
typedef struct uart_driver_ {
    UART_HandleTypeDef *huart;                      /**< Pointer to UART handle */

    ring_buffer_t ring_buffer_rx;                   /**< RX ring buffer */
    ring_buffer_t ring_buffer_tx;                   /**< TX ring buffer */

    uint8_t tx_buffer[UART_DRIVER_MAX_TX_BUFFER];   /**< TX buffer memory */
    uint8_t rx_buffer[UART_DRIVER_MAX_RX_BUFFER];   /**< RX buffer memory */
    volatile uint8_t rx_byte;                       /**< Last received byte */
    volatile bool tx_busy;                          /**< TX busy flag */

} uart_driver_t;

/**
 * @brief UART RX interrupt callback.
 *
 * Call this from the UART RX complete interrupt handler.
 *
 * @param uart_driver Pointer to uart_driver_t structure.
 */
void uart_driver_rx_it_callback(uart_driver_t *uart_driver);

/**
 * @brief UART TX interrupt callback.
 *
 * Call this from the UART TX complete interrupt handler.
 *
 * @param uart_driver Pointer to uart_driver_t structure.
 */
void uart_driver_tx_it_callback(uart_driver_t *uart_driver);

/**
 * @brief Initializes the UART driver.
 *
 * Sets up UART, ring buffers, and starts reception.
 *
 * @param uart_driver Pointer to uart_driver_t structure to initialize.
 * @param huart Pointer to UART handle.
 * @return true if initialization succeeded, false otherwise.
 */
bool uart_driver_init(uart_driver_t *uart_driver, UART_HandleTypeDef *huart);

/**
 * @brief Reconfigures the UART driver baud rate.
 *
 * Aborts ongoing transfers, deinitializes and reinitializes UART with new baud rate.
 *
 * @param uart_driver Pointer to uart_driver_t structure.
 * @param baud_rate New baud rate.
 * @return true if successful, false otherwise.
 */
bool uart_driver_reconfigure(uart_driver_t *uart_driver, uint32_t baud_rate);

/**
 * @brief Sends data over the UART driver.
 *
 * Pushes data into TX ring buffer and starts transmission if not busy.
 *
 * @param uart_driver Pointer to uart_driver_t structure.
 * @param data Pointer to data buffer.
 * @param length Number of bytes to send.
 * @return Number of bytes accepted for transmission.
 */
size_t uart_driver_send(uart_driver_t *uart_driver, uint8_t *data, size_t length);

/**
 * @brief Get a single received byte from the RX ring buffer.
 *
 * @param uart_driver Pointer to uart_driver_t structure.
 * @param byte Pointer to variable to store the received byte.
 * @return true if a byte was received, false otherwise.
 */
bool uart_driver_get_byte(uart_driver_t *uart_driver, uint8_t *byte);

#endif /* __UART_DRIVER_INC_ */

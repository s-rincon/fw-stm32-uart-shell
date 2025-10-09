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
 * @brief RX callback type for UART driver.
 *
 * Called when a packet is received.
 * @todo This should not be handled by uart driver, rather must be driven externally
 *
 * @param data Pointer to received data buffer.
 * @param length Length of received data.
 */
typedef void (*uart_driver_rx_callback_t)(uint8_t *data, uint16_t length);

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
    uart_driver_rx_callback_t rx_callback;          /**< RX callback function */
} uart_driver_t;

/**
 * @brief UART RX interrupt callback.
 *
 * Should be called from the UART RX interrupt handler.
 * Pushes the received byte into the RX ring buffer and restarts reception.
 *
 * @param uart_driver Pointer to uart_driver_t structure.
 */
void uart_driver_rx_it_callback(uart_driver_t *uart_driver);

/**
 * @brief UART TX interrupt callback.
 *
 * Should be called from the UART TX interrupt handler.
 * Pops the next byte from the TX ring buffer and transmits it.
 * If no more data is available, marks TX as not busy.
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
 * @param rx_callback RX packet callback function.
 */
void uart_driver_init(uart_driver_t *uart_driver, UART_HandleTypeDef *huart, uart_driver_rx_callback_t rx_callback);

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
 * @brief Polls for received packets and calls RX callback.
 *
 * Accumulates bytes until a packet delimiter (\r\n) is found, then calls RX callback.
 * Should be called periodically in main loop.
 *
 * @param uart_driver Pointer to uart_driver_t structure.
 */
void uart_driver_poll(uart_driver_t *uart_driver);

#endif /* __UART_DRIVER_INC_ */

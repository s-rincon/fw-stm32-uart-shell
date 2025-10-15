/**
 * @file shell.h
 * @brief UART shell interface for STM32 microcontrollers.
 *
 * Provides the shell API, configuration macros, and data structures
 * for a UART-based command-line shell with line editing and history.
 *
 * @author Santiago Rincon
 * @date 2025
 */

#ifndef __SHELL_INC_
#define __SHELL_INC_

#include <stdbool.h>
#include <stddef.h>
#include "main.h"
#include "uart_driver.h"

/**
 * @def SHELL_MAX_LENGTH
 * @brief Maximum length of the input command line (including null terminator).
 */
#ifndef SHELL_MAX_LENGTH
#define SHELL_MAX_LENGTH 128
#endif

/**
 * @def SHELL_HISTORY_SIZE
 * @brief Number of commands stored in history.
 */
#ifndef SHELL_HISTORY_SIZE
#define SHELL_HISTORY_SIZE 10
#endif

/**
 * @struct shell_history_t
 * @brief Command history buffer and navigation state.
 *
 * Stores the last SHELL_HISTORY_SIZE commands entered by the user.
 */
typedef struct {
    char commands[SHELL_HISTORY_SIZE][SHELL_MAX_LENGTH]; /**< History entries */
    int current_index;    /**< Index for next command to store */
    int count;            /**< Number of valid history entries */
    int browse_index;     /**< Index for browsing history */
} shell_history_t;

/**
 * @struct rx_command_t
 * @brief Input line buffer and cursor state.
 *
 * Stores the current input line and cursor position.
 */
typedef struct rx_command_ {
    uint8_t buffer[SHELL_MAX_LENGTH]; /**< Input line buffer */
    size_t length;                    /**< Current length of input */
    size_t cursor_pos;                /**< Cursor position in buffer */
} rx_command_t;

/**
 * @struct shell_t
 * @brief Shell instance structure.
 *
 * Contains all state for a shell session, including UART driver,
 * input buffer, and command history.
 */
typedef struct shell_ {
    uart_driver_t driver;    /**< UART driver instance */
    shell_history_t history; /**< Command history state */
    rx_command_t rx;         /**< Input line state */
} shell_t;

/**
 * @brief Get the UART driver instance from a shell.
 * @param shell Pointer to the shell instance.
 * @return Pointer to the uart_driver_t instance.
 */
static inline uart_driver_t *shell_get_driver_instance(shell_t *shell) {
    return &shell->driver;
}

/**
 * @brief Initializes the shell instance.
 * @param shell Pointer to the shell instance to initialize.
 * @param huart Pointer to the UART handle to use for communication.
 * @return true if initialization was successful, false otherwise.
 */
bool shell_init(shell_t *shell, UART_HandleTypeDef *huart);

/**
 * @brief Formatted print function for the shell.
 * Sends formatted output to UART.
 * @param shell Pointer to the shell instance.
 * @param format Printf-style format string.
 * @param ... Variable arguments.
 */
void shell_printf(shell_t *shell, const char *format, ...);

/**
 * @brief Sends raw bytes through the shell's UART interface.
 * @param shell Pointer to the shell instance.
 * @param data Pointer to the data to send.
 * @param len Number of bytes to send.
 * @return Number of bytes actually sent.
 */
size_t shell_send_bytes(shell_t *shell, uint8_t *data, size_t len);

/**
 * @brief Main shell processing loop.
 * Reads UART input and processes shell logic.
 * Handles escape sequences for arrow keys, printable characters, and line editing.
 * @param shell Pointer to the shell instance.
 */
void shell_task(shell_t *shell);

#endif /* __SHELL_INC_ */

#include "shell.h"

#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#include "target_ver.h"

/**
 * @brief Prints the startup banner with project information
 * @param shell Pointer to the shell instance
 */
static void shell_print_startup_message(shell_t *shell) {
    if (shell == NULL) {
        return;
    }

    shell_printf(shell, "****************************\r\n");
    shell_printf(shell, "Project: %s\r\n", PROJECT_DESCRIPTION);
    shell_printf(shell, "Version: %d.%d.%s\r\n", TARGET_VER_MAJOR, TARGET_VER_MINOR, TARGET_VER_DATE);
    shell_printf(shell, "Author: %s\r\n", AUTHOR);
    shell_printf(shell, "****************************\r\n\r\n");
}

/**
 * @brief Sends the command prompt to the user
 * @param shell Pointer to the shell instance
 */
static void shell_send_prompt(shell_t *shell) {
    if (shell == NULL) {
        return;
    }
    shell_printf(shell, "STM32 > ");
}

/**
 * @brief Processes a complete command line received from the user
 * @param shell Pointer to the shell instance
 * @param command Pointer to the command string
 * @param length Length of the command string
 */
static void shell_process_command(shell_t *shell, uint8_t *command, uint16_t length) {
    if ((shell == NULL) || (command == NULL) || (length == 0)) {
        return;
    }

    // Remove trailing \r\n
    while (length > 0 && (command[length - 1] == '\r' || command[length - 1] == '\n')) {
        command[length - 1] = '\0';
        length--;
    }

    // Skip empty commands
    if (length == 0) {
        shell_send_prompt(shell);
        return;
    }

    // Echo the command for now (can be replaced with actual command processing)
    shell_printf(shell, "Command received: %s\r\n", command);

    // TODO: Add actual command parsing and execution here

    shell_send_prompt(shell);
}

/**
 * @brief Callback function called when a complete line is received via UART
 * @param shell Pointer to the shell instance
 * @param data Pointer to the received data
 * @param length Length of the received data
 * @deprecated This function is kept for backward compatibility but not used internally
 */
void uart_shell_rx_callback(shell_t *shell, uint8_t *data, uint16_t length) {
    if ((shell == NULL) || (data == NULL)) {
        return;
    }

    shell_process_command(shell, data, length);
}

/**
 * @brief Main shell task that processes incoming UART data
 * This function should be called periodically from the main loop
 * @param shell Pointer to the shell instance
 */
void shell_task(shell_t *shell) {
    if (shell == NULL) {
        return;
    }

    static uint8_t input_buffer[SHELL_MAX_LENGTH];
    static size_t input_length = 0;
    uint8_t received_byte;

    // Process all available bytes
    while (uart_driver_get_byte(&shell->driver, &received_byte)) {

        // Handle buffer overflow
        if (input_length >= (SHELL_MAX_LENGTH - 1)) {
            shell_printf(shell, "\r\nError: Command too long. Max length: %d\r\n", SHELL_MAX_LENGTH - 1);
            input_length = 0;
            shell_send_prompt(shell);
            continue;
        }

        // Add byte to buffer
        input_buffer[input_length++] = received_byte;

        // Check for complete command (CRLF sequence)
        if (input_length >= 2) {
            if ((input_buffer[input_length - 2] == '\r') && (input_buffer[input_length - 1] == '\n')) {
                // Null-terminate the string
                input_buffer[input_length] = '\0';

                // Process the complete command
                shell_process_command(shell, input_buffer, input_length);

                // Reset buffer for next command
                input_length = 0;
            }
        }
    }
}

/**
 * @brief Sends raw bytes through the shell's UART interface
 * @param shell Pointer to the shell instance
 * @param data Pointer to the data to send
 * @param len Number of bytes to send
 * @return Number of bytes actually sent
 */
size_t shell_send_bytes(shell_t *shell, uint8_t *data, size_t len) {
    if ((shell == NULL) || (data == NULL) || (len == 0U)) {
        return 0U;
    }

    return uart_driver_send(&shell->driver, data, len);
}

/**
 * @brief Formatted print function for the shell
 * @param shell Pointer to the shell instance
 * @param format Printf-style format string
 * @param ... Variable arguments for the format string
 */
void shell_printf(shell_t *shell, const char *format, ...) {
    if ((shell == NULL) || (format == NULL)) {
        return;
    }

    char buffer[SHELL_MAX_LENGTH];
    va_list args;

    va_start(args, format);
    int len = vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    // Check for formatting errors or buffer overflow
    if (len > 0 && len < (int)sizeof(buffer)) {
        uart_driver_send(&shell->driver, (uint8_t *)buffer, (size_t)len);
    }
}

/**
 * @brief Initializes the shell instance
 * @param shell Pointer to the shell instance to initialize
 * @param huart Pointer to the UART handle to use for communication
 * @return true if initialization was successful, false otherwise
 */
bool shell_init(shell_t *shell, UART_HandleTypeDef *huart) {
    if ((shell == NULL) || (huart == NULL)) {
        return false;
    }

    // Clear the shell structure
    memset(shell, 0, sizeof(shell_t));

    // Initialize the UART driver
    if (!uart_driver_init(&shell->driver, huart)) {
        return false;
    }

    // Print welcome message and initial prompt
    shell_print_startup_message(shell);
    shell_send_prompt(shell);

    return true;
}

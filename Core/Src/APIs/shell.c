/**
 * @file shell.c
 * @brief UART shell implementation for STM32 microcontrollers.
 *
 * This file provides a simple command-line shell over UART, supporting
 * line editing, command history, and basic built-in commands.
 *
 * @author Santiago Rincon
 * @date 2025
 */

#include "shell.h"

#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>

#include "target_ver.h"
#include "cli_parser.h"

/**
 * @brief Prints the startup banner with project information.
 * @param shell Pointer to the shell instance.
 */
static void shell_print_startup_message(shell_t *shell);

/**
 * @brief Sends the command prompt to the user.
 * @param shell Pointer to the shell instance.
 */
static void shell_send_prompt(shell_t *shell);

/**
 * @brief Adds a command to the history buffer.
 * @param shell Pointer to the shell instance.
 * @param command Command string to add.
 */
static void shell_add_to_history(shell_t *shell, const char *command);

/**
 * @brief Clears the current input line on the terminal.
 * @param shell Pointer to the shell instance.
 */
static void shell_clear_line(shell_t *shell);

/**
 * @brief Redraws the current input line and positions cursor.
 * @param shell Pointer to the shell instance.
 */
static void shell_redraw_line(shell_t *shell);

/**
 * @brief Processes a complete command line received from the user.
 * @param shell Pointer to the shell instance.
 * @param command Command buffer.
 * @param length Length of the command.
 */
static void shell_process_command(shell_t *shell, uint8_t *command, uint16_t length);

/**
 * @brief Handles printable character input.
 * Inserts character at cursor position and updates display.
 * @param shell Pointer to the shell instance.
 * @param received_char Character received.
 */
static void handle_printable_character(shell_t *shell, uint8_t received_char);

/**
 * @brief Handles the ENTER key ('\r').
 * Processes the current input line.
 * @param shell Pointer to the shell instance.
 */
static void handle_carriage_return(shell_t *shell);

/**
 * @brief Handles the Backspace key.
 * Removes character before cursor and updates display.
 * @param shell Pointer to the shell instance.
 */
static void handle_backspace(shell_t *shell);

/**
 * @brief Moves cursor left.
 * @param shell Pointer to the shell instance.
 */
static void handle_cursor_left(shell_t *shell);

/**
 * @brief Moves cursor right.
 * @param shell Pointer to the shell instance.
 */
static void handle_cursor_right(shell_t *shell);

/**
 * @brief Handles up arrow (previous command in history).
 * @param shell Pointer to the shell instance.
 */
static void handle_cursor_up(shell_t *shell);

/**
 * @brief Handles down arrow (next command in history).
 * @param shell Pointer to the shell instance.
 */
static void handle_cursor_down(shell_t *shell);

/**
 * @brief Main shell processing loop.
 * Reads UART input and processes shell logic.
 * @param shell Pointer to the shell instance.
 */
void shell_task(shell_t *shell);

/**
 * @brief Formatted print function for the shell.
 * Sends formatted output to UART.
 * @param shell Pointer to the shell instance.
 * @param format Printf-style format string.
 * @param ... Variable arguments.
 */
void shell_printf(shell_t *shell, const char *format, ...);

/**
 * @brief Initializes the shell instance.
 * @param shell Pointer to the shell instance to initialize.
 * @param huart Pointer to the UART handle to use for communication.
 * @return true if initialization was successful, false otherwise.
 */
bool shell_init(shell_t *shell, UART_HandleTypeDef *huart);

static void shell_print_startup_message(shell_t *shell) {
    if (shell == NULL) {
        return;
    }

    shell_printf(shell, "****************************\r\n");
    shell_printf(shell, "Project: %s\r\n", PROJECT_DESCRIPTION);
    shell_printf(shell, "Version: %d.%d.%s\r\n", TARGET_VER_MAJOR, TARGET_VER_MINOR, TARGET_VER_DATE);
    shell_printf(shell, "Author: %s\r\n", AUTHOR);
    shell_printf(shell, "****************************\r\n");
}

static void shell_send_prompt(shell_t *shell) {
    if (shell == NULL) {
        return;
    }
    shell_printf(shell, PROMPT_STRING);
}

static void shell_add_to_history(shell_t *shell, const char *command) {
    if ((command == NULL) || (strlen(command) == 0)) {
        return;
    }

    // Don't add duplicate consecutive commands
    if (shell->history.count > 0) {
        size_t last_history_index = ((shell->history.current_index - 1) + SHELL_HISTORY_SIZE) % SHELL_HISTORY_SIZE;
        if (strcmp(shell->history.commands[last_history_index], command) == 0) {
            return;
        }
    }

    strncpy(shell->history.commands[shell->history.current_index], command, (SHELL_MAX_LENGTH - 1));
    shell->history.commands[shell->history.current_index][SHELL_MAX_LENGTH - 1] = '\0';

    shell->history.current_index = (shell->history.current_index + 1) % SHELL_HISTORY_SIZE;
    if (shell->history.count < SHELL_HISTORY_SIZE) {
        shell->history.count++;
    }

    shell->history.browse_index = shell->history.current_index;
}

static void shell_clear_line(shell_t *shell) {
    if (shell == NULL) {
        return;
    }

    // Move cursor to beginning of input
    for (size_t line_position = 0; line_position < shell->rx.cursor_pos; line_position++) {
        uart_driver_send(&shell->driver, (uint8_t *)"\b", 1);
    }

    // Clear the line
    for (size_t line_position = 0; line_position < shell->rx.length; line_position++) {
        uart_driver_send(&shell->driver, (uint8_t *)" ", 1);
    }

    // Move cursor back to beginning
    for (size_t line_position = 0; line_position < shell->rx.length; line_position++) {
        uart_driver_send(&shell->driver, (uint8_t *)"\b", 1);
    }
}

static void shell_redraw_line(shell_t *shell) {
    if (shell == NULL) {
        return;
    }

    // Print the buffer
    uart_driver_send(&shell->driver, shell->rx.buffer, shell->rx.length);

    // Position cursor correctly
    size_t chars_to_move_back = shell->rx.length - shell->rx.cursor_pos;
    for (size_t line_position = 0; line_position < chars_to_move_back; line_position++) {
        uart_driver_send(&shell->driver, (uint8_t *)"\b", 1);
    }
}

static void shell_process_command(shell_t *shell, uint8_t *command, uint16_t length) {
    if ((shell == NULL) || (command == NULL) || (length == 0)) {
        return;
    }

    // Trim whitespace and newlines
    while ((length > 0) && ((command[length - 1] == '\r') || (command[length - 1] == '\n') || (command[length - 1] == ' '))) {
        command[length - 1] = '\0';
        length--;
    }

    if (length == 0) {
        shell_printf(shell, NEWLINE_SEQ);
        shell_send_prompt(shell);
        return;
    }

    // Add to history
    shell_add_to_history(shell, (char *)command);

    shell_printf(shell, NEWLINE_SEQ);

    cli_parser_execute(shell, (char *) command);

    shell_send_prompt(shell);
}

static void handle_printable_character(shell_t *shell, uint8_t received_char) {
    if (shell == NULL) {
        return;
    }

    // Insert character at cursor position
    if (shell->rx.cursor_pos < shell->rx.length) {
        // Inserting in the middle - shift existing characters right
        size_t bytes_to_move = (shell->rx.length - shell->rx.cursor_pos);
        memmove(&shell->rx.buffer[shell->rx.cursor_pos + 1],
                &shell->rx.buffer[shell->rx.cursor_pos],
                bytes_to_move);
    }

    shell->rx.buffer[shell->rx.cursor_pos] = received_char;
    shell->rx.length++;
    shell->rx.buffer[shell->rx.length] = '\0';

    // Echo the character and handle display update
    if (shell->rx.cursor_pos == (shell->rx.length - 1)) {
        // Appending at the end - just echo the character
        uart_driver_send(&shell->driver, &received_char, 1);
    } else {
        // Inserted in the middle - redraw from cursor position to end
        size_t chars_from_cursor_to_end = (shell->rx.length - shell->rx.cursor_pos);
        uart_driver_send(&shell->driver, &shell->rx.buffer[shell->rx.cursor_pos], chars_from_cursor_to_end);

        // Move cursor back to correct position (after the inserted character)
        size_t cursor_backtrack_count = (chars_from_cursor_to_end - 1);
        for (size_t backtrack_idx = 0; backtrack_idx < cursor_backtrack_count; backtrack_idx++) {
            uart_driver_send(&shell->driver, (uint8_t *)"\b", 1);
        }
    }

    shell->rx.cursor_pos++;
}

static void handle_carriage_return(shell_t *shell) {
    if (shell == NULL) {
        return;
    }

    shell->rx.buffer[shell->rx.length] = '\0';
    shell_process_command(shell, shell->rx.buffer, shell->rx.length);

    // Reset input state
    shell->rx.length = 0;
    shell->rx.cursor_pos = 0;
    shell->history.browse_index = shell->history.current_index;
}

static void handle_backspace(shell_t *shell) {
    if ((shell == NULL) || (shell->rx.cursor_pos == 0)) {
        return;
    }

    // Move characters left to fill the gap
    size_t bytes_to_shift = (shell->rx.length - shell->rx.cursor_pos);
    memmove(&shell->rx.buffer[shell->rx.cursor_pos - 1],
            &shell->rx.buffer[shell->rx.cursor_pos],
            bytes_to_shift);

    shell->rx.cursor_pos--;
    shell->rx.length--;
    shell->rx.buffer[shell->rx.length] = '\0';

    // Update display
    uart_driver_send(&shell->driver, (uint8_t *)"\b", 1);
    size_t chars_from_cursor_to_end = (shell->rx.length - shell->rx.cursor_pos);
    uart_driver_send(&shell->driver, &shell->rx.buffer[shell->rx.cursor_pos], chars_from_cursor_to_end);
    uart_driver_send(&shell->driver, (uint8_t *)" \b", 2);

    // Move cursor back to correct position
    size_t cursor_backtrack_count = (shell->rx.length - shell->rx.cursor_pos);
    for (size_t backtrack_idx = 0; backtrack_idx < cursor_backtrack_count; backtrack_idx++) {
        uart_driver_send(&shell->driver, (uint8_t *)"\b", 1);
    }
}

static void handle_cursor_left(shell_t *shell) {
    if ((shell == NULL) || (shell->rx.cursor_pos == 0)) {
        return;
    }

    shell->rx.cursor_pos--;
    uart_driver_send(&shell->driver, (uint8_t *)"\b", 1);
}

static void handle_cursor_right(shell_t *shell) {
    if ((shell == NULL) || (shell->rx.cursor_pos >= shell->rx.length)) {
        return;
    }

    uart_driver_send(&shell->driver, &shell->rx.buffer[shell->rx.cursor_pos], 1);
    shell->rx.cursor_pos++;
}

static void handle_cursor_up(shell_t *shell) {
    if ((shell == NULL) || (shell->history.count == 0)) {
        return;
    }

    size_t previous_history_index = ((shell->history.browse_index - 1) + SHELL_HISTORY_SIZE) % SHELL_HISTORY_SIZE;

    // Check if we have a valid previous command
    size_t oldest_history_index = (shell->history.count < SHELL_HISTORY_SIZE) ? 0 : shell->history.current_index;
    if ((previous_history_index == oldest_history_index) && (shell->history.browse_index != shell->history.current_index)) {
        return;
    }

    shell->history.browse_index = previous_history_index;

    // Clear current line and load command from history
    shell_clear_line(shell);

    strcpy((char *)shell->rx.buffer, shell->history.commands[shell->history.browse_index]);
    shell->rx.length = strlen((char *)shell->rx.buffer);
    shell->rx.cursor_pos = shell->rx.length;

    shell_redraw_line(shell);
}

static void handle_cursor_down(shell_t *shell) {
    if ((shell == NULL) || (shell->history.count == 0)) {
        return;
    }

    if (shell->history.browse_index == shell->history.current_index) {
        // Already at newest, clear line
        shell_clear_line(shell);
        shell->rx.length = 0;
        shell->rx.cursor_pos = 0;
        shell->rx.buffer[0] = '\0';
        return;
    }

    shell->history.browse_index = (shell->history.browse_index + 1) % SHELL_HISTORY_SIZE;

    shell_clear_line(shell);

    if (shell->history.browse_index == shell->history.current_index) {
        // Back to current (empty) line
        shell->rx.length = 0;
        shell->rx.cursor_pos = 0;
        shell->rx.buffer[0] = '\0';
    } else {
        strcpy((char *)shell->rx.buffer, shell->history.commands[shell->history.browse_index]);
        shell->rx.length = strlen((char *)shell->rx.buffer);
        shell->rx.cursor_pos = shell->rx.length;
        shell_redraw_line(shell);
    }
}

void shell_task(shell_t *shell) {
    if (shell == NULL) return;

    static enum {
        STATE_NORMAL,
        STATE_ESC,
        STATE_CSI
    } parsing_state = STATE_NORMAL;

    uint8_t received_byte;

    while (uart_driver_get_byte(&shell->driver, &received_byte)) {

        // Handle buffer overflow
        if ((shell->rx.length >= (SHELL_MAX_LENGTH - 1)) && (received_byte != '\r') && (received_byte != 127)) {
            shell_printf(shell, NEWLINE_SEQ "Error: Command too long!" NEWLINE_SEQ);
            shell->rx.length = 0;
            shell->rx.cursor_pos = 0;
            shell_send_prompt(shell);
            continue;
        }

        switch (parsing_state) {
            case STATE_NORMAL:
                if (received_byte == 27) {
                    parsing_state = STATE_ESC;
                } else if (received_byte == '\r') {
                    handle_carriage_return(shell);
                } else if ((received_byte == 127) || (received_byte == 8)) {
                    handle_backspace(shell);
                } else if ((received_byte >= 32) && (received_byte <= 126)) {
                    handle_printable_character(shell, received_byte);
                }
                break;

            case STATE_ESC:
                if (received_byte == '[') {
                    parsing_state = STATE_CSI;
                } else {
                    parsing_state = STATE_NORMAL;
                }
                break;

            case STATE_CSI:
                switch (received_byte) {
                    case 'A':
                        handle_cursor_up(shell);
                        break;
                    case 'B':
                        handle_cursor_down(shell);
                        break;
                    case 'C':
                        handle_cursor_right(shell);
                        break;
                    case 'D':
                        handle_cursor_left(shell);
                        break;
                    default:
                        break;
                }
                parsing_state = STATE_NORMAL;
                break;
        }
    }
}

void shell_printf(shell_t *shell, const char *format, ...) {
    if ((shell == NULL) || (format == NULL)) {
        return;
    }

    char buffer[SHELL_MAX_LENGTH];
    va_list args;

    va_start(args, format);
    int len = vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    if ((len > 0) && (len < (int)sizeof(buffer))) {
        uart_driver_send(&shell->driver, (uint8_t *)buffer, (size_t)len);
    }
}

bool shell_init(shell_t *shell, UART_HandleTypeDef *huart) {
    if ((shell == NULL) || (huart == NULL)) {
        return false;
    }

    memset(shell, 0, sizeof(shell_t));

    if (!uart_driver_init(&shell->driver, huart)) {
        return false;
    }

    shell_print_startup_message(shell);
    shell_send_prompt(shell);

    return true;
}

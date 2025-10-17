/**
 * @file cli_parser.h
 * @brief Command parser interface for STM32 UART shell.
 *
 * Provides the CLI command parsing and dispatch API for processing
 * user commands and auto-completion functionality.
 *
 * @author Santiago Rincon
 * @date 2025
 */

#ifndef CLI_PARSER_H
#define CLI_PARSER_H

#include "shell.h"

/**
 * @enum tab_completion_result_t
 * @brief Tab completion result codes.
 */
typedef enum {
    TAB_COMPLETION_NO_MATCH = 0,        /**< No matches found */
    TAB_COMPLETION_SINGLE_MATCH = 1,    /**< Single match found */
    TAB_COMPLETION_HELP_SHOWN = 2,      /**< Help was shown for exact match */
    TAB_COMPLETION_MULTIPLE_MATCHES = 3 /**< Multiple matches found */
} tab_completion_result_t;

/**
 * @brief Parse and execute a CLI command line.
 * @param shell_parent Pointer to the shell instance.
 * @param command_line Null-terminated command line string.
 */
void cli_parser_execute(void *shell_parent, char *command_line);

/**
 * @brief Get the list of available commands for auto-completion.
 * @param commands Pointer to array of command strings (output).
 * @return Number of available commands.
 */
size_t cli_parser_get_commands(const char ***commands);

/**
 * @brief Handle TAB completion and return completion result.
 * @param shell_parent Pointer to the shell instance.
 * @param partial_input Partial command string to complete.
 * @param completion_buffer Buffer to store completed command (output).
 * @param buffer_size Size of completion buffer.
 * @return Tab completion result code.
 */
tab_completion_result_t cli_parser_handle_tab_completion(void *shell_parent, const char *partial_input, char *completion_buffer, size_t buffer_size);

#endif /* CLI_PARSER_H */

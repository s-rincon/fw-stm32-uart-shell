/**
 * @file cli_parser.h
 * @brief Command parser for STM32 UART shell.
 */

#ifndef __CLI_PARSER_INC_
#define __CLI_PARSER_INC_

/**
 * @brief Parses and executes a CLI command.
 * @param shell Pointer to the shell instance (for output).
 * @param command Null-terminated command string.
 */
void cli_parser_execute(void *shell_parent, char *command);

#endif /* __CLI_PARSER_INC_ */

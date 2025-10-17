/**
 * @file cli_parser.c
 * @brief Command parser implementation for STM32 UART shell.
 *
 * This file implements the CLI command parsing and dispatch logic,
 * including help, clear, history, and version commands.
 * Each command handler validates its arguments and prints usage/help as needed.
 *
 * @author Santiago Rincon
 * @date 2025
 */

#include "cli_parser.h"
#include <string.h>
#include <stdio.h>
#include "target_ver.h"
#include "shell.h"

#define TOTAL_COMMANDS          (4U)    /**< Total number of available commands */
#define COMMAND_MAX_LENGTH      (10U)   /**< Maximum length of command name */
#define CLI_MAX_ARGS            (5U)    /**< Maximum arguments per command */

#define TOO_MANY_ARGUMENTS_TEXT "too many arguments"    /**< Error message for excess arguments */
#define UNKNOWN_ARGUMENT_TEXT   "unknown argument"      /**< Error message for unknown arguments */
#define UNKNOWN_ARGUMENT_SEQ    UNKNOWN_ARGUMENT_TEXT "%s " NEWLINE_SEQ  /**< Formatted unknown argument message */

// --- Help text constants ---
static const char help_general_text[] =
    "Available commands:" NEWLINE_SEQ
    TAB_SEQ "help    - Show this help" NEWLINE_SEQ
    TAB_SEQ "clear   - Clear screen" NEWLINE_SEQ
    TAB_SEQ "history - Show command history" NEWLINE_SEQ
    TAB_SEQ "version - Show version info" NEWLINE_SEQ
    "Type 'help <command>' for details on a specific command." NEWLINE_SEQ NEWLINE_SEQ;

static const char help_clear_text[] =
    "clear: Clears the terminal screen." NEWLINE_SEQ
    TAB_SEQ "Usage: clear (no params)" NEWLINE_SEQ NEWLINE_SEQ;

static const char help_history_text[] =
    "history: Shows the command history." NEWLINE_SEQ
    TAB_SEQ "Usage: history (no params)" NEWLINE_SEQ NEWLINE_SEQ;

static const char help_version_text[] =
    "version: Shows firmware version information." NEWLINE_SEQ
    TAB_SEQ "Usage: version (no params)" NEWLINE_SEQ NEWLINE_SEQ;

// --- Command handler prototypes ---
/**
 * @brief Handle the 'help' command.
 * @param shell Pointer to the shell instance.
 * @param argc Argument count.
 * @param argv Argument vector.
 */
static void cli_cmd_help(shell_t *shell, int argc, char **argv);

/**
 * @brief Handle the 'clear' command.
 * @param shell Pointer to the shell instance.
 * @param argc Argument count.
 * @param argv Argument vector.
 */
static void cli_cmd_clear(shell_t *shell, int argc, char **argv);

/**
 * @brief Handle the 'history' command.
 * @param shell Pointer to the shell instance.
 * @param argc Argument count.
 * @param argv Argument vector.
 */
static void cli_cmd_history(shell_t *shell, int argc, char **argv);

/**
 * @brief Handle the 'version' command.
 * @param shell Pointer to the shell instance.
 * @param argc Argument count.
 * @param argv Argument vector.
 */
static void cli_cmd_version(shell_t *shell, int argc, char **argv);

// --- Available commands list ---
static const char *available_commands[] = {"help", "clear", "history", "version"};
static const size_t num_available_commands = sizeof(available_commands) / sizeof(available_commands[0]);


/**
 * @brief Execute CLI command with argument parsing and dispatch.
 *
 * Parses the command line into arguments and dispatches to the appropriate
 * command handler. Handles unknown commands with error messages.
 *
 * @param shell_parent Pointer to the shell instance (cast to shell_t).
 * @param command_line Command line string to parse and execute.
 */
void cli_parser_execute(void *shell_parent, char *command_line) {
    if ((shell_parent == NULL) || (command_line == NULL)) {
        return;
    }
    shell_t *shell = (shell_t *)shell_parent;

    char *argv[CLI_MAX_ARGS];
    int argc = 0;
    char *token = strtok(command_line, " ");
    while ((token != NULL) && (argc < (int)CLI_MAX_ARGS)) {
        argv[argc] = token;
        argc++;
        token = strtok(NULL, " ");
    }
    if (argc == 0) {
        return;
    }

    if (strcmp(argv[0], "help") == 0) {
        cli_cmd_help(shell, argc, argv);
    } else if (strcmp(argv[0], "clear") == 0) {
        cli_cmd_clear(shell, argc, argv);
    } else if (strcmp(argv[0], "history") == 0) {
        cli_cmd_history(shell, argc, argv);
    } else if (strcmp(argv[0], "version") == 0) {
        cli_cmd_version(shell, argc, argv);
    } else {
        shell_printf(shell, "Unknown command or argument: %s" NEWLINE_SEQ, argv[0]);
        shell_printf(shell, "Type 'help' for available commands." NEWLINE_SEQ NEWLINE_SEQ);
    }
}

static void cli_cmd_help(shell_t *shell, int argc, char **argv) {
    if (argc > 3) {
        shell_printf(shell, TOO_MANY_ARGUMENTS_TEXT NEWLINE_SEQ);
        return;
    }
    if (argc == 1) {
        shell_printf(shell, help_general_text);
    } else {
        const char *cmd = argv[1];
        if (strcmp(cmd, "clear") == 0) {
            shell_printf(shell, help_clear_text);
        } else if (strcmp(cmd, "history") == 0) {
            shell_printf(shell, help_history_text);
        } else if (strcmp(cmd, "version") == 0) {
            shell_printf(shell, help_version_text);
        } else if (strcmp(cmd, "help") == 0) {
            // Ignore on purpose
        } else {
            shell_printf(shell, "help: " UNKNOWN_ARGUMENT_SEQ, cmd);
        }
    }
}

static void cli_cmd_clear(shell_t *shell, int argc, char **argv) {
    if (argc > 2) {
        shell_printf(shell, TOO_MANY_ARGUMENTS_TEXT NEWLINE_SEQ);
        return;
    }
    if (argc == 2) {
        if (strcmp(argv[1], "help") == 0) {
            shell_printf(shell, help_clear_text);
        } else {
            shell_printf(shell, "clear:  " UNKNOWN_ARGUMENT_SEQ, argv[1]);
        }
        return;
    }
    shell_clear_screen(shell);
}

static void cli_cmd_history(shell_t *shell, int argc, char **argv) {
    if (argc > 2) {
        shell_printf(shell, TOO_MANY_ARGUMENTS_TEXT NEWLINE_SEQ);
        return;
    }
    if (argc == 2) {
        if (strcmp(argv[1], "help") == 0) {
            shell_printf(shell, help_history_text);
        } else {
            shell_printf(shell, "history: " UNKNOWN_ARGUMENT_SEQ, argv[1]);
        }
        return;
    }
    shell_print_history(shell);
}

static void cli_cmd_version(shell_t *shell, int argc, char **argv) {
    if (argc > 2) {
        shell_printf(shell, TOO_MANY_ARGUMENTS_TEXT NEWLINE_SEQ);
        return;
    }
    if (argc == 2) {
        if (strcmp(argv[1], "help") == 0) {
            shell_printf(shell, help_version_text);
        } else {
            shell_printf(shell, "version: " UNKNOWN_ARGUMENT_SEQ, argv[1]);
        }
        return;
    }
    shell_printf(shell, "Version: %d.%d.%s" NEWLINE_SEQ NEWLINE_SEQ, TARGET_VER_MAJOR, TARGET_VER_MINOR, TARGET_VER_DATE);
}

size_t cli_parser_get_commands(const char ***commands) {
    if (commands != NULL) {
        *commands = available_commands;
    }
    return num_available_commands;
}

tab_completion_result_t cli_parser_handle_tab_completion(void *shell_parent, const char *partial_input, char *completion_buffer, size_t buffer_size) {
    if ((shell_parent == NULL) || (partial_input == NULL) || (completion_buffer == NULL) || (buffer_size == 0U)) {
        return TAB_COMPLETION_NO_MATCH;
    }

    shell_t *shell = (shell_t *)shell_parent;
    size_t input_len = strlen(partial_input);

    (void)strncpy(completion_buffer, partial_input, input_len);
    completion_buffer[input_len] = '\0';

    char safe_input[SHELL_MAX_LENGTH];
    (void)memset(safe_input, 0, sizeof(safe_input));
    (void)strncpy(safe_input, partial_input, input_len);
    safe_input[input_len] = '\0';

    /* Check for exact command match */
    for (size_t cmd_idx = 0U; cmd_idx < num_available_commands; cmd_idx++) {
        if (strncmp(safe_input, available_commands[cmd_idx], strlen(available_commands[cmd_idx])) == 0) {
            /* Show help for this command */
            char help_line[32];
            (void)snprintf(help_line, sizeof(help_line), "%s help", available_commands[cmd_idx]);

            shell_printf(shell, NEWLINE_SEQ);
            cli_parser_execute(shell, help_line);
            return TAB_COMPLETION_HELP_SHOWN;
        }
    }

    size_t matches = 0U;
    const char *single_match = NULL;
    for (size_t cmd_idx = 0U; cmd_idx < num_available_commands; cmd_idx++) {
        if (strncmp(safe_input, available_commands[cmd_idx], input_len) == 0) {
            matches++;
            single_match = available_commands[cmd_idx];
        }
    }

    tab_completion_result_t result;
    if (matches == 1U) {
        (void)strncpy(completion_buffer, single_match, strlen(single_match));
        completion_buffer[strlen(single_match)] = '\0';
        result = TAB_COMPLETION_SINGLE_MATCH;
    } else if (matches > 1U) {
        /* Show options */
        shell_printf(shell, NEWLINE_SEQ "%s ", (input_len == 0U) ? "Available:" : "Options:");
        for (size_t i = 0U; i < num_available_commands; i++) {
            if (strncmp(safe_input, available_commands[i], input_len) == 0) {
                shell_printf(shell, "%s ", available_commands[i]);
            }
        }
        shell_printf(shell, NEWLINE_SEQ NEWLINE_SEQ);
        result = TAB_COMPLETION_MULTIPLE_MATCHES;
    } else {
        result = TAB_COMPLETION_NO_MATCH;
    }

    return result;
}

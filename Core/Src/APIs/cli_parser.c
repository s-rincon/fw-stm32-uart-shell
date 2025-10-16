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

#define CLI_MAX_ARGS 5
#define TOO_MANY_ARGUMENTS_TEXT "too many arguments"
#define UNKNOWN_ARGUMENT_TEXT   "unknown argument"
#define UNKNOWN_ARGUMENT_SEQ    UNKNOWN_ARGUMENT_TEXT "%s " NEWLINE_SEQ

// --- Help text constants ---
static const char help_general_text[] =
    "Available commands:" NEWLINE_SEQ
    TAB_SEQ "help    - Show this help" NEWLINE_SEQ
    TAB_SEQ "clear   - Clear screen" NEWLINE_SEQ
    TAB_SEQ "history - Show command history" NEWLINE_SEQ
    TAB_SEQ "version - Show version info" NEWLINE_SEQ
    "Type 'help <command>' for details on a specific command." NEWLINE_SEQ;

static const char help_clear_text[] =
    "clear: Clears the terminal screen." NEWLINE_SEQ
    TAB_SEQ "Usage: clear (no params)" NEWLINE_SEQ;

static const char help_history_text[] =
    "history: Shows the command history." NEWLINE_SEQ
    TAB_SEQ "Usage: history (no params)" NEWLINE_SEQ;

static const char help_version_text[] =
    "version: Shows firmware version information." NEWLINE_SEQ
    TAB_SEQ "Usage: version (no params)" NEWLINE_SEQ;

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


void cli_parser_execute(void *shell_parent, char *command_line) {
    if ((shell_parent == NULL) || (command_line == NULL)) {
        return;
    }
    shell_t *shell = (shell_t *)shell_parent;

    char *argv[CLI_MAX_ARGS];
    int argc = 0;
    char *token = strtok(command_line, " ");
    while (token != NULL && argc < CLI_MAX_ARGS) {
        argv[argc++] = token;
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
        shell_printf(shell, "Type 'help' for available commands." NEWLINE_SEQ);
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
        }else {
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
    shell_printf(shell, "Version: %d.%d.%s" NEWLINE_SEQ, TARGET_VER_MAJOR, TARGET_VER_MINOR, TARGET_VER_DATE);
}

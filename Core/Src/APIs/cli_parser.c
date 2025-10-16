#include "cli_parser.h"
#include <string.h>
#include <stdio.h>

#include "target_ver.h"
#include "shell.h"

#define CLI_MAX_ARGS 5

#define TOO_MANY_ARGUMENTS_TEXT "too many arguments"

// --- Command handler prototypes ---
static void cli_cmd_help(shell_t *shell, int argc, char **argv);
static void cli_cmd_clear(shell_t *shell, int argc, char **argv);
static void cli_cmd_history(shell_t *shell, int argc, char **argv);
static void cli_cmd_version(shell_t *shell, int argc, char **argv);
static void cli_cmd_unknown(shell_t *shell, int argc, char **argv);

void cli_parser_execute(void *shell_parent, char *command_line) {
    if ((shell_parent == NULL) || (command_line == NULL)) {
        return;
    }

    shell_t *shell = (shell_t *)shell_parent;

    // Parse command and up to 4 parameters
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

    // Dispatch to command handlers
    if (strcmp(argv[0], "help") == 0) {
        cli_cmd_help(shell, argc, argv);
    } else if (strcmp(argv[0], "clear") == 0) {
        cli_cmd_clear(shell, argc, argv);
    } else if (strcmp(argv[0], "history") == 0) {
        cli_cmd_history(shell, argc, argv);
    } else if (strcmp(argv[0], "version") == 0) {
        cli_cmd_version(shell, argc, argv);
    } else {
        cli_cmd_unknown(shell, argc, argv);
    }
}

// --- Command handler implementations ---

static void cli_cmd_help(shell_t *shell, int argc, char **argv) {
    if (argc > 2) {
        shell_printf(shell, TOO_MANY_ARGUMENTS_TEXT NEWLINE_SEQ);
        return;
    }
    shell_printf(shell, "Available commands:" NEWLINE_SEQ);
    shell_printf(shell, "  help    - Show this help" NEWLINE_SEQ);
    shell_printf(shell, "  clear   - Clear screen" NEWLINE_SEQ);
    shell_printf(shell, "  history - Show command history" NEWLINE_SEQ);
    shell_printf(shell, "  version - Show version info" NEWLINE_SEQ);
}

static void cli_cmd_clear(shell_t *shell, int argc, char **argv) {
    if (argc > 1) {
        shell_printf(shell, TOO_MANY_ARGUMENTS_TEXT NEWLINE_SEQ);
        return;
    }
    shell_clear_screen(shell);
}

static void cli_cmd_history(shell_t *shell, int argc, char **argv) {
    if (argc > 1) {
        shell_printf(shell, TOO_MANY_ARGUMENTS_TEXT NEWLINE_SEQ);
        return;
    }
    shell_print_history(shell);
}

static void cli_cmd_version(shell_t *shell, int argc, char **argv) {
    if (argc > 1) {
        shell_printf(shell, TOO_MANY_ARGUMENTS_TEXT NEWLINE_SEQ);
        return;
    }
    shell_printf(shell, "Version: %d.%d.%s" NEWLINE_SEQ, TARGET_VER_MAJOR, TARGET_VER_MINOR, TARGET_VER_DATE);
}

static void cli_cmd_unknown(shell_t *shell, int argc, char **argv) {
    (void)argc;
    shell_printf(shell, "Unknown command: %s" NEWLINE_SEQ, argv[0]);
    shell_printf(shell, "Type 'help' for available commands." NEWLINE_SEQ);
}

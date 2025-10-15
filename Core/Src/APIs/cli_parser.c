#include "cli_parser.h"
#include <string.h>
#include <stdio.h>

#include "target_ver.h"
#include "shell.h"



void cli_parser_execute(void *shell_parent, char *command) {
    if ((shell_parent == NULL) || (command == NULL)) {
        return;
    }

    shell_t *shell = (shell_t *)shell_parent;

    // Example built-in commands
    if (strcmp(command, "help") == 0) {
        shell_printf(shell, "Available commands:" NEWLINE_SEQ);
        shell_printf(shell, "  help    - Show this help" NEWLINE_SEQ);
        shell_printf(shell, "  clear   - Clear screen" NEWLINE_SEQ);
        shell_printf(shell, "  history - Show command history" NEWLINE_SEQ);
        shell_printf(shell, "  version - Show version info" NEWLINE_SEQ);
    } else if (strcmp(command, "clear") == 0) {
        shell_printf(shell, "\033[2J\033[H");
    } else if (strcmp(command, "history") == 0) {
        shell_printf(shell, "Command history:" NEWLINE_SEQ);
        for (size_t i = 0; i < shell->history.count; i++) {
            size_t idx = ((shell->history.current_index - shell->history.count + i) + SHELL_HISTORY_SIZE) % SHELL_HISTORY_SIZE;
            shell_printf(shell, "  %u: %s" NEWLINE_SEQ, (i + 1), shell->history.commands[idx]);
        }
    } else if (strcmp(command, "version") == 0) {
        shell_printf(shell, "Version: %d.%d.%s" NEWLINE_SEQ, TARGET_VER_MAJOR, TARGET_VER_MINOR, TARGET_VER_DATE);
    } else {
        shell_printf(shell, "Unknown command: %s" NEWLINE_SEQ, command);
        shell_printf(shell, "Type 'help' for available commands." NEWLINE_SEQ);
    }
}

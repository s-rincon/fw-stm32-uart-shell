# STM32 UART Shell Overview

## Introduction

This project implements a robust UART-based shell for STM32 microcontrollers. The shell provides a command-line interface over UART, supporting interactive line editing, command history, and a modular command parser. It is designed for embedded debugging, device configuration, and runtime interaction.

---

## Architecture

- **shell.c / shell.h**: Implements the shell state machine, line editing, history, and prompt logic. Handles user input and output over UART.
- **cli_parser.c / cli_parser.h**: Implements the command parser and dispatcher. Receives parsed command lines from the shell and executes the appropriate handler.
- **uart_driver.c / uart_driver.h**: Provides a low-level, register-based UART driver with circular TX/RX buffers. Used by the shell for all UART communication.

---

## Command Flow

1. **User types a command** in a terminal connected to the STM32 UART.
2. **shell.c** receives and buffers input, supports editing, and detects command completion (ENTER).
3. On command completion, **shell.c** calls `cli_parser_execute(shell, command_line)`.
4. **cli_parser.c** parses the command and up to 4 arguments, dispatches to the appropriate handler, and prints results using `shell_printf`.
5. **shell.c** manages command history and prompt display.

---

## Supported Commands

| Command         | Description                   | Arguments         | Help Syntax                |
|-----------------|------------------------------|-------------------|----------------------------|
| `help`          | Show all commands or help for a specific command | `[command]` (optional) | `help` or `help <command>` |
| `clear`         | Clear the terminal screen     | `help` (optional) | `clear` or `clear help`    |
| `history`       | Show command history          | `help` (optional) | `history` or `history help`|
| `version`       | Show firmware version info    | `help` (optional) | `version` or `version help`|

- **No other arguments are accepted** for these commands. If an unknown argument is passed, an error message is shown.
- Typing `help <command>` or `<command> help` will print usage and parameter information for that command.

---

## Command Argument Handling

- The shell supports up to **4 arguments** per command (including the command itself).
- Each command handler validates its arguments:
  - If too many arguments are provided, `"too many arguments"` is printed.
  - If an unknown argument is provided (other than `help`), `"unknown argument"` is printed.
  - For `help`, if a command is unknown, `"help: unknown argument <cmd>"` is printed.

---

## UART Driver Relationship

- The shell uses a **register-based UART driver** (`uart_driver.c`) for all communication.
- The driver uses circular buffers for TX and RX, and is interrupt-driven for efficiency.
- All shell output (including command responses and prompts) is sent via `uart_driver_send`.
- The shell is decoupled from the hardware abstraction layer (HAL) and interacts directly with UART registers for performance and portability.

---

## CLI Parser Relationship

- The shell is responsible for **input, editing, and history**.
- The CLI parser (`cli_parser.c`) is responsible for **command parsing and dispatch**.
- The shell passes the command line to the parser, which splits it into arguments and calls the appropriate handler.
- The parser uses the shell's output functions to print responses and errors.

---

## Example Session

```
STM32 > help
Available commands:
    help    - Show this help
    clear   - Clear screen
    history - Show command history
    version - Show version info
Type 'help <command>' for details on a specific command.
STM32 > version
Version: 1.0.202406
STM32 > clear help
clear: Clears the terminal screen.
    Usage: clear (no params)
STM32 > foo
Unknown command or argument: foo
Type 'help' for available commands.
```

---

## Extending the Shell

- To add new commands, implement a new handler in `cli_parser.c` and update the dispatch logic.
- To change UART behavior, modify `uart_driver.c` (e.g., buffer sizes, baud rate, or interrupt handling).
- To customize prompt or history, edit `shell.c`.

---

## Notes

- The shell is designed for VT100-compatible terminals (e.g., PuTTY, minicom).
- Command history size and input buffer length are configurable via macros in `shell.h`.
- The shell and parser are modular and can be reused in other STM32 projects.

---

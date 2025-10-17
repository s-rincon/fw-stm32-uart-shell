# STM32 UART Shell

A robust, feature-rich UART shell for STM32 microcontrollers with line editing, command history, and auto-completion.

## Features

- **Interactive Line Editing**: Insert, delete, and navigate through command lines
- **Command History**: Navigate through previously entered commands with arrow keys
- **Tab Auto-Completion**: Complete commands and show help with TAB key
- **Built-in Commands**: help, clear, history, version
- **Modular Design**: Easy to extend with new commands
- **Register-Based UART**: Direct register access for optimal performance
- **VT100 Compatible**: Works with PuTTY, minicom, and other terminal emulators

## Binary Files

Pre-compiled firmware binaries are available in the `bin/` folder:

- **Latest Release**: `FW-STM32-UART-SHELL-V1.0.20251017.hex`
- **Format**: Intel HEX format for direct flashing
- **Target**: STM32F4xx series (configurable for other families)

### Flashing Instructions

1. Use STM32CubeProgrammer or your preferred flashing tool
2. Flash the `.hex` file to your STM32 microcontroller
3. Connect a UART terminal at 115200 baud, 8N1
4. Reset the device to see the shell prompt

## Usage

### Basic Commands

```
STM32 > help
Available commands:
    help    - Show this help
    clear   - Clear screen
    history - Show command history
    version - Show version info
Type 'help <command>' for details on a specific command.

STM32 > version
Version: 1.0.20251017

STM32 > clear
[clears screen]
```

### Auto-Completion

- Press **TAB** to auto-complete commands
- Press **TAB** on complete command to show help
- Multiple matches show available options

### Line Editing

- **Arrow Keys**: Navigate history (↑↓) and cursor (←→)
- **Backspace**: Delete character before cursor
- **Insert Mode**: Type to insert at cursor position

## Building from Source

### Prerequisites

- STM32CubeIDE or compatible toolchain
- STM32CubeMX (optional, for hardware configuration)
- Target STM32 development board

### Project Structure

```
fw-stm32-uart-shell/
├── bin/                    # Pre-compiled binaries
├── Core/
│   ├── Inc/
│   │   └── APIs/
│   │       ├── shell.h     # Shell interface
│   │       ├── cli_parser.h # Command parser interface
│   │       └── uart_driver.h # UART driver interface
│   └── Src/
│       └── APIs/
│           ├── shell.c     # Shell implementation
│           ├── cli_parser.c # Command parser
│           └── uart_driver.c # Register-based UART driver
├── doc/                    # Documentation
└── CHANGELOG.md           # Version history
```

### Build Steps

1. Open project in STM32CubeIDE
2. Configure UART pins in `main.c` or CubeMX
3. Build project (Ctrl+B)
4. Flash to target device

## Configuration

### UART Settings

Default configuration (modify in `shell.h`):
- **Baud Rate**: 115200
- **Data Bits**: 8
- **Parity**: None
- **Stop Bits**: 1
- **Flow Control**: None

### Buffer Sizes

```c
#define SHELL_MAX_LENGTH 128    // Command line length
#define SHELL_HISTORY_SIZE 10   // Number of history entries
```

## Extending the Shell

### Adding New Commands

1. Add command to `available_commands[]` in `cli_parser.c`
2. Add handler function `cli_cmd_yourcommand()`
3. Add dispatch case in `cli_parser_execute()`
4. Update help text

Example:
```c
static void cli_cmd_status(shell_t *shell, int argc, char **argv) {
    shell_printf(shell, "System Status: OK\r\n");
}
```

## Hardware Requirements

- STM32 microcontroller (F4xx series recommended)
- UART peripheral configured
- Terminal application (PuTTY, Tera Term, etc.)

## License

This project is provided as-is for educational and development purposes.

## Author

Santiago Rincon - 2025

## Version

Current Release: **v1.0.20251017**

See [CHANGELOG.md](CHANGELOG.md) for version history.

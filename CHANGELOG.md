# Changelog

All notable changes to the STM32 UART Shell project will be documented in this file.

## [1.0.20251017] - 2025-01-17

### Added
- Interactive UART shell with command prompt
- Line editing with cursor movement (left/right arrows)
- Character insertion and deletion at cursor position
- Command history with up/down arrow navigation
- Tab auto-completion for commands
- Help display when TAB pressed on complete commands
- `help` command - Shows all commands or help for specific command
- `clear` command - Clears terminal screen
- `history` command - Shows command history
- `version` command - Shows firmware version
- All commands support `<command> help` syntax
- Register-based UART driver (no HAL dependency)
- Circular TX/RX buffers for reliable communication
- Interrupt-driven UART operation
- Modular CLI parser for easy command extension
- VT100 terminal compatibility
- MISRA C compliance
- Full Doxygen documentation
- Configurable command line buffer size (default: 128 chars)
- Configurable history size (default: 10 commands)
- Configurable UART buffer sizes
- Support for multiple STM32 UART peripherals
- Binary release: `FW-STM32-UART-SHELL-V1.0.20251017.hex`

---

**Download**: [FW-STM32-UART-SHELL-V1.0.20251017.hex](bin/FW-STM32-UART-SHELL-V1.0.20251017.hex)

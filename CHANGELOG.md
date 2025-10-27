# Changelog

All notable changes to the STM32 UART Shell project will be documented in this file.

## [1.1.20251027] - 2025-10-27

### Added
- LED driver module (`led_driver.c/.h`) with on/off/toggle/blink support
- Heartbeat LED on `HEARTBEAT_LED_Pin` blinking every 500 ms (configured in `main.c`)
- User LED instance (`user_led`) integrated into main loop
- New `led` CLI command with subcommands:
	- `led on` — turn the user LED on
	- `led off` — turn the user LED off
	- `led toggle` — toggle the user LED
	- `led blink <ms>` — start blinking with a period in milliseconds (1–10000)
	- `led get_state` — display current LED state or blinking status

### Changed
- CLI help and TAB auto-completion updated to include the `led` command
- Shell main loop calls `led_driver_task()` to service blinking

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

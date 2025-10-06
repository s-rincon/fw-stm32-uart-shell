# 🧰 UART Debug Shell (STM32 Firmware)

A lightweight, modular **UART Command Line Interface (CLI)** for STM32 microcontrollers.  
This project provides a simple but professional-grade shell to control and debug embedded systems through serial commands.

---

## 🧠 Overview

The **UART Debug Shell** allows developers to send commands via UART (for example, using PuTTY or TeraTerm) to interact with firmware functions in real time.

### Example
```

> help
> Available commands:
> led on   - Turn the LED on
> led off  - Turn the LED off
> status   - Show system status

```

It’s a minimal but extensible framework designed to demonstrate:
- **Clean firmware architecture**
- **Interrupt-driven UART communication**
- **Command parsing and dispatching**

---

## ⚙️ Features

- Modular C structure (drivers, CLI core, command handlers)
- UART interrupt or DMA RX/TX handling
- Easy command registration and extension
- Built-in basic commands (`help`, `led on/off`, `status`, etc.)
- Portable and ready for integration in any embedded project
- Cleanly documented and maintainable

---

## 🧩 Project Structure
```
TBD
```

*(Structure is a placeholder and will evolve as the project grows.)*

---

## 🧱 Architecture

### High-Level Flow
```

[ UART RX Interrupt ]
        ↓
[ RX Buffer / Queue ]
        ↓
[ CLI Parser ]
        ↓
[ Command Handlers ]
        ↓
[ UART TX for responses ]

```

Each module has a single responsibility:
- **uart_driver** → handles low-level UART operations  
- **cli_core** → parses user input and dispatches commands  
- **cli_commands** → implements the actual command functions  

---

## 🧪 Example Output
```

> version
> UART Debug Shell v0.1
> Built on: 2025-10-06

> led on
> LED turned ON

> status
> System OK | Temp: 32.1°C | Uptime: 00:04:23

````

---

## 🛠️ Build Instructions

You can build this project using **STM32CubeIDE**.

### STM32CubeIDE
1. Import project as “Existing Code as Makefile Project”
2. Select your target MCU or board (e.g., STM32F429-DISC)
3. Build and flash the firmware

---

## 🔌 Serial Configuration

| Parameter | Value  |
| --------- | ------ |
| Baudrate  | 115200 |
| Data bits | 8      |
| Parity    | None   |
| Stop bits | 1      |
| Flow Ctrl | None   |

---

## 🚀 Planned Features

* [ ] Implement LED driver
* [ ] Implement DMA-based UART
* [ ] Implement CLI parser
* [ ] Add history and autocomplete
* [ ] Support floating-point and hex parsing
* [ ] Add `log` and `config` commands

---

## 📜 License

This project is licensed under the **MIT License** — see the [LICENSE](LICENSE) file for details.

---

## ✍️ Author

**Santiago Rincón Carreño**  

Embedded Software Developer

🌐 [Github Account](https://github.com/s-rincon)  

💼 [LinkedIn](https://www.linkedin.com/in/santiago-rinconc)

📧 [santiagorinconc.05@gmail.com](mailto:santiagorinconc.05@gmail.com)

---

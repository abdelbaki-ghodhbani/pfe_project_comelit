# STM32 + ESP8266 Wi-Fi Home Control

> Final-year engineering project (PFE) — Embedded Software Engineer internship at **Comelit Group**, Feb–Jun 2024.

Bare-metal firmware for an **STM32F401RE (Nucleo-64)** that drives an **ESP8266** Wi-Fi module through AT commands and hosts a small **TCP/HTTP web server**. Any browser on the same network can open the board's IP address and switch a device (here, the on-board LED on PA5) **on or off** remotely.

## Highlights

- **Register-level drivers** — UART, GPIO and SysTick are configured directly through CMSIS registers (no HAL), including baud-rate computation
- **Interrupt-driven circular buffers** for the ESP8266 UART and a debug UART, with helpers to search and parse AT responses (`+IPD`, `OK`, …)
- **Reusable ESP82xx library** — reset, self-test, station mode, AP connection, local IP lookup, multi-connection mode and TCP server creation
- **Embedded web UI** — HTML pages are served straight from the MCU and update to show the current light status
- **Debug console** over USART2 (ST-Link virtual COM port) that logs every step of the Wi-Fi setup

## Architecture

```
Browser ──HTTP──► ESP8266 (TCP server, port 80) ──UART/AT──► STM32F401RE ──GPIO──► Light (PA5)
                                                                  │
                                                                  └──USART2──► PC debug console
```

## Project structure

```
stm32_esp826_pfeproject/
├── Inc/ , Src/
│   ├── main.c              # Application entry: init + server loop
│   ├── esp82xx_driver.c    # UART init (ESP + debug), SysTick delay, printf redirect
│   ├── esp82xx_lib.c       # ESP8266 AT-command library and web server
│   ├── circular_buffer.c   # Interrupt-driven UART ring buffers and response parsing
│   └── hardware_module.c   # Light (GPIO) control
├── Startup/                # Startup assembly
└── STM32F401RETX_*.ld      # Linker scripts
F4_chip_headers/            # CMSIS device headers
```

## Hardware

| Component | Role |
|---|---|
| NUCLEO-F401RE | Main MCU (ARM Cortex-M4) |
| ESP8266 module | Wi-Fi module, AT firmware |
| LED / relay on PA5 | Controlled device |

## Getting started

1. Open `stm32_esp826_pfeproject` in **STM32CubeIDE**.
2. In `Src/main.c`, set your network credentials:
   ```c
   #define SSID_NAME  "YOUR_WIFI_SSID"
   #define PASSKEY    "YOUR_WIFI_PASSWORD"
   ```
3. Wire the ESP8266 to the ESP UART pins and power it from 3.3 V.
4. Build and flash, then open a serial terminal on the ST-Link COM port to read the board's IP address.
5. Open `http://<board-ip>` in a browser and use the buttons to switch the light.

## Skills demonstrated

`Embedded C` · `STM32` · `Bare-metal / CMSIS` · `UART & interrupts` · `ESP8266 AT commands` · `TCP/IP` · `HTML` · `Git`

## Author

**Abdelbaki Ghodhbani** — [LinkedIn](https://www.linkedin.com/in/abdelbaki-ghodhbani) · [GitHub](https://github.com/abdelbaki-ghodhbani)

# Bare-Metal STM32 + ESP8266 Wi-Fi Home Control

> Graduation project (PFE), Embedded Software Engineer internship at **Comelit Group**, Ariana, Tunisia (Feb–Jun 2024)
> *Development of drivers and an IoT application*, National Engineering School of Monastir (ENIM)

Bare-metal firmware for the **NUCLEO-F401RE** board (STM32F401RE, ARM Cortex-M4). It drives an **ESP8266-01** Wi-Fi module with AT commands and turns it into a small **TCP/HTTP web server**. Any browser on the same network can open the board's IP address and switch a device, here the user LED **LD2 on PA5**, **on or off** from a web page.

> ⚠️ **Hardware-specific, bare-metal code.** The project uses **no HAL and no CubeMX**. All peripherals are configured by writing directly to the **STM32F401RE registers** through the CMSIS device header. It runs as-is only on a **NUCLEO-F401RE**. For any other board or MCU, see [Porting to another STM32](#porting-to-another-stm32-board).

---

## System architecture

```
                         UART1 (PA9/PA10, 115200 8N1)             Wi-Fi (802.11 b/g/n)
 ┌──────────────┐ GPIO  ┌──────────────────────┐  AT cmd/rsp  ┌────────────────┐        ┌──────────────┐
 │ User LED LD2 │◄──────┤  NUCLEO-F401RE       ├─────────────►│  ESP8266-01    │◄──────►│ Web browser  │
 │     (PA5)    │       │  STM32F401RE @16 MHz │◄─────────────┤  TCP server :80│  HTTP  │ (any device) │
 └──────────────┘       └──────────┬───────────┘              └────────────────┘        └──────────────┘
                                   │ UART2 (PA2/PA3, 115200 8N1) via ST-LINK virtual COM port
                                   ▼
                            PC serial terminal (RealTerm): debug log
```

## Low-level drivers developed

| Module | File | What it does (register level) |
|---|---|---|
| **UART driver** | `esp82xx_driver.c` | Enables the clocks in `RCC->AHB1ENR` / `APB1ENR` / `APB2ENR`, sets the pins to alternate function **AF7** (`GPIOA->MODER`, `AFR[0]`, `AFR[1]`), computes `BRR` from the peripheral clock, enables TX/RX and the NVIC interrupts for **USART1** (ESP8266) and **USART2** (debug) |
| **System timebase** | `esp82xx_driver.c` | Millisecond delay on the **SysTick** timer: 24-bit down-counter reloaded with 16 000 cycles (1 ms at 16 MHz), polling `COUNTFLAG` |
| **Circular buffers** | `circular_buffer.c` | Four 512-byte ring buffers (RX/TX for each UART), filled and drained by the `USART1`/`USART2` IRQ handlers (`RXNEIE`/`TXEIE`), plus helpers to search and parse AT responses (`is_response`, `copy_up_to_string`, `get_next_strs`) |
| **ESP8266 library** | `esp82xx_lib.c` | AT-command sequence: reset, self-test, station mode, AP connection, local IP lookup, multi-connection mode, TCP server; HTTP request routing and HTML responses |
| **Hardware module** | `hardware_module.c` | GPIO output driver for the controlled device (PA5 set/reset through `GPIOA->ODR`) |
| **printf retarget** | `esp82xx_driver.c` | `__io_putchar()` sends `printf` output to USART2 |

### AT commands used

| Command | Role |
|---|---|
| `AT+RST` | Reset the ESP8266 |
| `AT` | Start-up test |
| `AT+CWMODE=1` | Station mode |
| `AT+CWJAP="ssid","pwd"` | Connect to the access point |
| `AT+CIFSR` | Read the local IP address |
| `AT+CIPMUX=1` | Enable multiple connections |
| `AT+CIPSERVER=1,80` | Start a TCP server on port 80 |
| `AT+CIPSEND=<id>,<len>` | Send the HTTP response |
| `AT+CIPCLOSE=5` | Close all connections after each response |

### Request handling

1. The ESP8266 forwards each incoming request as `+IPD,<link_id>,...`.
2. The firmware reads the request line up to `HTTP/1.1` and looks for the path:
   - `GET /ledon`: PA5 goes high and the firmware returns the "light ON" page.
   - `GET /ledoff` (or any other path): PA5 goes low and the firmware returns the "light OFF" page.
3. It sends the HTML page with `AT+CIPSEND`, waits for `SEND OK`, then closes the connection.

---

## Hardware configuration (NUCLEO-F401RE)

| Parameter | Value |
|---|---|
| MCU | STM32F401RET6: Cortex-M4F, 512 KB Flash, 96 KB SRAM |
| System clock | **16 MHz HSI** (reset default, no PLL). `SYS_FREQ`, `APB1_CLK` and `APB2_CLK` = 16 MHz |
| ESP8266 UART | **USART1**: PA9 = TX (D8), PA10 = RX (D2), AF7, **115200 baud 8N1** |
| Debug UART | **USART2**: PA2 = TX, PA3 = RX, AF7, **115200 baud 8N1**, routed to the ST-LINK virtual COM port (USB) |
| Controlled output | **PA5**: user LED LD2 (D13), push-pull output |
| Timebase | SysTick, processor clock, 1 ms tick |

### Wiring

| ESP8266-01 pin | NUCLEO-F401RE |
|---|---|
| VCC | 3.3 V |
| GND | GND |
| CH_PD / EN | 3.3 V |
| RST | 3.3 V |
| GPIO0, GPIO2 | 3.3 V (normal boot mode) |
| TX | **PA10** (USART1_RX, D2) |
| RX | **PA9** (USART1_TX, D8) |

> The ESP8266 draws current peaks of about 300 mA while transmitting. A dedicated 3.3 V regulator gives more stable Wi-Fi than the Nucleo's 3V3 pin. The ESP8266 must run the stock **AT firmware at 115200 baud**.

---

## Build & deployment

**Requirements:** [STM32CubeIDE](https://www.st.com/en/development-tools/stm32cubeide.html) (GNU Arm toolchain included), a NUCLEO-F401RE, a USB cable, an ESP8266-01 and a serial terminal (RealTerm, PuTTY, etc.).

1. **Clone** the repository. Keep the folder layout: the project reaches the CMSIS headers through the relative path `../../F4_chip_headers/...`.
   ```
   pfe_project_comelit/
   ├── F4_chip_headers/chip_headers/CMSIS/   # CMSIS core + STM32F4xx device headers
   └── stm32_esp826_pfeproject/              # STM32CubeIDE project
   ```
2. **Import** it into STM32CubeIDE: *File → Import → General → Existing Projects into Workspace*, then select `stm32_esp826_pfeproject`.
3. **Configure** the Wi-Fi credentials in `Src/main.c`:
   ```c
   #define SSID_NAME  "YOUR_WIFI_SSID"
   #define PASSKEY    "YOUR_WIFI_PASSWORD"
   ```
4. **Build** with *Project → Build Project*. The project is preconfigured with:
   - target MCU `STM32F401RETx`, FPU `fpv4-sp-d16`, hard-float ABI
   - preprocessor symbols `STM32F401xE`, `STM32F4`, `STM32`
   - include paths `../Inc` and the two CMSIS folders
   - linker script `STM32F401RETX_FLASH.ld`, startup file `Startup/startup_stm32f401retx.s`
5. **Flash and debug**: connect the Nucleo over USB and run *Run → Debug As → STM32 C/C++ Application* (ST-LINK GDB server). The `stm32_esp826_pfeproject.launch` configuration is included.
6. **Monitor**: open the ST-LINK COM port at **115200 8N1**. The boot log shows every step:
   ```
   Reset was successful...
   AT Startup test successful...
   Mode station set successfully...
   Connecting to access point...
   Connected : "YOUR_WIFI_SSID"
   Local IP Address :  192.168.x.x
   Multiple connections enabled....
   Please connect to the Local IP above....
   ```
7. **Use**: browse to `http://<local-ip>` from any device on the same network and click **Turn ON** / **Turn OFF**.

---

## Porting to another STM32 board

Because the code writes directly to registers, moving to another MCU or board takes these changes:

| What | Where | Change |
|---|---|---|
| **CMSIS device header** | `F4_chip_headers/.../Device/ST/<family>/Include` | Use the device header of the new MCU (e.g. `stm32f411xe.h`, `stm32l476xx.h`), taken from ST's CMSIS pack for that family |
| **Device define** | *Project Properties → C/C++ Build → Settings → Preprocessor* | Replace `STM32F401xE` with the new part (e.g. `STM32F411xE`). `stm32f4xx.h` uses it to pick the right header |
| **Target MCU** | *Project Properties → C/C++ Build → Settings → MCU Settings* | Select the new part |
| **Startup file** | `Startup/startup_*.s` | Use the vector table / startup file of the new part (IRQ names and positions differ) |
| **Linker script** | `STM32F401RETX_FLASH.ld` | Use the one for the new part's Flash/RAM sizes |
| **Clock constants** | `esp82xx_driver.c` | Update `SYS_FREQ`, `APB1_CLK`, `APB2_CLK` if the clock differs; `BRR` and `SYSTICK_LOAD_VAL` depend on them |
| **Pins & alternate functions** | `esp82xx_driver.c`, `hardware_module.c` | Check the datasheet for the UART TX/RX pins, their **AF number** and the LED pin, then update the `MODER` / `AFR` / `ODR` bit positions |
| **RCC enable bits** | `esp82xx_driver.c`, `hardware_module.c` | GPIO and USART clock-enable bits and buses (`AHB1ENR`, `APB1ENR`, `APB2ENR`) differ between families (e.g. STM32L4 uses `AHB2ENR` for GPIO) |
| **Register names** | `esp82xx_driver.c`, `circular_buffer.c` | Some families use a newer USART (`ISR`/`TDR`/`RDR` instead of `SR`/`DR`, as on STM32F7/L4/G0) |
| **IRQ handlers** | `esp82xx_lib.c` | Match `USART1_IRQHandler` / `USART2_IRQHandler` to the names in the new startup file |

Other STM32F4 Nucleo boards (e.g. **NUCLEO-F411RE**, **NUCLEO-F446RE**) need the fewest changes: the device define, startup file, linker script and clock constants. The pinout and register layout are the same.

---

## Project structure

```
stm32_esp826_pfeproject/
├── Inc/
│   ├── esp82xx_driver.h      # UART + SysTick driver API
│   ├── esp82xx_lib.h         # ESP8266 library API, ESP_Config struct
│   ├── circular_buffer.h     # ring-buffer API (512 bytes per buffer)
│   └── hardware_module.h     # light control API
├── Src/
│   ├── main.c                # init sequence + server loop
│   ├── esp82xx_driver.c      # register-level UART1/UART2, SysTick delay, printf retarget
│   ├── esp82xx_lib.c         # AT-command library, HTTP routing, HTML pages
│   ├── circular_buffer.c     # interrupt-driven ring buffers, response parsing
│   ├── hardware_module.c     # PA5 GPIO driver
│   ├── syscalls.c, sysmem.c  # newlib stubs (generated)
├── Startup/startup_stm32f401retx.s
├── STM32F401RETX_FLASH.ld / STM32F401RETX_RAM.ld
└── stm32_esp826_pfeproject.launch
F4_chip_headers/chip_headers/CMSIS/     # CMSIS core + STM32F4xx device headers (ST / Arm)
```

The repository also contains the reference documents used during development: the Cortex-M4 generic user guide, the Nucleo user manual and the ESP8266 AT instruction set.

## Future improvements

- Replace the HTTP polling server with **MQTT** for real-time bidirectional data exchange
- Improve the web interface (status feedback, more devices, entry-door control)
- Add timeouts to the AT-response waits so the firmware recovers from Wi-Fi errors

## Skills demonstrated

`Embedded C` · `Bare-metal / register-level programming` · `CMSIS` · `STM32F4 (Cortex-M4)` · `UART & NVIC interrupts` · `SysTick` · `Circular buffers` · `ESP8266 AT commands` · `TCP/IP & HTTP` · `HTML/CSS` · `STM32CubeIDE` · `Git`

## Author

**Abdelbaki Ghodhbani**, Embedded Software Engineer: [LinkedIn](https://www.linkedin.com/in/abdelbaki-ghodhbani) · [GitHub](https://github.com/abdelbaki-ghodhbani)

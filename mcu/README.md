<!-- Codex: These implementation and testing notes were added by Codex. -->
# Lab 6 implementation notes

The MCU code handles SPI temperature measurements, UART requests, LED commands,
and the complete HTML response. It uses the supplied ESP8266 webserver program.

`src/main.c` stores the webpage in C strings and inserts the actual temperature
and LED state before sending it over UART. `src/webpage.html` documents the same
layout with two placeholders; the MCU does not open an HTML file at runtime.
To change the live page, edit the HTML strings in `main.c` as well as the template.

All comments added by Codex start with `Codex:`. Original comments, including
starter TODO comments, are preserved; those TODO sections now contain code.

## Connections

| MCU pin | Connection |
| --- | --- |
| PA4 | DS1722 CE (active high) |
| PA5 | DS1722 SCLK |
| PA6 | DS1722 SDO (MCU MISO) |
| PA7 | DS1722 SDI (MCU MOSI) |
| PA9 | ESP8266 RX (MCU TX) |
| PA10 | ESP8266 TX (MCU RX) |
| PB3 | Board LED |

Use a common ground, the lab's 3.3 V supply, and connect DS1722 SERMODE to VDDD.
The FPGA/MCU DIP switches must be open for these pins, as the lab specifies.

## Build and check on the board

1. Include `src/main.c` and all seven `lib/*.c` sources in your STM32L432KC
   SEGGER project. Add `lib` to its include paths and retain its CMSIS/startup files.
   Headers and `webpage.html` are not extra C source files to compile.
2. Build and flash the MCU. The lab ESP uses USART1 at 125000 baud, 8N1.
3. Join the board's Wi-Fi and visit `http://192.168.4.1/`.
4. Check LED on/off, confirm a refresh keeps its state, and observe temperature.
5. Capture CE, SCLK, SDO and SDI on the logic analyzer for the lab report.
   A read sends 0x01 followed by two dummy bytes with CE high throughout.

SPI uses mode 1, 8-bit frames and MSB first. With the starter's 80 MHz clock,
BR=6 gives 625 kHz. The sensor performs continuous 9-bit conversions; the page
shows one decimal place, appropriate for its 0.5 degree resolution.

Source compilation/link checks can run without the board. Wiring, flashing,
live readings, and logic-analyzer captures still require the actual hardware.

<!-- Codex: The simplification notes below were added by Codex. -->
## Course-level coding choices

The code keeps the tutorial and starter switch/case structures for GPIO and
UART selection. Added code uses the same basic if/else statements, for/while
loops, ordinary functions, strings, and CMSIS register operations taught in
the course. The APIs and hardware settings are kept the same.

The extra UART stop check in main is omitted because this lab always initializes
USART1 with the valid 125000 baud setting. The 151 ms sensor startup wait stays
so the first conversion is complete. The request array limit and newline check
stay; a single terminating zero is written after receiving the complete line.

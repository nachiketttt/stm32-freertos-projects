# STM32 FreeRTOS Projects

FreeRTOS projects on Nucleo F446RE, manually integrated without CubeMX middleware.
Built with STM32CubeIDE on Fedora Linux.

## Hardware & Stack
- Board: STM32F446RE (Nucleo-64)
- RTOS: FreeRTOS Kernel V11.1.0
- Debug: SWV ITM Data Console and Segger Systemview V3.62

## Projects

| # | Project | Concepts |
|---|---------|----------|
| 001 | Task Creation | xTaskCreate, preemptive vs cooperative scheduling |

## Setup
- HAL timebase moved to TIM6, SysTick reserved for FreeRTOS
- FreeRTOS handlers mapped via defines in FreeRTOSConfig.h
- stdout buffering disabled via setvbuf for ITM printf

## Issue resolved to turn on UART based recording for Segger Systemview software
The older version 3.62 was used as the latest version crashes immediately when start recording button is pressed
Also the DWT registers are read only or turned off by default to save power in this specific nucleo board
- DEMCR (0xE000EDFC): Set bit 24 (TRCENA) to power on the global Debug/Trace architecture
- DWT_LAR (0xE0001FB0): Wrote 0xC5ACCE55 to unlock write access to the cycle counter registers
- DWT_CYCCNT (0xE0001004): Reset to 0 to provide a clean starting point for the high-resolution timer
- DWT_CTRL (0xE0001000): Set bit 0 (CYCCNTENA) to finally start the cycle counter for event timestamping

## Highlights

### SEGGER SystemView timeline showing task switching for 001_Task (In cooperative mode)
![systemview](/img/001_sysview_printf.png)


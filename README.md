# STM32 FreeRTOS Projects

FreeRTOS projects on Nucleo F446RE, manually integrated without CubeMX middleware.
Built with STM32CubeIDE on Fedora Linux.

## Hardware & Stack
- Board: STM32F446RE (Nucleo-64)
- RTOS: FreeRTOS Kernel V11.1.0
- Debug: SWV ITM Data Console

## Projects

| # | Project | Concepts |
|---|---------|----------|
| 001 | Task Creation | xTaskCreate, vTaskDelay, preemptive vs cooperative scheduling |

## Setup
- HAL timebase moved to TIM6, SysTick reserved for FreeRTOS
- FreeRTOS handlers mapped via defines in FreeRTOSConfig.h
- stdout buffering disabled via setvbuf for ITM printf

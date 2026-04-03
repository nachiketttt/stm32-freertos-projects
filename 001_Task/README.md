# 001_Task

Two FreeRTOS tasks printing to SWV ITM Data Console.

## Output

| Mode | Result |
|------|--------|
| Preemptive + time slicing | ![preemptive no delay](../img/preemptive%no%delay.png) |
| Preemptive + vTaskDelay | ![preemptive delay](../img/preemptive%and%delay.png) |
| Cooperative + taskYIELD | ![cooperative](../img/cooperative%scheduling.png) |

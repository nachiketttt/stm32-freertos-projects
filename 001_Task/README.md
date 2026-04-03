# 001_Task

Two FreeRTOS tasks printing to SWV ITM Data Console.

## Output

| Mode | Result |
|------|--------|
| Preemptive + time slicing | ![preemptive no delay](../img/preemptive_no_delay.png) |
| Preemptive + vTaskDelay | ![preemptive delay](../img/preemptive_delay.png) |
| Cooperative + taskYIELD | ![cooperative](../img/cooperative_scheduling.png) |

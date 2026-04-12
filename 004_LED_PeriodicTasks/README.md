# 003_Led_Blocktasks

Three FreeRTOS tasks independently controlling three LEDs at different
toggle rates. Also vTaskDelayUntil is used to get periodic toggling of leds at a fixed "absolute" time
- Same connections as the earlier led project have been used
- This particular exercise won't have a lot of difference in the output as it isn't noticeable for this exercise

## Tasks

| Task | LED | GPIO | Toggle Rate | Priority |
|------|-----|------|-------------|----------|
| LED_green_task | Green | PA0 | 1000ms | 2 |
| LED_yellow_task | Yellow | PA1 | 800ms | 2 |
| LED_red_task | Red | PA4 | 400ms | 2 |



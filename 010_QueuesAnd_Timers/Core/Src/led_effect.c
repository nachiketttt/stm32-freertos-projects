/*
 * led_effect.c
 *
 *  Created on: Apr 20, 2026
 *      Author: nachiket
 */

#include "main.h"

void led_effect(int n)
{
	led_effect_stop();
	xTimerStart(handle_led_timer[n-1], portMAX_DELAY);
}

void LED_effect1(void)
{
	turn_off_all_leds();
	static int flag=1;
	//all leds turning on and off simultaneously
	(flag ^= 1) ? turn_off_all_leds():turn_on_all_leds();
}

void LED_effect2(void)
{
	turn_off_all_leds();
	static int flag=1;
	//odd and even leds turning on simultaneously
	(flag ^= 1) ? turn_on_even_leds():turn_on_odd_leds();
}

void LED_effect3(void)
{
	turn_off_all_leds();
	//turns leds on and off one by one in a row starting from led 1
	static int i=0;
	led_control(0x01<<(i++%4));
}

void LED_effect4(void)
{
	turn_off_all_leds();
	//turns leds on and off one by one in a row starting from led 4 (reverse of effect 3)
	//0x08 is 1000 in bin
	static int i=0;
	led_control(0x08>>(i++%4));
}

/*   Helper functions   */
void led_effect_stop(void)
{
	for(int i;i<4;i++)
		xTimerStop(handle_led_timer[i], portMAX_DELAY);
}

void turn_off_all_leds(void)
{
	HAL_GPIO_WritePin(GPIOA, LED1, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOA, LED2, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOA, LED3, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOA, LED4, GPIO_PIN_RESET);
}

void turn_on_all_leds(void)
{
	HAL_GPIO_WritePin(GPIOA, LED1, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOA, LED2, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOA, LED3, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOA, LED4, GPIO_PIN_SET);
}

void turn_on_even_leds(void)
{
	HAL_GPIO_WritePin(GPIOA, LED1, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOA, LED2, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOA, LED3, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOA, LED4, GPIO_PIN_SET);
}

void turn_on_odd_leds(void)
{
	HAL_GPIO_WritePin(GPIOA, LED1, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOA, LED2, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOA, LED3, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOA, LED4, GPIO_PIN_RESET);
}

void led_control(int value)
{
	uint16_t leds[] = {LED1, LED2, LED3, LED4};

	for(int i = 0; i < 4; i++)
	    HAL_GPIO_WritePin(GPIOA, leds[i], (value >> i) & 0x1);
}







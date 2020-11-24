/*
 * @Descripttion: 
 * @version: 第一版
 * @Author: 叮咚蛋
 * @Date: 2020-10-28 16:06:45
 * @LastEditors: 叮咚蛋
 * @LastEditTime: 2020-10-28 16:11:17
 * @FilePath: \hu_sir-contorl\USER\INC\led.h
 */
#ifndef _LED_H
#define _LED_H
#include "sys.h"
#include "includes.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#define LED_RED_OFF  			GPIOA->BSRRL = PIN4
#define LED_YELLOW_OFF  		GPIOA->BSRRL = PIN5
#define LED_BLUE_OFF    	    GPIOA->BSRRL = PIN6
#define LED_GREEN_OFF  			GPIOA->BSRRL = PIN7

#define LED_RED_ON   			GPIOA->BSRRH = PIN4
#define LED_YELLOW_ON   		GPIOA->BSRRH = PIN5
#define LED_BLUE_ON   			GPIOA->BSRRH = PIN6
#define LED_GREEN_ON   			GPIOA->BSRRH = PIN7

#define LED_RED_TOGGLE			GPIOA->ODR ^= PIN4
#define LED_YELLOW_TOGGLE 	    GPIOA->ODR ^= PIN5
#define LED_BLUE_TOGGLE			GPIOA->ODR ^= PIN6
#define LED_GREEN_TOGGLE		GPIOA->ODR ^= PIN7

/****LED??****/
enum LED_MODE { normal, error};

void LED_Configuration(void);
void Led_Show(void);

#endif

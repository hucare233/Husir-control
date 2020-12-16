/*
 * @Descripttion: 
 * @version: ???
 * @Author: ???
 * @Date: 2020-12-15 19:29:29
 * @LastEditors: ???
 * @LastEditTime: 2020-12-16 14:08:03
 * @FilePath: \hu_sir-contorl\USER\SRC\beep.c
 */
#include "beep.h"
u8 beep_on_num;
/** 
  * @brief  ±¨¾¯Æ÷³õÊ¼»¯
  */
void Beep_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	BEEP_OFF;
}

void Beep_Show(u8 num)
{
	for (int i = 0; i < num; i++)
	{
		BEEP_ON;
		OSTimeDly(2000);
		BEEP_OFF;
		OSTimeDly(2000);
	}
}

void Beep_Start(void)
{
	BEEP_ON;
	Delay_ms(40);
	BEEP_OFF;
	Delay_ms(40);
	BEEP_ON;
	Delay_ms(40);
	BEEP_OFF;
	Delay_ms(40);
	BEEP_ON;
	Delay_ms(40);
	BEEP_OFF;
	Delay_ms(40);
	BEEP_ON;
	Delay_ms(40);
	BEEP_OFF;
	Delay_ms(40);
	BEEP_ON;
	Delay_ms(40);
	BEEP_OFF;
	Delay_ms(40);
}

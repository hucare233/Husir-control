#include "beep.h"
u8 beep_on_num;
/** 
  * @brief  ��������ʼ��
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
		Delay_ms(200);
		BEEP_OFF;
		Delay_ms(200);
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

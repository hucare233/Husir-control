/*
 * @Descripttion: 出队定时器
 * @version: 第一版
 * @Author: 叮咚蛋
 * @Date: 2020-10-28 16:06:45
 * @LastEditors: 叮咚蛋
 * @LastEditTime: 2020-12-16 15:11:41
 * @FilePath: \hu_sir-contorl\USER\SRC\tim3.c
 */
#include "tim3.h"
void TIM3_Configuration()
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	TIM_TimeBaseStructure.TIM_Prescaler = 84 - 1; //84MHZ
	TIM_TimeBaseStructure.TIM_Period = 199;		  //200us
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

	TIM_ARRPreloadConfig(TIM3, ENABLE);
	TIM_ClearFlag(TIM3, TIM_FLAG_Update);
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
	TIM_Cmd(TIM3, ENABLE);
}

void TIM3_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
	{
		TIM_ClearFlag(TIM3, TIM_FLAG_Update);

		//系统事件
		run_time.clk += 0.0002f;
		if (can1_sendqueue.Rear != can1_sendqueue.Front)
			Can_DeQueue(CAN_1, &can1_sendqueue);
		if (can2_sendqueue.Rear != can2_sendqueue.Front)
			Can_DeQueue(CAN_2, &can2_sendqueue);
		if (True == queue_flag.Can1ControlList)
		{
			TraversalControlList(CAN1_MesgSentList, &can1_sendqueue, CAN_1);
		}
		if (True == queue_flag.Can2ControlList)
		{
			TraversalControlList(CAN2_MesgSentList, &can2_sendqueue, CAN_2);
		}
	}
}

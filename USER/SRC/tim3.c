#include "tim3.h"
void TIM3_Configuration()
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef         NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);	

	TIM_TimeBaseStructure.TIM_Prescaler = 84-1;                               //84MHZ
	TIM_TimeBaseStructure.TIM_Period =199;	 																	//200us
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseStructure);

	TIM_ARRPreloadConfig(TIM3, ENABLE);
	TIM_ClearFlag(TIM3, TIM_FLAG_Update);
	TIM_ITConfig(TIM3, TIM_IT_Update,ENABLE);
	TIM_Cmd(TIM3, ENABLE);	

}

void TIM3_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM3,TIM_IT_Update)!= RESET) 
		{
			TIM_ClearFlag(TIM3, TIM_FLAG_Update);
			
			//ϵͳ�¼�
			run_time.clk += 0.0002f;
			
			//������Detect���һ�Σ��������������¿ռ�
				Detect();
			if (can1_sendqueue.Rear != can1_sendqueue.Front)
				CAN_DeQueue(CAN_1 ,&can1_sendqueue);
			if (can2_sendqueue.Rear != can2_sendqueue.Front)
				CAN_DeQueue(CAN_2 ,&can2_sendqueue);
		}
}

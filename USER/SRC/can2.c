#include "can2.h"

static u8 Registration[2][10]={{'B','S','U','M','S','P','M','M','M','M'},
															{'G','T','M','O','P','X','O','O','O','O'}};
/** 
  * @brief  CAN2初始化
  */
void CAN2_Configuration()
{
	NVIC_InitTypeDef NVIC_InitStructure;
	CAN_InitTypeDef CAN_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	CAN_FilterInitTypeDef CAN_FilterInitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN2, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	 
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource5, GPIO_AF_CAN2);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_CAN2);
	
	/* Configure CAN pin: RX  TX*/
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
  
		/* CAN RX interrupt */
	NVIC_InitStructure.NVIC_IRQChannel=CAN2_RX0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	CAN_DeInit(CAN2);
	CAN_StructInit(&CAN_InitStructure);

		/* CAN cell init */
  CAN_InitStructure.CAN_TTCM=DISABLE;  											//非时间触发通道模式
  CAN_InitStructure.CAN_ABOM=DISABLE;  											//软件对CAN_MCR寄存器的INRQ位置1，随后清0，一旦监测到128次连续11位的隐性位，就退出离线状态
  CAN_InitStructure.CAN_AWUM=DISABLE;  											//睡眠模式由软件唤醒
  CAN_InitStructure.CAN_NART=DISABLE;  											//禁止报文自动发送，即只发送一次，无论结果如何
  CAN_InitStructure.CAN_RFLM=DISABLE;  											//报文不锁定，新的覆盖旧的
  CAN_InitStructure.CAN_TXFP=DISABLE;  											//发送FIFO的优先级由标识符决定
  CAN_InitStructure.CAN_Mode=CAN_Mode_Normal;								//CAN硬件工作在正常模式

	/* Seting BaudRate */
	CAN_InitStructure.CAN_SJW=CAN_SJW_1tq;										//重新同步跳跃宽度为一个时间单位
	CAN_InitStructure.CAN_BS1=CAN_BS1_9tq; 										//时间段1占用8个时间单位
	CAN_InitStructure.CAN_BS2=CAN_BS2_4tq;										//时间段2占用7个时间单位
	CAN_InitStructure.CAN_Prescaler=3; 												//分频系数（Fdiv）
	CAN_Init(CAN2, &CAN_InitStructure); 											//初始化CAN1
	
	/* 波特率计算公式: BaudRate = APB1时钟频率/Fdiv/（SJW+BS1+BS2） */
	/* 42MHz/3/(1+9+4)=1Mhz */

  /* CAN filter init */
	CAN_FilterInitStructure.CAN_FilterNumber = 16;																			
	CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdList;
	CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_16bit;

	CAN_FilterInitStructure.CAN_FilterIdHigh =0X281<< 5;														//标识符列表模式
	CAN_FilterInitStructure.CAN_FilterIdLow =0X282 << 5;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh =0X283 << 5;
	CAN_FilterInitStructure.CAN_FilterMaskIdLow =0x284 << 5;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_FilterFIFO0;
	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
	CAN_FilterInit(&CAN_FilterInitStructure);
	
	CAN_FilterInitStructure.CAN_FilterNumber = 17;																				//屏蔽位模式
	CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdList;
	CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_16bit;

	CAN_FilterInitStructure.CAN_FilterIdHigh =0X285<< 5;														//标识符列表模式
	CAN_FilterInitStructure.CAN_FilterIdLow =0X286 << 5;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh =0X287 << 5;
	CAN_FilterInitStructure.CAN_FilterMaskIdLow =0x288 << 5;												//标识符列表模式
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_FilterFIFO0;
	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
	CAN_FilterInit(&CAN_FilterInitStructure);
	
	//错误反馈
	CAN_FilterInitStructure.CAN_FilterNumber = 15;																				//屏蔽位模式
	CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdList;
	CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_16bit;

	CAN_FilterInitStructure.CAN_FilterIdHigh =0X181<< 5;														//标识符列表模式
	CAN_FilterInitStructure.CAN_FilterIdLow =0X182 << 5;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh =0X183 << 5;
	CAN_FilterInitStructure.CAN_FilterMaskIdLow =0x184 << 5;												//标识符列表模式
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_FilterFIFO0;
	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
	CAN_FilterInit(&CAN_FilterInitStructure);

	CAN_FilterInitStructure.CAN_FilterNumber = 14;																				//屏蔽位模式
	CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdList;
	CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_16bit;

	CAN_FilterInitStructure.CAN_FilterIdHigh =0X081<< 5;														//标识符列表模式
	CAN_FilterInitStructure.CAN_FilterIdLow =0X082 << 5;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh =0X083 << 5;
	CAN_FilterInitStructure.CAN_FilterMaskIdLow =0x084 << 5;												//标识符列表模式
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_FilterFIFO0;
	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
	CAN_FilterInit(&CAN_FilterInitStructure);
	
	CAN_ITConfig(CAN2,CAN_IT_FMP0, ENABLE);
}

/** 
  * @brief  CAN2接收中断
  */
// 4.32ms 
void CAN2_RX0_IRQHandler(void)
{
	u8 i,m;
	CanRxMsg rx_message;		 
	CAN_ClearITPendingBit(CAN2, CAN_IT_FMP0);
	CAN_ClearFlag(CAN2, CAN_IT_FMP0);
	if(CAN_GetITStatus(CAN2, CAN_IT_FMP0) != RESET)
	{
		CAN_Receive(CAN2, CAN_FIFO0, &rx_message);
		i=rx_message.StdId&0xF;
		switch(rx_message.StdId)
		{
			/* 底盘驱动电机 */
			case ID_Drive_Motor1_RX: case ID_Drive_Motor2_RX: case ID_Drive_Motor3_RX: case ID_Drive_Motor4_RX:
			{
				switch(rx_message.Data[0])
				{
					case 'P'://反馈位置
					{
						if('X' == rx_message.Data[1])
						{
							EncodeFloatData(&Motor_Para[0][i-1],&rx_message.Data[4]);
							Check(0,i-1);
						}
						break;
					}
					case 'V'://反馈速度
					{
						if('X' == rx_message.Data[1])
						{
							EncodeFloatData(&Motor_Para[1][i-1],&rx_message.Data[4]);
							Check(1,i-1);
						}
						break;
					}
					case 'I'://反馈电流
					{
						if('Q' == rx_message.Data[1])
						{
							EncodeFloatData(&Motor_Para[2][i-1],&rx_message.Data[4]);
							Check(2,i-1);
						}
						if('D' == rx_message.Data[1])
						{
							EncodeFloatData(&Motor_Para[3][i-1],&rx_message.Data[4]);
							Check(3,i-1);
						}
						break;
					}
				}
				
				for(m=0;m<10;m++)
				{
					if(Registration[1][m] == rx_message.Data[0])
					{
						if(Registration[2][m] == rx_message.Data[1])
						{
							CAN2_MesgSentList[0].ReceiveNumber += 1;
							CAN2_MesgSentList[0].TimeOut = 0;
							CAN2_MesgSentList[0].SendSem --;
							if(can2_sendqueue.head[can2_sendqueue.Front].WaitFlag==TRUE && can2_sendqueue.Rear!=can2_sendqueue.Front)
							{
								if((can2_sendqueue.head[can2_sendqueue.Front].Id&0xF)==i\
									&&can2_sendqueue.head[can2_sendqueue.Front].Data[0]==rx_message.Data[0]\
									&&can2_sendqueue.head[can2_sendqueue.Front].Data[1]==rx_message.Data[1])
								can2_sendqueue.Front = (can2_sendqueue.Front + 1) % can2_sendqueue.Can_sendqueuesize;
							}
							break;
						}
					}
				}
				break;
			}
			case ID_Streeing_Motor5_RX: case ID_Streeing_Motor6_RX: case ID_Streeing_Motor7_RX: case ID_Streeing_Motor8_RX:
			{
				break;
			}
			case 0x081: case 0x082: case 0x083: case 0x084: case 0x181: case 0x182: case 0x183: case 0x184:
			{
				
				FlagPara.Motor_Emer_Flag=TRUE; 
				FlagPara.Motor_Emer_Code=(0x1<<28)|(rx_message.StdId<<16)|(rx_message.Data[0]<<8)|rx_message.Data[1];
				FlagPara.Error_Flag|=MOTOR_ERR;
				break;
			}
		}
	}
}

void Check(u8 ind_1,u8 ind_2)
{
	if(Detect_Flag[ind_1][ind_2]>0)
	{
		switch (Detect_Flag[ind_1][ind_2])
		{
			case MORE:
			{
				if(Detect_Para[ind_1][ind_2]>Motor_Para[ind_1][ind_2])
					OSFlagPost(Motor_Detect, 1<<(ind_1*4+ind_2), OS_FLAG_SET,Err_MotorDetect);
				break;
			}
			case LESS:
			{
				if(Detect_Para[ind_1][ind_2]<Motor_Para[ind_1][ind_2])
					OSFlagPost(Motor_Detect, 1<<(ind_1*4+ind_2), OS_FLAG_SET,Err_MotorDetect);
				break;
			}
			case EQUAL:
			{
				if(ABS(Detect_Para[ind_1][ind_2]-Motor_Para[ind_1][ind_2])<10)
					OSFlagPost(Motor_Detect, 1<<(ind_1*4+ind_2), OS_FLAG_SET,Err_MotorDetect);
				break;
			}
		}
	}
}

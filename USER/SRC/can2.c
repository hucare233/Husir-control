#include "can2.h"

static u8 Registration[2][10]={{'B','S','U','M','S','P','M','M','M','M'},
															{'G','T','M','O','P','X','O','O','O','O'}};
/** 
  * @brief  CAN2��ʼ��
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
  CAN_InitStructure.CAN_TTCM=DISABLE;  											//��ʱ�䴥��ͨ��ģʽ
  CAN_InitStructure.CAN_ABOM=DISABLE;  											//�����CAN_MCR�Ĵ�����INRQλ��1�������0��һ����⵽128������11λ������λ�����˳�����״̬
  CAN_InitStructure.CAN_AWUM=DISABLE;  											//˯��ģʽ���������
  CAN_InitStructure.CAN_NART=DISABLE;  											//��ֹ�����Զ����ͣ���ֻ����һ�Σ����۽�����
  CAN_InitStructure.CAN_RFLM=DISABLE;  											//���Ĳ��������µĸ��Ǿɵ�
  CAN_InitStructure.CAN_TXFP=DISABLE;  											//����FIFO�����ȼ��ɱ�ʶ������
  CAN_InitStructure.CAN_Mode=CAN_Mode_Normal;								//CANӲ������������ģʽ

	/* Seting BaudRate */
	CAN_InitStructure.CAN_SJW=CAN_SJW_1tq;										//����ͬ����Ծ���Ϊһ��ʱ�䵥λ
	CAN_InitStructure.CAN_BS1=CAN_BS1_9tq; 										//ʱ���1ռ��8��ʱ�䵥λ
	CAN_InitStructure.CAN_BS2=CAN_BS2_4tq;										//ʱ���2ռ��7��ʱ�䵥λ
	CAN_InitStructure.CAN_Prescaler=3; 												//��Ƶϵ����Fdiv��
	CAN_Init(CAN2, &CAN_InitStructure); 											//��ʼ��CAN1
	
	/* �����ʼ��㹫ʽ: BaudRate = APB1ʱ��Ƶ��/Fdiv/��SJW+BS1+BS2�� */
	/* 42MHz/3/(1+9+4)=1Mhz */

  /* CAN filter init */
	CAN_FilterInitStructure.CAN_FilterNumber = 16;																			
	CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdList;
	CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_16bit;

	CAN_FilterInitStructure.CAN_FilterIdHigh =0X281<< 5;														//��ʶ���б�ģʽ
	CAN_FilterInitStructure.CAN_FilterIdLow =0X282 << 5;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh =0X283 << 5;
	CAN_FilterInitStructure.CAN_FilterMaskIdLow =0x284 << 5;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_FilterFIFO0;
	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
	CAN_FilterInit(&CAN_FilterInitStructure);
	
	CAN_FilterInitStructure.CAN_FilterNumber = 17;																				//����λģʽ
	CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdList;
	CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_16bit;

	CAN_FilterInitStructure.CAN_FilterIdHigh =0X285<< 5;														//��ʶ���б�ģʽ
	CAN_FilterInitStructure.CAN_FilterIdLow =0X286 << 5;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh =0X287 << 5;
	CAN_FilterInitStructure.CAN_FilterMaskIdLow =0x288 << 5;												//��ʶ���б�ģʽ
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_FilterFIFO0;
	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
	CAN_FilterInit(&CAN_FilterInitStructure);
	
	//������
	CAN_FilterInitStructure.CAN_FilterNumber = 15;																				//����λģʽ
	CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdList;
	CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_16bit;

	CAN_FilterInitStructure.CAN_FilterIdHigh =0X181<< 5;														//��ʶ���б�ģʽ
	CAN_FilterInitStructure.CAN_FilterIdLow =0X182 << 5;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh =0X183 << 5;
	CAN_FilterInitStructure.CAN_FilterMaskIdLow =0x184 << 5;												//��ʶ���б�ģʽ
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_FilterFIFO0;
	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
	CAN_FilterInit(&CAN_FilterInitStructure);

	CAN_FilterInitStructure.CAN_FilterNumber = 14;																				//����λģʽ
	CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdList;
	CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_16bit;

	CAN_FilterInitStructure.CAN_FilterIdHigh =0X081<< 5;														//��ʶ���б�ģʽ
	CAN_FilterInitStructure.CAN_FilterIdLow =0X082 << 5;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh =0X083 << 5;
	CAN_FilterInitStructure.CAN_FilterMaskIdLow =0x084 << 5;												//��ʶ���б�ģʽ
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_FilterFIFO0;
	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
	CAN_FilterInit(&CAN_FilterInitStructure);
	
	CAN_ITConfig(CAN2,CAN_IT_FMP0, ENABLE);
}

/** 
  * @brief  CAN2�����ж�
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
			/* ����������� */
			case ID_Drive_Motor1_RX: case ID_Drive_Motor2_RX: case ID_Drive_Motor3_RX: case ID_Drive_Motor4_RX:
			{
				switch(rx_message.Data[0])
				{
					case 'P'://����λ��
					{
						if('X' == rx_message.Data[1])
						{
							EncodeFloatData(&Motor_Para[0][i-1],&rx_message.Data[4]);
							Check(0,i-1);
						}
						break;
					}
					case 'V'://�����ٶ�
					{
						if('X' == rx_message.Data[1])
						{
							EncodeFloatData(&Motor_Para[1][i-1],&rx_message.Data[4]);
							Check(1,i-1);
						}
						break;
					}
					case 'I'://��������
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

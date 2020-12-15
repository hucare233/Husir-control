#include "can1.h"
/** 
  * @brief  CAN1��ʼ��
  */
void CAN1_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	CAN_InitTypeDef CAN_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	CAN_FilterInitTypeDef CAN_FilterInitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	 
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource11, GPIO_AF_CAN1);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource12, GPIO_AF_CAN1);
	
	  /* Configure CAN pin: RX A11  TX A12 */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
  
		/* CAN RX interrupt */
	NVIC_InitStructure.NVIC_IRQChannel=CAN1_RX0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	CAN_DeInit(CAN1);
	CAN_StructInit(&CAN_InitStructure);
	
		/* CAN cell init */
  CAN_InitStructure.CAN_TTCM=DISABLE;  											//��ʱ�䴥��ͨ��ģʽ
  CAN_InitStructure.CAN_ABOM=DISABLE;  											//�����CAN_MCR�Ĵ�����INRQλ��1�������0��һ����⵽128������11λ������λ�����˳�����״̬
  CAN_InitStructure.CAN_AWUM=DISABLE;  											//˯��ģʽ���������
  CAN_InitStructure.CAN_NART=DISABLE;  											//DISABLE�򿪱����Զ����ͣ�����ʱ���ط�
  CAN_InitStructure.CAN_RFLM=DISABLE;  											//���Ĳ��������µĸ��Ǿɵ�
  CAN_InitStructure.CAN_TXFP=DISABLE;  											//����FIFO�����ȼ��ɱ�ʶ������
  CAN_InitStructure.CAN_Mode=CAN_Mode_Normal;								//CANӲ������������ģʽ

	  /* Seting BaudRate */
	CAN_InitStructure.CAN_SJW=CAN_SJW_1tq;										//����ͬ����Ծ���Ϊһ��ʱ�䵥λ
	CAN_InitStructure.CAN_BS1=CAN_BS1_9tq; 										//ʱ���1ռ��8��ʱ�䵥λ
	CAN_InitStructure.CAN_BS2=CAN_BS2_4tq;										//ʱ���2ռ��7��ʱ�䵥λ
	CAN_InitStructure.CAN_Prescaler=3; 												//��Ƶϵ����Fdiv��
	CAN_Init(CAN1, &CAN_InitStructure); 											//��ʼ��CAN1
	
	/* �����ʼ��㹫ʽ: BaudRate = APB1ʱ��Ƶ��/Fdiv/��SJW+BS1+BS2�� */
	/* 42MHz/3/(1+9+4)=1Mhz */

  /* CAN filter init */
	CAN_FilterInitStructure.CAN_FilterNumber = 0;																							//����λģʽ
	CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
	CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;
	
	CAN_FilterInitStructure.CAN_FilterIdHigh =(uint16_t)((uint32_t)(ID_GyrScope_RX<<3)>>16);//		 0000 0000 0000 1111 0001 0101 0001 0101    
	CAN_FilterInitStructure.CAN_FilterIdLow =(uint16_t)((uint32_t)(ID_GyrScope_RX<<3)>>0);
	
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh =(uint16_t)((uint32_t)(0X1FF0FFFFu<<3)>>16);// 0001 1111 1111 0000 1111 1111 1111 1111 
	CAN_FilterInitStructure.CAN_FilterMaskIdLow =(uint16_t)((uint32_t)(0X1FF0FFFFu<<3)>>0);
	
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_FilterFIFO0;
	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
	CAN_FilterInit(&CAN_FilterInitStructure);
	
	CAN_ITConfig(CAN1,CAN_IT_FMP0, ENABLE);
}

/** 
  * @brief  CAN1�����ж�
  */
void CAN1_RX0_IRQHandler(void)
{
	CanRxMsg rx_message;
	CAN_ClearITPendingBit(CAN1, CAN_IT_FMP0);
	CAN_ClearFlag(CAN1, CAN_IT_FMP0);
	if(CAN_GetITStatus(CAN1, CAN_IT_FMP0) != RESET)
		{
			CAN_Receive(CAN1, CAN_FIFO0, &rx_message);
			switch (rx_message.ExtId)
				{
				case ID_GyrScope_RX:
					{
					if (0x41 == rx_message.Data[0])
					{
						switch (rx_message.Data[1])
						{
						case 'P':
						{
							if (8 != rx_message.DLC)
							{
								//ErrFlag[DataLengthErr]++;
								break;
							}
							else
							{
								gyroscope.init_success = TRUE;
								position_now.x = (float)((s16)((u16)(rx_message.Data[3] << 8 | rx_message.Data[2]))) * 0.1f;
								position_now.y = (float)((s16)((u16)(rx_message.Data[5] << 8 | rx_message.Data[4]))) * 0.1f;
								position_now.angle = (float)((s16)((u16)(rx_message.Data[7] << 8 | rx_message.Data[6]))) * 0.1f;

								// position_now.y = (float)((s16)((u16)(rx_message.Data[3] << 8 | rx_message.Data[2]))) * 0.1f;
								// position_now.x = -(float)((s16)((u16)(rx_message.Data[5] << 8 | rx_message.Data[4]))) * 0.1f;
								// position_now.angle = (float)((s16)((u16)(rx_message.Data[7] << 8 | rx_message.Data[6]))) * 0.1f;
							}
							break;
						}
						case 'S':
						{
							if (8 != rx_message.DLC)
							{
								//ErrFlag[DataLengthErr]++;
								break;
							}
							else
							{
								velocity_abs.Vx = (double)((s16)(((u16)(rx_message.Data[3]) << 8) | rx_message.Data[2])) * 0.1;
								velocity_abs.Vy = (double)((s16)(((u16)(rx_message.Data[5]) << 8) | rx_message.Data[4])) * 0.1;
								velocity_now.Vw = (double)((s16)(((u16)(rx_message.Data[7]) << 8) | rx_message.Data[6])) * 0.1;

								// velocity_abs.Vy = (double)((s16)(((u16)(rx_message.Data[3]) << 8) | rx_message.Data[2])) * 0.1;
								// velocity_abs.Vx = -(double)((s16)(((u16)(rx_message.Data[5]) << 8) | rx_message.Data[4])) * 0.1;
								// velocity_now.Vw = (double)((s16)(((u16)(rx_message.Data[7]) << 8) | rx_message.Data[6])) * 0.1;

								velocity_now.Vx = velocity_abs.Vx * cos(position_now.angle / 180 * PI) + velocity_abs.Vy * sin(position_now.angle / 180 * PI);
								velocity_now.Vy = (-velocity_abs.Vx * sin(position_now.angle / 180 * PI) + velocity_abs.Vy * cos(position_now.angle / 180 * PI));
								velocity_now.V = sqrt(velocity_now.Vx * velocity_now.Vx + velocity_now.Vy * velocity_now.Vy);
							}
							break;
						}
					}
				}
			}
		}
	}
}
		

void gyroscopes_whu(u8 waitflag)//�Լ�
{
	if(Rear1==can1_sendqueue.Front)
	{
		FlagPara.Error_Flag|=CAN1_QUEUE_FULL;
		return;
	}
	else
	{
		can1_sendqueue.head[can1_sendqueue.Rear].Id = 0X00010200;
		can1_sendqueue.head[can1_sendqueue.Rear].DLC =0x08;
		can1_sendqueue.head[can1_sendqueue.Rear].Data[0]=0x01;
		can1_sendqueue.head[can1_sendqueue.Rear].Data[1]='R';
		can1_sendqueue.head[can1_sendqueue.Rear].Data[2]= 0;
		can1_sendqueue.head[can1_sendqueue.Rear].Data[3]= 0;
		can1_sendqueue.head[can1_sendqueue.Rear].Data[4]= 0;
		can1_sendqueue.head[can1_sendqueue.Rear].Data[5]= 0;
		can1_sendqueue.head[can1_sendqueue.Rear].Data[6]= 0;
		can1_sendqueue.head[can1_sendqueue.Rear].Data[7]= 0;
		
		can1_sendqueue.head[can1_sendqueue.Rear].WaitFlag=waitflag;
	}
	can1_sendqueue.Rear=Rear1;	
}






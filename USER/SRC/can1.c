/*
 * @Descripttion: can1
 * @version: ��һ��
 * @Author: ���˵�
 * @Date: 2020-12-15 19:29:29
 * @LastEditors: ���˵�
 * @LastEditTime: 2020-12-16 10:06:14
 * @FilePath: \hu_sir-contorl\USER\SRC\can1.c
 */

#include "can1.h"

// volatile float motorspeed[8] = {0};	   //Ϊ���ת�� r/min TODO: ��Ӧ�÷����⣬�Լ����Ƿ����ã���ɾ��
// volatile float MotorPosition[8] = {0}; //���յ���Ϊ���Ƕȣ���Ҫ�������ǰ�Ƕ� ��λ����

/**
  * @brief  CAN1��ʼ��
  */
MesgControlGrp CAN1_MesgSentList[CAN1_NodeNumber];

void Can1_Configuration(u8 prep_prio, u8 sub_prio)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	CAN_InitTypeDef CAN_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	CAN_FilterInitTypeDef CAN_FilterInitStructure;

	// RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1 | RCC_AHB1Periph_GPIOA, ENABLE);
	// GPIO_PinAFConfig(GPIOA, GPIO_PinSource11 | GPIO_PinSource12, GPIO_AF_CAN1); FIXME: �𾪣����͵���Ļ����㣬��Ȼ��ʼ��ʧ���ˣ�ֻ�ܰ�����������ֲ��ܳɹ�,��~~ 2020-09-02

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource11, GPIO_AF_CAN1);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource12, GPIO_AF_CAN1);

	/* Configure CAN pin: RX A11  TX A12 */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* CAN RX interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = CAN1_RX0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = prep_prio;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = sub_prio;
	// NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	// NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	CAN_DeInit(CAN1);
	CAN_StructInit(&CAN_InitStructure);

	/* CAN cell init */
	CAN_InitStructure.CAN_TTCM = DISABLE;		  //��ʱ�䴥��ͨ��ģʽ
	CAN_InitStructure.CAN_ABOM = DISABLE;		  //�����CAN_MCR�Ĵ�����INRQλ��1�������0��һ����⵽128������11λ������λ�����˳�����״̬
	CAN_InitStructure.CAN_AWUM = DISABLE;		  //˯��ģʽ���������
	CAN_InitStructure.CAN_NART = DISABLE;		  //��ֹ�����Զ����ͣ���ֻ����һ�Σ����۽�����
	CAN_InitStructure.CAN_RFLM = DISABLE;		  //���Ĳ��������µĸ��Ǿɵ�
	CAN_InitStructure.CAN_TXFP = DISABLE;		  //����FIFO�����ȼ��ɱ�ʶ������
	CAN_InitStructure.CAN_Mode = CAN_Mode_Normal; //CANӲ������������ģʽ

	/* Seting BaudRate */
	CAN_InitStructure.CAN_SJW = CAN_SJW_1tq; //����ͬ����Ծ���Ϊһ��ʱ�䵥λ
	CAN_InitStructure.CAN_BS1 = CAN_BS1_9tq; //ʱ���1ռ��8��ʱ�䵥λ
	CAN_InitStructure.CAN_BS2 = CAN_BS2_4tq; //ʱ���2ռ��7��ʱ�䵥λ
	CAN_InitStructure.CAN_Prescaler = 3;	 //��Ƶϵ����Fdiv��
	CAN_Init(CAN1, &CAN_InitStructure);		 //��ʼ��CAN1

	/* �����ʼ��㹫ʽ: BaudRate = APB1ʱ��Ƶ��/Fdiv/��SJW+BS1+BS2�� */
	/* 42MHz/3/(1+9+4)=1Mhz */

	/* CAN filter init */
	CAN_FilterInitStructure.CAN_FilterNumber = 0; //����λģʽ
	CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
	CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;
	CAN_FilterInitStructure.CAN_FilterIdHigh = (uint16_t)((uint32_t)(0X00FF0101 << 3) >> 16);	   //000	0 0000 1111 1111 0000 0001 0000 0001
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = (uint16_t)((uint32_t)(0X1F00FFFFu << 3) >> 16); //000	1 1111 0000 0000 1111 1111 1111 1111
	CAN_FilterInitStructure.CAN_FilterIdLow = (uint16_t)((uint32_t)(0X00FF0101 << 3) >> 0);
	CAN_FilterInitStructure.CAN_FilterMaskIdLow = (uint16_t)((uint32_t)(0X1F00FFFEu << 3) >> 0);
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_FilterFIFO0;
	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
	CAN_FilterInit(&CAN_FilterInitStructure);

	CAN_ClearITPendingBit(CAN1, CAN_IT_FMP0);
	CAN_ClearFlag(CAN1, CAN_IT_FMP0);
	CAN_ITConfig(CAN1, CAN_IT_FMP0, ENABLE);
}
void CAN1_RX0_IRQHandler(void)
{
	CanRxMsg rx_message;
	//���������־λ�϶���������ģ���Ϊ������������û��������������ǿ⺯��������˵����
	/*
	*          10. To control CAN events you can use one of the following two methods:
	*               - Check on CAN flags using the CAN_GetFlagStatus() function.
	*               - Use CAN interrupts through the function CAN_ITConfig() at
	*                 initialization phase and CAN_GetITStatus() function into
	*                 interrupt routines to check if the event has occurred or not.
	*             After checking on a flag you should clear it using CAN_ClearFlag()
	*             function. And after checking on an interrupt event you should
	*             clear it using CAN_ClearITPendingBit() function.
	*/
	CAN_ClearITPendingBit(CAN1, CAN_IT_FMP0);
	CAN_ClearFlag(CAN1, CAN_IT_FMP0);
	if (CAN_GetITStatus(CAN1, CAN_IT_FMP0) != RESET)
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
						gyroscope.init_success = True;
						position_now.x = (float)((s16)((u16)(rx_message.Data[3] << 8 | rx_message.Data[2]))) * 0.1f;
						position_now.y = (float)((s16)((u16)(rx_message.Data[5] << 8 | rx_message.Data[4]))) * 0.1f;
						position_now.angle = (float)((s16)((u16)(rx_message.Data[7] << 8 | rx_message.Data[6]))) * 0.1f;
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
						/*��ȡ�����ٶ�*/
						velocity_abs.Vx = (double)((s16)(((u16)(rx_message.Data[3]) << 8) | rx_message.Data[2])) * 0.1;
						velocity_abs.Vy = (double)((s16)(((u16)(rx_message.Data[5]) << 8) | rx_message.Data[4])) * 0.1;
						velocity_now.Vw = (double)((s16)(((u16)(rx_message.Data[7]) << 8) | rx_message.Data[6])) * 0.1;

                        /*���ݾ����ٶȼ�������ٶ�*/
						velocity_now.Vx = velocity_abs.Vx * cos(position_now.angle / 180 * PI) + velocity_abs.Vy * sin(position_now.angle / 180 * PI);
						velocity_now.Vy = (-velocity_abs.Vx * sin(position_now.angle / 180 * PI) + velocity_abs.Vy * cos(position_now.angle / 180 * PI));

						velocity_now.V = sqrt(velocity_now.Vx * velocity_now.Vx + velocity_now.Vy * velocity_now.Vy);
					}
					break;
				}
				case 'C': //���ڸ�ʲĪ
				{
					CAN1_MesgSentList[0].ReceiveNumber += 1;
					CAN1_MesgSentList[0].TimeOut = 0;
					CAN1_MesgSentList[0].SendSem--;
					CAN1_MesgSentList[0].SentQueue.Front = (CAN1_MesgSentList[0].SentQueue.Front + 1) % CAN1_MesgSentList[0].SentQueue.Can_sendqueuesize;
					break;
				}
				case 'X': /* ���Ŀ��ƿ鷢�ͱ�־λ��һ */
				{
					CAN1_MesgSentList[0].ReceiveNumber += 1;
					CAN1_MesgSentList[0].TimeOut = 0;
					CAN1_MesgSentList[0].SendSem--;
					CAN1_MesgSentList[0].SentQueue.Front = (CAN1_MesgSentList[0].SentQueue.Front + 1) % CAN1_MesgSentList[0].SentQueue.Can_sendqueuesize;
					break;
				}
				case 'R': //��λ
				{
					/* ���Ŀ��ƿ鷢�ͱ�־λ��һ */
					CAN1_MesgSentList[0].ReceiveNumber += 1;
					CAN1_MesgSentList[0].TimeOut = 0;
					CAN1_MesgSentList[0].SendSem--;
					CAN1_MesgSentList[0].SentQueue.Front = (CAN1_MesgSentList[0].SentQueue.Front + 1) % CAN1_MesgSentList[0].SentQueue.Can_sendqueuesize;
					OSFlagPost(FlagCan1Check, 0x01, OS_FLAG_SET, ErrorCan1);
					break;
				}
				case 'A':  //ѯ��������״̬
				{
					if (rx_message.Data[2] == 'S' && rx_message.Data[3] == 'K')
						gyroscope.init_success = rx_message.Data[4];
					break;
				}
				}
			}

			else if (0x40 == rx_message.Data[0]) //�Լ��ź�
				OSFlagPost(FlagCan1Check, 0x01, OS_FLAG_SET, ErrorCan1);
			else
				//ErrFlag[FuncIDErr]++;
				break;
		}
		case ID_DT5035X_RX:
		{
			if (0x43 == rx_message.Data[0])
			{
				switch (rx_message.Data[1])
				{
				case 'B':
				{
					CAN1_MesgSentList[1].ReceiveNumber += 1;
					CAN1_MesgSentList[1].TimeOut = 0;
					CAN1_MesgSentList[1].SendSem--;
					CAN1_MesgSentList[1].SentQueue.Front = (CAN1_MesgSentList[1].SentQueue.Front + 1) % CAN1_MesgSentList[4].SentQueue.Can_sendqueuesize;
					break;
				}
				case 'S':
				{
					//ʲô����
					CAN1_MesgSentList[1].ReceiveNumber += 1;
					CAN1_MesgSentList[1].TimeOut = 0;
					CAN1_MesgSentList[1].SendSem--;
					CAN1_MesgSentList[1].SentQueue.Front = (CAN1_MesgSentList[1].SentQueue.Front + 1) % CAN1_MesgSentList[4].SentQueue.Can_sendqueuesize;
					break;
				}
				case 'D':
				{
					OS_ENTER_CRITICAL();

					//Dt50XNum++;
					//Dt50XDis = (float)((rx_message.Data[3] << 8 | rx_message.Data[2])) * 0.1f;
					OS_EXIT_CRITICAL();
					break;
				}
				default: //ʲô����
						 //					//ErrFlag[SubIDtan90]++;
					break;
				}
			}
			else if (0x40 == rx_message.Data[0])
				OSFlagPost(FlagCan1Check, 0x0001, OS_FLAG_SET, ErrorCan1);
			else
				break;
		}
		case ID_DT5035Y_RX:
		{
			if (0x42 == rx_message.Data[0])
			{
				switch (rx_message.Data[1])
				{
				case 'B':
					CAN1_MesgSentList[3].ReceiveNumber += 1;
					CAN1_MesgSentList[3].TimeOut = 0;
					CAN1_MesgSentList[3].SendSem--;
					CAN1_MesgSentList[3].SentQueue.Front = (CAN1_MesgSentList[3].SentQueue.Front + 1) % CAN1_MesgSentList[3].SentQueue.Can_sendqueuesize;
					break;

				case 'S': //����ָ��
					/* ���Ŀ��ƿ鷢�ͱ�־λ��һ */
					CAN1_MesgSentList[3].ReceiveNumber += 1;
					CAN1_MesgSentList[3].TimeOut = 0;
					CAN1_MesgSentList[3].SendSem--;
					CAN1_MesgSentList[3].SentQueue.Front = (CAN1_MesgSentList[3].SentQueue.Front + 1) % CAN1_MesgSentList[3].SentQueue.Can_sendqueuesize;
					break;

				case 'D': //����ָ��
					/* ���Ŀ��ƿ鷢�ͱ�־λ��һ */
					//Dt50YNum++;

					OS_ENTER_CRITICAL();
					//Dt50YDis = ((rx_message.Data[3] << 8) | rx_message.Data[2]) / 10.0f;
					OS_EXIT_CRITICAL();

					//									if(FieldChoose==1)//�쳡
					//										PosNowYDt50=(float)(FieldChoose*(//Dt50YDis+HalfLength)*cos(PositionNow.angle));//δת90��֮ǰ
					//									else if(FieldChoose==-1)
					//										PosNowYDt50=(float)((FieldChoose*(//Dt50YDis+HalfLength)*cos(PositionNow.angle)));//δת90��֮ǰ
					//Dt50YDisSum += //Dt50YDis;
					break;

				case 'F': //����ָ��
					/* //ÿ���ϵ������ֻ��ѡ��һ�Σ���ֹ�����н������к������ı䵼�·ɳ����������ֻ���ֱ���dt50���Ըı� */
					CAN1_MesgSentList[3].ReceiveNumber += 1;
					CAN1_MesgSentList[3].TimeOut = 0;
					CAN1_MesgSentList[3].SendSem--;
					CAN1_MesgSentList[3].SentQueue.Front = (CAN1_MesgSentList[3].SentQueue.Front + 1) % CAN1_MesgSentList[3].SentQueue.Can_sendqueuesize;
					//					if(rx_message.Data[2]=='L'&&rx_message.Data[3]=='D'&&user.area_side==0)
					//					{
					//						if(rx_message.Data[4]=='B')
					//							user.area_side=-1;
					//						else if(rx_message.Data[4]=='R')
					//							user.area_side=1;
					//					}
					break;

				case 'F' + 0x20:
					break;

				default:
					//ErrFlag[SubIDtan90]++;
					break;
				}
			}
			else
				break;
		}
		default:
		{
			//BEEP_ON;
			//			sprintf(user.error,"%s","RubishMsg");
		}
		break;
		}
	}
}

/**
  * @brief  CAN1�Լ�
  */
void Can1selfcheck(u8 InConGrpFlag)
{
	if (Rear1 == can1_sendqueue.Front)
	{
		queue_flag.can1_queue_full++;
		// user.err = "full"; Beep_Show(1);
		return;
	}
	else
	{
		can1_sendqueue.node[can1_sendqueue.Rear].Id = ID_BroadCast_TX;
		can1_sendqueue.node[can1_sendqueue.Rear].DLC = 0X05;

		can1_sendqueue.node[can1_sendqueue.Rear].Data[0] = 0X00;
		can1_sendqueue.node[can1_sendqueue.Rear].Data[1] = 'C';

		can1_sendqueue.node[can1_sendqueue.Rear].Data[2] = 'W';
		can1_sendqueue.node[can1_sendqueue.Rear].Data[3] = 'H';
		can1_sendqueue.node[can1_sendqueue.Rear].Data[4] = 'U';
		can1_sendqueue.node[can1_sendqueue.Rear].InConGrpFlag = InConGrpFlag;
	}
	can1_sendqueue.Rear = Rear1;
}

/**
  * @brief ����������X����
  */
void GyroSetX(float x, u8 InConGrpFlag)
{

	if (Rear1 == can1_sendqueue.Front)
	{
		queue_flag.can1_queue_full++;
		return;
	}
	else
	{
		can1_sendqueue.node[can1_sendqueue.Rear].Id = ID_GyrScope_TX;
		can1_sendqueue.node[can1_sendqueue.Rear].DLC = 0X04;

		can1_sendqueue.node[can1_sendqueue.Rear].Data[0] = 0X01;
		can1_sendqueue.node[can1_sendqueue.Rear].Data[1] = 'X';
		can1_sendqueue.node[can1_sendqueue.Rear].Data[2] = (uint8_t)((int16_t)(x * 10));
		can1_sendqueue.node[can1_sendqueue.Rear].Data[3] = (uint8_t)((int16_t)(x * 10) >> 8);

		can1_sendqueue.node[can1_sendqueue.Rear].InConGrpFlag = InConGrpFlag;
	}
	can1_sendqueue.Rear = Rear1;
}

/*** 
 * @brief  ��λ�ӻ�
 * @author δ����
 * @param {ID_GyrScope_TX, 0x00, 1} 
 * @return {void} 
 * @CreatedTime 2020-09-05 15:27:50
 */
void ResetSlave(u32 ID, u8 FunID, u8 InConGrpFlag)
{
	if (Rear1 == can1_sendqueue.Front)
	{
		queue_flag.can1_queue_full++;
		return;
	}

	else
	{
		can1_sendqueue.node[can1_sendqueue.Rear].Id = ID;
		can1_sendqueue.node[can1_sendqueue.Rear].DLC = 0x04;

		can1_sendqueue.node[can1_sendqueue.Rear].Data[0] = FunID;
		can1_sendqueue.node[can1_sendqueue.Rear].Data[1] = 'R';
		can1_sendqueue.node[can1_sendqueue.Rear].Data[2] = 'R';
		can1_sendqueue.node[can1_sendqueue.Rear].Data[3] = 'S';
		can1_sendqueue.node[can1_sendqueue.Rear].InConGrpFlag = InConGrpFlag;
	}

	can1_sendqueue.Rear = Rear1;
}

/**
  * @brief ����������Y����
  */
void GyroSetY(float y, u8 InConGrpFlag)
{

	if (Rear1 == can1_sendqueue.Front)
	{
		queue_flag.can1_queue_full++;
		return;
	}
	else
	{
		can1_sendqueue.node[can1_sendqueue.Rear].Id = ID_GyrScope_TX;
		can1_sendqueue.node[can1_sendqueue.Rear].DLC = 0X04;

		can1_sendqueue.node[can1_sendqueue.Rear].Data[0] = 0X01;
		can1_sendqueue.node[can1_sendqueue.Rear].Data[1] = 'Y';
		can1_sendqueue.node[can1_sendqueue.Rear].Data[2] = (uint8_t)((int16_t)(y * 10));
		can1_sendqueue.node[can1_sendqueue.Rear].Data[3] = (uint8_t)((int16_t)(y * 10) >> 8);

		can1_sendqueue.node[can1_sendqueue.Rear].InConGrpFlag = InConGrpFlag;
	}
	can1_sendqueue.Rear = Rear1;
}

/**
  * @brief ����������X����ͽǶ�
  */
void GyroSetXandA(float x, float A, u8 InConGrpFlag)
{

	if (Rear1 == can1_sendqueue.Front)
	{
		queue_flag.can1_queue_full++;
		return;
	}
	else
	{
		can1_sendqueue.node[can1_sendqueue.Rear].Id = ID_GyrScope_TX;
		can1_sendqueue.node[can1_sendqueue.Rear].DLC = 0X04;

		can1_sendqueue.node[can1_sendqueue.Rear].Data[0] = 0X01;
		can1_sendqueue.node[can1_sendqueue.Rear].Data[1] = 'L';
		can1_sendqueue.node[can1_sendqueue.Rear].Data[2] = (uint8_t)((int16_t)(x * 10));
		can1_sendqueue.node[can1_sendqueue.Rear].Data[3] = (uint8_t)((int16_t)(x * 10) >> 8);
		can1_sendqueue.node[can1_sendqueue.Rear].Data[4] = (uint8_t)((int16_t)(A * 10));
		can1_sendqueue.node[can1_sendqueue.Rear].Data[5] = (uint8_t)((int16_t)(A * 10) >> 8);

		can1_sendqueue.node[can1_sendqueue.Rear].InConGrpFlag = InConGrpFlag;
	}
	can1_sendqueue.Rear = Rear1;
}
/**
  * @brief  �����ض�λ���
  */
void ResetPosition_EN(float x, float y, float angle, u8 InConGrpFlag)//ASK:�������������ݵĴ�����ʲô
{

	if (Rear1 == can1_sendqueue.Front)
	{
		queue_flag.can1_queue_full++;
		return;
	}
	else
	{
		can1_sendqueue.node[can1_sendqueue.Rear].Id = ID_GyrScope_TX;
		can1_sendqueue.node[can1_sendqueue.Rear].DLC = 0X08;

		can1_sendqueue.node[can1_sendqueue.Rear].Data[0] = 0X01;
		can1_sendqueue.node[can1_sendqueue.Rear].Data[1] = 'R';

		can1_sendqueue.node[can1_sendqueue.Rear].Data[2] = (uint8_t)((int16_t)(x * 10));
		can1_sendqueue.node[can1_sendqueue.Rear].Data[3] = (uint8_t)((int16_t)(x * 10) >> 8);
		can1_sendqueue.node[can1_sendqueue.Rear].Data[4] = (uint8_t)((int16_t)(y * 10));
		can1_sendqueue.node[can1_sendqueue.Rear].Data[5] = (uint8_t)((int16_t)(y * 10) >> 8);
		can1_sendqueue.node[can1_sendqueue.Rear].Data[6] = (uint8_t)((int16_t)(angle * 10));
		can1_sendqueue.node[can1_sendqueue.Rear].Data[7] = (uint8_t)((int16_t)(angle * 10) >> 8);
		can1_sendqueue.node[can1_sendqueue.Rear].InConGrpFlag = InConGrpFlag;
	}
	can1_sendqueue.Rear = Rear1;
}

/**
  * @brief  ǰ4�����Ѱ��
  */
void FrontDJIMotor_SZ()
{
	if (Rear1 == can1_sendqueue.Front)
	{
		queue_flag.can1_queue_full++;
		return;
	}
	else
	{
		can1_sendqueue.node[can1_sendqueue.Rear].Id = ID_DJIMotor_F4_TX;
		can1_sendqueue.node[can1_sendqueue.Rear].DLC = 0X04;

		can1_sendqueue.node[can1_sendqueue.Rear].Data[0] = 0X01;
		can1_sendqueue.node[can1_sendqueue.Rear].Data[1] = 'Z' + 0x40;

		can1_sendqueue.node[can1_sendqueue.Rear].Data[2] = 'Z';
		can1_sendqueue.node[can1_sendqueue.Rear].Data[3] = 'C';
	}
	can1_sendqueue.Rear = Rear1;
}

void FrontDJIMotor_ASK(void)
{
	if (Rear1 == can1_sendqueue.Front)
	{
		queue_flag.can1_queue_full++;
		return;
	}
	else
	{
		can1_sendqueue.node[can1_sendqueue.Rear].Id = ID_DJIMotor_F4_TX;
		can1_sendqueue.node[can1_sendqueue.Rear].DLC = 0X04;

		can1_sendqueue.node[can1_sendqueue.Rear].Data[0] = 0X01;
		can1_sendqueue.node[can1_sendqueue.Rear].Data[1] = 'A' + 0x80;

		can1_sendqueue.node[can1_sendqueue.Rear].Data[2] = 'L';
		can1_sendqueue.node[can1_sendqueue.Rear].Data[3] = 'L';
	}
	can1_sendqueue.Rear = Rear1;
}
/**
  * @brief  ǰ4�����Ѱ���ٶȣ����r/min��
  */
void FrontDJI_SZ_Speed(s16 speed)
{
	if (Rear1 == can1_sendqueue.Front)
	{
		queue_flag.can1_queue_full++;
		return;
	}
	else
	{
		can1_sendqueue.node[can1_sendqueue.Rear].Id = ID_DJIMotor_F4_TX;
		can1_sendqueue.node[can1_sendqueue.Rear].DLC = 0X07;

		can1_sendqueue.node[can1_sendqueue.Rear].Data[0] = 0X02;
		can1_sendqueue.node[can1_sendqueue.Rear].Data[1] = 'Z' + 0x60;

		can1_sendqueue.node[can1_sendqueue.Rear].Data[2] = 'Z';
		can1_sendqueue.node[can1_sendqueue.Rear].Data[3] = 'S';
		can1_sendqueue.node[can1_sendqueue.Rear].Data[4] = 0;
		EncodeS16Data(&speed, &can1_sendqueue.node[can1_sendqueue.Rear].Data[5]);
	}
	can1_sendqueue.Rear = Rear1;
}
/**
  * @brief  ����λ��ģʽ�ٶȣ����r/min��
  */
void FrontDJI_SetPTPSpeed(u8 ID, s16 Speed)
{
	if (Rear1 == can1_sendqueue.Front)
	{
		queue_flag.can1_queue_full++;
		return;
	}
	else
	{
		//				if(ID>=1&&ID<=4)
		//				{
		can1_sendqueue.node[can1_sendqueue.Rear].Id = ID_DJIMotor_F4_TX;
		//				}
		//				else
		//				{
		//				can1_sendqueue.node[can1_sendqueue.Rear].Id = ID_DJIMotor_B4_TX;
		//				}
		can1_sendqueue.node[can1_sendqueue.Rear].DLC = 0X07;

		can1_sendqueue.node[can1_sendqueue.Rear].Data[0] = 0X01;
		can1_sendqueue.node[can1_sendqueue.Rear].Data[1] = 'P' + 0x40;

		can1_sendqueue.node[can1_sendqueue.Rear].Data[2] = 'P';
		can1_sendqueue.node[can1_sendqueue.Rear].Data[3] = 'S';

		can1_sendqueue.node[can1_sendqueue.Rear].Data[4] = ID;
		EncodeS16Data(&Speed, &can1_sendqueue.node[can1_sendqueue.Rear].Data[5]);
	}
	can1_sendqueue.Rear = Rear1;
}
/**
  * @brief  ����λ��ģʽλ�ã����Ƕȣ���λ���ȣ�
  */
void FrontDJI_SetPTPPosition(u8 ID, s16 pulse)
{
	if (Rear1 == can1_sendqueue.Front) //��������
	{
		queue_flag.can1_queue_full++;
		return;
	}
	else
	{

		can1_sendqueue.node[can1_sendqueue.Rear].Id = ID_DJIMotor_F4_TX;

		can1_sendqueue.node[can1_sendqueue.Rear].DLC = 0X07;

		can1_sendqueue.node[can1_sendqueue.Rear].Data[0] = 0X01;
		can1_sendqueue.node[can1_sendqueue.Rear].Data[1] = 'P' + 0x60;

		can1_sendqueue.node[can1_sendqueue.Rear].Data[2] = 'P';
		can1_sendqueue.node[can1_sendqueue.Rear].Data[3] = 'X';

		can1_sendqueue.node[can1_sendqueue.Rear].Data[4] = ID;
		EncodeS16Data(&pulse, &can1_sendqueue.node[can1_sendqueue.Rear].Data[5]);
	}
	can1_sendqueue.Rear = Rear1;
}
/**
  * @brief  �����ٶ�ģʽ�ٶ� �����r/min��
  */
void FrontDJI_SetSpeed(u8 ID, s16 speed)
{
	if (Rear1 == can1_sendqueue.Front)
	{
		queue_flag.can1_queue_full++;
		return;
	}
	else
	{
		//				if(ID>=1&&ID<=4)
		//				{
		can1_sendqueue.node[can1_sendqueue.Rear].Id = ID_DJIMotor_F4_TX;
		//				}
		//				else
		//				{
		//				can1_sendqueue.node[can1_sendqueue.Rear].Id = ID_DJIMotor_B4_TX;
		//				}
		can1_sendqueue.node[can1_sendqueue.Rear].DLC = 0X07;

		can1_sendqueue.node[can1_sendqueue.Rear].Data[0] = 0X01;
		can1_sendqueue.node[can1_sendqueue.Rear].Data[1] = 'S' + 0x20;

		can1_sendqueue.node[can1_sendqueue.Rear].Data[2] = 'S';
		can1_sendqueue.node[can1_sendqueue.Rear].Data[3] = 'D';

		can1_sendqueue.node[can1_sendqueue.Rear].Data[4] = ID;
		EncodeS16Data(&speed, &can1_sendqueue.node[can1_sendqueue.Rear].Data[5]);
	}
	can1_sendqueue.Rear = Rear1;
}
/**
  * @brief ���õ���ģʽ����
  */
void FrontDJI_SetCurrent(u8 ID, s16 current)
{
	if (Rear1 == can1_sendqueue.Front)
	{
		queue_flag.can1_queue_full++;
		return;
	}
	else
	{
		//				if(ID>=1&&ID<=4)
		//				{
		can1_sendqueue.node[can1_sendqueue.Rear].Id = ID_DJIMotor_F4_TX;
		//				}
		//				else
		//				{
		//				can1_sendqueue.node[can1_sendqueue.Rear].Id = ID_DJIMotor_B4_TX;
		//				}
		can1_sendqueue.node[can1_sendqueue.Rear].DLC = 0X07;

		can1_sendqueue.node[can1_sendqueue.Rear].Data[0] = 0X01;
		can1_sendqueue.node[can1_sendqueue.Rear].Data[1] = 'L' + 0x20;

		can1_sendqueue.node[can1_sendqueue.Rear].Data[2] = 'S';
		can1_sendqueue.node[can1_sendqueue.Rear].Data[3] = 'L';

		can1_sendqueue.node[can1_sendqueue.Rear].Data[4] = ID;
		EncodeS16Data(&current, &can1_sendqueue.node[can1_sendqueue.Rear].Data[5]);
	}
	can1_sendqueue.Rear = Rear1;
}
/**
  * @brief ѡ����ģʽ
  */
void FrontDJI_ChooseMode(u8 ID, u8 mode)
{
	if (Rear1 == can1_sendqueue.Front)
	{
		queue_flag.can1_queue_full++;
		return;
	}
	else
	{
		//				if(ID>=1&&ID<=4)
		//				{
		can1_sendqueue.node[can1_sendqueue.Rear].Id = ID_DJIMotor_F4_TX;
		//				}
		//				else
		//				{
		//				can1_sendqueue.node[can1_sendqueue.Rear].Id = ID_DJIMotor_B4_TX;
		//				}
		can1_sendqueue.node[can1_sendqueue.Rear].DLC = 0X06;

		can1_sendqueue.node[can1_sendqueue.Rear].Data[0] = 0X01;
		can1_sendqueue.node[can1_sendqueue.Rear].Data[1] = 'C' + 0x20;

		can1_sendqueue.node[can1_sendqueue.Rear].Data[2] = 'C';
		can1_sendqueue.node[can1_sendqueue.Rear].Data[3] = 'M';

		can1_sendqueue.node[can1_sendqueue.Rear].Data[4] = ID;
		can1_sendqueue.node[can1_sendqueue.Rear].Data[5] = mode;
	}
	can1_sendqueue.Rear = Rear1;
}
/**
  * @brief �ͷŵ��
  */
void FrontDJI_ReleaseMotor(void)
{
	if (Rear1 == can1_sendqueue.Front)
	{
		queue_flag.can1_queue_full++;
		return;
	}
	else
	{

		can1_sendqueue.node[can1_sendqueue.Rear].Id = ID_DJIMotor_F4_TX;

		can1_sendqueue.node[can1_sendqueue.Rear].DLC = 0X04;

		can1_sendqueue.node[can1_sendqueue.Rear].Data[0] = 0X01;
		can1_sendqueue.node[can1_sendqueue.Rear].Data[1] = 'R' + 0x20;

		can1_sendqueue.node[can1_sendqueue.Rear].Data[2] = 'R';
		can1_sendqueue.node[can1_sendqueue.Rear].Data[3] = 'L';
	}
	can1_sendqueue.Rear = Rear1;
}
/**
  * @brief ��ͣ���
  */
void FrontDJI_StopMotor(void)
{
	if (Rear1 == can1_sendqueue.Front)
	{
		queue_flag.can1_queue_full++;
		return;
	}
	else
	{

		can1_sendqueue.node[can1_sendqueue.Rear].Id = ID_DJIMotor_F4_TX;

		can1_sendqueue.node[can1_sendqueue.Rear].DLC = 0X04;

		can1_sendqueue.node[can1_sendqueue.Rear].Data[0] = 0X01;
		can1_sendqueue.node[can1_sendqueue.Rear].Data[1] = 'T' + 0x20;

		can1_sendqueue.node[can1_sendqueue.Rear].Data[2] = 'S';
		can1_sendqueue.node[can1_sendqueue.Rear].Data[3] = 'T';
	}
	can1_sendqueue.Rear = Rear1;
}
/**
  * @brief ʹ�ܵ��
  */
void FrontDJI_EnableMotor(void)
{
	if (Rear1 == can1_sendqueue.Front)
	{
		queue_flag.can1_queue_full++;
		return;
	}
	else
	{

		can1_sendqueue.node[can1_sendqueue.Rear].Id = ID_DJIMotor_F4_TX;

		can1_sendqueue.node[can1_sendqueue.Rear].DLC = 0X04;

		can1_sendqueue.node[can1_sendqueue.Rear].Data[0] = 0X01;
		can1_sendqueue.node[can1_sendqueue.Rear].Data[1] = 'B' + 0x20;

		can1_sendqueue.node[can1_sendqueue.Rear].Data[2] = 'B';
		can1_sendqueue.node[can1_sendqueue.Rear].Data[3] = 'G';
	}
	can1_sendqueue.Rear = Rear1;
}
/**
  * @brief ��ѯ���λ�ã�����Ϊ���Ƕȣ���λ���ȣ�
  */
void FrontDJI_AskPosition(u8 ID)
{
	if (Rear1 == can1_sendqueue.Front)
	{
		queue_flag.can1_queue_full++;
		return;
	}
	else
	{
		//				if(ID>=1&&ID<=4)
		//				{
		can1_sendqueue.node[can1_sendqueue.Rear].Id = ID_DJIMotor_F4_TX;
		//				}
		//				else
		//				{
		//				can1_sendqueue.node[can1_sendqueue.Rear].Id = ID_DJIMotor_B4_TX;
		//				}
		can1_sendqueue.node[can1_sendqueue.Rear].DLC = 0X05;

		can1_sendqueue.node[can1_sendqueue.Rear].Data[0] = 0X01;
		can1_sendqueue.node[can1_sendqueue.Rear].Data[1] = 'A' + 0x20;

		can1_sendqueue.node[can1_sendqueue.Rear].Data[2] = 'A';
		can1_sendqueue.node[can1_sendqueue.Rear].Data[3] = 'P';
		can1_sendqueue.node[can1_sendqueue.Rear].Data[4] = ID;
	}
	can1_sendqueue.Rear = Rear1;
}
/**
  * @brief ��ѯ����ٶ� ������Ϊ���ת�� r/min��
  */
void FrontDJI_AskSpeed(u8 ID)
{
	if (Rear1 == can1_sendqueue.Front)
	{
		queue_flag.can1_queue_full++;
		return;
	}
	else
	{
		//				if(ID>=1&&ID<=4)
		//				{
		can1_sendqueue.node[can1_sendqueue.Rear].Id = ID_DJIMotor_F4_TX;
		//				}
		//				else
		//				{
		//				can1_sendqueue.node[can1_sendqueue.Rear].Id = ID_DJIMotor_B4_TX;
		//				}
		can1_sendqueue.node[can1_sendqueue.Rear].DLC = 0X05;

		can1_sendqueue.node[can1_sendqueue.Rear].Data[0] = 0X01;
		can1_sendqueue.node[can1_sendqueue.Rear].Data[1] = 'A' + 0x40;

		can1_sendqueue.node[can1_sendqueue.Rear].Data[2] = 'A';
		can1_sendqueue.node[can1_sendqueue.Rear].Data[3] = 'S';
		can1_sendqueue.node[can1_sendqueue.Rear].Data[4] = ID;
	}
	can1_sendqueue.Rear = Rear1;
}
void AngleBegin(void)
{
	if (Rear1 == can1_sendqueue.Front)
	{
		queue_flag.can1_queue_full++;
		return;
	}
	else
	{
		can1_sendqueue.node[can1_sendqueue.Rear].Id = ID_CAM_TX;
		can1_sendqueue.node[can1_sendqueue.Rear].DLC = 0X04;
		can1_sendqueue.node[can1_sendqueue.Rear].Data[0] = 0X02;
		can1_sendqueue.node[can1_sendqueue.Rear].Data[1] = 'A' + 0x20;
		can1_sendqueue.node[can1_sendqueue.Rear].Data[2] = 'S';
		can1_sendqueue.node[can1_sendqueue.Rear].Data[3] = 'K';
	}
	can1_sendqueue.Rear = Rear1;
}
void AngleStop(void)
{
	if (Rear1 == can1_sendqueue.Front)
	{
		queue_flag.can1_queue_full++;
		return;
	}
	else
	{
		can1_sendqueue.node[can1_sendqueue.Rear].Id = ID_CAM_TX;
		can1_sendqueue.node[can1_sendqueue.Rear].DLC = 0X04;
		can1_sendqueue.node[can1_sendqueue.Rear].Data[0] = 0X02;
		can1_sendqueue.node[can1_sendqueue.Rear].Data[1] = 'S' + 0x20;
		can1_sendqueue.node[can1_sendqueue.Rear].Data[2] = 'T';
		can1_sendqueue.node[can1_sendqueue.Rear].Data[3] = 'P';
	}
	can1_sendqueue.Rear = Rear1;
}

void UnlockTouchCheckF(void)
{
	if (Rear1 == can1_sendqueue.Front)
	{
		queue_flag.can1_queue_full++;
		return;
	}
	else
	{
		can1_sendqueue.node[can1_sendqueue.Rear].Id = ID_DJIMotor_F4_TX;
		can1_sendqueue.node[can1_sendqueue.Rear].DLC = 0X04;
		can1_sendqueue.node[can1_sendqueue.Rear].Data[0] = 0X01;
		can1_sendqueue.node[can1_sendqueue.Rear].Data[1] = 'C' + 0x20;
		can1_sendqueue.node[can1_sendqueue.Rear].Data[2] = 'K';
		can1_sendqueue.node[can1_sendqueue.Rear].Data[3] = 'K';
	}
	can1_sendqueue.Rear = Rear1;
}

void LockTouchCheckF(void)
{
	if (Rear1 == can1_sendqueue.Front)
	{
		queue_flag.can1_queue_full++;
		return;
	}
	else
	{
		can1_sendqueue.node[can1_sendqueue.Rear].Id = ID_DJIMotor_F4_TX;
		can1_sendqueue.node[can1_sendqueue.Rear].DLC = 0X04;
		can1_sendqueue.node[can1_sendqueue.Rear].Data[0] = 0X01;
		can1_sendqueue.node[can1_sendqueue.Rear].Data[1] = 'C' + 0x20;
		can1_sendqueue.node[can1_sendqueue.Rear].Data[2] = 'G';
		can1_sendqueue.node[can1_sendqueue.Rear].Data[3] = 'G';
	}
	can1_sendqueue.Rear = Rear1;
}

/**
  * @brief  ������λ���ؼ��
  */
void UnlockSwitchCheck(void)
{
	if (Rear1 == can1_sendqueue.Front)
	{
		queue_flag.can1_queue_full++;
		return;
	}
	else
	{
		can1_sendqueue.node[can1_sendqueue.Rear].Id = ID_steerE_TX;
		can1_sendqueue.node[can1_sendqueue.Rear].DLC = 0X04;
		can1_sendqueue.node[can1_sendqueue.Rear].Data[0] = 0X01;
		can1_sendqueue.node[can1_sendqueue.Rear].Data[1] = 'C' + 0x20;
		can1_sendqueue.node[can1_sendqueue.Rear].Data[2] = 'K';
		can1_sendqueue.node[can1_sendqueue.Rear].Data[3] = 'K';
	}
	can1_sendqueue.Rear = Rear1;
}

/**
  * @brief  �ر���λ���ؼ��
  */
void LockSwitchCheck(void)
{
	if (Rear1 == can1_sendqueue.Front)
	{
		queue_flag.can1_queue_full++;
		return;
	}
	else
	{
		can1_sendqueue.node[can1_sendqueue.Rear].Id = ID_steerE_TX;
		can1_sendqueue.node[can1_sendqueue.Rear].DLC = 0X04;
		can1_sendqueue.node[can1_sendqueue.Rear].Data[0] = 0X01;
		can1_sendqueue.node[can1_sendqueue.Rear].Data[1] = 'C' + 0x20;
		can1_sendqueue.node[can1_sendqueue.Rear].Data[2] = 'G';
		can1_sendqueue.node[can1_sendqueue.Rear].Data[3] = 'G';
	}
	can1_sendqueue.Rear = Rear1;
}

/**
  * @brief  �ر���λ���ط���
  */
void STOPSwitch(void)
{
	if (Rear1 == can1_sendqueue.Front)
	{
		queue_flag.can1_queue_full++;
		return;
	}
	else
	{
		can1_sendqueue.node[can1_sendqueue.Rear].Id = ID_steerE_TX;
		can1_sendqueue.node[can1_sendqueue.Rear].DLC = 0X03;
		can1_sendqueue.node[can1_sendqueue.Rear].Data[0] = 0X01;
		can1_sendqueue.node[can1_sendqueue.Rear].Data[1] = 'B' + 0x20;
		can1_sendqueue.node[can1_sendqueue.Rear].Data[2] = 'G' + 0x20;
	}
	can1_sendqueue.Rear = Rear1;
}

/**
  * @brief  ������翪�ؼ��
  */
void UnlockUPtoHILLCheck(void)
{
	if (Rear1 == can1_sendqueue.Front)
	{
		queue_flag.can1_queue_full++;
		return;
	}
	else
	{
		can1_sendqueue.node[can1_sendqueue.Rear].Id = ID_steerE_TX;
		can1_sendqueue.node[can1_sendqueue.Rear].DLC = 0X04;
		can1_sendqueue.node[can1_sendqueue.Rear].Data[0] = 0X02;
		can1_sendqueue.node[can1_sendqueue.Rear].Data[1] = 'C' + 0x20;
		can1_sendqueue.node[can1_sendqueue.Rear].Data[2] = 'K';
		can1_sendqueue.node[can1_sendqueue.Rear].Data[3] = 'K';
	}
	can1_sendqueue.Rear = Rear1;
}
/**
  * @brief  �رչ�翪�ؼ��
  */
void LockUPtoHILLCheck(void)
{
	if (Rear1 == can1_sendqueue.Front)
	{
		queue_flag.can1_queue_full++;
		return;
	}
	else
	{
		can1_sendqueue.node[can1_sendqueue.Rear].Id = ID_steerE_TX;
		can1_sendqueue.node[can1_sendqueue.Rear].DLC = 0X04;
		can1_sendqueue.node[can1_sendqueue.Rear].Data[0] = 0X02;
		can1_sendqueue.node[can1_sendqueue.Rear].Data[1] = 'C' + 0x20;
		can1_sendqueue.node[can1_sendqueue.Rear].Data[2] = 'G';
		can1_sendqueue.node[can1_sendqueue.Rear].Data[3] = 'G';
	}
	can1_sendqueue.Rear = Rear1;
}

/**
  * @brief  �򿪶��
  */
void opensteerE(void)
{
	if (Rear1 == can1_sendqueue.Front)
	{
		queue_flag.can1_queue_full++;
		return;
	}
	else
	{
		can1_sendqueue.node[can1_sendqueue.Rear].Id = ID_steerE_TX;
		can1_sendqueue.node[can1_sendqueue.Rear].DLC = 0X04;
		can1_sendqueue.node[can1_sendqueue.Rear].Data[0] = 'O';
		can1_sendqueue.node[can1_sendqueue.Rear].Data[1] = 'P';
		can1_sendqueue.node[can1_sendqueue.Rear].Data[2] = 'E';
		can1_sendqueue.node[can1_sendqueue.Rear].Data[3] = 'N';
	}
	can1_sendqueue.Rear = Rear1;
}
/**
  * @brief  �رն��
  */
void closesteerE(void)
{
	if (Rear1 == can1_sendqueue.Front)
	{
		queue_flag.can1_queue_full++;
		return;
	}
	else
	{
		can1_sendqueue.node[can1_sendqueue.Rear].Id = ID_steerE_TX;
		can1_sendqueue.node[can1_sendqueue.Rear].DLC = 0X04;
		can1_sendqueue.node[can1_sendqueue.Rear].Data[0] = 'C';
		can1_sendqueue.node[can1_sendqueue.Rear].Data[1] = 'L';
		can1_sendqueue.node[can1_sendqueue.Rear].Data[2] = 'O';
		can1_sendqueue.node[can1_sendqueue.Rear].Data[3] = 'S';
	}
	can1_sendqueue.Rear = Rear1;
}

/**
  * @brief  ���pid�л���walk
  */
void MotorToWalk_CAN1(void)
{
	if (Rear1 == can1_sendqueue.Front)
	{
		queue_flag.can1_queue_full++;
		return;
	}
	else
	{
		can1_sendqueue.node[can1_sendqueue.Rear].Id = ID_DJIMotor_F4_TX;
		can1_sendqueue.node[can1_sendqueue.Rear].DLC = 0X04;
		can1_sendqueue.node[can1_sendqueue.Rear].Data[0] = 'P';
		can1_sendqueue.node[can1_sendqueue.Rear].Data[1] = 'I';
		can1_sendqueue.node[can1_sendqueue.Rear].Data[2] = 'D';
		can1_sendqueue.node[can1_sendqueue.Rear].Data[3] = 'W';
	}
	can1_sendqueue.Rear = Rear1;
}

/**
  * @brief  ���pid�л���jump��bound
  */
void MotorToJump_CAN1(void)
{
	if (Rear1 == can1_sendqueue.Front)
	{
		queue_flag.can1_queue_full++;
		return;
	}
	else
	{
		can1_sendqueue.node[can1_sendqueue.Rear].Id = ID_DJIMotor_F4_TX;
		can1_sendqueue.node[can1_sendqueue.Rear].DLC = 0X04;
		can1_sendqueue.node[can1_sendqueue.Rear].Data[0] = 'P';
		can1_sendqueue.node[can1_sendqueue.Rear].Data[1] = 'I';
		can1_sendqueue.node[can1_sendqueue.Rear].Data[2] = 'D';
		can1_sendqueue.node[can1_sendqueue.Rear].Data[3] = 'J';
	}
	can1_sendqueue.Rear = Rear1;
}

/**
  * @brief   dt50�ر�ʱ��ͨ��ģʽ
	* @object
  */
void DT50STY(u32 ID, u8 InConGrpFlag)
{
	if (Rear1 == can1_sendqueue.Front)
	{
		queue_flag.can1_queue_full++;
		return;
	}
	else
	{
		can1_sendqueue.node[can1_sendqueue.Rear].Id = ID;
		can1_sendqueue.node[can1_sendqueue.Rear].DLC = 0X04;

		can1_sendqueue.node[can1_sendqueue.Rear].Data[0] = 0x05;
		can1_sendqueue.node[can1_sendqueue.Rear].Data[1] = 'S';
		can1_sendqueue.node[can1_sendqueue.Rear].Data[2] = 'S';
		can1_sendqueue.node[can1_sendqueue.Rear].Data[3] = 'T';
		can1_sendqueue.node[can1_sendqueue.Rear].InConGrpFlag = InConGrpFlag;
	}
	can1_sendqueue.Rear = Rear1;
}

/**
  * @brief   dt50���β�ѯ
	* @object
  */
void AskDT50Y(u32 ID, u8 num, u8 InConGrpFlag)
{
	if (Rear1 == can1_sendqueue.Front)
	{
		queue_flag.can1_queue_full++;
		return;
	}
	else
	{
		can1_sendqueue.node[can1_sendqueue.Rear].Id = ID;
		can1_sendqueue.node[can1_sendqueue.Rear].DLC = 0X05;

		can1_sendqueue.node[can1_sendqueue.Rear].Data[0] = 0x05;
		can1_sendqueue.node[can1_sendqueue.Rear].Data[1] = 'A';

		can1_sendqueue.node[can1_sendqueue.Rear].Data[2] = 'S';
		can1_sendqueue.node[can1_sendqueue.Rear].Data[3] = 'K';
		can1_sendqueue.node[can1_sendqueue.Rear].Data[4] = num;
		can1_sendqueue.node[can1_sendqueue.Rear].InConGrpFlag = InConGrpFlag;
	}
	can1_sendqueue.Rear = Rear1;
}

/**
  * @brief   dt50����ʱ��ͨ��ģʽ
	* @object
  */

void DT50BGX(u32 ID, u8 Time, u8 InConGrpFlag)
{
	if (Rear1 == can1_sendqueue.Front)
	{
		queue_flag.can1_queue_full++;
		return;
	}
	else
	{
		can1_sendqueue.node[can1_sendqueue.Rear].Id = ID;
		can1_sendqueue.node[can1_sendqueue.Rear].DLC = 0X05;

		can1_sendqueue.node[can1_sendqueue.Rear].Data[0] = 0x06;
		can1_sendqueue.node[can1_sendqueue.Rear].Data[1] = 'B';

		can1_sendqueue.node[can1_sendqueue.Rear].Data[2] = 'B';
		can1_sendqueue.node[can1_sendqueue.Rear].Data[3] = 'G';
		can1_sendqueue.node[can1_sendqueue.Rear].Data[4] = Time;
		can1_sendqueue.node[can1_sendqueue.Rear].InConGrpFlag = InConGrpFlag;
	}
	can1_sendqueue.Rear = Rear1;
}

/**
  * @brief   dt50�ر�ʱ��ͨ��ģʽ
	* @object
  */
void DT50STX(u32 ID, u8 InConGrpFlag)
{
	if (Rear1 == can1_sendqueue.Front)
	{
		queue_flag.can1_queue_full++;
		return;
	}
	else
	{
		can1_sendqueue.node[can1_sendqueue.Rear].Id = ID;
		can1_sendqueue.node[can1_sendqueue.Rear].DLC = 0X04;

		can1_sendqueue.node[can1_sendqueue.Rear].Data[0] = 0x06;
		can1_sendqueue.node[can1_sendqueue.Rear].Data[1] = 'S';
		can1_sendqueue.node[can1_sendqueue.Rear].Data[2] = 'S';
		can1_sendqueue.node[can1_sendqueue.Rear].Data[3] = 'T';
		can1_sendqueue.node[can1_sendqueue.Rear].InConGrpFlag = InConGrpFlag;
	}
	can1_sendqueue.Rear = Rear1;
}

/**
  * @brief   dt50���β�ѯ
	* @object
  */
void AskDT50X(u32 ID, u8 num, u8 InConGrpFlag)
{
	if (Rear1 == can1_sendqueue.Front)
	{
		queue_flag.can1_queue_full++;
		return;
	}
	else
	{
		can1_sendqueue.node[can1_sendqueue.Rear].Id = ID;
		can1_sendqueue.node[can1_sendqueue.Rear].DLC = 0X05;

		can1_sendqueue.node[can1_sendqueue.Rear].Data[0] = 0x06;
		can1_sendqueue.node[can1_sendqueue.Rear].Data[1] = 'A';

		can1_sendqueue.node[can1_sendqueue.Rear].Data[2] = 'S';
		can1_sendqueue.node[can1_sendqueue.Rear].Data[3] = 'K';
		can1_sendqueue.node[can1_sendqueue.Rear].Data[4] = num; //����dt50���ݴ������ﵽ����֮���Զ��ر�
		can1_sendqueue.node[can1_sendqueue.Rear].InConGrpFlag = InConGrpFlag;
	}
	can1_sendqueue.Rear = Rear1;
}

/**
  * @brief   dt50����ʱ��ͨ��ģʽ
	* @object
  */

void DT50BGXL(u32 ID, u8 Time, u8 InConGrpFlag)
{
	if (Rear1 == can1_sendqueue.Front)
	{
		queue_flag.can1_queue_full++;
		return;
	}

	else
	{
		can1_sendqueue.node[can1_sendqueue.Rear].Id = ID;
		can1_sendqueue.node[can1_sendqueue.Rear].DLC = 0X05;

		can1_sendqueue.node[can1_sendqueue.Rear].Data[0] = 0x07;
		can1_sendqueue.node[can1_sendqueue.Rear].Data[1] = 'B';

		can1_sendqueue.node[can1_sendqueue.Rear].Data[2] = 'B';
		can1_sendqueue.node[can1_sendqueue.Rear].Data[3] = 'G';
		can1_sendqueue.node[can1_sendqueue.Rear].Data[4] = Time;
		can1_sendqueue.node[can1_sendqueue.Rear].InConGrpFlag = InConGrpFlag;
	}
	can1_sendqueue.Rear = Rear1;
}

/**
  * @brief   dt50�ر�ʱ��ͨ��ģʽ
	* @object
  */
void DT50STXL(u32 ID, u8 InConGrpFlag)
{
	if (Rear1 == can1_sendqueue.Front)
	{
		queue_flag.can1_queue_full++;
		return;
	}
	else
	{
		can1_sendqueue.node[can1_sendqueue.Rear].Id = ID;
		can1_sendqueue.node[can1_sendqueue.Rear].DLC = 0X04;

		can1_sendqueue.node[can1_sendqueue.Rear].Data[0] = 0x07;
		can1_sendqueue.node[can1_sendqueue.Rear].Data[1] = 'S';
		can1_sendqueue.node[can1_sendqueue.Rear].Data[2] = 'S';
		can1_sendqueue.node[can1_sendqueue.Rear].Data[3] = 'T';
		can1_sendqueue.node[can1_sendqueue.Rear].InConGrpFlag = InConGrpFlag;
	}
	can1_sendqueue.Rear = Rear1;
}

/**
  * @brief   dt50���β�ѯ
	* @object
  */
void AskDT50XL(u32 ID, u8 num, u8 InConGrpFlag)
{
	if (Rear1 == can1_sendqueue.Front)
	{
		queue_flag.can1_queue_full++;
		return;
	}
	else
	{
		can1_sendqueue.node[can1_sendqueue.Rear].Id = ID;
		can1_sendqueue.node[can1_sendqueue.Rear].DLC = 0X05;

		can1_sendqueue.node[can1_sendqueue.Rear].Data[0] = 0x07;
		can1_sendqueue.node[can1_sendqueue.Rear].Data[1] = 'A';

		can1_sendqueue.node[can1_sendqueue.Rear].Data[2] = 'S';
		can1_sendqueue.node[can1_sendqueue.Rear].Data[3] = 'K';
		can1_sendqueue.node[can1_sendqueue.Rear].Data[4] = num; //����dt50���ݴ������ﵽ����֮���Զ��ر�
		can1_sendqueue.node[can1_sendqueue.Rear].InConGrpFlag = InConGrpFlag;
	}
	can1_sendqueue.Rear = Rear1;
}

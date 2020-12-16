/*
 * @Descripttion: 底盘，转向
 * @version: 第一版
 * @Author: 叮咚蛋
 * @Date: 2020-12-15 19:29:29
 * @LastEditors: 叮咚蛋
 * @LastEditTime: 2020-12-16 15:09:26
 * @FilePath: \hu_sir-contorl\USER\SRC\can2.c
 */

#include "can2.h"

s32 ElmoPulse = 0;
s32 ElmoPulselast = 0;
s32 WheelSpeedNow[4] = {0};
float WheelIQElectricity[8] = {0.0};
float WheelIDElectricity[8] = {0.0};
MesgControlGrp CAN2_MesgSentList[CAN2_NodeNumber];
short int wheel_angle_now[4] = {0};

/**
  * @brief  CAN2初始化
  */
void Can2_Configuration(u8 prep_prio, u8 sub_prio) // 重新复习can结构体 TODO:
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
	NVIC_InitStructure.NVIC_IRQChannel = CAN2_RX0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = prep_prio;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = sub_prio;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	CAN_DeInit(CAN2);
	CAN_StructInit(&CAN_InitStructure);

	/* CAN cell init */
	CAN_InitStructure.CAN_TTCM = DISABLE;		  //非时间触发通道模式
	CAN_InitStructure.CAN_ABOM = DISABLE;		  //软件对CAN_MCR寄存器的INRQ位置1，随后清0，一旦监测到128次连续11位的隐性位，就退出离线状态
	CAN_InitStructure.CAN_AWUM = DISABLE;		  //睡眠模式由软件唤醒
	CAN_InitStructure.CAN_NART = DISABLE;		  //禁止报文自动发送，即只发送一次，无论结果如何
	CAN_InitStructure.CAN_RFLM = DISABLE;		  //报文不锁定，新的覆盖旧的
	CAN_InitStructure.CAN_TXFP = DISABLE;		  //发送FIFO的优先级由标识符决定
	CAN_InitStructure.CAN_Mode = CAN_Mode_Normal; //CAN硬件工作在正常模式

	/* Seting BaudRate */
	CAN_InitStructure.CAN_SJW = CAN_SJW_1tq; //重新同步跳跃宽度为一个时间单位
	CAN_InitStructure.CAN_BS1 = CAN_BS1_9tq; //时间段1占用8个时间单位
	CAN_InitStructure.CAN_BS2 = CAN_BS2_4tq; //时间段2占用7个时间单位
	CAN_InitStructure.CAN_Prescaler = 3;	 //分频系数（Fdiv）
	CAN_Init(CAN2, &CAN_InitStructure);		 //初始化CAN1

	/* 波特率计算公式: BaudRate = APB1时钟频率/Fdiv/（SJW+BS1+BS2） */
	/* 42MHz/3/(1+9+4)=1Mhz */

	/* CAN filter init */
	CAN_FilterInitStructure.CAN_FilterNumber = 15;
	CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdList;
	CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_16bit;

	CAN_FilterInitStructure.CAN_FilterIdHigh = 0X281 << 5; //标识符列表模式
	CAN_FilterInitStructure.CAN_FilterIdLow = 0X282 << 5;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0X283 << 5;
	CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0x284 << 5;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_FilterFIFO0;
	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
	CAN_FilterInit(&CAN_FilterInitStructure);

	CAN_FilterInitStructure.CAN_FilterNumber = 16; //屏蔽位模式
	CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdList;
	CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_16bit;

	CAN_FilterInitStructure.CAN_FilterIdHigh = 0X285 << 5; //标识符列表模式
	CAN_FilterInitStructure.CAN_FilterIdLow = 0X286 << 5;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0X287 << 5;
	CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0x288 << 5; //标识符列表模式
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_FilterFIFO0;
	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
	CAN_FilterInit(&CAN_FilterInitStructure);

	//错误反馈
	CAN_FilterInitStructure.CAN_FilterNumber = 14; //屏蔽位模式
	CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdList;
	CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_16bit;

	CAN_FilterInitStructure.CAN_FilterIdHigh = 0X181 << 5; //标识符列表模式
	CAN_FilterInitStructure.CAN_FilterIdLow = 0X182 << 5;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0X183 << 5;
	CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0x184 << 5; //标识符列表模式
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_FilterFIFO0;
	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
	CAN_FilterInit(&CAN_FilterInitStructure);

	CAN_FilterInitStructure.CAN_FilterNumber = 13; //屏蔽位模式
	CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdList;
	CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_16bit;

	CAN_FilterInitStructure.CAN_FilterIdHigh = 0X081 << 5; //标识符列表模式
	CAN_FilterInitStructure.CAN_FilterIdLow = 0X082 << 5;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0X083 << 5;
	CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0x084 << 5; //标识符列表模式
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_FilterFIFO0;
	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
	CAN_FilterInit(&CAN_FilterInitStructure);

	CAN_ITConfig(CAN2, CAN_IT_FMP0, ENABLE);
}
u8 MOFlag[4] = {0};

/**
  * @brief  CAN2
  */
CanRxMsg rx_mes;

void CAN2_RX0_IRQHandler(void)
{
	int ErroCode = 0;
	char str_tempp[15] = "well";
	CanRxMsg rx_message;
	CAN_ClearITPendingBit(CAN2, CAN_IT_FMP0);
	CAN_ClearFlag(CAN2, CAN_IT_FMP0);
	if (CAN_GetITStatus(CAN2, CAN_IT_FMP0) != RESET)
	{
		CAN_Receive(CAN2, CAN_FIFO0, &rx_message);
		rx_mes = rx_message;
		if ((rx_message.StdId >= 0x081 && rx_message.StdId <= 0x084) || (rx_message.StdId >= 0x181 && rx_message.StdId <= 0x184)) //电机故障处理
		{
			if (rx_message.Data[0] == 0x40 || rx_message.Data[0] == 0x18)
			{
				switch (rx_message.StdId) // TODO: error id
				{
				case 0x81:
					//sprintf(user.error,"%s","Motor1Err");
					//BREAK_ALL_MOTOR;
					break;
				case 0x181:
					//	sprintf(user.error,"%s","Motor1Err");
					//BREAK_ALL_MOTOR;
					break;
				case 0x82:
					//		sprintf(user.error,"%s","Motor2Err");
					//BREAK_ALL_MOTOR;
					break;
				case 0x182:
					//sprintf(user.error,"%s","Motor2Err");
					//BREAK_ALL_MOTOR;
					break;
				case 0x83:
					//sprintf(user.error,"%s","Motor3Err");
					//BREAK_ALL_MOTOR;
					break;
				case 0x183:
					//sprintf(user.error,"%s","Motor3Err");
					//BREAK_ALL_MOTOR;
					break;
				case 0x84:
					//sprintf(user.error,"%s","Motor4Err");
					//BREAK_ALL_MOTOR;
					break;
				case 0x184:
					//sprintf(user.error,"%s","Motor4Err");
					//BREAK_ALL_MOTOR;
					break;

				default:
					break;
				}
			}
		}
		else
		{
			switch (rx_message.StdId)
			{
			case ID_Elmo_Motor1_RX:
			{
				switch (rx_message.Data[0])
				{
				case 'V': //主速度
				{
					if ('X' == rx_message.Data[1])
					{
						OSFlagPost(FlagCan2Check, 0x0001, OS_FLAG_SET, ErrorCan2);
						DecodeS32To4ByteNone(&WheelSpeedNow[0], &rx_message.Data[4]);
						WheelSpeedNow[0] /= (motor.encoder_resolution / 15.0f); // / 15 = 4 / 60，这个4是elmo那边的一个转换，暂时不用改，如果出现给电机的速度和实际看起来不对，就需要注意了。
					}
					break;
				}
				case 'M': //启动，释放
				{
					if ('O' == rx_message.Data[1])
					{
						/*  报文控制快是不是可以动刀*/
						CAN2_MesgSentList[0].ReceiveNumber += 1;
						CAN2_MesgSentList[0].TimeOut = 0;
						CAN2_MesgSentList[0].SendSem--;
						MOFlag[0]++;
					}
					break;
				}

				case 'S':
				{
					if ('T' == rx_message.Data[1]) //???ˉ
					{

						/*  报文控制块发送标志位减一  */
						CAN2_MesgSentList[0].ReceiveNumber += 1;
						CAN2_MesgSentList[0].TimeOut = 0;
						CAN2_MesgSentList[0].SendSem--;
						CAN2_MesgSentList[0].SentQueue.Front = (CAN2_MesgSentList[0].SentQueue.Front + 1) % CAN2_MesgSentList[0].SentQueue.Can_sendqueuesize;
					}
					break;
				}

				case 'B': //执行当前指令
				{
					if ('G' == rx_message.Data[1])
					{
					}
					break;
				}
				case 'I': //?′DDμ±?°??á?
				{
					if ('Q' == rx_message.Data[1])
					{
						DecodeFloatData(&WheelIQElectricity[0], &rx_message.Data[4]);
					}
					else if ('D' == rx_message.Data[1])
					{
						DecodeFloatData(&WheelIDElectricity[0], &rx_message.Data[4]);
					}
					break;
				}

				case 'E':
				{
					if ('C' == rx_message.Data[1])
					{
						DecodeS32To4ByteNone(&ErroCode, &rx_message.Data[4]);
						sprintf(user.error, "%3d", (int)ErroCode);
						strcat(user.error, "Elmo1");
					}
					break;
				}

				case 'P':
				{
					if ('X' == rx_message.Data[1])
					{
					}
					break;
				}
				}
				break;
			}
			case ID_Elmo_Motor2_RX:
			{
				switch (rx_message.Data[0])
				{
				case 'V': //主速度
				{
					if ('X' == rx_message.Data[1])
					{
						OSFlagPost(FlagCan2Check, 0x0002, OS_FLAG_SET, ErrorCan2);
						DecodeS32To4ByteNone(&WheelSpeedNow[0], &rx_message.Data[4]);
						WheelSpeedNow[1] /= (motor.encoder_resolution / 15.0f);
					}
					break;
				}
				case 'M': //启动，释放
				{
					if ('O' == rx_message.Data[1])
					{
						//						/* ±¨???????é·￠?í±ê??????ò? */
						//						CAN2_MesgSentList[0].ReceiveNumber += 1;
						//						CAN2_MesgSentList[0].TimeOut = 0;
						//						CAN2_MesgSentList[0].SendSem --;
						//						MOFlag[0]++;不重复处理启动指令
					}
					break;
				}

				case 'S':
				{
					if ('T' == rx_message.Data[1]) //???ˉ
					{

						//						/* ±¨???????é·￠?í±ê??????ò? */
						//						CAN2_MesgSentList[0].ReceiveNumber += 1;
						//						CAN2_MesgSentList[0].TimeOut = 0;
						//						CAN2_MesgSentList[0].SendSem --;
						//						CAN2_MesgSentList[0].SentQueue.Front = (CAN2_MesgSentList[0].SentQueue.Front+1)%CAN2_MesgSentList[0].SentQueue.Can_sendqueuesize;
					}
					break;
				}

				case 'B': //?′DDμ±?°??á?
				{
					if ('G' == rx_message.Data[1])
					{
					}
					break;
				}
				case 'I': //?′DDμ±?°??á?
				{
					if ('Q' == rx_message.Data[1])
					{
						DecodeFloatData(&WheelIQElectricity[0], &rx_message.Data[4]);
					}
					else if ('D' == rx_message.Data[1])
					{
						DecodeFloatData(&WheelIDElectricity[0], &rx_message.Data[4]);
					}
					break;
				}

				case 'E':
				{
					if ('C' == rx_message.Data[1])
					{
						DecodeS32To4ByteNone(&ErroCode, &rx_message.Data[4]);
						sprintf(user.error, "%3d", (int)ErroCode);
						strcat(user.error, "Elmo2");
					}
					break;
				}

				case 'P':
				{
					if ('X' == rx_message.Data[1])
					{
					}
					break;
				}
				}
				break;
			}
			case ID_Elmo_Motor3_RX:
			{
				switch (rx_message.Data[0])
				{
				case 'V': //主速度
				{
					if ('X' == rx_message.Data[1])
					{
						OSFlagPost(FlagCan2Check, 0x0004, OS_FLAG_SET, ErrorCan2);
						DecodeS32To4ByteNone(&WheelSpeedNow[0], &rx_message.Data[4]);
						WheelSpeedNow[2] /= (motor.encoder_resolution / 15.0f);
					}
					break;
				}
				case 'M': //启动，释放
				{
					if ('O' == rx_message.Data[1])
					{
						/* ±¨???????é·￠?í±ê??????ò? */
						//						CAN2_MesgSentList[0].ReceiveNumber += 1;
						//						CAN2_MesgSentList[0].TimeOut = 0;
						//						CAN2_MesgSentList[0].SendSem --;
						//						MOFlag[0]++;
					}
					break;
				}

				case 'S':
				{
					if ('T' == rx_message.Data[1]) //???ˉ
					{

						/* ±¨???????é·￠?í±ê??????ò? */
						//						CAN2_MesgSentList[0].ReceiveNumber += 1;
						//						CAN2_MesgSentList[0].TimeOut = 0;
						//						CAN2_MesgSentList[0].SendSem --;
						//						CAN2_MesgSentList[0].SentQueue.Front = (CAN2_MesgSentList[0].SentQueue.Front+1)%CAN2_MesgSentList[0].SentQueue.Can_sendqueuesize;
					}
					break;
				}

				case 'B': //?′DDμ±?°??á?
				{
					if ('G' == rx_message.Data[1])
					{
					}
					break;
				}
				case 'I': //?′DDμ±?°??á?
				{
					if ('Q' == rx_message.Data[1])
					{
						DecodeFloatData(&WheelIQElectricity[0], &rx_message.Data[4]);
					}
					else if ('D' == rx_message.Data[1])
					{
						DecodeFloatData(&WheelIDElectricity[0], &rx_message.Data[4]);
					}
					break;
				}

				case 'E':
				{
					if ('C' == rx_message.Data[1])
					{
						DecodeS32To4ByteNone(&ErroCode, &rx_message.Data[4]);
						sprintf(user.error, "%3d", (int)ErroCode);
						strcat(user.error, "Elmo3");
					}
					break;
				}

				case 'P':
				{
					if ('X' == rx_message.Data[1])
					{
					}
					break;
				}
				}
				break;
			}
				/********************************************************** 底盘4号电机 *********************************************************************/
			case ID_Elmo_Motor4_RX:
			{
				switch (rx_message.Data[0])
				{
				case 'V': //主速度
				{
					if ('X' == rx_message.Data[1])
					{
						OSFlagPost(FlagCan2Check, 0x0008, OS_FLAG_SET, ErrorCan2);
						DecodeS32To4ByteNone(&WheelSpeedNow[3], &rx_message.Data[4]);
						WheelSpeedNow[3] /= (motor.encoder_resolution / 15.0f);
					}
					break;
				}
				case 'M': //启动/释放
				{
					if ('O' == rx_message.Data[1])
					{
						MOFlag[0]++;
						//									/* 报文控制块发送标志位减一 */
						//									CAN2_MesgSentList[0].ReceiveNumber += 1;
						//									CAN2_MesgSentList[0].TimeOut = 0;
						//									CAN2_MesgSentList[0].SendSem --;
						//									CAN2_MesgSentList[0].SentQueue.Front = (CAN2_MesgSentList[0].SentQueue.Front+1)%CAN2_MesgSentList[0].SentQueue.Can_sendqueuesize;
					}
					break;
				}

				case 'S':
				{
					if ('T' == rx_message.Data[1]) //制动
					{

						//									/* 报文控制块发送标志位减一 */
						//									CAN2_MesgSentList[0].ReceiveNumber += 1;
						//									CAN2_MesgSentList[0].TimeOut = 0;
						//									CAN2_MesgSentList[0].SendSem --;
						//									CAN2_MesgSentList[0].SentQueue.Front = (CAN2_MesgSentList[0].SentQueue.Front+1)%CAN2_MesgSentList[0].SentQueue.Can_sendqueuesize;
					}
					break;
				}

				case 'B': //执行当前指令
				{
					if ('G' == rx_message.Data[1])
					{
						OSFlagPost(FlagCan2Check, 0x0008, OS_FLAG_SET, ErrorCan2);
					}
					break;
				}
				case 'I': //执行当前指令
				{
					if ('Q' == rx_message.Data[1])
					{
						DecodeFloatData(&WheelIQElectricity[3], &rx_message.Data[4]);
					}
					else if ('D' == rx_message.Data[1])
					{
						DecodeFloatData(&WheelIDElectricity[3], &rx_message.Data[4]);
					}
					break;
				}

				case 'E':
				{
					if ('C' == rx_message.Data[1])
					{
						DecodeS32To4ByteNone(&ErroCode, &rx_message.Data[4]);
						sprintf(user.error, "%3d", (int)ErroCode);
						strcat(user.error, "Elmo4");
					}
					break;
				}
				}
				break;
			}
			default:
			{
				// 底盘转向电机
				if (0x288 >= rx_message.StdId && 0x285 <= rx_message.StdId)
				{
					OSFlagPost(FlagCan2Check, 0x0010, OS_FLAG_SET, ErrorCan2);
					switch (rx_message.Data[0])
					{
					case 'M':
					{
						if ('O' == rx_message.Data[1])
						{
							CAN2_MesgSentList[(0x000f & rx_message.StdId) - 1].ReceiveNumber += 1;
							CAN2_MesgSentList[(0x000f & rx_message.StdId) - 1].TimeOut = 0;
							CAN2_MesgSentList[(0x000f & rx_message.StdId) - 1].SendSem--;
							CAN2_MesgSentList[(0x000f & rx_message.StdId) - 1].SentQueue.Front = (CAN2_MesgSentList[(0x000f & rx_message.StdId) - 1].SentQueue.Front + 1) % CAN2_MesgSentList[(0x000f & rx_message.StdId) - 1].SentQueue.Can_sendqueuesize;
						}
						break;
					}

					case 'A':
					{
						if ('S' == rx_message.Data[1] && 'K' == rx_message.Data[2])
						{
							// if (ID_Streeing_Motor5_RX == rx_message.StdId)
							// 	queue_flag.CarSzFinish1 = rx_message.Data[3];
							// else if (ID_Streeing_Motor6_RX == rx_message.StdId)
							// 	queue_flag.CarSzFinish2 = rx_message.Data[3];
							// else if (ID_Streeing_Motor7_RX == rx_message.StdId)
							// 	queue_flag.CarSzFinish3 = rx_message.Data[3];
							// else
							// 	queue_flag.CarSzFinish4 = rx_message.Data[3];
						}
						break;
					}

					case 'S':
					{
						if ('T' == rx_message.Data[1])
						{
							CAN2_MesgSentList[(0x000f & rx_message.StdId) - 1].ReceiveNumber += 1;
							CAN2_MesgSentList[(0x000f & rx_message.StdId) - 1].TimeOut = 0;
							CAN2_MesgSentList[(0x000f & rx_message.StdId) - 1].SendSem--;
							CAN2_MesgSentList[(0x000f & rx_message.StdId) - 1].SentQueue.Front = (CAN2_MesgSentList[(0x000f & rx_message.StdId) - 1].SentQueue.Front + 1) % CAN2_MesgSentList[(0x000f & rx_message.StdId) - 1].SentQueue.Can_sendqueuesize;
						}

						else if ('P' == rx_message.Data[1])
						{
							CAN2_MesgSentList[(0x000f & rx_message.StdId) - 1].ReceiveNumber += 1;
							CAN2_MesgSentList[(0x000f & rx_message.StdId) - 1].TimeOut = 0;
							CAN2_MesgSentList[(0x000f & rx_message.StdId) - 1].SendSem--;
							CAN2_MesgSentList[(0x000f & rx_message.StdId) - 1].SentQueue.Front = (CAN2_MesgSentList[(0x000f & rx_message.StdId) - 1].SentQueue.Front + 1) % CAN2_MesgSentList[(0x000f & rx_message.StdId) - 1].SentQueue.Can_sendqueuesize;
						}
						break;
					}

					case 'P':
					{
						if ('I' == rx_message.Data[1] && 'D' == rx_message.Data[2])
						{
							CAN2_MesgSentList[(0x000f & rx_message.StdId) - 1].ReceiveNumber += 1;
							CAN2_MesgSentList[(0x000f & rx_message.StdId) - 1].TimeOut = 0;
							CAN2_MesgSentList[(0x000f & rx_message.StdId) - 1].SendSem--;
							CAN2_MesgSentList[(0x000f & rx_message.StdId) - 1].SentQueue.Front = (CAN2_MesgSentList[(0x000f & rx_message.StdId) - 1].SentQueue.Front + 1) % CAN2_MesgSentList[(0x000f & rx_message.StdId) - 1].SentQueue.Can_sendqueuesize;
						}
						break;
					}

					case 'B': //执行当前指令
					{
						if ('G' == rx_message.Data[1])
						{
						}

						if ('B' == rx_message.Data[1] && 'G' == rx_message.Data[2])
						{
							// DecodeS16To2ByteNone(&WheelAngleNow[(0x000f & rx_message.StdId) - 1], &rx_message.Data[4]);
						}
						break;
					}

					case 'E':
					{
						if ('C' == rx_message.Data[1])
						{
							ErroCode = rx_message.Data[2];
							sprintf(user.error, "%1d", (int)ErroCode);
							strcat(user.error, "TurnErr");
							ErroCode = rx_message.Data[3];
							sprintf(str_tempp, "%2d", (int)ErroCode);
							strcat(user.error, str_tempp);
						}
						break;
					}
					}
					//	  BEEP_ON();
					//			sprintf(user.error,"%s","RubishMsg");
					break;
				}
			}
			}
		}
	}
}
/**
  * @brief  CAN2自检
  */
void Can2selfcheck(u8 InConGrpFlag)
{
	if (Rear2 == can2_sendqueue.Front)
	{
		queue_flag.can2_queue_full++;
		return;
	}
	else
	{
		can2_sendqueue.node[can2_sendqueue.Rear].Id = ID_BroadCast_TX;
		can2_sendqueue.node[can2_sendqueue.Rear].DLC = 0X05;

		can2_sendqueue.node[can2_sendqueue.Rear].Data[0] = 0X00;
		can2_sendqueue.node[can2_sendqueue.Rear].Data[1] = 'C';

		can2_sendqueue.node[can2_sendqueue.Rear].Data[2] = 'W';
		can2_sendqueue.node[can2_sendqueue.Rear].Data[3] = 'H';
		can2_sendqueue.node[can2_sendqueue.Rear].Data[4] = 'U';
		can2_sendqueue.node[can2_sendqueue.Rear].InConGrpFlag = InConGrpFlag;
	}
	can2_sendqueue.Rear = Rear2;
}

/********************************End of File************************************/

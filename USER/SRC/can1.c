/*
 * @Descripttion: can1
 * @version: 第一版
 * @Author: 叮咚蛋
 * @Date: 2020-12-15 19:29:29
 * @LastEditors: 叮咚蛋
 * @LastEditTime: 2020-12-16 10:06:14
 * @FilePath: \hu_sir-contorl\USER\SRC\can1.c
 */

#include "can1.h"

// volatile float motorspeed[8] = {0};	   //为轴后转速 r/min TODO: 不应该放在这，以及，是否有用，待删除
// volatile float MotorPosition[8] = {0}; //接收到的为轴后角度，需要换算成轴前角度 单位：度

/**
  * @brief  CAN1初始化
  */
MesgControlGrp CAN1_MesgSentList[CAN1_NodeNumber];

void Can1_Configuration(u8 prep_prio, u8 sub_prio)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	CAN_InitTypeDef CAN_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	CAN_FilterInitTypeDef CAN_FilterInitStructure;

	// RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1 | RCC_AHB1Periph_GPIOA, ENABLE);
	// GPIO_PinAFConfig(GPIOA, GPIO_PinSource11 | GPIO_PinSource12, GPIO_AF_CAN1); FIXME: 震惊，这个偷懒的或运算，竟然初始化失败了，只能按照下面的这种才能成功,噗~~ 2020-09-02

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
	CAN_Init(CAN1, &CAN_InitStructure);		 //初始化CAN1

	/* 波特率计算公式: BaudRate = APB1时钟频率/Fdiv/（SJW+BS1+BS2） */
	/* 42MHz/3/(1+9+4)=1Mhz */

	/* CAN filter init */
	CAN_FilterInitStructure.CAN_FilterNumber = 0; //屏蔽位模式
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
	//这样清除标志位肯定是有问题的，因为函数参数表里没有这个参数，但是库函数有如下说法。
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
						/*获取绝对速度*/
						velocity_abs.Vx = (double)((s16)(((u16)(rx_message.Data[3]) << 8) | rx_message.Data[2])) * 0.1;
						velocity_abs.Vy = (double)((s16)(((u16)(rx_message.Data[5]) << 8) | rx_message.Data[4])) * 0.1;
						velocity_now.Vw = (double)((s16)(((u16)(rx_message.Data[7]) << 8) | rx_message.Data[6])) * 0.1;

                        /*根据绝对速度计算相对速度*/
						velocity_now.Vx = velocity_abs.Vx * cos(position_now.angle / 180 * PI) + velocity_abs.Vy * sin(position_now.angle / 180 * PI);
						velocity_now.Vy = (-velocity_abs.Vx * sin(position_now.angle / 180 * PI) + velocity_abs.Vy * cos(position_now.angle / 180 * PI));

						velocity_now.V = sqrt(velocity_now.Vx * velocity_now.Vx + velocity_now.Vy * velocity_now.Vy);
					}
					break;
				}
				case 'C': //里在干什莫
				{
					CAN1_MesgSentList[0].ReceiveNumber += 1;
					CAN1_MesgSentList[0].TimeOut = 0;
					CAN1_MesgSentList[0].SendSem--;
					CAN1_MesgSentList[0].SentQueue.Front = (CAN1_MesgSentList[0].SentQueue.Front + 1) % CAN1_MesgSentList[0].SentQueue.Can_sendqueuesize;
					break;
				}
				case 'X': /* 报文控制块发送标志位减一 */
				{
					CAN1_MesgSentList[0].ReceiveNumber += 1;
					CAN1_MesgSentList[0].TimeOut = 0;
					CAN1_MesgSentList[0].SendSem--;
					CAN1_MesgSentList[0].SentQueue.Front = (CAN1_MesgSentList[0].SentQueue.Front + 1) % CAN1_MesgSentList[0].SentQueue.Can_sendqueuesize;
					break;
				}
				case 'R': //复位
				{
					/* 报文控制块发送标志位减一 */
					CAN1_MesgSentList[0].ReceiveNumber += 1;
					CAN1_MesgSentList[0].TimeOut = 0;
					CAN1_MesgSentList[0].SendSem--;
					CAN1_MesgSentList[0].SentQueue.Front = (CAN1_MesgSentList[0].SentQueue.Front + 1) % CAN1_MesgSentList[0].SentQueue.Can_sendqueuesize;
					OSFlagPost(FlagCan1Check, 0x01, OS_FLAG_SET, ErrorCan1);
					break;
				}
				case 'A':  //询问陀螺仪状态
				{
					if (rx_message.Data[2] == 'S' && rx_message.Data[3] == 'K')
						gyroscope.init_success = rx_message.Data[4];
					break;
				}
				}
			}

			else if (0x40 == rx_message.Data[0]) //自检信号
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
					//什么操作
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
				default: //什么东西
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

				case 'S': //动作指令
					/* 报文控制块发送标志位减一 */
					CAN1_MesgSentList[3].ReceiveNumber += 1;
					CAN1_MesgSentList[3].TimeOut = 0;
					CAN1_MesgSentList[3].SendSem--;
					CAN1_MesgSentList[3].SentQueue.Front = (CAN1_MesgSentList[3].SentQueue.Front + 1) % CAN1_MesgSentList[3].SentQueue.Can_sendqueuesize;
					break;

				case 'D': //动作指令
					/* 报文控制块发送标志位减一 */
					//Dt50YNum++;

					OS_ENTER_CRITICAL();
					//Dt50YDis = ((rx_message.Data[3] << 8) | rx_message.Data[2]) / 10.0f;
					OS_EXIT_CRITICAL();

					//									if(FieldChoose==1)//红场
					//										PosNowYDt50=(float)(FieldChoose*(//Dt50YDis+HalfLength)*cos(PositionNow.angle));//未转90°之前
					//									else if(FieldChoose==-1)
					//										PosNowYDt50=(float)((FieldChoose*(//Dt50YDis+HalfLength)*cos(PositionNow.angle)));//未转90°之前
					//Dt50YDisSum += //Dt50YDis;
					break;

				case 'F': //动作指令
					/* //每次上电红蓝场只能选择一次，防止车在行进过程中红蓝场改变导致飞车，这个变量只有手柄和dt50可以改变 */
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
  * @brief  CAN1自检
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
  * @brief 陀螺仪重置X坐标
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
 * @brief  复位从机
 * @author 未定义
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
  * @brief 陀螺仪重置Y坐标
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
  * @brief 陀螺仪重置X坐标和角度
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
  * @brief  设置重定位零点
  */
void ResetPosition_EN(float x, float y, float angle, u8 InConGrpFlag)//ASK:发送这三个数据的处理是什么
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
  * @brief  前4个电机寻零
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
  * @brief  前4个电机寻零速度（轴后，r/min）
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
  * @brief  设置位置模式速度（轴后，r/min）
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
  * @brief  设置位置模式位置（轴后角度，单位：度）
  */
void FrontDJI_SetPTPPosition(u8 ID, s16 pulse)
{
	if (Rear1 == can1_sendqueue.Front) //队列已满
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
  * @brief  设置速度模式速度 （轴后，r/min）
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
  * @brief 设置电流模式电流
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
  * @brief 选择电机模式
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
  * @brief 释放电机
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
  * @brief 急停电机
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
  * @brief 使能电机
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
  * @brief 查询电机位置（反馈为轴后角度，单位：度）
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
  * @brief 查询电机速度 （反馈为轴后转速 r/min）
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
  * @brief  开启限位开关检测
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
  * @brief  关闭限位开关检测
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
  * @brief  关闭限位开关发送
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
  * @brief  开启光电开关检测
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
  * @brief  关闭光电开关检测
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
  * @brief  打开舵机
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
  * @brief  关闭舵机
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
  * @brief  电机pid切换到walk
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
  * @brief  电机pid切换到jump和bound
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
  * @brief   dt50关闭时间通信模式
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
  * @brief   dt50单次查询
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
  * @brief   dt50开启时间通信模式
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
  * @brief   dt50关闭时间通信模式
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
  * @brief   dt50单次查询
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
		can1_sendqueue.node[can1_sendqueue.Rear].Data[4] = num; //发送dt50数据次数，达到次数之后自动关闭
		can1_sendqueue.node[can1_sendqueue.Rear].InConGrpFlag = InConGrpFlag;
	}
	can1_sendqueue.Rear = Rear1;
}

/**
  * @brief   dt50开启时间通信模式
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
  * @brief   dt50关闭时间通信模式
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
  * @brief   dt50单次查询
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
		can1_sendqueue.node[can1_sendqueue.Rear].Data[4] = num; //发送dt50数据次数，达到次数之后自动关闭
		can1_sendqueue.node[can1_sendqueue.Rear].InConGrpFlag = InConGrpFlag;
	}
	can1_sendqueue.Rear = Rear1;
}

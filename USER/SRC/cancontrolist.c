/*
 * @Descripttion: 报文控制表
 * @version: 第一版
 * @Author: 叮咚蛋
 * @Date: 2020-12-10 20:21:29
 * @LastEditors: 叮咚蛋
 * @LastEditTime: 2020-12-20 14:49:13
 * @FilePath: \hu_sir-contorl\USER\SRC\cancontrolist.c
 */

#include "cancontrolist.h"

/*** 
 * @brief 初始化can报文控制链表 // FIXME:
 * @author: 叮咚蛋
 * @Date: 
 */
void InitCANControlList(MesgControlGrp *CAN_MesgSentList, u8 CAN_x)
{
	u8 i;
	u8 CAN_NodeNumber = 0;
	if (CAN_1 == CAN_x)
	{
		queue_flag.Can1ControlList = True;
		CAN_NodeNumber = CAN1_NodeNumber;
	}
	else
	{
		queue_flag.Can2ControlList = True;
		CAN_NodeNumber = CAN2_NodeNumber;
	}
	for (i = 0; i < CAN_NodeNumber; i++)
	{
		CAN_MesgSentList[i].SendSem = 0;
		CAN_MesgSentList[i].TimeOut = 0;
		CAN_MesgSentList[i].QUEUEFullTimeout = 0;
		CAN_MesgSentList[i].ReceiveNumber = 0;
		CAN_MesgSentList[i].SendNumber = 0;
		CAN_MesgSentList[i].SentQueue.Can_sendqueuesize = CAN_CONTROLQUEUESIZE;
		CAN_MesgSentList[i].SentQueue.Front = 0;
		CAN_MesgSentList[i].SentQueue.Rear = 0;
		CAN_MesgSentList[i].SentQueue.node[0].InConGrpFlag = 0;
	}
}

/**
 * @author: 叮咚蛋
 * @brief: 遍历报文发送状态
 */

void TraversalControlList(MesgControlGrp *CAN_MesgSentList, CanSendqueue *can_queue, u8 CAN_x)
{
	u8 i = 0;
	u8 CAN_NodeNumber = 0;
	if (CAN_1 == CAN_x)
	{
		CAN_NodeNumber = CAN1_NodeNumber;
	}
	else
	{
		CAN_NodeNumber = CAN2_NodeNumber;
	}
	//遍历
	for (i = 0; i < CAN_NodeNumber; i++)
	{
		if (CAN_MesgSentList[i].SendSem >= 1)
		{
			if (CAN_MesgSentList[i].TimeOut++ >= 30)
			{
				if ((can_queue->Rear + 1) % (can_queue->Can_sendqueuesize) == (can_queue->Front))
				{
					CAN_MesgSentList[i].QUEUEFullTimeout++;
					if (CAN_1 == CAN_x)
						queue_flag.can1_queue_full++;
					else
						queue_flag.can2_queue_full++;
					return;
				}
				else if (CAN_MesgSentList[i].SentQueue.Rear == CAN_MesgSentList[i].SentQueue.Front)
				{
					queue_flag.can_queue_empty = True;
					CAN_MesgSentList[i].QUEUEFullTimeout = 0;
					return;
				}
				//				else
				//				{
				//					CAN_MesgSentList[i].SendSem--;
				//					CAN_MesgSentList[i].TimeOut = 0;
				//					CAN_MesgSentList[i].QUEUEFullTimeout = 0;
				//					can_queue->node[can_queue->Rear].Id = CAN_MesgSentList[i].SentQueue.node[CAN_MesgSentList[i].SentQueue.Front].Id;
				//					can_queue->node[can_queue->Rear].DLC = CAN_MesgSentList[i].SentQueue.node[CAN_MesgSentList[i].SentQueue.Front].DLC;
				//					can_queue->node[can_queue->Rear].InConGrpFlag = CAN_MesgSentList[i].SentQueue.node[CAN_MesgSentList[i].SentQueue.Front].InConGrpFlag;

				//					memcpy(&(can_queue->node[can_queue->Rear].Data[0]),
				//						   &(CAN_MesgSentList[i].SentQueue.node[CAN_MesgSentList[i].SentQueue.Front].Data[0]),
				//						   sizeof(u8) * can_queue->node[can_queue->Rear].DLC);
				//				}
				//				can_queue->Rear = (can_queue->Rear + 1) % (can_queue->Can_sendqueuesize);
				//				CAN_MesgSentList[i].SentQueue.Front = (CAN_MesgSentList[i].SentQueue.Front + 1) % CAN_MesgSentList[i].SentQueue.Can_sendqueuesize;
			}
			/***保护措施***/
			if (CAN_MesgSentList[i].QUEUEFullTimeout > 10 || CAN_MesgSentList[i].SendNumber - CAN_MesgSentList[i].ReceiveNumber > 200)
			{
				Beep_Show(8);
				CAN_MesgSentList[i].SendSem = -10000; //把信号置负，抛弃此节点，产生err
				if (CAN_1 == CAN_x)
				{
					switch (i) //看是哪个子板错误
					{
					case 0:
						sprintf(user.error, "%s", "GyroNodeErr");
						break;
					case 1:
						break;
					case 2:
						sprintf(user.error, "%s", "DT50Nodeerr");
						break;
					case 3:
						break;
					case 4:
						break;
					case 5:
						break;
					case 6:
						break;
					default:
						break;
					}
				}
				else
				{
					switch (i)
					{
					case 0:
						sprintf(user.error, "%s", "elmoA err");
						break;
					case 1:
						break;
					case 2:
						break;
					default:
						break;
					}
				}
			}
		}
	}
}

/**
  * @brief    将正要出队的报文放入对应板子的控制块中
  */

void CANMesgControlList(MesgControlGrp *CAN_MesgSentList, CanSendqueue *can_queue, u8 CAN_x)
{
	volatile u16 ControlListID = 0xFE; //[i]对应的子板节点
	u8 CAN_NodeNumber = 0;

	if (CAN_1 == CAN_x)
	{
		CAN_NodeNumber = CAN1_NodeNumber;
		switch (can_queue->node[can_queue->Front].Id)
		{
		//判断哪个节点,并把它映射到number中
		case ID_GyrScope_TX:
			ControlListID = 0;
			break;
		default:
			break;
		}
		if (ControlListID < CAN_NodeNumber)
		{
			CAN_MesgSentList[ControlListID].SendSem++;
			CAN_MesgSentList[ControlListID].SendNumber++;
			CAN_MesgSentList[ControlListID].TimeOut++;
			if (CAN_MesgSentList[ControlListID].TimeOut > 10)
			{
				queue_flag.Can1_ErrNode |= (1 << ControlListID);
			}
		}
	}
	else
	{
		CAN_NodeNumber = CAN2_NodeNumber;
		switch (can_queue->node[can_queue->Front].Id)
		{
		//哪个节点
		case 0x300: //将elmo广播帧放入报文控制块，
			ControlListID = 0;
			break;
		case 0x301:
			ControlListID = 1;
			break;

		case 0x302:
			ControlListID = 2;
			break;

		case 0x303:
			ControlListID = 3;
			break;

		case 0x304:
			ControlListID = 4;
			break;
		case 0x306: //23转向，，左侧的

			ControlListID = 5;
			break;

		case 0x305: //14转向，，右侧的

			ControlListID = 6;
			break;

		default:
			break;
		}
	}
	if (ControlListID < CAN_NodeNumber) //想要屏蔽某块板子只需再次加上 &&ControlListID ！= X
	{
		if ((CAN_MesgSentList[ControlListID].SentQueue.Rear + 1) % CAN_MesgSentList[ControlListID].SentQueue.Can_sendqueuesize == CAN_MesgSentList[ControlListID].SentQueue.Front)
		{
			if (CAN_1 == CAN_x)
				queue_flag.can1_queue_full++;
			else
				queue_flag.can2_queue_full++;
		}
		else //可以再次屏蔽id
		{
			CAN_MesgSentList[ControlListID].SendSem++;
			CAN_MesgSentList[ControlListID].SendNumber++;
			CAN_MesgSentList[ControlListID].TimeOut++;
			if (CAN_MesgSentList[ControlListID].TimeOut > 100)
			{
				queue_flag.Can2_ErrNode |= (1 << ControlListID);
			}
		}
	}
}

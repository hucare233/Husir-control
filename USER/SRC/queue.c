#include "queue.h"

/** 
  * @brief    CAN队列初始化  
  */
void Can_SendqueueInit()
{
	can1_sendqueue.Can_sendqueuesize = CAN_SENDQUEUESIZE;
	can1_sendqueue.Front = 0;
	can1_sendqueue.Rear = 0;
	FlagPara.Can1_Tx_NoMailBox=0;	
	
	can2_sendqueue.Can_sendqueuesize = CAN_SENDQUEUESIZE;
	can2_sendqueue.Front = 0;
	can2_sendqueue.Rear = 0;
	FlagPara.Can2_Tx_NoMailBox=0;	
}
/** 
  * @brief        CAN出队函数  
  */
void CAN_DeQueue(u8 CAN_x, Can_Sendqueue *can_queue)
{
	CanTxMsg TxMessage;
	u8 CanSendCount = 0;

	if((FlagPara.Mode_Run == BRAKE) && (can_queue->head[can_queue->Front].Data[0] == 'B' \
		&& can_queue->head[can_queue->Front].Data[1] == 'G'))
	{
		can_queue->Front = (can_queue->Front + 1) % can_queue->Can_sendqueuesize; //刹车时不发bg指令，确保电机能锁死。
		return;
	}
	
	if(can_queue->head[can_queue->Front].Id<0x400)//队列报文塞进结构体
	{
		TxMessage.StdId = can_queue->head[can_queue->Front].Id;
		TxMessage.IDE = CAN_ID_STD;
	}
	else
	{
		TxMessage.ExtId = can_queue->head[can_queue->Front].Id;
		TxMessage.IDE = CAN_ID_EXT;
	}
	TxMessage.DLC = can_queue->head[can_queue->Front].DLC;
	TxMessage.RTR = CAN_RTR_DATA;
	memcpy(&TxMessage.Data[0], &(can_queue->head[can_queue->Front].Data[0]), TxMessage.DLC);
	
	if(CAN_x == CAN_2) //电机的报文
	{
		while ((CAN_Transmit(CAN2, &TxMessage)) == CAN_TxStatus_NoMailBox)
		{
			CanSendCount++;
			if (CanSendCount > 10) break;
		}
		if (CanSendCount > 10)
		{
			FlagPara.Can2_Tx_NoMailBox++;
			if(FlagPara.Can2_Tx_NoMailBox>10)
				FlagPara.Warning_Flag|=NO_MAILBOX;
		}
		else
		{
			FlagPara.Can2_Tx_NoMailBox=0;
			FlagPara.Warning_Flag&=~NO_MAILBOX;
			CANMesgControlList(CAN2_MesgSentList, &can2_sendqueue, CAN_2);
			if(can_queue->head[can_queue->Front].WaitFlag==FALSE)
				can_queue->Front = (can_queue->Front + 1) % can_queue->Can_sendqueuesize;
		}
	}
	else if(CAN_x == CAN_1)
	{
		while ((CAN_Transmit(CAN1, &TxMessage)) == CAN_TxStatus_NoMailBox)
		{
			CanSendCount++;
			if (CanSendCount > 10) break;
		}
		if (CanSendCount > 10)
		{
			FlagPara.Can1_Tx_NoMailBox++;
			if(FlagPara.Can1_Tx_NoMailBox>10)
				FlagPara.Warning_Flag|=NO_MAILBOX;
		}
		else
		{
			FlagPara.Can1_Tx_NoMailBox=0;
			FlagPara.Warning_Flag&=~NO_MAILBOX;
			CANMesgControlList(CAN1_MesgSentList, &can1_sendqueue, CAN_1);
			if(can_queue->head[can_queue->Front].WaitFlag==FALSE)
				can_queue->Front = (can_queue->Front + 1) % can_queue->Can_sendqueuesize;
		}
	}
	return;
}
/** 
  * @brief   检测报文
  */
static int num[2]={0};
static u8 half=0;
void Detect()
{
	if(half==CAN_1)
	{
		if(FlagPara.Can1DetectFlag==ENABLE)
		{
			while(num[0]<12)
			{
				if(Det_Pointer[num[0]]>0)
				{
					Func[num[0]/4]((num[0]%4)+1,FALSE);
					break;
				}
				num[0]++;
			}
			if(num[0]>=12)
				num[0]=0;
		}
		half=CAN_2;
	}
	else
	{
		half=CAN_1;
	}
}

#include "cancontrolist.h"

/** 
  * @brief  初始化CAN报文控制表
  */
void InitCANControlList(MesgControlGrp* CAN_MesgSentList , u8 CAN_x)
{
	u8 i = 0;
	u8 CAN_NodeNumber;
	if(CAN_1 == CAN_x)
	{
		CAN_NodeNumber = CAN1_NodeNumber;
		FlagPara.Can1_ErrNode=0;
	}
	else
	{
		CAN_NodeNumber = CAN2_NodeNumber;
		FlagPara.Can2_ErrNode=0;
	}
	
	for(i = 0; i < CAN_NodeNumber; i++)
	{
		CAN_MesgSentList[i].SendSem = 0;
		CAN_MesgSentList[i].TimeOut = 0;
		CAN_MesgSentList[i].QUEUEFullTimeout = 0;
		CAN_MesgSentList[i].ReceiveNumber = 0;
		CAN_MesgSentList[i].SendNumber = 0;
	}
}
/** 
  * @brief   对相应节点信息进行记录并判断
  */ 
void CANMesgControlList(MesgControlGrp* CAN_MesgSentList , Can_Sendqueue* can_queue , u8 CAN_x)
{
	u16 ControlListID=0xFF;
	u8 CAN_NodeNumber;

	if( CAN_1 == CAN_x )
	{
		CAN_NodeNumber = CAN1_NodeNumber;
		switch(can_queue->head[can_queue->Front].Id)
		{
			case 0x00010200:
				ControlListID = 0;
			break;
			default:
			break;
		}
		if(ControlListID<CAN_NodeNumber)
		{
			CAN_MesgSentList[ControlListID].SendSem ++;
			CAN_MesgSentList[ControlListID].SendNumber ++;
			CAN_MesgSentList[ControlListID].TimeOut ++;
			if(CAN_MesgSentList[ControlListID].TimeOut>10)
			{
				FlagPara.Can1_ErrNode|=(1<<ControlListID);
				FlagPara.Error_Flag|=CAN1_NODE_LOSE;
			}
		}
	}
	else if( CAN_2 == CAN_x )
	{
		CAN_NodeNumber = CAN2_NodeNumber;
		switch(can_queue->head[can_queue->Front].Id)	
		{
			case 0x300:
				ControlListID = CAN_NodeNumber + 1;
				break;
			
			case 0x301:
				ControlListID = 0;
				break;
			
			case 0x302:
				ControlListID = 1;
				break;
			
			case 0x303:
				ControlListID = 2;
				break;
			
			case 0x304:
				ControlListID = 3;
				break;

				default:
				break;
		}
		if(ControlListID<CAN_NodeNumber)
		{
			CAN_MesgSentList[ControlListID].SendSem ++;
			CAN_MesgSentList[ControlListID].SendNumber ++;
			CAN_MesgSentList[ControlListID].TimeOut ++;
			if(CAN_MesgSentList[ControlListID].TimeOut>100)
			{
				FlagPara.Can2_ErrNode|=(1<<ControlListID);
				FlagPara.Error_Flag|=CAN2_NODE_LOSE;
			}
		}
	}
}

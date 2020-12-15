#include "motor.h"

/** 
  * @brief  Elmo节点初始化
  */
void Elmo_Ele(u8 waitflag)
{
	if(Rear2==can2_sendqueue.Front)
	{
		FlagPara.Error_Flag|=CAN2_QUEUE_FULL;
		return;
	}
	else
	{
		can2_sendqueue.head[can2_sendqueue.Rear].Id = 0x00;
		can2_sendqueue.head[can2_sendqueue.Rear].DLC =0x02;
		can2_sendqueue.head[can2_sendqueue.Rear].Data[0]=1;
		can2_sendqueue.head[can2_sendqueue.Rear].Data[1]=0;
		can2_sendqueue.head[can2_sendqueue.Rear].WaitFlag=waitflag;
	}
	can2_sendqueue.Rear=Rear2;	
}

/** 
  * @brief  命令语句
  */
void Commend(u8 id,u8 data0,u8 data1,u8 flag,void* data)
{
	if(Rear2==can2_sendqueue.Front)
	{
		FlagPara.Error_Flag|=CAN2_QUEUE_FULL;
		return;
	}
	else
	{
		can2_sendqueue.head[can2_sendqueue.Rear].Id =0x300+id;
		can2_sendqueue.head[can2_sendqueue.Rear].DLC =0x08;
		can2_sendqueue.head[can2_sendqueue.Rear].Data[0]=data0;
		can2_sendqueue.head[can2_sendqueue.Rear].Data[1]=data1;
		can2_sendqueue.head[can2_sendqueue.Rear].Data[2]=0;
		can2_sendqueue.head[can2_sendqueue.Rear].Data[3]=0;
		EncodeFloatData(data, &can2_sendqueue.head[can2_sendqueue.Rear].Data[4]);
		can2_sendqueue.head[can2_sendqueue.Rear].WaitFlag=flag;
	}
	can2_sendqueue.Rear=Rear2;
}

/** 
  * @brief  查询语句
  */
void Query(u8 ID,u8 data0,u8 data1,u8 waitflag)
{
	if(Rear2==can2_sendqueue.Front)
	{
		FlagPara.Error_Flag|=CAN2_QUEUE_FULL;
		return;
	}
	else
	{
		can2_sendqueue.head[can2_sendqueue.Rear].Id =0x300+ID;
		can2_sendqueue.head[can2_sendqueue.Rear].DLC =0x04;
		can2_sendqueue.head[can2_sendqueue.Rear].Data[0]=data0;
		can2_sendqueue.head[can2_sendqueue.Rear].Data[1]=data1;
		can2_sendqueue.head[can2_sendqueue.Rear].Data[2]=0;
		can2_sendqueue.head[can2_sendqueue.Rear].Data[3]=0;
		can2_sendqueue.head[can2_sendqueue.Rear].WaitFlag=waitflag;
	}
	can2_sendqueue.Rear=Rear2;
}

/* 使能/失能Elmo电机 */
void Elmo_Enable(u8 ID,u8 waitflag,float state)
{
	Commend(ID,'M','O',waitflag,&state);
}

/* Elmo运动模式 */
void Elmo_MODE_Select(u8 ID,u8 waitflag,float mode)
{
	Commend(ID,'U','M',waitflag,&mode);
}

/* 重置Elmo绝对位置 */
void Elmo_Reset_position(u8 ID,u8 waitflag,float position)
{
	Commend(ID,'P','X',waitflag,&position);
}

/* 设置Elmo位置模式运动速度 */
void Elmo_PTP_speed(u8 ID,u8 waitflag,float speed)
{
	Commend(ID,'S','P',waitflag,&speed);
}

/* 设置Elmo位置模式运动位置 */
void Elmo_PTP_position(u8 ID,u8 waitflag,float position)
{
	Commend(ID,'P','A',waitflag,&position);
}

/* Elmo开始运动 */
void Elmo_Begin(u8 ID,u8 waitflag)
{
	Query(ID,'B','G',waitflag);
}

/* Elmo停止运动 */
void Elmo_Stop(u8 ID,u8 waitflag)
{
	Query(ID,'S','T',waitflag);
}

/* 查询Elmo绝对位置 */
void Elmo_Query_Position(u8 ID,u8 waitflag)
{
	Query(ID,'P','X',waitflag);
}

/** 
  * @brief  电磁阀
  */
void Solenoid_Valve (u8 waitflag, u8 VL, u8 flag)
{
	if(Rear2==can2_sendqueue.Front)
	{
		FlagPara.Error_Flag|=CAN2_QUEUE_FULL;
		return;
	}
	else
	{
		can2_sendqueue.head[can2_sendqueue.Rear].Id = 0x00030103;
		can2_sendqueue.head[can2_sendqueue.Rear].DLC =0x08;
		can2_sendqueue.head[can2_sendqueue.Rear].Data[0]='V';
		can2_sendqueue.head[can2_sendqueue.Rear].Data[1]='L';
		can2_sendqueue.head[can2_sendqueue.Rear].Data[2]=VL;
		can2_sendqueue.head[can2_sendqueue.Rear].Data[3]=flag;
		can2_sendqueue.head[can2_sendqueue.Rear].Data[4]=0;
		can2_sendqueue.head[can2_sendqueue.Rear].Data[5]=0;
		can2_sendqueue.head[can2_sendqueue.Rear].Data[6]=0;
		can2_sendqueue.head[can2_sendqueue.Rear].Data[7]=0;
		can2_sendqueue.head[can2_sendqueue.Rear].WaitFlag=waitflag;
	}
	can2_sendqueue.Rear=Rear2;	
}



void ENABLE_ALL_MOTOR(void)
{
	Elmo_Enable(0,FALSE,ENABLE);
	
	FlagPara.Mode_Run = ENABLE;
}
/* 速度模式速度 */
void Elmo_C_Speed(u8 ID,u8 waitflag,float speed)
{
	Commend(ID,'J','V',waitflag,&speed);
}

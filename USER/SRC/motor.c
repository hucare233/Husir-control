#include "motor.h"

/** 
  * @brief  Elmo�ڵ��ʼ��
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
  * @brief  �������
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
  * @brief  ��ѯ���
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

/* ʹ��/ʧ��Elmo��� */
void Elmo_Enable(u8 ID,u8 waitflag,float state)
{
	Commend(ID,'M','O',waitflag,&state);
}

/* Elmo�˶�ģʽ */
void Elmo_MODE_Select(u8 ID,u8 waitflag,float mode)
{
	Commend(ID,'U','M',waitflag,&mode);
}

/* ����Elmo����λ�� */
void Elmo_Reset_position(u8 ID,u8 waitflag,float position)
{
	Commend(ID,'P','X',waitflag,&position);
}

/* ����Elmoλ��ģʽ�˶��ٶ� */
void Elmo_PTP_speed(u8 ID,u8 waitflag,float speed)
{
	Commend(ID,'S','P',waitflag,&speed);
}

/* ����Elmoλ��ģʽ�˶�λ�� */
void Elmo_PTP_position(u8 ID,u8 waitflag,float position)
{
	Commend(ID,'P','A',waitflag,&position);
}

/* Elmo��ʼ�˶� */
void Elmo_Begin(u8 ID,u8 waitflag)
{
	Query(ID,'B','G',waitflag);
}

/* Elmoֹͣ�˶� */
void Elmo_Stop(u8 ID,u8 waitflag)
{
	Query(ID,'S','T',waitflag);
}

/* ��ѯElmo����λ�� */
void Elmo_Query_Position(u8 ID,u8 waitflag)
{
	Query(ID,'P','X',waitflag);
}

/** 
  * @brief  ��ŷ�
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
/* �ٶ�ģʽ�ٶ� */
void Elmo_C_Speed(u8 ID,u8 waitflag,float speed)
{
	Commend(ID,'J','V',waitflag,&speed);
}

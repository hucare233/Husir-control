#include "lcd.h"

static char str_Well[10];
static char str_Err[10];
static char str_War[10];
static char str_temp[8];//�������ֱ���������
static u8 order[3]={0};
static u8 half[3]={0};
void UsartLCDshow()
{
  u8 i = 0;//���ڴ������ݰ����±�
	FlagPara.Usart2DmaSendFinish=FALSE;
	/**
  *@brief ������
	*/
	/* ��ʾ���Ӽ�״̬ */
	USART.TxBuffer_UASRT2[i++]=0xee;
	USART.TxBuffer_UASRT2[i++]=0xb1;
	USART.TxBuffer_UASRT2[i++]=0x12;
	USART.TxBuffer_UASRT2[i++]=0;
	USART.TxBuffer_UASRT2[i++]=0;//ҳ��ID
	USART.TxBuffer_UASRT2[i++]=0;
	USART.TxBuffer_UASRT2[i++]=1;//����ID
	USART.TxBuffer_UASRT2[i++]=0x00;
	
	if(order[0]<4) order[0]++;
		else order[0]=1;
	switch (FlagPara.Status_Controller)
	{
		case WELL:
		{
			switch(order[0])
			{
				case 1:
					sprintf(str_Well,"%s","WELL");
				break;
				
				case 2:
					sprintf(str_Well,"%s","WELL - ");
				break;
				
				case 3:
					sprintf(str_Well,"%s","WELL -- ");
				break;
				
				case 4:
					sprintf(str_Well,"%s","WELL --- ");
				break;
				
				default:
					break;
			}
			break;
		}
		case ERR_Status:
		{
			switch(order[0])
			{
				case 1:
					sprintf(str_Well,"%s","ERR ");
				break;
				
				case 2:
					sprintf(str_Well,"%s","ERR  - ");
				break;
				
				case 3:
					sprintf(str_Well,"%s","ERR  -- ");
				break;
				
				case 4:
					sprintf(str_Well,"%s","ERR  --- ");
				break;
				
				default:
					break;
			}
			break;
		}
		case WAR_Status:
		{
			switch(order[0])
			{
				case 1:
					sprintf(str_Well,"%s","WAR ");
				break;
				
				case 2:
					sprintf(str_Well,"%s","WAR  - ");
				break;
				
				case 3:
					sprintf(str_Well,"%s","WAR  -- ");
				break;
				
				case 4:
					sprintf(str_Well,"%s","WAR  --- ");
				break;
				
				default:
					break;
			}
			break;
		}
	}
	
	USART.TxBuffer_UASRT2[i++]=strlen(str_Well);
	strcpy((char*)(&USART.TxBuffer_UASRT2[i]),str_Well);
	i+=strlen(str_Well);
	
	USART.TxBuffer_UASRT2[i++]=0xff;
	USART.TxBuffer_UASRT2[i++]=0xfc;	
	USART.TxBuffer_UASRT2[i++]=0xff;
	USART.TxBuffer_UASRT2[i++]=0xff;
	
	/* ��ʾ״̬�� */
	USART.TxBuffer_UASRT2[i++]=0xee;
	USART.TxBuffer_UASRT2[i++]=0xb1;
	USART.TxBuffer_UASRT2[i++]=0x12;
	USART.TxBuffer_UASRT2[i++]=0;
	USART.TxBuffer_UASRT2[i++]=0;//ҳ��ID
	USART.TxBuffer_UASRT2[i++]=0;
	
	if(half[0]==0)
	{
		USART.TxBuffer_UASRT2[i++]=2;//����ID
		USART.TxBuffer_UASRT2[i++]=0;	
		sprintf(str_temp,"%#X\n",FlagPara.Error_Flag);
		half[0]++;
	}
	else 
	{
		USART.TxBuffer_UASRT2[i++]=3;//����ID
		USART.TxBuffer_UASRT2[i++]=0;	
		sprintf(str_temp,"%#X\n",FlagPara.Warning_Flag);
		half[0]--;
	}
	
	USART.TxBuffer_UASRT2[i++]=strlen(str_temp);
	strcpy((char*)(&USART.TxBuffer_UASRT2[i]),str_temp);
	i+=strlen(str_temp);
	
	USART.TxBuffer_UASRT2[i++]=0xff;
	USART.TxBuffer_UASRT2[i++]=0xfc;	
	USART.TxBuffer_UASRT2[i++]=0xff;
	USART.TxBuffer_UASRT2[i++]=0xff;
	
	/* ��ʾWAR��ϸ״̬ */
	USART.TxBuffer_UASRT2[i++]=0xee;
	USART.TxBuffer_UASRT2[i++]=0xb1;
	USART.TxBuffer_UASRT2[i++]=0x12;
	USART.TxBuffer_UASRT2[i++]=0;
	USART.TxBuffer_UASRT2[i++]=0;//ҳ��ID
	USART.TxBuffer_UASRT2[i++]=0;
	USART.TxBuffer_UASRT2[i++]=9;//����ID
	USART.TxBuffer_UASRT2[i++]=0;	
	
	if(FlagPara.Warning_Flag>0)
	{
		if( order[2]<10 && (FlagPara.Warning_Flag&NO_MAILBOX)>0)
		{
			sprintf(str_War,"%s","NO_MAILBOX");
		}
		else if( order[2]<20 && order[2]>=10 && (FlagPara.Warning_Flag&MOTOR_WAR)>0)
		{
			sprintf(str_War,"%s","MOTOR_WAR");
		}
		order[2]=(order[2]+1)%20;
	}
	else
		sprintf(str_War,"%s","o(��_��)o");
	
	
	USART.TxBuffer_UASRT2[i++]=strlen(str_War);
	strcpy((char*)(&USART.TxBuffer_UASRT2[i]),str_War);
	i+=strlen(str_War);
	
	USART.TxBuffer_UASRT2[i++]=0xff;
	USART.TxBuffer_UASRT2[i++]=0xfc;	
	USART.TxBuffer_UASRT2[i++]=0xff;
	USART.TxBuffer_UASRT2[i++]=0xff;
	
	/* ��ʾERR��ϸ״̬ */
	USART.TxBuffer_UASRT2[i++]=0xee;
	USART.TxBuffer_UASRT2[i++]=0xb1;
	USART.TxBuffer_UASRT2[i++]=0x12;
	USART.TxBuffer_UASRT2[i++]=0;
	USART.TxBuffer_UASRT2[i++]=0;//ҳ��ID
	USART.TxBuffer_UASRT2[i++]=0;
	USART.TxBuffer_UASRT2[i++]=8;//����ID
	USART.TxBuffer_UASRT2[i++]=0;	
	
	if(FlagPara.Error_Flag>0)
	{
		if( order[1]<10 && (FlagPara.Error_Flag&CAN1_QUEUE_FULL)>0)
		{
			sprintf(str_Err,"%s","QUEUE_FU1");
		}
		else if( order[1]<20 && order[1]>=10 && (FlagPara.Error_Flag&CAN2_QUEUE_FULL)>0)
		{
			sprintf(str_Err,"%s","QUEUE_FU2");
		}
		else if( order[1]<30 && order[1]>=20 && (FlagPara.Error_Flag&CAN1_NODE_LOSE)>0)
		{
			sprintf(str_Err,"%s","NODE_LOS1");
		}
		else if( order[1]<40 && order[1]>=30 && (FlagPara.Error_Flag&CAN2_NODE_LOSE)>0)
		{
			sprintf(str_Err,"%s","NODE_LOS2");
		}
		else if( order[1]<50 && order[1]>=40 && (FlagPara.Error_Flag&OFF_TRACK)>0)
		{
			sprintf(str_Err,"%s","OFF_TRACK");
		}
		else if( order[1]<60 && order[1]>=50 && (FlagPara.Error_Flag&MOTOR_ERR)>0)
		{
			sprintf(str_Err,"%s","MOTOR_ERR");
		}
		order[1]=(order[1]+1)%60;
	}
	else
		sprintf(str_Err,"%s","o(��_��)o");
	
	USART.TxBuffer_UASRT2[i++]=strlen(str_Err);
	strcpy((char*)(&USART.TxBuffer_UASRT2[i]),str_Err);
	i+=strlen(str_Err);
	
	USART.TxBuffer_UASRT2[i++]=0xff;
	USART.TxBuffer_UASRT2[i++]=0xfc;	
	USART.TxBuffer_UASRT2[i++]=0xff;
	USART.TxBuffer_UASRT2[i++]=0xff;
	
	/* ��ʾ�ڵ�״̬ */
	USART.TxBuffer_UASRT2[i++]=0xee;
	USART.TxBuffer_UASRT2[i++]=0xb1;
	USART.TxBuffer_UASRT2[i++]=0x12;
	USART.TxBuffer_UASRT2[i++]=0;
	USART.TxBuffer_UASRT2[i++]=0;//ҳ��ID
	USART.TxBuffer_UASRT2[i++]=0;
	
	if(half[1]==0)
	{
		USART.TxBuffer_UASRT2[i++]=4;//����ID
		USART.TxBuffer_UASRT2[i++]=0;	
		sprintf(str_temp,"%#X\n",FlagPara.Can1_ErrNode);
		half[1]++;
	}
	else 
	{
		USART.TxBuffer_UASRT2[i++]=5;//����ID
		USART.TxBuffer_UASRT2[i++]=0;	
		sprintf(str_temp,"%#X\n",FlagPara.Can2_ErrNode);
		half[1]--;
	}
	
	USART.TxBuffer_UASRT2[i++]=strlen(str_temp);
	strcpy((char*)(&USART.TxBuffer_UASRT2[i]),str_temp);
	i+=strlen(str_temp);
	
	USART.TxBuffer_UASRT2[i++]=0xff;
	USART.TxBuffer_UASRT2[i++]=0xfc;	
	USART.TxBuffer_UASRT2[i++]=0xff;
	USART.TxBuffer_UASRT2[i++]=0xff;
	/**
  *@brief ������������
	*/
	/* x */
	USART.TxBuffer_UASRT2[i++]=0xee;
	USART.TxBuffer_UASRT2[i++]=0xb1;
	USART.TxBuffer_UASRT2[i++]=0x12;
	USART.TxBuffer_UASRT2[i++]=0;
	USART.TxBuffer_UASRT2[i++]=1;//ҳ��ID
	USART.TxBuffer_UASRT2[i++]=0;
	USART.TxBuffer_UASRT2[i++]=1;//����ID
	USART.TxBuffer_UASRT2[i++]=0;	
		
	sprintf(str_temp,"%4.2f",(float)position_now.x);

	USART.TxBuffer_UASRT2[i++]=strlen(str_temp);
	strcpy((char*)(&USART.TxBuffer_UASRT2[i]),str_temp);
	i+=strlen(str_temp);
	
	USART.TxBuffer_UASRT2[i++]=0xff;
	USART.TxBuffer_UASRT2[i++]=0xfc;	
	USART.TxBuffer_UASRT2[i++]=0xff;
	USART.TxBuffer_UASRT2[i++]=0xff;
	/* y */
	USART.TxBuffer_UASRT2[i++]=0xee;
	USART.TxBuffer_UASRT2[i++]=0xb1;
	USART.TxBuffer_UASRT2[i++]=0x12;
	USART.TxBuffer_UASRT2[i++]=0;
	USART.TxBuffer_UASRT2[i++]=1;//ҳ��ID
	USART.TxBuffer_UASRT2[i++]=0;
	USART.TxBuffer_UASRT2[i++]=2;//����ID
	USART.TxBuffer_UASRT2[i++]=0;	
		
	sprintf(str_temp,"%4.2f",(float)position_now.y);

	USART.TxBuffer_UASRT2[i++]=strlen(str_temp);
	strcpy((char*)(&USART.TxBuffer_UASRT2[i]),str_temp);
	i+=strlen(str_temp);
	
	USART.TxBuffer_UASRT2[i++]=0xff;
	USART.TxBuffer_UASRT2[i++]=0xfc;	
	USART.TxBuffer_UASRT2[i++]=0xff;
	USART.TxBuffer_UASRT2[i++]=0xff;
	
	/* angle */
	USART.TxBuffer_UASRT2[i++]=0xee;
	USART.TxBuffer_UASRT2[i++]=0xb1;
	USART.TxBuffer_UASRT2[i++]=0x12;
	USART.TxBuffer_UASRT2[i++]=0;
	USART.TxBuffer_UASRT2[i++]=1;//ҳ��ID
	USART.TxBuffer_UASRT2[i++]=0;
	USART.TxBuffer_UASRT2[i++]=3;//����ID
	USART.TxBuffer_UASRT2[i++]=0;	
		
	sprintf(str_temp,"%4.2f",(float)position_now.angle);

	USART.TxBuffer_UASRT2[i++]=strlen(str_temp);
	strcpy((char*)(&USART.TxBuffer_UASRT2[i]),str_temp);
	i+=strlen(str_temp);
	
	USART.TxBuffer_UASRT2[i++]=0xff;
	USART.TxBuffer_UASRT2[i++]=0xfc;	
	USART.TxBuffer_UASRT2[i++]=0xff;
	USART.TxBuffer_UASRT2[i++]=0xff;
	
	/* vx */
	USART.TxBuffer_UASRT2[i++]=0xee;
	USART.TxBuffer_UASRT2[i++]=0xb1;
	USART.TxBuffer_UASRT2[i++]=0x12;
	USART.TxBuffer_UASRT2[i++]=0;
	USART.TxBuffer_UASRT2[i++]=1;//ҳ��ID
	USART.TxBuffer_UASRT2[i++]=0;
	USART.TxBuffer_UASRT2[i++]=4;//����ID
	USART.TxBuffer_UASRT2[i++]=0;	
		
	sprintf(str_temp,"%4.2f",(float)velocity_now.Vx);

	USART.TxBuffer_UASRT2[i++]=strlen(str_temp);
	strcpy((char*)(&USART.TxBuffer_UASRT2[i]),str_temp);
	i+=strlen(str_temp);
	
	USART.TxBuffer_UASRT2[i++]=0xff;
	USART.TxBuffer_UASRT2[i++]=0xfc;	
	USART.TxBuffer_UASRT2[i++]=0xff;
	USART.TxBuffer_UASRT2[i++]=0xff;
	
	/* vy */
	USART.TxBuffer_UASRT2[i++]=0xee;
	USART.TxBuffer_UASRT2[i++]=0xb1;
	USART.TxBuffer_UASRT2[i++]=0x12;
	USART.TxBuffer_UASRT2[i++]=0;
	USART.TxBuffer_UASRT2[i++]=1;//ҳ��ID
	USART.TxBuffer_UASRT2[i++]=0;
	USART.TxBuffer_UASRT2[i++]=5;//����ID
	USART.TxBuffer_UASRT2[i++]=0;	
		
	sprintf(str_temp,"%4.2f",(float)velocity_now.Vy);

	USART.TxBuffer_UASRT2[i++]=strlen(str_temp);
	strcpy((char*)(&USART.TxBuffer_UASRT2[i]),str_temp);
	i+=strlen(str_temp);
	
	USART.TxBuffer_UASRT2[i++]=0xff;
	USART.TxBuffer_UASRT2[i++]=0xfc;	
	USART.TxBuffer_UASRT2[i++]=0xff;
	USART.TxBuffer_UASRT2[i++]=0xff;
	
	/* vabs */
	USART.TxBuffer_UASRT2[i++]=0xee;
	USART.TxBuffer_UASRT2[i++]=0xb1;
	USART.TxBuffer_UASRT2[i++]=0x12;
	USART.TxBuffer_UASRT2[i++]=0;
	USART.TxBuffer_UASRT2[i++]=1;//ҳ��ID
	USART.TxBuffer_UASRT2[i++]=0;
	USART.TxBuffer_UASRT2[i++]=6;//����ID
	USART.TxBuffer_UASRT2[i++]=0;	
		
	sprintf(str_temp,"%4.2f",(float)velocity_now.V);

	USART.TxBuffer_UASRT2[i++]=strlen(str_temp);
	strcpy((char*)(&USART.TxBuffer_UASRT2[i]),str_temp);
	i+=strlen(str_temp);
	
	USART.TxBuffer_UASRT2[i++]=0xff;
	USART.TxBuffer_UASRT2[i++]=0xfc;	
	USART.TxBuffer_UASRT2[i++]=0xff;
	USART.TxBuffer_UASRT2[i++]=0xff;
	
	
	DMA_SetCurrDataCounter(DMA1_Stream6,i);
	DMA_Cmd(DMA1_Stream6, ENABLE);
}

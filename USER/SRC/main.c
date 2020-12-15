#include "main.h"
int main(void)
{
	SystemInit();
  NVIC_SetPriorityGrouping(NVIC_PriorityGroup_3);	
	
	LED_Configuration();
	Beep_Configuration();	
	Led8_Configuration();
	KEY_Configuration();
	
	/* ͨ�� */
	CAN1_Configuration();//CAN1��ʼ��������CAN2��ʼ��֮ǰ�������ܳ�ʼ��
	CAN2_Configuration();
	Can_SendqueueInit();
	USART1_Configuration();//��������ʾ����
	USART2_Configuration();//���ڴ�����ʾ���ֱ�	
	USART3_Configuration();
	USART4_Configuration();//��������ͷ
	USART5_Configuration();//����˫��ͨ��
	InitCANControlList(CAN1_MesgSentList, CAN_1);
	InitCANControlList(CAN2_MesgSentList, CAN_2);
	
  OSInit();
	OSTaskCreate(TaskStart, (void *)0, &start_task_stk[START_TASK_STK_SIZE - 1], START_TASK_PRIO);
	OSStart();	//���治�ܼ��κ����
	return 0;
}
/* ��ʼ���� */
Velocity spe={0};
static void TaskStart(void *pdata)
{
	pdata=pdata;
	OS_CPU_SysTickInit();

	TIM3_Configuration();
	
	//�����ź���������
	Motor_Detect = OSFlagCreate(0x0000, Err_MotorDetect);
	
	ParaInit();
	
	OSTaskCreate(TaskRun, (void *)0, &task_run_stk[TASK_RUN_STK_SIZE - 1], TASK_RUN_PRIO);
	OSTaskCreate(TaskStatus, (void *)0, &task_status_stk[TASK_STATUS_STK_SIZE - 1], TASK_STATUS_PRIO);
	OSTaskCreate(TaskBeep, (void *)0, &task_beep_stk[TASK_BEEP_STK_SIZE - 1], TASK_BEEP_PRIO);
	OSTaskCreate(TaskScope, (void *)0, &task_Scope_stk[TASK_SCOPE_STK_SIZE - 1 ], TASK_SCOPE_PRIO);
	OSTaskCreate(TaskLED, (void *)0, &task_led_stk[TASK_LED_STK_SIZE - 1], TASK_LED_PRIO);
	
	Led8DisData(5);
	
	for(;;)//���CAN·
	{
		;
		OSTimeDly(10000);
		if(FlagPara.Task_Begin_Flag==TRUE)
			break;
	}
	for (;;)
	{
		switch(MovePara.direction_flag)
		{
			case 0:
			{
				Pid_Act();
				break;
			}
			case 1://x������
			{
				spe.Vx=MovePara.v_want;
				spe.Vy=0;
				calculate_wheel_speed(spe);
				OSTimeDly(MovePara.time);
				MovePara.direction_flag=0;
				FlagPara.Mode_Run = BRAKE;	
				
				break;
			}	
			case 2://y������
			{
				spe.Vx=0;
				spe.Vy=MovePara.v_want;
				calculate_wheel_speed(spe);
				OSTimeDly(MovePara.time);
				MovePara.direction_flag=0;
				FlagPara.Mode_Run = BRAKE;	
				break;
			}	
			case 3://x������
			{
				spe.Vx=-MovePara.v_want;
				spe.Vy=0;
				calculate_wheel_speed(spe);
				OSTimeDly(MovePara.time);
				MovePara.direction_flag=0;
				FlagPara.Mode_Run = BRAKE;	
				break;
			}	
			case 4://y������
			{
				spe.Vx=0;
				spe.Vy=-MovePara.v_want;
				calculate_wheel_speed(spe);
				OSTimeDly(MovePara.time);
				MovePara.direction_flag=0;
				FlagPara.Mode_Run = BRAKE;	
				break;
			}	
			default:
			break;
		}
		OSTimeDly(97); //TODO:�о�һ�µ��׸�����ʱ�䣬
	}
}



/* Run */
static void TaskRun(void *pdata)
{
	for(;;)
	{
		switch(MovePara.line_flag)
		{
			case 1:
			{
				RunPoint(MovePara.x_want, MovePara.y_want, MovePara.angle_want,
								 0, MovePara.v_want, 0,
					       50, 50,
								 20, 70,
					       FALSE);
				MovePara.line_flag = 0;
				break;
			}
			default:
			break;
		}
		OSTimeDly(97);
	}
}

/* Status */
//static u8 Status_Task=0;
static void TaskStatus(void *pdata)
{
	for(;;)
	{
		if(FlagPara.Error_Flag>0)
		{
			FlagPara.Status_Controller=ERR_Status;
			//��������ʱ������Ҫ����
//			OSTaskSuspend(TASK_RUN_PRIO);
//			Status_Task|=BALL_INDEX;
//			Emergency();
		}
		else if(FlagPara.Warning_Flag>0)
			FlagPara.Status_Controller=WAR_Status;
		else
		{
			FlagPara.Status_Controller=WELL;
//			if((Status_Task&BALL_INDEX)==1)
//				OSTaskResume(TASK_RUN_PRIO);
		}
		OSTimeDly(5);
	}
}

/* Beep */
static void TaskBeep(void *pdata)
{
	int i;
	for(;;)
	{
		if(FlagPara.Status_Controller==ERR_Status)
//			BEEP_ON;
		;
		else if(FlagPara.Status_Controller==WAR_Status)
		{
//			BEEP_ON;
			OSTimeDly(200);
			BEEP_OFF;
			OSTimeDly(200);
		}
		else if(beep>0)
		{
			i=beep;
			beep=0;
			while(i>0)
			{
//				BEEP_ON;
				OSTimeDly(50);
				BEEP_OFF;
				i--;
			}
		}
		OSTimeDly(5);
	}
}

/* Scope */
static int time=1000;
static void TaskScope(void *pdata)
{
	for(;;)
	{
		if(FlagPara.Usart2DmaSendFinish==TRUE)
		{
			time--;
			UsartLCDshow();//��һ��10ms����
		}
		else
			time++;
		LED_GREEN_TOGGLE;
		OSTimeDly(time);
	}
}

/* LED */
static void TaskLED(void *pdata)
{
	for(;;)
	{
		switch (FlagPara.Mode_Red_Blue)
		{
			case UNSELECTED:
			{
				LED_RED_TOGGLE;
				LED_BLUE_TOGGLE;
				break;
			}
			case RED:
			{
				LED_RED_TOGGLE;
				LED_BLUE_OFF;
				break;
			}
			case BLUE:
			{
				LED_RED_OFF;
				LED_BLUE_TOGGLE;
				break;
			}
		}
		if(FlagPara.Mode_Auto==ENABLE)
			LED_YELLOW_TOGGLE;
		if(FlagPara.Error_Flag>0)
			LED_RED_ON;
		else if(FlagPara.Warning_Flag>0)
			LED_YELLOW_ON;
		else if(FlagPara.Warning_Flag==0)
			LED_YELLOW_OFF;

		OSTimeDly(2500);
	}
}

void Trigger(u8 ind,float Data,u8 type)
{
	Det_Para_Pointer[ind]=Data;//����ֵ
	Det_Pointer[ind]=type;//��������
	FlagPara.Can1DetectFlag=ENABLE;//����������
	OSFlagPend(Motor_Detect, (1<<ind), OS_FLAG_WAIT_SET_ALL|OS_FLAG_CONSUME, 0, Err_MotorDetect);
	if(FlagPara.Mode_Auto==ENABLE)
		FlagPara.Can1DetectFlag=DISABLE;//�رռ�����
}
void Emergency(void)
{
	;
	OSTimeDly(1000);
}

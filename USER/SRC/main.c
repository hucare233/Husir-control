#include "main.h"
int main(void)
{
	SystemInit();
  NVIC_SetPriorityGrouping(NVIC_PriorityGroup_3);	
	
	LED_Configuration();
	Beep_Configuration();	
	Led8_Configuration();
	KEY_Configuration();
	
	/* 通信 */
	CAN1_Configuration();//CAN1初始化必须在CAN2初始化之前，否则不能初始化
	CAN2_Configuration();
	Can_SendqueueInit();
	USART1_Configuration();//用于虚拟示波器
	USART2_Configuration();//用于串口显示屏手柄	
	USART3_Configuration();
	USART4_Configuration();//用于摄像头
	USART5_Configuration();//用于双车通信
	InitCANControlList(CAN1_MesgSentList, CAN_1);
	InitCANControlList(CAN2_MesgSentList, CAN_2);
	
  OSInit();
	OSTaskCreate(TaskStart, (void *)0, &start_task_stk[START_TASK_STK_SIZE - 1], START_TASK_PRIO);
	OSStart();	//后面不能加任何语句
	return 0;
}
/* 起始任务 */
Velocity spe={0};
static void TaskStart(void *pdata)
{
	pdata=pdata;
	OS_CPU_SysTickInit();

	TIM3_Configuration();
	
	//创建信号量（集）
	Motor_Detect = OSFlagCreate(0x0000, Err_MotorDetect);
	
	ParaInit();
	
	OSTaskCreate(TaskRun, (void *)0, &task_run_stk[TASK_RUN_STK_SIZE - 1], TASK_RUN_PRIO);
	OSTaskCreate(TaskStatus, (void *)0, &task_status_stk[TASK_STATUS_STK_SIZE - 1], TASK_STATUS_PRIO);
	OSTaskCreate(TaskBeep, (void *)0, &task_beep_stk[TASK_BEEP_STK_SIZE - 1], TASK_BEEP_PRIO);
	OSTaskCreate(TaskScope, (void *)0, &task_Scope_stk[TASK_SCOPE_STK_SIZE - 1 ], TASK_SCOPE_PRIO);
	OSTaskCreate(TaskLED, (void *)0, &task_led_stk[TASK_LED_STK_SIZE - 1], TASK_LED_PRIO);
	
	Led8DisData(5);
	
	for(;;)//检测CAN路
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
			case 1://x正方向
			{
				spe.Vx=MovePara.v_want;
				spe.Vy=0;
				calculate_wheel_speed(spe);
				OSTimeDly(MovePara.time);
				MovePara.direction_flag=0;
				FlagPara.Mode_Run = BRAKE;	
				
				break;
			}	
			case 2://y正方向
			{
				spe.Vx=0;
				spe.Vy=MovePara.v_want;
				calculate_wheel_speed(spe);
				OSTimeDly(MovePara.time);
				MovePara.direction_flag=0;
				FlagPara.Mode_Run = BRAKE;	
				break;
			}	
			case 3://x负方向
			{
				spe.Vx=-MovePara.v_want;
				spe.Vy=0;
				calculate_wheel_speed(spe);
				OSTimeDly(MovePara.time);
				MovePara.direction_flag=0;
				FlagPara.Mode_Run = BRAKE;	
				break;
			}	
			case 4://y负方向
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
		OSTimeDly(97); //TODO:研究一下到底给多少时间，
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
			//出现问题时节制主要任务
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
			UsartLCDshow();//发一次10ms左右
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
	Det_Para_Pointer[ind]=Data;//触发值
	Det_Pointer[ind]=type;//触发类型
	FlagPara.Can1DetectFlag=ENABLE;//开启检测队列
	OSFlagPend(Motor_Detect, (1<<ind), OS_FLAG_WAIT_SET_ALL|OS_FLAG_CONSUME, 0, Err_MotorDetect);
	if(FlagPara.Mode_Auto==ENABLE)
		FlagPara.Can1DetectFlag=DISABLE;//关闭检测队列
}
void Emergency(void)
{
	;
	OSTimeDly(1000);
}

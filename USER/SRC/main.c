/*
 * @Descripttion: 主函数
 * @version: 第一版
 * @Author: 叮咚蛋
 * @Date: 2020-12-13 15:19:59
 * @LastEditors: 叮咚蛋
 * @LastEditTime: 2020-12-16 11:13:16
 * @FilePath: \hu_sir-contorl\USER\SRC\main.c
 */
#include "main.h"

float points_zero[3] = {0.0f, 0.0f, 0.0f};		   //point of power_on
float points_run_start[3] = {-10.0f, 10.0f, 0.0f}; //point of run_start

/* points_bezier_x[8] = {x0, y0, x1, y1,
						 x2, y2, x3, y3} */
//poin	of the first bezier
float points_bezier_1[8] = {-10.0f + 0.00f, 10.0f,
							-10 - 23.3524f, 10 + 10.0000f,
							-10 - 45.7909f, 10 + 24.0795f,
							-10 - 63.5019f, 10 + 39.8475f};
float points_bezier_2[8] = {-10 - 63.5019f, 10 + 39.8475f,
							-10 - 98.9240f, 10 + 88.9240f,
							-10 - 91.3770f, 10 + 128.706f,
							-10 - 49.0000f, 10 + 150.000f};
float points_bezier_3[8] = {-10 - 49.0000f, 10 + 150.000f,
							-10 - 0.33330f, 10 + 174.789f,
							-10 - 0.33330f, 10 + 245.211f,
							-10 - 49.0000f, 10 + 270.000f};
float points_bezier_4[8] = {-10 - 49.0000f, 10 + 270.000f,
							-10 - 98.3330f, 10 + 294.789f,
							-10 - 98.3330f, 10 + 365.211f,
							-10 - 49.0000f, 10 + 390.000f};
float points_bezier_5[8] = {-10 - 49.0000f, 10 + 390.000f,
							-10 - 0.33330f, 10 + 414.789f,
							-10 - 0.33330f, 10 + 485.210f,
							-10 - 49.0000f, 10 + 510.000f};
float points_bezier_6[8] = {-10 - 49.0000f, 10 + 510.000f,
							-10 - 98.3330f, 10 + 534.789f,
							-10 - 98.3330f, 10 + 605.211f,
							-10 - 49.0000f, 10 + 630.000f};

int main(void)
{
	SystemInit();
	NVIC_SetPriorityGrouping(NVIC_PriorityGroup_3);
	para_init();

	LED_Configuration();
	Beep_Configuration();
	Led8_Configuration();
	KEY_Configuration();

	/* 通信 */
	Can1_Configuration(1, 0);
	Can2_Configuration(2, 0);
	Can_SendqueueInit();
	USART1_Configuration(); //用于虚拟示波器
	USART2_Configuration(); //用于串口显示屏手柄
	USART3_Configuration();
	USART4_Configuration(); //用于摄像头
	USART5_Configuration(); //用于双车通信
	InitCANControlList(CAN1_MesgSentList, CAN_1);
	InitCANControlList(CAN2_MesgSentList, CAN_2);

	OSInit();
	OSTaskCreate(TaskStart, (void *)0, &start_task_stk[START_TASK_STK_SIZE - 1], START_TASK_PRIO);
	OSStart(); //后面不能加任何语句
	return 0;
}

static void TaskStart(void *pdata)
{
	pdata = pdata;

	OS_CPU_SysTickInit();

	OS_ENTER_CRITICAL();
	/**********信号*********/
	RUN = OSSemCreate(0);
	FlagCan1Check = OSFlagCreate(0x0000, ErrorCan1);
	FlagCan2Check = OSFlagCreate(0x0000, ErrorCan2);

	Elmo_Ele(0);	  // 启动can通信协议, 这个指令必须在应急指令发出之后发送才有效，否则会开启协议失败，只要保证上强再上就不会发生这情况
					  ///
	Can1selfcheck(0); //流程号复位，要多次执行？
	Can1selfcheck(0);
	Elmo_GetVX(0); //TODO:can2自检，封装一个函数，协议改一改，自检写个循环，多发几次

	OSFlagPend(FlagCan1Check, 0x0000, OS_FLAG_WAIT_SET_ALL, 500, ErrorCan1);
	OSFlagPend(FlagCan2Check, 0x0000, OS_FLAG_WAIT_SET_ALL, 500, ErrorCan2);

	//	OSStatInit();	//初始化统计任务,不要开，会死机。没啥用的东
	/*******创建任务*********/

	OSTaskCreate(TaskLED, (void *)0, &led_task_stk[LED_TASK_STK_SIZE - 1], LED_TASK_PRIO);
	OSTaskCreate(TaskLCD, (void *)0, &lcd_task_stk[LCD_TASK_STK_SIZE - 1], LCD_TASK_PRIO);
	OSTaskCreate(TaskRUN, (void *)0, &run_task_stk[RUN_TASK_STK_SIZE - 1], RUN_TASK_PRIO);
	OSTaskCreate(TaskBEEP, (void *)0, &beep_task_stk[BEEP_TASK_STK_SIZE - 1], BEEP_TASK_PRIO);
	// OSTaskCreate(TaskMECHANISM, (void *)0, &mechanism_task_stk[MECHANISM_TASK_STK_SIZE - 1], MECHANISM_TASK_PRIO);
	OSTaskCreate(TaskGyroscope, (void *)0, &gyroscope_task_stk[GYROSCOPE_TASK_STK_SIZE - 1], GYROSCOPE_TASK_PRIO);
	//	OSTaskCreate(TaskDT50, (void *)0, &dt50_task_stk[DT50_TASK_STK_SIZE- 1],DT50_TASK_PRIO);

	OS_EXIT_CRITICAL();

	TIM3_Configuration();
	/************************初始化完毕*****************************/

	Beep_Start(); // can be a piece of music if you want
	OSTimeDly(2000);

	for (;;)
	{
		pid_act();
		OSTimeDly(97); //TODO:研究一下到底给多少时间，
	}
}

static void TaskRUN(void *pdata)
{
	pdata = pdata;

	DISABLE_ALL_MOTOR;
	turn_motor_init(); // 底盘转向电机参数初始化

	OSTimeDly(10000);
	Beep_Show(2); // 2声提示，可以摆车了。

	SemPend(RUN); // 等待锁车信号

	ENABLE_ALL_MOTOR;
	BREAK_ALL_MOTOR; // 加上break_motor，确保电机不会发生任何动作,靠enable其实效果一样，都可以锁住车，但是为了防止一手意外赋值速度，直接break
	ResetPosition_EN(0, 0, 0.0f, 1);

	for (;;)
	{
		if ('W' != user.error[0] && 'e' != user.error[1])
		{
			BEEP_ON;
			OSTimeDly(10000);
			BEEP_OFF;
			OSTimeDly(10000);
		}
		else
		{
			Beep_Show(3); // 准备跑车了
			break;
		}
	}

re_user_set:
	switch ((u8)user.identity)
	{
	case Master: //主控
	{
		SemPend(RUN);

		/************底盘流程**************/
	re_master:
		RunPoint(0, 200, 90,
				 20, 50, 0,
				 20, 30, 20, 70,
				 True);
		RunPoint(-200, 200, 180,
				 20, 50, 0,
				 20, 30, 20, 70,
				 True);
		RunPoint(-50, 50, 0,
				 20, 50, 0,
				 20, 30, 20, 70,
				 True);

		Rotate(360, 30, False);
		ResetPosition_EN(-50, 50, 0, 1); // 角度归零

		SemPend(RUN);
		RunPoint(points_run_start[0], points_run_start[1], points_run_start[2],
				 20, 20, 0,
				 0, 10, 0, 0,
				 False);

		SemPend(RUN);
		RunBezier(points_bezier_1, 0.0f,
				  20, 50, 50,
				  0.3f, 0.0f, 0.0f, 0.0f,
				  True, True);
		RunBezier(points_bezier_2, 0.0f,
				  50, 50, 50,
				  0.0f, 0.0f, 0.0f, 0.0f,
				  True, True);
		RunBezier(points_bezier_3, 0.0f,
				  50, 50, 50,
				  0.0f, 0.0f, 0.0f, 0.0f,
				  True, True);
		RunBezier(points_bezier_4, 0.0f,
				  50, 50, 50,
				  0.0f, 0.0f, 0.0f, 0.0f,
				  True, True);
		RunBezier(points_bezier_5, 0.0f,
				  50, 50, 50,
				  0.0f, 0.0f, 0.0f, 0.0f,
				  True, True);
		RunBezier(points_bezier_6, 0.0f,
				  50, 50, 0,
				  0.0f, 0.3f, 0.0f, 0.0f,
				  False, True);

		SemPend(RUN);
		RunPoint(points_zero[0], points_zero[1], points_zero[2],
				 20, 50, 0,
				 20, 30, 20, 70,
				 False);

		goto re_master;
	}
	case Mechanism: //机构组
	{
	re_mech:
		SemPend(RUN);
		for (;;)
		{
			// OSSemPend(SHUT, ErrorSHUT);
			OSTimeDly(500);
			goto re_mech;
		}
	}
	case Location: // 定位组
	{
	re_location:
		SemPend(RUN);
		RunPoint(debug_posi.points[debug_posi.xyr][0], debug_posi.points[debug_posi.xyr][1], debug_posi.points[debug_posi.xyr][2],
				 debug_posi.speed_start, debug_posi.speed_max_limited, 0,
				 debug_posi.dis_up, debug_posi.dis_down, 0, 0,
				 False);

		SemPend(RUN);
		RunPoint(0, 0, 0,
				 debug_posi.speed_start, debug_posi.speed_max_limited, 0,
				 debug_posi.dis_up, debug_posi.dis_down, 0, 0,
				 False);

		goto re_location;
	}
	default:
		break;
	}
	for (;;) // 担心手抖，上面多了个break，程序就跑飞了，还不知道
	{
		BEEP_ON;
		OSTimeDly(10000);
		BEEP_OFF;
		OSTimeDly(10000);
	}
}
/*** 
 * @brief 
 * @author 叮咚蛋
 * @Date 2020-08-09 10:18:40
 */
static void TaskLCD(void *pdata)
{
	pdata = pdata;

	OSTimeDly(500);

	for (;;)
	{

		OSTimeDly(500);
	}
	for (;;)
	{
		BEEP_ON;
		OSTimeDly(10000);
		BEEP_OFF;
		OSTimeDly(10000);
	}
}
/*** 
 * @brief  在搜索零时检查陀螺仪的数据可能正在漂移
 * @author 叮咚蛋
 * @Date 2020-08-09 10:19:48
 */
static void TaskGyroscope(void *pdata)
{
	pdata = pdata;
	for (;;)
	{
		if (False == gyroscope.init_success)
		{
			gyroscope.over_time++;
			if (gyroscope.over_time > 50)
			{
				Beep_Show(8);
				sprintf(user.error, "%s", "GyroInitErr");
				break;
			}
			OSTimeDly(400);
		}
		else
		{
			gyroscope.check_drift = True;
			break;
		}
	}
	for (;;)
	{
		if (True == gyroscope.check_drift)
		{
			gyroscope.angle_check[0] = position_now.angle;
			if (gyroscope.angle_check[0] - gyroscope.angle_check[1] > 0.05f) // angle_now - angle_last > 0.05度，
			{
				gyroscope.angle_err++;
			}
			gyroscope.angle_check[1] = gyroscope.angle_check[0]; // 更新角度值
			OSTimeDly(200);

			if (gyroscope.angle_err > 3)
			{
				Beep_Show(8);
				sprintf(user.error, "%s", "GyroDrift");
				gyroscope.state = Error;
				gyroscope.check_drift = False;
				break;
			}
			else
			{
				gyroscope.state = Fine;
				gyroscope.check_drift = False;
				break;
			}
		}
		else
			OSTimeDly(500);
	}
	for (;;)
	{
		if ((ABS(angle_pid.err_now) > 20) || (ABS(run_pid.err_now) > 50)) // 跑飞了
		{
			if (enable == motor.state && (True == run.pid_run || True == run.pid_point)) //FIXME:
			{
				BREAK_ALL_MOTOR;
				Beep_Show(8);
				sprintf(user.error, "%s", "OutOfControl");
			}
			OSTimeDly(4000); // 检测周期为400ms
		}
		else
			OSTimeDly(4000);
	}
	for (;;)
	{
		BEEP_ON;
		OSTimeDly(10000);
		BEEP_OFF;
		OSTimeDly(10000);
	}
}
/*** 
 * @brief 
 * @author 叮咚蛋
 * @Date 2020-08-09 10:19:06
 */
static void TaskLED(void *pdata)
{
	pdata = pdata;
	for (;;)
	{
		if ((user.area_side == red_area && Master == user.identity) || (x_dir == debug_posi.xyr && Location == user.identity))
		{
			LED_RED_ON;
			LED_BLUE_OFF;
		}
		else
		{
			LED_BLUE_ON;
			LED_RED_OFF;
		}
		Led8DisData(lcd.screen_id);
		LED_GREEN_ON;
		OSTimeDly(5000);
		LED_GREEN_OFF;
		OSTimeDly(5000);
	}
	for (;;)
	{
		BEEP_ON;
		OSTimeDly(10000);
		BEEP_OFF;
		OSTimeDly(10000);
	}
}
u8 num_temp = 0;
u8 BEEP_OFF_flag = False;
/*** 
 * @brief 
 * @author 未定义
 * @Date 2020-08-10 17:28:32
 */
static void TaskBEEP(void *pdata)
{
	u8 i = 0;
	pdata = pdata;
	for (;;)
	{
		num_temp = beep_on_num;
		if (0 == num_temp)
		{
			// PlayMusic(two_tigers);
			OSTimeDly(2000);
		}
		else
		{
			for (i = 0; i < num_temp; i++)
			{
				if (False == BEEP_OFF_flag)
				{
					BEEP_ON;
					OSTimeDly(1000);
					BEEP_OFF;
					OSTimeDly(1000);
					beep_on_num--;
				}
				else
				{
					beep_on_num = 0;
					break;
				}
			}
			OSTimeDly(2000);
		}
	}
	for (;;)
	{
		BEEP_ON;
		OSTimeDly(10000);
		BEEP_OFF;
		OSTimeDly(10000);
	}
}

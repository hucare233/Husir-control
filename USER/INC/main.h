#ifndef _MAIN_H
#define _MAIN_H

#include "stm32f4xx.h"
#include "tim.h"
#include "can1.h"
#include "can2.h"
#include "usart1.h"
#include "usart2.h"
#include "usart3.h"
#include "usart4.h"
#include "usart5.h"
#include "beep.h"
#include "led.h"
#include "led8.h"
#include "key.h"
#include "lcd.h"
#include "visual_scope.h"
#include "motor.h"
#include "param.h"
#include "pid.h"
#include "run.h"

#define START_TASK_PRIO          5
#define TASK_RUN_PRIO						10
#define TASK_STATUS_PRIO      	15
#define TASK_BEEP_PRIO      		20
#define TASK_SCOPE_PRIO      		25
#define TASK_LED_PRIO      			30

#define START_TASK_STK_SIZE         1024
#define TASK_RUN_STK_SIZE   			  1024
#define TASK_STATUS_STK_SIZE   			1024
#define TASK_BEEP_STK_SIZE   			  1024
#define TASK_SCOPE_STK_SIZE   			1024
#define TASK_LED_STK_SIZE   			  1024

__align(8) OS_STK start_task_stk[START_TASK_STK_SIZE];
__align(8) OS_STK task_run_stk[TASK_RUN_STK_SIZE];
__align(8) OS_STK task_status_stk[TASK_STATUS_STK_SIZE];
__align(8) OS_STK task_beep_stk[TASK_BEEP_STK_SIZE];
__align(8) OS_STK task_Scope_stk[TASK_SCOPE_STK_SIZE];
__align(8) OS_STK task_led_stk[TASK_LED_STK_SIZE];

static void TaskStart(void *arg);
static void TaskRun(void *arg);
static void TaskStatus(void *arg);
static void TaskBeep(void *arg);
static void TaskScope(void *arg);
static void TaskLED(void *arg);

void Trigger(u8 ind,float Data,u8 type);
void Emergency(void);
#endif

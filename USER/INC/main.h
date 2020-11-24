#ifndef _MAIN_H
#define _MAIN_H

#include "stm32f4xx.h"
#include "includes.h"

#define START_TASK_PRIO          5
#define TASK_LED_PRIO      			60
#define TASK_Scope_PRIO      		55

#define START_TASK_STK_SIZE         256

__align(8) OS_STK start_task_stk[START_TASK_STK_SIZE];

static void TaskStart(void *arg);

#endif

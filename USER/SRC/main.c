#include "main.h"


int main(void)
{
	SystemInit();
  NVIC_SetPriorityGrouping(NVIC_PriorityGroup_3);	
	
  OSInit();
	OSTaskCreate(TaskStart, (void *)0, &start_task_stk[START_TASK_STK_SIZE - 1], START_TASK_PRIO);
	OSStart();
	
	return 0;
}
/**
  * @brief  起始任务
  */
static void TaskStart(void *pdata)
{
	pdata = pdata;
	OS_CPU_SysTickInit();
	
	for(;;)
	{
		OSTimeDly(10000);
	}
}


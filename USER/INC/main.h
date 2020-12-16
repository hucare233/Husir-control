/*** 
 * @Date: 2020-07-25 10:35:58
 * @LastEditors   叮咚蛋
 * @LastEditTime  2020-09-20 22:55:03
 * @FilePath      \Project\User\inc\main.h
 */
#ifndef _MAIN_H_
#define _MAIN_H_

#include "stm32f4xx.h"
#include "param.h"
#include "led.h"
#include "usart1.h"
#include "usart2.h"
#include "usart3.h"
#include "usart4.h"
#include "usart5.h"
#include "can1.h"
#include "can2.h"
#include "includes.h"
#include "beep.h"
#include "led8.h"
#include "queue.h"
#include "cancontrolist.h"
#include "visual_scope.h"
#include "pid.h"
#include "tim3.h"
#include "motor.h"
#include "key.h"
#include "lcd.h"

#define START_TASK_PRIO 5
#define LED_TASK_PRIO 60
#define BEEP_TASK_PRIO 55
#define RUN_TASK_PRIO 10
#define MECHANISM_TASK_PRIO 15
#define DT50_TASK_PRIO 25
#define LCD_TASK_PRIO 30
#define GYROSCOPE_TASK_PRIO 35

#define START_TASK_STK_SIZE 1024
#define LED_TASK_STK_SIZE 1024
#define RUN_TASK_STK_SIZE 1024
#define MECHANISM_TASK_STK_SIZE 1024
#define DT50_TASK_STK_SIZE 1024
#define LCD_TASK_STK_SIZE 1024
#define BEEP_TASK_STK_SIZE 1024
#define GYROSCOPE_TASK_STK_SIZE 1024

/* 为了解决请求信号量失败，采用宏定义重定向了ossempend，每次请求前，先清零sem*/
#define SemPend(sem)                   \
    {                                  \
        sem->OSEventCnt = 0;           \
        OSSemPend(sem, 0, Error##sem); \
    }

__align(8) OS_STK start_task_stk[START_TASK_STK_SIZE];         /* 如果任务中要使用printf来打印浮点数据的话一定要8字节对齐 */
__align(8) OS_STK run_task_stk[RUN_TASK_STK_SIZE];             /* 如果任务中要使用printf来打印浮点数据的话一定要8字节对齐 */
__align(8) OS_STK led_task_stk[LED_TASK_STK_SIZE];             /* 如果任务中要使用printf来打印浮点数据的话一定要8字节对齐 */
__align(8) OS_STK mechanism_task_stk[MECHANISM_TASK_STK_SIZE]; /* 如果任务中要使用printf来打印浮点数据的话一定要8字节对齐 */
__align(8) OS_STK dt50_task_stk[DT50_TASK_STK_SIZE];           /* 如果任务中要使用printf来打印浮点数据的话一定要8字节对齐 */
__align(8) OS_STK lcd_task_stk[LCD_TASK_STK_SIZE];             /* 如果任务中要使用printf来打印浮点数据的话一定要8字节对齐 */
__align(8) OS_STK beep_task_stk[BEEP_TASK_STK_SIZE];           /* 如果任务中要使用printf来打印浮点数据的话一定要8字节对齐 */
__align(8) OS_STK gyroscope_task_stk[GYROSCOPE_TASK_STK_SIZE]; /* 如果任务中要使用printf来打印浮点数据的话一定要8字节对齐 */

static void TaskStart(void *pdata);
static void TaskLED(void *pdata);
// static void TaskMECHANISM(void *pdata);
static void TaskRUN(void *pdata);
static void TaskSHUT(void *pdata);
static void TaskDT50(void *pdata);
static void TaskLCD(void *pdata);
static void TaskBEEP(void *pdata);
static void TaskGyroscope(void *pdata);

void SetFlagDt50_1(u8 whichPoint);

#endif

//########################################################################################$
//########################################################################################$
//###########################################!  '&########################################$
//#################################&:  |######$|@#########################################$
//##################################&;:$######$|@#########################################$
//####################################%!@#####$|@#########################################$
//#####################################|!#####$;!:`.'!&###################################$
//####################################&!`               !#################################$
//################################%.                     ;################################$
//##############################@:                        `%##############################$
//##############################|               :@###@;    !##############################$
//##############################|     .'`      `$#####&'   ;######&'    !#################$
//#####@:    :&################$`    !###&'     !#####|    :@####|       |################$
//#####|      |#################|    '&##|.       .'`     '&####;       !#################$
//####@:     :@#################@:                        :@##@:       |#######| .;&######$
//####%.    .%####&@#############%`                       |##@:      .%######$'     :@####$
//####;     ;##@:    .|############%.                   `$###;       |######|       ;#####$
//###$`     .      . .%#####&;;&####@;             .'!$#####%.      '&####@:       :@#####$
//###!               .%##&'     |########@@#########@$!::;$#!        .|@@;        :@######$
//##&'               .;'         !##&'     .|#####&'       ;!                    ;########$
//##|  .   '$##$'                '$@:       .%####%`        ;:                  !#########$
//##@;   !#####%.             `|###$`                       `%;               '&##########$
//############;   .        ;@######!         !@####%`        ;#@;           .%############$
//############&'       '$#########@:       `%##&&###&'       .%####$:.   `|@##############$
//##############|. .|#############%`        ''`.              :@##########################$
//################################!                            |##########################$
//###############################@:           .|@%`            :@#########################$
//###############################$`           |###$`            |#########################$
//###############################|            |###@:            :@########################$
//###############################!           .%####!            .%########################$
//###############################;            %####%             ;########################$
//##############################@'            |####&`            `$#######################$
//##############################&'            %#####;             |#######################$
//##############################&'           .%#####%`            !#######################$
//###############################;           .%######|          .%########################$
//#################################$;.      `%#########$|!|%&#############################$

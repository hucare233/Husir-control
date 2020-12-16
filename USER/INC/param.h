/*
 * @Descripttion: ???
 * @version: ???
 * @Author: ???
 * @Date: 2020-12-15 19:29:28
 * @LastEditors: ???
 * @LastEditTime: 2020-12-16 10:08:43
 * @FilePath: \hu_sir-contorl\USER\INC\param.h
 */

#ifndef _MASTER_PARA_H
#define _MASTER_PARA_H

#include "stm32f4xx.h"
#include "includes.h"
#include "struct.h"
#include "MyMath.h"

/* extern */
extern UserFlagStruct user;
extern volatile TimeStruct run_time;
extern volatile RunFlagStruct run;
extern volatile LcdStruct lcd;
extern volatile DebugPosiStruct debug_posi;
extern volatile MotorFlagStruct motor;
extern volatile GyroscopeFlagStruct gyroscope;
extern volatile CameraFlagStruct camera;
extern volatile UsartDmaFlagStruct usart_dma;
extern volatile MechanismFlagStruct mechanism;
extern volatile ChassisStruct chassis;
extern u8 USART1_DMA_SendBuf[USART_SENDQUEUESIZE];
extern u8 USART2_DMA_SendBuf[USART_SENDQUEUESIZE];
extern u8 USART3_DMA_SendBuf[USART_SENDQUEUESIZE];
extern u8 UART4_DMA_SendBuf[USART_SENDQUEUESIZE];
extern u8 UART5_DMA_SendBuf[USART_SENDQUEUESIZE];

extern OS_CPU_SR cpu_sr;
extern OS_EVENT *RUN;
extern OS_EVENT *RDY;
extern OS_EVENT *BALL;
extern OS_EVENT *SHUT;
extern OS_FLAG_GRP *FlagCan1Check;
extern OS_FLAG_GRP *FlagCan2Check;
extern INT8U *ErrorRUN;
extern INT8U *ErrorRDY;
extern INT8U *ErrorBALL;
extern INT8U *ErrorSHUT;
extern INT8U *ErrorCan1;
extern INT8U *ErrorCan2;

void para_init(void);
#endif

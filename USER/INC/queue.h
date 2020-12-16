#ifndef __QUEUE_H
#define __QUEUE_H

#include "param.h"
#include "string.h"
#include "beep.h"
#include "stm32f4xx_can.h"
#define CAN_1 (u8)(0x01 << 0)
#define CAN_2 (u8)(0x01 << 1)

/********* u8  //ErrFlag[5] **********/

/***********************************/

/*************** u32 CAN_ID_RX ******************/
#define ID_GyrScope_RX 0x00020101
#define ID_3508_Mec_RX 0x00030101 //3508四个机构电机的驱动板
#define ID_DT5035X_RX 0x00040101
#define ID_DT5035Y_RX 0x00090101
#define ID_ARM_RX 0x00030101
/*************** u32 Elmo_ID_RX ******************/
#define ID_Elmo_Elv_RX 0x000
#define ID_Elmo_Motor1_RX 0x281
#define ID_Elmo_Motor2_RX 0x282
#define ID_Elmo_Motor3_RX 0x283
#define ID_Elmo_Motor4_RX 0x284
//两个ID对应同一块板子
#define ID_3508_Motor5_RX 0x285 //14驱动板
#define ID_3508_Motor6_RX 0x286
/************************************************/

/*************** u32 CAN_ID_TX ******************/
#define ID_BroadCast_TX 0x00010000
#define ID_GyrScope_TX 0x00010200
#define ID_DJIMotor_F4_TX 0x00012000
#define ID_DJIMotor_B4_TX 0x00012100
#define ID_3508_Mec_TX 0x00010300
#define ID_CAM_TX 0x00010400
#define ID_DT5035Y_TX 0x00010500
#define ID_DT5035X_TX 0x00010600
#define ID_DT5035XL_TX 0x00010700 /************************************************/
#define ID_steerE_TX 0x00013000
#define ID_begin_TX 0x00012000
/************************************************/

/*************** u32 Func_ID ******************/
#define Func_ID_BroadCast 0x00
#define FuncID_GyrScope 0x01
#define FuncID_Cylinder 0x05
#define FuncID_Camera 0x08
#define FuncID_Multiple 0x06
#define FuncID_Arm 0X02
/************************************************/

#define Rear2 ((can2_sendqueue.Rear + 1) % can2_sendqueue.Can_sendqueuesize)
#define Rear1 ((can1_sendqueue.Rear + 1) % can1_sendqueue.Can_sendqueuesize)
#define CAN1_NodeNumber 6u
#define CAN2_NodeNumber 8u

extern CanSendqueue can1_sendqueue;
extern CanSendqueue can2_sendqueue;
extern volatile QueueFlagStruct queue_flag;
extern MesgControlGrp CAN1_MesgSentList[CAN1_NodeNumber];
extern MesgControlGrp CAN2_MesgSentList[CAN2_NodeNumber];
extern void CANMesgControlList(MesgControlGrp *CAN_MesgSentList, CanSendqueue *can_queue, u8 CAN_x);
extern UsartSendqueue usart1_sendqueue;
extern UsartSendqueue usart2_sendqueue;
extern UsartSendqueue usart3_sendqueue;
extern UsartSendqueue uart4_sendqueue;
extern UsartSendqueue uart5_sendqueue;

void Can_SendqueueInit(void);
void usart_queue_init(void);
void Can_EnQueue(CanSendqueue *can_queue, CanSendStruct *Pcansendstruct);
// void Can_DeQueue(u8 CAN_x, CanSendqueue *can_queue);
void Can_DeQueue(u8 CAN_x, CanSendqueue *can_queue);
void Usart_SetEmpty(UsartSendqueue *queue);
u8 Usart_EnQueue(UsartSendqueue *usart_queue, uint8_t *send_buffer, u16 buffer_size);
void Usart_DeQueue(UsartSendqueue *usart_queue, u8 *usartDMA_tx_buf);
u16 Usart_GetQueueLength(UsartSendqueue *queue);

#endif

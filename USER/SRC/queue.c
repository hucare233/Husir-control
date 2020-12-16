/*
 * @Descripttion: 队列
 * @version: 第一版
 * @Author: 叮咚蛋
 * @Date: 2020-12-15 19:34:33
 * @LastEditors: 叮咚蛋
 * @LastEditTime: 2020-12-16 15:21:17
 * @FilePath: \hu_sir-contorl\USER\SRC\queue.c
 */

#include "queue.h"

//TODO:抽时间重新学习一波can_bus，感觉，可以好好改一改
CanSendqueue can1_sendqueue;
CanSendqueue can2_sendqueue;
UsartSendqueue usart1_sendqueue;
UsartSendqueue usart2_sendqueue;
UsartSendqueue usart3_sendqueue;
UsartSendqueue uart4_sendqueue;
UsartSendqueue uart5_sendqueue;

volatile QueueFlagStruct queue_flag;

//串口队列
//#define IsEmpty() (queue->front == queue->rear)
//#define IsFull()  (((queue->rear + 1) % (USART_SENDQUEUESIZE)) == queue->front)

/*** 
 * @description: 
 * @param {void} 
 * @return {void} 
 * @author: 叮咚蛋
 * @Date: 2020-08-04 23:28:19
 */
void Can_SendqueueInit(void)
{
	can1_sendqueue.Front = 0;
	can1_sendqueue.Rear = 0;
	can1_sendqueue.Can_sendqueuesize = CAN_SENDQUEUESIZE;

	can2_sendqueue.Can_sendqueuesize = CAN_SENDQUEUESIZE;
	can2_sendqueue.Front = 0;
	can2_sendqueue.Rear = 0;
}

/**
  * @brief			can入队,基本不用，因为，手动在函数里入队，不好统一
  */
void Can_EnQueue(CanSendqueue *can_queue, CanSendStruct *Pcansendstruct)
{
	if (((can_queue->Rear + 1) % can_queue->Can_sendqueuesize) == can_queue->Front)
	{
		// queue_flag.can1_queue_full++;
		return;
	}
	else
		memcpy(&can_queue->node[can_queue->Rear], Pcansendstruct, sizeof(CanSendStruct));
	can_queue->Rear = ((can_queue->Rear + 1) % can_queue->Can_sendqueuesize);

	return;
}

/**
  * @brief			can出队
  */
float Can_Tx_NoMailBox = 0.0;
CanTxMsg tx_msg;
void Can_DeQueue(u8 CAN_x, CanSendqueue *can_queue)
{
	CanTxMsg TxMessage;
	if (can_queue->Rear == can_queue->Front)
	{
		queue_flag.usart_queue_full = True;
		return;
	}
	else
	{
		queue_flag.usart_queue_full = False;

		if (can_queue->node[can_queue->Front].Id < 0x800)//epos id 0x60x
		{
			TxMessage.StdId = can_queue->node[can_queue->Front].Id;
			TxMessage.IDE = CAN_ID_STD;
		}
		else
		{
			TxMessage.ExtId = can_queue->node[can_queue->Front].Id;
			TxMessage.IDE = CAN_ID_EXT;
		}
		if (CAN_1 == CAN_x)
		{
			CANMesgControlList(CAN1_MesgSentList, &can1_sendqueue, CAN_1);
		}
		else
		{
			CANMesgControlList(CAN2_MesgSentList, &can2_sendqueue, CAN_2);
		}
		TxMessage.DLC = can_queue->node[can_queue->Front].DLC;
		TxMessage.RTR = CAN_RTR_DATA;
		memcpy(&TxMessage.Data[0], &(can_queue->node[can_queue->Front].Data[0]), sizeof(u8) * TxMessage.DLC);
		if (CAN_1 == CAN_x)
		{
			if (CAN_Transmit(CAN1, &TxMessage) == CAN_TxStatus_NoMailBox)
				Can_Tx_NoMailBox++;
		}
		else
		{
			if (CAN_Transmit(CAN2, &TxMessage) == CAN_TxStatus_NoMailBox)
				Can_Tx_NoMailBox++;
		}
		tx_msg = TxMessage;
		can_queue->Front = (can_queue->Front + 1) % can_queue->Can_sendqueuesize;
	}
}
/**
  * @brief        USART清空队列
  */
void usart_queue_init(void)
{
	usart1_sendqueue.usart_sendqueuesize = USART_SENDQUEUESIZE;
	usart1_sendqueue.Front = 0;
	usart1_sendqueue.Rear = 0;

	usart2_sendqueue.usart_sendqueuesize = USART_SENDQUEUESIZE;
	usart2_sendqueue.Front = 0;
	usart2_sendqueue.Rear = 0;

	usart3_sendqueue.usart_sendqueuesize = USART_SENDQUEUESIZE;
	usart3_sendqueue.Front = 0;
	usart3_sendqueue.Rear = 0;

	uart4_sendqueue.usart_sendqueuesize = USART_SENDQUEUESIZE;
	uart4_sendqueue.Front = 0;
	uart4_sendqueue.Rear = 0;

	uart5_sendqueue.usart_sendqueuesize = USART_SENDQUEUESIZE;
	uart5_sendqueue.Front = 0;
	uart5_sendqueue.Rear = 0;
}

/*** 
 * @brief  USART入队函数
 * @param  usart_queue 串口队列;
 * @param  send_buffer 外部暂存数据数组首地址
 * @param  buffer_size 入队数据size，外部传入
 * @return True of False flag, Depends on whether the data is successfully enqueued
 * @author 未定义
 * @CreatedTime 2020-09-08 20:07:50
 */
u8 Usart_EnQueue(UsartSendqueue *usart_queue, uint8_t *send_buffer, u16 buffer_size)
{
	if (((usart_queue->Rear + 1) % usart_queue->usart_sendqueuesize) == usart_queue->Front)
	{
		queue_flag.usart_queue_full = True;
		sprintf(user.error, "%s", "UsartQueueFull");
		// Beep_Show(8);
		return False;
	}
	else
	{
		queue_flag.usart_queue_full = False;

		memcpy(&usart_queue->Data[usart_queue->Rear], send_buffer, buffer_size);
	}

	usart_queue->Rear = (usart_queue->Rear + buffer_size) % (usart_queue->usart_sendqueuesize);
	return True;
}

/**
  * @brief        
  */
/*** 
 * @brief  USART出队函数
 * @param  usart_queue 串口队列
 * @param  usartDMA_tx_buf 外部传入发送数组首地址
 * @return void
 * @author 未定义
 * @CreatedTime 2020-09-08 20:54:27
 */
void Usart_DeQueue(UsartSendqueue *usart_queue, u8 *usartDMA_tx_buf)
{
	if (usart_queue->Rear == usart_queue->Front)
	{
		queue_flag.usart_queue_empty = True;
		return;
	}
	else
	{
		queue_flag.usart_queue_empty = False;

		usart_queue->length = Usart_GetQueueLength(usart_queue);

		memcpy(usartDMA_tx_buf, &usart_queue->Data[usart_queue->Front], usart_queue->length);
		usart_queue->Front = (usart_queue->Front + usart_queue->length) % (usart_queue->usart_sendqueuesize);
	}
}

/**
  * @brief        USART获取队列长度
  */
u16 Usart_GetQueueLength(UsartSendqueue *queue)
{
	if (queue->Rear > queue->Front)
		return (queue->Rear - queue->Front);
	else if (queue->Rear < queue->Front)
		return (USART_SENDQUEUESIZE - (queue->Front - queue->Rear));
	else
		return 0;
}

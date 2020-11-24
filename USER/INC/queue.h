#ifndef __QUEUE_H
#define __QUEUE_H

#include "string.h"
#include "stm32f4xx_can.h"
#include "param.h"
#include "cancontrolist.h"

void Can_SendqueueInit(void);
void CAN_DeQueue(u8 CAN_x , Can_Sendqueue *can_queue);
void Detect(void);

#endif

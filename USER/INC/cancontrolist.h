#ifndef __CANCONTROLLIST_H_
#define __CANCONTROLLIST_H_

#include "queue.h"
#include "beep.h"
#include "includes.h"

#define CAN_CONTROLQUEUESIZE 5

void TraversalControlList(MesgControlGrp *CAN_MesgSentList, CanSendqueue *can_queue, u8 CAN_x);
void InitCANControlList(MesgControlGrp *CAN_MesgSentList, u8 CAN_x);

#endif

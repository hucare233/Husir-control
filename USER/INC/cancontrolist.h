#ifndef __CANCONTROLLIST_H
#define __CANCONTROLLIST_H

#include "stdio.h"
#include "param.h"

void InitCANControlList(MesgControlGrp* CAN_MesgSentList , u8 CAN_x);
void TraversalControlList(MesgControlGrp* CAN_MesgSentList , Can_Sendqueue* can_queue , u8 CAN_x);
void CANMesgControlList(MesgControlGrp* CAN_MesgSentList , Can_Sendqueue* can_queue , u8 CAN_x);

#endif

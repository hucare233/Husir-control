#ifndef __MOTOR_H
#define __MOTOR_H

#include "stm32f4xx.h"
#include "param.h"
#include "queue.h"

void Elmo_Ele(u8 waitflag);
void Elmo_Enable(u8 ID,u8 waitflag,float state);
void Elmo_C_Speed(u8 ID,u8 waitflag,float speed);
void Elmo_Query_Position(u8 ID,u8 waitflag);
void Elmo_Reset_position(u8 ID,u8 waitflag,float position);
void Elmo_PTP_speed(u8 ID,u8 waitflag,float speed);
void Elmo_PTP_position(u8 ID,u8 waitflag,float position);
void Elmo_Begin(u8 ID,u8 waitflag);
void Elmo_Stop(u8 ID,u8 waitflag);
void Elmo_MODE_Select(u8 ID,u8 waitflag,float mode);

void Commend(u8 id,u8 data0,u8 data1,u8 flag,void* data);
void Query(u8 id,u8 data0,u8 data1,u8 flag);
	
void ENABLE_ALL_MOTOR(void);	
	
#endif


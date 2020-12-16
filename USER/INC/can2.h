/*
 * @Descripttion: 
 * @version: µÚÒ»°æ
 * @Author: ¶£ßËµ°
 * @Date: 2020-12-15 19:29:28
 * @LastEditors: ¶£ßËµ°
 * @LastEditTime: 2020-12-16 10:09:38
 * @FilePath: \hu_sir-contorl\USER\INC\can2.h
 */

#ifndef __CAN2_H_
#define __CAN2_H_

#include "stm32f4xx.h"
#include "delay.h"
#include "led.h"
#include "beep.h"
#include "param.h"
#include "queue.h"
#include "macro.h"
#include "includes.h"

void Can2_Configuration(u8 prep_prio, u8 sub_prio);
void Can2selfcheck(u8 InConGrpFlag);
void BackDJIMotor_SZ(void);
void BackDJI_SZ_Speed(s16 speed);
void BackDJI_SetPTPSpeed(u8 ID, s16 Speed);
void BackDJI_SetPTPPosition(u8 ID, s16 pulse);
void BackDJI_SetSpeed(u8 ID, s16 speed);
void BackDJI_SetCurrent(u8 ID, s16 current);
void BackDJI_ChooseMode(u8 ID, u8 mode);
void BackDJI_ReleaseMotor(void);
void BackDJI_StopMotor(void);
void BackDJI_EnableMotor(void);
void BackDJI_AskPosition(u8 ID);
void BackDJI_AskSpeed(u8 ID);
void BackDJIMotor_ASK(void);
void UnlockTouchCheckB(void);
void LockTouchCheckB(void);
//void DecodeFloatData(float* f, unsigned char* buff);
extern INT8U *ErrorCan2;
extern OS_FLAG_GRP *FlagCan2Check;

extern volatile float MotorPosition[8]; //?¨®¨º?¦Ì?¦Ì??a?¨¢o¨®???¨¨¡ê?D¨¨¨°a????3¨¦?¨¢?¡ã???¨¨ ¦Ì£¤??¡êo?¨¨

void MotorToWalk_CAN2(void);
void MotorToJump_CAN2(void);
#endif

/********************************End of File************************************/

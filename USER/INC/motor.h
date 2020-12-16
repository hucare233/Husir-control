#ifndef __MOTOR_H_
#define __MOTOR_H_
#include "stm32f4xx.h"
#include "queue.h"
#include "macro.h"
#include "includes.h"
#include "param.h"

/** 
  * @brief MotorControl
  */
#define BREAK_ALL_MOTOR         \
	{                           \
		Elmo_StopMotor(0, 1);   \
		turn_motor_ST(5, 1);    \
		turn_motor_ST(6, 1);    \
		turn_motor_ST(7, 1);    \
		turn_motor_ST(8, 1);    \
		motor.state = breaking; \
	}

#define DISABLE_ALL_MOTOR        \
	{                            \
		Elmo_RelieveMotor(0, 1); \
		turn_motor_mo(5, 0, 1);  \
		turn_motor_mo(6, 0, 1);  \
		turn_motor_mo(7, 0, 1);  \
		turn_motor_mo(8, 0, 1);  \
		motor.state = disable;   \
	}

#define ENABLE_ALL_MOTOR        \
	{                           \
		Elmo_StartMotor(0, 1);  \
		turn_motor_mo(5, 1, 1); \
		turn_motor_mo(6, 1, 1); \
		turn_motor_mo(7, 1, 1); \
		turn_motor_mo(8, 1, 1); \
		motor.state = enable;   \
	}

/*********************************************************/

enum __ELMO_MODE
{
	torque = 1,				 //转矩
	speed,					 //速度
	stepper,				 //步进
	DualFeedback_Position,	 //双反馈位置
	SingleFeedback_Position, //单反馈位置
};
extern s32 ElmoPulse;
extern s32 ElmoPulselast;

extern CanSendqueue can1_sendqueue;
extern CanSendqueue can2_sendqueue;
extern volatile MotorFlagStruct motor;

void EncodeFloatData(float *f, u8 *buff);
void Elmo_Ele(u8 InConGrpFlag);
void Elmo_Setspeed(u8 ID, s32 speed, u8 InConGrpFlag);
void Elmo_SetAC(u8 ID, s32 ac);
void AskElmo_MasterPosition(u8 ID);
void Elmo_MasterPosition(u8 ID, s32 pulse);
void Elmo_AbsolutePosition(u8 ID, s32 pulse);
void Elmo_StartMotor(u8 ID, u8 InConGrpFlag);
void Elmo_SelectMode(u8 ID, s32 Mode);
void Elmo_PTP_Speed(u8 ID, s32 speed);
void Elmo_RelieveMotor(u8 ID, u8 InConGrpFlag);
void Elmo_StopMotor(u8 ID, u8 InConGrpFlag);
void Elmo_Begin(u8 ID, u8 InConGrpFlag);
void Elmo_GetError(void);
void Elmo_GetVX(u8 InConGrpFlag);
void Elmo_GetIQA(u8 ID);
void Elmo_GetIDA(u8 ID);
void Elmo_SetTC(u8 ID, float current);
void __ELMOSETPX(char *y, u8 ID);
void Elmo_SetParameter(u8 ID, u8 fst, u8 snd, u8 index, u8 value, u8 InConGrpFlag);
void turn_motor_ask(u8 ID, u8 InConGrpFlag);
void turn_motor_ST(u8 ID, u8 InConGrpFlag);
void turn_motor_mo(u8 ID, u8 Status, u8 InConGrpFlag);
void turn_motor_singlePAx30(u8 ID, u8 WheelNum, float FrontAngle, u8 InConGrpFlag);
void turn_motor_4PAx30(float FrontAngle1, float FrontAngle2, float FrontAngle3, float FrontAngle4, u8 InConGrpFlag);
void turn_motor_init(void);
#endif

/********************************End of File************************************/

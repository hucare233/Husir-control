/*** 
 * @Date         : 2020-07-25 10:35:57
 * @LastEditors   ���˵�
 * @LastEditTime  2020-09-12 14:52:30
 * @FilePath      \Project\User\inc\can1.h
 */
#ifndef _CAN1_H
#define _CAN1_H

#include "stm32f4xx.h"
#include "delay.h"
#include "led.h"
#include "beep.h"
#include "param.h"
#include "macro.h"
#include "includes.h"
#include "run.h"

// extern u8 count_send[4];
// extern u8 Judge_Send;
// extern u8 Flag_Ready;
// extern volatile u8 //ErrFlagag[5];
// extern u8 Place;
// extern u8 Flag_SendStart;
// extern u8 Flag_ARSA;
// extern u8 send_over_flag;
// extern u8 receive_count;

extern CanSendqueue can1_sendqueue;
extern OS_FLAG_GRP *FlagCan1Check;
extern INT8U *ErrorCan1;

void Can1_Configuration(u8 prep_prio, u8 sub_prio);
void Can1selfcheck(u8 InConGrpFlag);
void FrontDJIMotor_SZ(void);
void FrontDJI_SZ_Speed(s16 speed);
void FrontDJI_SetPTPSpeed(u8 ID, s16 Speed);
void FrontDJI_SetPTPPosition(u8 ID, s16 pulse);
void FrontDJI_SetSpeed(u8 ID, s16 speed);
void FrontDJI_SetCurrent(u8 ID, s16 current);
void FrontDJI_ChooseMode(u8 ID, u8 mode);
void FrontDJI_ReleaseMotor(void);
void FrontDJI_StopMotor(void);
void FrontDJI_EnableMotor(void);
void FrontDJI_AskPosition(u8 ID);
void FrontDJI_AskSpeed(u8 ID);
void FrontDJIMotor_ASK(void);
void AngleStop(void);
void AngleBegin(void);
void UnlockTouchCheckF(void);
void LockTouchCheckF(void);

void UnlockSwitchCheck(void);
void LockSwitchCheck(void);
void STOPSwitch(void);
void UnlockUPtoHILLCheck(void);
void LockUPtoHILLCheck(void);
void STOPSwitch(void);
void opensteerE(void);
void closesteerE(void);
void GyroSetY(float y, u8 InConGrpFlag);
void GyroSetXandA(float x, float A, u8 InConGrpFlag);
void ResetPosition_EN(float x, float y, float angle, u8 InConGrpFlag);
void GyroSetX(float x, u8 InConGrpFlag);

void MotorToWalk_CAN1(void);
void MotorToJump_CAN1(void);
void ResetSlave(u32 ID, u8 FunID, u8 InConGrpFlag);

/*************count_send**************/
enum
{
    ARSA = 0,
    MRA = 1,
    TZ2 = 2,
    TZ3 = 3
};
/*************ͨѶ�����****************/

/*****************ErrFlagag *********************/
#define FuncIDErr 0x01    //���������
#define SubIDNone 0x02    //�ӹ����벻����
#define DataLenthErr 0x03 //���ݳ��Ȳ�����
#define SubUseErr 0x04    //�ӹ���ʹ�ô���
#define ArgErr 0x05       //��������
/*****************�������********************/

/*****************FaultID**********************/
#define Timeout 0x01      //����ʱ
#define BusOFF 0x02       //���߹ر�
#define TxErrEnough 0x03  //���ʹ���Ĵ����ۼ�ֵ����
#define RxErrEnough 0x04  //���ܴ���Ĵ����ۼ�ֵ����
#define ReceiveNone 0x05  //Ԥ������ʱ����δ�ӵ��ӵ���������
#define SelfTestNone 0x06 //�ϵ��Ԥ��ʱ����δ�յ��Լ��ź�
/*****************���ϱ���*********************/

// void SelfTest(void);
// void Reset_OK(u8 FuncID);
// void Fault_Send(u8 FaultID);
// void Fuck(u8 Flag_Start);
// u8 Send_ImageWrong(u8 place);
// void DT50STY(u32 ID, u8 InConGrpFlag);
// void DT50BGXL(u32 ID, u8 Time, u8 InConGrpFlag);
// void DT50STXL(u32 ID, u8 InConGrpFlag);
// void AskDT50XL(u32 ID, u8 num, u8 InConGrpFlag);
// void DT50STX(u32 ID, u8 InConGrpFlag);
// void AskDT50X(u32 ID, u8 num, u8 InConGrpFlag);
// void DT50BGX(u32 ID, u8 Time, u8 InConGrpFlag);
// void AskDT50Y(u32 ID, u8 num, u8 InConGrpFlag);

#endif

#ifndef _PARAM_H_
#define _PARAM_H_

#include "stm32f4xx.h"
#include "ucos_ii.h"
#include "flag.h"
#include "struct.h"
#include "motor.h"

extern volatile RunFlagStruct run;
extern volatile TimeStruct run_time;
extern volatile GyroscopeFlagStruct gyroscope;
extern volatile MotorFlagStruct motor;
extern volatile ChassisStruct chassis;

/* 全局变量 */
extern INT8U *Err_MotorDetect;
extern OS_FLAG_GRP *Motor_Detect;

//以下二维数组均按position,speed,current顺序建立
extern float Motor_Para[3][4];//电机参数
extern u8 Detect_Flag[3][4];//UNUSED,MORE,LESS,EQUAL
extern u8*Det_Pointer;
extern float Detect_Para[3][4];//检测触发数值
extern float*Det_Para_Pointer;
extern int beep;
extern void (*Func[3])(u8,u8);




/* 结构体 */
typedef struct//标志位结构体
{
	u8	Task_Begin_Flag;//FALSE/TRUE
	u8	Status_Controller;//WELL,WAR_Status,ERR_Status
	u16	Warning_Flag;
	u16	Error_Flag;
	u8	Mode_Run;//DISABLE,ENABLE,BRAKE
	u8	Mode_Red_Blue;//UNSELECTED,REDMODE,BULEMODE
	u8	Mode_Auto;//ENABLE/DISABLE
	u8	Motor_Emer_Flag;//TRUE/FALSE
	s32	Motor_Emer_Code;
	/* 通信 */
	u16	Can1_ErrNode;
	u16	Can2_ErrNode;
	u8	Can1DetectFlag;//ENABLE/DISABLE
	u8	Can2DetectFlag;//ENABLE/DISABLE
	int	Can1_Tx_NoMailBox;
	int	Can2_Tx_NoMailBox;
	int	Usart1DmaSendFinish;
	int	Usart2DmaSendFinish;
	int	Usart3DmaSendFinish;
	int	Usart4DmaSendFinish;
	int	Usart5DmaSendFinish;
}FlagStruct;

typedef struct//运动参数设置结构体
{
	float x_want;
	float y_want;
	float angle_want;
	float v_start;
	float v_want;
	float v_end;
	u8		line_flag;
	u8		direction_flag;
	int		time;
}MoveStruct;


extern FlagStruct FlagPara;
extern MoveStruct MovePara;

/* CAN */
extern Can_Sendqueue can1_sendqueue;
extern Can_Sendqueue can2_sendqueue;

extern MesgControlGrp CAN1_MesgSentList[CAN1_NodeNumber];
extern MesgControlGrp CAN2_MesgSentList[CAN2_NodeNumber];

/* Usart */
extern Usart_Struct USART;

void ParaInit(void);
void EncodeFloatData(float* f, unsigned char* buff);
void DncodeFloatData(float* f, unsigned char* buff);

#endif


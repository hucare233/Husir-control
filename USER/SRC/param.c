#include "param.h"

volatile RunFlagStruct run;
volatile TimeStruct run_time;
volatile GyroscopeFlagStruct gyroscope;
volatile MotorFlagStruct motor;
volatile ChassisStruct chassis;

/* 全局变量 */
INT8U *Err_MotorDetect;
OS_FLAG_GRP *Motor_Detect;

//以下二维数组均按position,speed,current顺序建立
float Motor_Para[3][4]={0};//电机实际参数
u8 Detect_Flag[3][4]={0};//检测标志位
u8*Det_Pointer=&(Detect_Flag[0][0]);//检测报文当前顺序指针
float Detect_Para[3][4]={0};//检测触发值
float*Det_Para_Pointer=&Detect_Para[0][0];
int beep=0;
void (*Func[3])(u8,u8)={Elmo_Query_Position,Elmo_Query_Position,Elmo_Query_Position};

/* 结构体 */
FlagStruct FlagPara;
MoveStruct MovePara={0};

/* CAN */
Can_Sendqueue can1_sendqueue;
Can_Sendqueue can2_sendqueue;

MesgControlGrp CAN1_MesgSentList[CAN1_NodeNumber];
MesgControlGrp CAN2_MesgSentList[CAN2_NodeNumber];

/* Usart */
Usart_Struct USART;

void ParaInit()
{
		/* init motor struct */
	motor.encoder_resolution = 4096; // EC 30: 1000   U10: 4096
	motor.reduction = 1.0f;			 // EC 30: 169/9 = 18.7778  U10: 1

	chassis.WheelD = 15.1f;
	chassis.K_V_motor = motor.reduction * 60.0f / (PI * chassis.WheelD);
	chassis.static_friction_coefficient = 0.7f;
	chassis.dynamic_friction_coefficient = 0.6f;

	run.speed_max_limited = 100.0f; // user should set or reset the limited speed befor your run_task to ensure your and robot's serity

	/* init gyroscope struct */
	gyroscope.init_success = FALSE;
	gyroscope.state = ERR_Status;
	
	FlagPara.Task_Begin_Flag=FALSE;
	FlagPara.Status_Controller=WELL;
	FlagPara.Warning_Flag=0;
	FlagPara.Error_Flag=0;
	FlagPara.Mode_Run=BRAKE;
	FlagPara.Mode_Red_Blue=UNSELECTED;
	FlagPara.Mode_Auto=DISABLE;
	FlagPara.Can1DetectFlag=DISABLE;
	FlagPara.Can2DetectFlag=DISABLE;
	FlagPara.Motor_Emer_Flag=FALSE;
	FlagPara.Motor_Emer_Code=0;
	
	FlagPara.Usart1DmaSendFinish=TRUE;
	FlagPara.Usart2DmaSendFinish=TRUE;
	FlagPara.Usart3DmaSendFinish=TRUE;
	FlagPara.Usart4DmaSendFinish=TRUE;
	FlagPara.Usart5DmaSendFinish=TRUE;
	
}

void EncodeFloatData(float* f, u8* buff)
{
	s32 date;
	date=* f;
	*buff=date&0x000000FF;
	*(buff+1)=(date&0x0000FF00)>>8;
	*(buff+2)=(date&0x00FF0000)>>16;
	*(buff+3)=(date&0xFF000000)>>24;
}

void DncodeFloatData(float* f, unsigned char* buff)
{
	s32 date;
	date=*buff|(*(buff+1)<<8)|(*(buff+2)<<16)|(*(buff+3)<<24);
	* f=date;
}



/*** 
 * @Date: 2020-07-25 10:35:58
 * @LastEditors   未定义
 * @LastEditTime  2020-09-25 16:47:48
 * @FilePath      \Project\User\inc\MyStruct.h
 */
#ifndef __MY_STRUCT_H
#define __MY_STRUCT_H
#include "stm32f4xx.h"

//TODO:必须重构结构体，对各个部分都需要好好分类，

//这个数据，是不是可以通过监控一波来给定一个size
#define CAN_SENDQUEUESIZE 30
#define CAN_HAVESENDQUEUESIZE 5
#define USART_SENDQUEUESIZE 500

//表示flag的状态，0表示假，1表示真，具有普适性
#define True 1
#define False 0
#define Fine 1
#define Error 0
#define On 1
#define Off 0

enum _ErrFlag__
{
	FuncIDErr = 0, //     0x00
	SubIDtan90,	   // 	  0x01
	DataLengthErr, //     0x02
	SubIDUseErr,   //     0x03
	DataErr		   //  	  0x04
};

//底盘坐标与角度
typedef struct _POSITION
{
	/* data */
	float x; //x axis
	float y; //y axis
	float angle;
} Position;

//通用笛卡尔坐标系XY坐标
typedef struct _COORDINATES
{
	/* data */
	float x; //x axis
	float y; //y axis
} Coordinates;

//底盘速度矢量
typedef struct _VELOCITY
{
	/* data */
	float V;
	float Vx;
	float Vy;
	float Vw;
} Velocity;

//贝塞尔曲线上点的相应属性
typedef struct _BEZIERPOINT
{
	/* data */
	float x;		 //x axis
	float y;		 //y axis
	float t;		 //t in bezier_T
	float curvature; //曲率
} BezierPoint;

//Bezier曲线描述结构体TODO:
typedef struct _BEZIERSTRUCT
{
	/* data */
	Coordinates P[4]; //四个控制点
	float x_t[4];	  //x参数方程的系数
	float y_t[4];	  //y参数方程的系数
} BezierStruct;

//单个舵轮的运动状态量
typedef struct _WHEELINFO
{
	/* data */
	float V;
	float Vx;
	float Vy;
	float Angle; //由vx与vy计算
	u8 quadrant; //根据正负判定象限
} WheelInfo;

//pid结构体
typedef struct _PIDSTRUCT
{
	/* data */
	float kp;
	float ki;
	float kd;
	/* 增量式 */
	float err_last; //偏差值
	float err_now;
	float err_before;
	/* 位置式 */
	float err_sum;
	/* 输出 */
	float out;
} PidStruct;

//time结构体，存放一些监控的时间变量
typedef struct _TIMESTRUCT
{
	/* flag */
	u8 count_time;
	/* data */
	float clk;
	float run_start;
	float run_end;
	float run_use;
	float up_start;
	float up_end;
	float up_use;
	float down_start;
	float down_end;
	float down_use;
} TimeStruct;

//DT测距数据相应结构体
typedef struct _DTSTRUCT
{
	/* 不同方向的DT数据 */
	float x1_dis;
	float x2_dis;
	float y1_dis;
	float y2_dis;
	/* flag */
	u8 FlagDt50_1;
} DTStruct;

//调试定位结构体
typedef struct _DEBUGPOSISTRUCT
{
	/* data */
	u8 xyr; //debug_mode：x or y or rotate
	float speed_max_limited;
	float speed_start;
	float speed_rotate; //自转速度
	float points[2][3];
	float points_zero[3];
	float dis_up; //distance for speed up
	float dis_down;
} DebugPosiStruct;

enum _DEBUG_POSI
{
	x_dir,
	y_dir,
	rotate_dir,
};

//can & usart queue flag，
typedef struct _QUEUEFLAGSTRUCT
{
	/* flag */
	u8 can1_queue_full;
	u8 can2_queue_full;
	u8 can_queue_empty;
	u8 usart_queue_empty;
	u8 usart_queue_full;
	u8 Can1ControlList; //FIXME:
	u8 Can2ControlList;
	u8 Can1_ErrNode;
	u8 Can2_ErrNode;
	u8 Can1_Tx_NoMailBox;
	u8 Can2_Tx_NoMailBox;
} QueueFlagStruct;

//flags of RunPoint or run_bizier
typedef struct _RUNFLAGSTRUCT
{
	/* flag */
	u8 speed_up;	  //是否需要加速
	u8 speed_up_over; //加速完成标志
	u8 speed_up_mode; //加速方式
	u8 speed_down;
	u8 speed_down_over;		   //减速完成标志
	u8 speed_down_mode;		   //减速方式
	u8 speed_down_mode_change; //啥玩意，保留，待删除//FIXME:
	u8 rotate;
	u8 rotate_over;	  //旋转完成标志
	u8 follow_angle;  //贝塞尔上跟随曲线切线角度
	u8 continue_next; //继续下一次运动，不停下

	u8 run_break;	  //意外中断当前run进程flag
	u8 no_line_K;	  //木得斜率
	u8 auto_type;	  //RunPoint or RunBezier
	u8 pid_line_type; //直线PID调节方式，调X还是调Y,  0  ，1：斜率小于1,2， ,斜率大于1,斜率不存在
	u8 pid_mode;	  //pid的多种模式，需要集成一下，酝酿一波这好几种pid效果;resevered
	u8 pid_run;		  //对run进行pid跑
	u8 pid_point;	  //对point进行pid锁点

	/* data */
	float line_k;	   //直线斜率
	float line_b;	   //直线截距
	float line_k_cos;  //直线倾角余弦值
	float speed_want;  // 每阶段匀速保持速度
	float speed_end;   // 每阶段最后速度
	float speed_start; // 每阶段起步速度
	float path_angle;
	float delta_t;
	float up_t;	  //加速到的t值
	float down_t; //开始减速时的t值
	float v_start_brake;
	float Proportion;
	float up_dis;			 //加速距离
	float down_dis;			 //减速距离
	float goal_dis;			 //目标总距离
	float rotate_start;		 //开始旋转角度的距离
	float rotate_need;		 //旋转需要走多少距离
	float speed_max_limited; //限制最大速度
	float now_dis;			 //reserved
} RunFlagStruct;

enum _ENUM_RUN_
{
	/* speed_down_mode */
	SQUAREROOT = 0,
	LINE,
	SQUARE,
	CUBE,
	CUBEBUFFER,

	/* auto_type */
	RUNPOINT,
	RUNBEZIER,

	/* pid_line_type */
	pid_line_x,
	pid_line_y,
};

//flags of elmo & motor
enum _MOTORSTATE
{
	breaking,
	enable,
	disable,
};

typedef struct _MOTORFLAGSTRUCT
{
	/* flag & data */
	enum _MOTORSTATE state;
	u32 encoder_resolution;
	float reduction;
} MotorFlagStruct;

//flags of gyroscope
typedef struct _GYROSCOPEFLAGSTRUCT
{
	/* flag */
	u8 check_drift;
	u8 init_success;
	u8 state;
	u8 angle_err;
	/* data */
	float angle_check[2];
	u16 over_time;
} GyroscopeFlagStruct;

//flags of camera
typedef struct _CAMERAFLAGSTRUCT
{
	/* flag */
	u8 read_data;
	u8 on_off;
} CameraFlagStruct;

typedef enum
{
	USART1_Tx_BufferSize=51,//串口一发送数据长度
	USART2_Tx_BufferSize=600,//串口二发送数据长度
	USART3_Tx_BufferSize=32,//串口三发送数据长度
	
	USART1_Rx_BufferSize=32,//串口一接收数据长度（接收中断内使用）
	USART2_Rx_BufferSize=32,//串口二接收数据长度（接收中断内使用）
	USART3_Rx_BufferSize=32,//串口三接收数据长度（接收中断内使用）
	
}Usart_Enum;

typedef struct
{
	u8 TxBuffer_UASRT1[USART1_Tx_BufferSize];//串口一发送数据
	u8 RxBuffer_USART1[USART1_Rx_BufferSize];//串口一接收数据	

	u8 TxBuffer_UASRT2[USART2_Tx_BufferSize];//串口二发送数据
	u8 RxBuffer_USART2[USART2_Rx_BufferSize];//串口二接收数据

	u8 TxBuffer_UASRT3[USART3_Tx_BufferSize];//串口三发送数据
	u8 RxBuffer_USART3[USART3_Rx_BufferSize];//串口三接收数据
	
}Usart_Struct;

//flags of usart_dma
typedef struct _USARTDMAFLAGSTRUCT
{
	/* flag */
	u8 Usart1DmaSendFinish;
	u8 Usart2DmaSendFinish;
	u8 Usart3DmaSendFinish;
	u8 Usart4DmaSendFinish;
	u8 Usart5DmaSendFinish;
} UsartDmaFlagStruct;

//flags of mechanism
typedef struct _MECHANISMFLAGSTRUCT
{
	/* flag */
	u8 Grasp_Mechanism3508Status;
	u8 Chain_Mechanism3508Status;
	u8 Pitch_Mechanism3508Status;
	u8 Token_Mechanism3508Status;
	u8 ServoStatus;
	u8 ResetToken;
	u8 ResetTokenRun;
	u8 RunPointShot;
	u8 AdjustShotPoint;
	u8 runShotFlag;
} MechanismFlagStruct;

//串口屏相应结构体
typedef struct _LCDSTRUCT
{
	/* flag */
	u8 screen_id;	  //当前画面id
	u8 switch_screen; //是否切换画面
	u8 show_area;	  //显示红蓝场
	u8 show_speed;	  //显示速度在仪表盘
	u8 read_id;		  //上电后读取手柄当前id
} LcdStruct;

//flags of user
typedef struct _USERFLAGSTRUCT
{
	/* flag */
	u8 identity;	   // master or etc..
	u8 run_mode;	   // auto or ps2
	s8 area_side;	   //red or blue, : now_side,
	s8 area_side_last; // last_side. (for LCD show)
	u8 debug_mode;	   //point or bezier or ps2_play
	/* data */
	char error[20];
	u16 overtime;
} UserFlagStruct;

//enum for user
enum _USER
{
	Master = 0,
	Location = 1,
	Mechanism = 2,
	Other = 4,

	auto_mode,
	ps2_mode,

	red_area = -1,
	blue_area = 1,

	debug_point,
	debug_bezier,
};

typedef struct _CHASSIS
{
	/* some parameters of the chassis */
	/* 舵轮 */
	float width_half;  // 四个轮子组成的矩形在y轴上投影总长度一半
	float length_half; // 四个轮子组成的矩形在x轴上投影总长度一半
	/* 全向轮 */
	float omni_wheel2center; // 全向轮距离车中心的垂直距离

	float WheelD;						// 轮径 cm
	float reduction_turn;				// 舵齿轮减速比
	float K_V_motor;					// 转换脉冲系数FIXME:  Reduction*60.0f/(PI*WheelD)
	float static_friction_coefficient;	// 静摩擦系数
	float dynamic_friction_coefficient; // 静摩擦系数

} ChassisStruct;

// typedef struct _FLAGSTRUCT
// {
// 	/* data */
// 	//不知道是啥的东西，保留
// 	u8 ResetNum;
// 	u8 ResetRun;
// 	u8 ShotPosition;
// 	u8 FirstRun;
// 	u8 DataErr;
// 	u8 count_time_flag;
// 	u8 EmergencyStart;
// 	u8 CountStart;
// 	u8 PowerSwitch;
// 	u8 BeepNum;
// 	u8 PidFinish;
// 	u8 ContinueRun;

// } FlagStruct;

/* 简化过的CAN报文 */
typedef struct _CAN_SENDSTRUCT
{
	u32 Id;
	u8 DLC;
	u8 Data[8];
	u8 InConGrpFlag;
} CanSendStruct;

/*************************************简化过的Usart报文************************************** */
typedef struct _USARTSENDSTRUCT
{
	u8 DLC;
	char Data[5];
	u8 InConGrpFlag;
} UsartSendstruct;

/* CAN报文队列 */
typedef struct _Can_HaveSendqueue
{
	u8 Can_sendqueuesize;
	u8 Front, Rear;
	CanSendStruct node[CAN_HAVESENDQUEUESIZE];
} Can_HaveSendqueue;

/* CAN报文队列 */
typedef struct _Can_Sendqueue
{
	u8 Can_sendqueuesize;
	u16 Front, Rear;
	CanSendStruct node[CAN_SENDQUEUESIZE];
} CanSendqueue;

typedef struct _Usart_Sendqueue
{
	u16 usart_sendqueuesize;
	u16 length;
	u16 Front, Rear;
	uint8_t Data[USART_SENDQUEUESIZE];
} UsartSendqueue;

typedef struct _MesgControlGrp
{
	s32 SendNumber;
	s32 ReceiveNumber;
	u32 QUEUEFullTimeout;
	u8 TimeOut;
	s32 SendSem;
	Can_HaveSendqueue SentQueue;
} MesgControlGrp;

#endif

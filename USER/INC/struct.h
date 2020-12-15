#ifndef __STRUCT_H
#define __STRUCT_H
#include "stm32f4xx.h"
//本文件定义CAN、USART、RUN相关量
#define CAN1_NodeNumber				 4u
#define CAN2_NodeNumber				 8u

#define CAN_SENDQUEUESIZE 		 50

/*** CAN_Related ***/	
typedef struct//简化过的CAN报文/
{
  u32 Id;
  u8 DLC;
  u8 Data[8];
	u8 WaitFlag;//FALSE/TRUE
}Can_Sendstruct;

typedef struct//CAN报文队列
{
	uint8_t Can_sendqueuesize;
	uint16_t Front,Rear;
	Can_Sendstruct head[CAN_SENDQUEUESIZE];	
}Can_Sendqueue;

typedef struct//CAN报文控制列表
{
		s32 SendNumber;
		s32	ReceiveNumber;
		u32 QUEUEFullTimeout;
		u8  TimeOut;
		s32	SendSem;       
}MesgControlGrp;

/*** USART_Related ***/	
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

/*** USART_Related ***/	
typedef struct
{
	float x;
	float y;
	float angle;
}Position;

typedef struct
{
	float x;
	float y;
}Coordinates;

typedef struct//底盘速度矢量
{
	float V;
	float Vx;
	float Vy;
	float Vw;
}Velocity;

typedef struct//单个舵轮的运动状态量
{
	/* data */
	float V;
	float Vx;
	float Vy;
	float Angle; //由vx与vy计算
	u8 quadrant; //根据正负判定象限
} WheelInfo;

typedef struct//pid结构体
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

typedef struct//time结构体，存放一些监控的时间变量
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

typedef struct//DT测距数据相应结构体
{
	/* 不同方向的DT数据 */
	float x1_dis;
	float x2_dis;
	float y1_dis;
	float y2_dis;
	/* flag */
	u8 FlagDt50_1;
} DTStruct;

typedef struct//调试定位结构体
{
	/* data */
	u8 xyr;			//debug_mode：x or y or rotate
	u8 process_num; // num of process

	float speed_max_limited;
	float speed_start;
	float speed_rotate; //自转速度
	float points[2][3];
	float points_zero[3];
	float dis_up; //distance for speed up
	float dis_down;
} DebugPosiStruct;

typedef struct //跑动标志位
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

enum
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

typedef struct
{
	/* flag & data */
	u32 encoder_resolution;
	float reduction;
} MotorFlagStruct;

typedef struct//flags of gyroscope
{
	/* flag */
	u8 check_drift;
	u8 init_success;
	u8 state;
	/* data */
	float angle_check[2];
	u16 over_time;
} GyroscopeFlagStruct;

typedef struct//flags of camera
{
	/* flag */
	u8 read_data;
	u8 on_off;
} CameraFlagStruct;

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

//enum for user
enum{
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



#endif


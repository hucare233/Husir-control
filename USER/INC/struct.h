/*** 
 * @Date: 2020-07-25 10:35:58
 * @LastEditors   δ����
 * @LastEditTime  2020-09-25 16:47:48
 * @FilePath      \Project\User\inc\MyStruct.h
 */
#ifndef __MY_STRUCT_H
#define __MY_STRUCT_H
#include "stm32f4xx.h"

//TODO:�����ع��ṹ�壬�Ը������ֶ���Ҫ�ú÷��࣬

//������ݣ��ǲ��ǿ���ͨ�����һ��������һ��size
#define CAN_SENDQUEUESIZE 30
#define CAN_HAVESENDQUEUESIZE 5
#define USART_SENDQUEUESIZE 500

//��ʾflag��״̬��0��ʾ�٣�1��ʾ�棬����������
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

//����������Ƕ�
typedef struct _POSITION
{
	/* data */
	float x; //x axis
	float y; //y axis
	float angle;
} Position;

//ͨ�õѿ�������ϵXY����
typedef struct _COORDINATES
{
	/* data */
	float x; //x axis
	float y; //y axis
} Coordinates;

//�����ٶ�ʸ��
typedef struct _VELOCITY
{
	/* data */
	float V;
	float Vx;
	float Vy;
	float Vw;
} Velocity;

//�����������ϵ����Ӧ����
typedef struct _BEZIERPOINT
{
	/* data */
	float x;		 //x axis
	float y;		 //y axis
	float t;		 //t in bezier_T
	float curvature; //����
} BezierPoint;

//Bezier���������ṹ��TODO:
typedef struct _BEZIERSTRUCT
{
	/* data */
	Coordinates P[4]; //�ĸ����Ƶ�
	float x_t[4];	  //x�������̵�ϵ��
	float y_t[4];	  //y�������̵�ϵ��
} BezierStruct;

//�������ֵ��˶�״̬��
typedef struct _WHEELINFO
{
	/* data */
	float V;
	float Vx;
	float Vy;
	float Angle; //��vx��vy����
	u8 quadrant; //���������ж�����
} WheelInfo;

//pid�ṹ��
typedef struct _PIDSTRUCT
{
	/* data */
	float kp;
	float ki;
	float kd;
	/* ����ʽ */
	float err_last; //ƫ��ֵ
	float err_now;
	float err_before;
	/* λ��ʽ */
	float err_sum;
	/* ��� */
	float out;
} PidStruct;

//time�ṹ�壬���һЩ��ص�ʱ�����
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

//DT���������Ӧ�ṹ��
typedef struct _DTSTRUCT
{
	/* ��ͬ�����DT���� */
	float x1_dis;
	float x2_dis;
	float y1_dis;
	float y2_dis;
	/* flag */
	u8 FlagDt50_1;
} DTStruct;

//���Զ�λ�ṹ��
typedef struct _DEBUGPOSISTRUCT
{
	/* data */
	u8 xyr; //debug_mode��x or y or rotate
	float speed_max_limited;
	float speed_start;
	float speed_rotate; //��ת�ٶ�
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

//can & usart queue flag��
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
	u8 speed_up;	  //�Ƿ���Ҫ����
	u8 speed_up_over; //������ɱ�־
	u8 speed_up_mode; //���ٷ�ʽ
	u8 speed_down;
	u8 speed_down_over;		   //������ɱ�־
	u8 speed_down_mode;		   //���ٷ�ʽ
	u8 speed_down_mode_change; //ɶ���⣬��������ɾ��//FIXME:
	u8 rotate;
	u8 rotate_over;	  //��ת��ɱ�־
	u8 follow_angle;  //�������ϸ����������߽Ƕ�
	u8 continue_next; //������һ���˶�����ͣ��

	u8 run_break;	  //�����жϵ�ǰrun����flag
	u8 no_line_K;	  //ľ��б��
	u8 auto_type;	  //RunPoint or RunBezier
	u8 pid_line_type; //ֱ��PID���ڷ�ʽ����X���ǵ�Y,  0  ��1��б��С��1,2�� ,б�ʴ���1,б�ʲ�����
	u8 pid_mode;	  //pid�Ķ���ģʽ����Ҫ����һ�£�����һ����ü���pidЧ��;resevered
	u8 pid_run;		  //��run����pid��
	u8 pid_point;	  //��point����pid����

	/* data */
	float line_k;	   //ֱ��б��
	float line_b;	   //ֱ�߽ؾ�
	float line_k_cos;  //ֱ���������ֵ
	float speed_want;  // ÿ�׶����ٱ����ٶ�
	float speed_end;   // ÿ�׶�����ٶ�
	float speed_start; // ÿ�׶����ٶ�
	float path_angle;
	float delta_t;
	float up_t;	  //���ٵ���tֵ
	float down_t; //��ʼ����ʱ��tֵ
	float v_start_brake;
	float Proportion;
	float up_dis;			 //���پ���
	float down_dis;			 //���پ���
	float goal_dis;			 //Ŀ���ܾ���
	float rotate_start;		 //��ʼ��ת�Ƕȵľ���
	float rotate_need;		 //��ת��Ҫ�߶��پ���
	float speed_max_limited; //��������ٶ�
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
	USART1_Tx_BufferSize=51,//����һ�������ݳ���
	USART2_Tx_BufferSize=600,//���ڶ��������ݳ���
	USART3_Tx_BufferSize=32,//�������������ݳ���
	
	USART1_Rx_BufferSize=32,//����һ�������ݳ��ȣ������ж���ʹ�ã�
	USART2_Rx_BufferSize=32,//���ڶ��������ݳ��ȣ������ж���ʹ�ã�
	USART3_Rx_BufferSize=32,//�������������ݳ��ȣ������ж���ʹ�ã�
	
}Usart_Enum;

typedef struct
{
	u8 TxBuffer_UASRT1[USART1_Tx_BufferSize];//����һ��������
	u8 RxBuffer_USART1[USART1_Rx_BufferSize];//����һ��������	

	u8 TxBuffer_UASRT2[USART2_Tx_BufferSize];//���ڶ���������
	u8 RxBuffer_USART2[USART2_Rx_BufferSize];//���ڶ���������

	u8 TxBuffer_UASRT3[USART3_Tx_BufferSize];//��������������
	u8 RxBuffer_USART3[USART3_Rx_BufferSize];//��������������
	
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

//��������Ӧ�ṹ��
typedef struct _LCDSTRUCT
{
	/* flag */
	u8 screen_id;	  //��ǰ����id
	u8 switch_screen; //�Ƿ��л�����
	u8 show_area;	  //��ʾ������
	u8 show_speed;	  //��ʾ�ٶ����Ǳ���
	u8 read_id;		  //�ϵ���ȡ�ֱ���ǰid
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
	/* ���� */
	float width_half;  // �ĸ�������ɵľ�����y����ͶӰ�ܳ���һ��
	float length_half; // �ĸ�������ɵľ�����x����ͶӰ�ܳ���һ��
	/* ȫ���� */
	float omni_wheel2center; // ȫ���־��복���ĵĴ�ֱ����

	float WheelD;						// �־� cm
	float reduction_turn;				// ����ּ��ٱ�
	float K_V_motor;					// ת������ϵ��FIXME:  Reduction*60.0f/(PI*WheelD)
	float static_friction_coefficient;	// ��Ħ��ϵ��
	float dynamic_friction_coefficient; // ��Ħ��ϵ��

} ChassisStruct;

// typedef struct _FLAGSTRUCT
// {
// 	/* data */
// 	//��֪����ɶ�Ķ���������
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

/* �򻯹���CAN���� */
typedef struct _CAN_SENDSTRUCT
{
	u32 Id;
	u8 DLC;
	u8 Data[8];
	u8 InConGrpFlag;
} CanSendStruct;

/*************************************�򻯹���Usart����************************************** */
typedef struct _USARTSENDSTRUCT
{
	u8 DLC;
	char Data[5];
	u8 InConGrpFlag;
} UsartSendstruct;

/* CAN���Ķ��� */
typedef struct _Can_HaveSendqueue
{
	u8 Can_sendqueuesize;
	u8 Front, Rear;
	CanSendStruct node[CAN_HAVESENDQUEUESIZE];
} Can_HaveSendqueue;

/* CAN���Ķ��� */
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

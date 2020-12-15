#ifndef __STRUCT_H
#define __STRUCT_H
#include "stm32f4xx.h"
//���ļ�����CAN��USART��RUN�����
#define CAN1_NodeNumber				 4u
#define CAN2_NodeNumber				 8u

#define CAN_SENDQUEUESIZE 		 50

/*** CAN_Related ***/	
typedef struct//�򻯹���CAN����/
{
  u32 Id;
  u8 DLC;
  u8 Data[8];
	u8 WaitFlag;//FALSE/TRUE
}Can_Sendstruct;

typedef struct//CAN���Ķ���
{
	uint8_t Can_sendqueuesize;
	uint16_t Front,Rear;
	Can_Sendstruct head[CAN_SENDQUEUESIZE];	
}Can_Sendqueue;

typedef struct//CAN���Ŀ����б�
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

typedef struct//�����ٶ�ʸ��
{
	float V;
	float Vx;
	float Vy;
	float Vw;
}Velocity;

typedef struct//�������ֵ��˶�״̬��
{
	/* data */
	float V;
	float Vx;
	float Vy;
	float Angle; //��vx��vy����
	u8 quadrant; //���������ж�����
} WheelInfo;

typedef struct//pid�ṹ��
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

typedef struct//time�ṹ�壬���һЩ��ص�ʱ�����
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

typedef struct//DT���������Ӧ�ṹ��
{
	/* ��ͬ�����DT���� */
	float x1_dis;
	float x2_dis;
	float y1_dis;
	float y2_dis;
	/* flag */
	u8 FlagDt50_1;
} DTStruct;

typedef struct//���Զ�λ�ṹ��
{
	/* data */
	u8 xyr;			//debug_mode��x or y or rotate
	u8 process_num; // num of process

	float speed_max_limited;
	float speed_start;
	float speed_rotate; //��ת�ٶ�
	float points[2][3];
	float points_zero[3];
	float dis_up; //distance for speed up
	float dis_down;
} DebugPosiStruct;

typedef struct //�ܶ���־λ
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



#endif


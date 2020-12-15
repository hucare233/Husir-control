#ifndef __FLAG_H
#define __FLAG_H

enum{UNSELECTED,RED,BLUE};/* Red or Blue */
enum{WELL,WAR_Status,ERR_Status};/* Status */
enum{/*DISABLE,ENABLE*/BRAKE=2};/* Run Mode */
enum{FuncIDErr,SubIDtan90,DataLengthErr,SubIDUseErr,DataErr};/* ErrFlag */
enum{CURRENTMODE = 1,SPEEDMODE = 2,POSITIONMODE = 5};/* Elmo Mode */
enum{UNUSED,MORE,LESS,EQUAL};/* Detect mode */
enum{Det_Pos_1,Det_Pos_2,Det_Pos_3,Det_Pos_4,Det_Spe_1,Det_Spe_2,Det_Spe_3,Det_Spe_4,Det_Cur_1,Det_Cur_2,Det_Cur_3,Det_Cur_4};/* Detect_index*/

/* 警告占位 */
#define NO_MAILBOX                     0x0001
#define MOTOR_WAR                      0x0002

/* 错误占位 */
#define CAN1_QUEUE_FULL                0x0001
#define CAN2_QUEUE_FULL                0x0002
#define CAN1_NODE_LOSE                 0x0004
#define CAN2_NODE_LOSE                 0x0008
#define OFF_TRACK                      0x0010
#define MOTOR_ERR                      0x0020

/* 定位子板ID */
#define ID_GyrScope_RX						 0x00020101
#define ID_DT5035X_RX							 0x00040101
#define ID_DT5035Y_RX							 0x00090101

/* 驱动电机ID */
#define ID_Drive_Motor1_RX              0x281  
#define ID_Drive_Motor2_RX            	0x282
#define ID_Drive_Motor3_RX       	    	0x283
#define ID_Drive_Motor4_RX              0x284

/* 转向电机ID */
#define ID_Streeing_Motor5_RX           0x285				
#define ID_Streeing_Motor6_RX           0x286 				
#define ID_Streeing_Motor7_RX           0x287				
#define ID_Streeing_Motor8_RX           0x288 

/* CANControlist_related */
#define Rear2 		  ((can2_sendqueue.Rear+1)%can2_sendqueue.Can_sendqueuesize)
#define Rear1 		  ((can1_sendqueue.Rear+1)%can1_sendqueue.Can_sendqueuesize)
#define CAN_1 			0
#define CAN_2				1

/* 常用函数 */
#define ABS(x) ((x)>0? (x):(-(x)))//取绝对值
#define SIG(x) ((x)>=0? 1:-1)	//判断正负
#define CountDistance(x_from, y_from, x_to, y_to)   (float)(sqrt((x_to - x_from) * (x_to - x_from) + (y_to - y_from) * (y_to - y_from)))//计算两点间距离

/* 特殊值 */
#define PI ((float)3.141592654)
#define g  (9.8)
#define cos_30 0.8660254f
#define cos_60 0.5f
#define cos_45 0.7071067f
#define sin_30 0.5f
#define sin_60 0.8660254f
#define sin_45 0.7071067f

#endif

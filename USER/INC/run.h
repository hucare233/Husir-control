#ifndef __RUN_H_
#define __RUN_H_
#include "stm32f4xx.h"
#include "param.h"
#include "MyMath.h"
#include "beep.h"
#include "pid.h"
#include "includes.h"
#include "motor.h"

#define STEER // steer or omin ....etc
#define ELMO  // elmo or epos

extern volatile Position position_start;
extern volatile Position position_goal;
extern volatile Position position_want;
extern volatile Position lock_point;
extern volatile Position Radar;
extern volatile Position position_now;
extern volatile Position start_turn_point;

extern volatile Velocity velocity_now;
extern volatile Velocity velocity_start;
extern volatile Velocity velocity_goal;
extern volatile Velocity velocity_set;
extern volatile Velocity speed_max_limited;
extern volatile Velocity velocity_abs;
extern volatile WheelInfo Wheel[4];

extern BezierStruct BezierData; //Bezier������ز���

//�����������߼������̬·����

extern volatile BezierPoint bezier_point1;
extern volatile BezierPoint bezier_point2;
static void set_wheel_speed(float wheelspeed[3]);
void calculate_wheel_speed(Velocity speed_wantset);
void RunPointSpeedUp(void);
void RunPointSpeedDown(void); //�ú��о��˶�ѧ�㷨
void RunPointRotate(void);    //��ֱ�����߱���ת����ת�ӳ���
void SetPointSpeedBase(void); //����PID����֮ǰ��ԭʼ�ٶ�
void SetChassisVelocity(Velocity volatile absolutespeed, float angle);
void RunBezierSpeedDown(void);
void RunBezierSpeedUp(void);
void CountBezierXYbyT(BezierStruct volatile *bezier_para, BezierPoint volatile *BezierPoint);
void RunBezierRotate(void);
void RunBezierFollowAngle(void);
void SetBezier(float p0x, float p0y, float p1x, float p1y, float p2x,
               float p2y, float p3x, float p3y);
void CountBezierCurvaturebyT(BezierStruct volatile *bezier_para, BezierPoint volatile *BezierPoint);
void RunBezier(float *points, float angle_want,
               float v_start, float v_want, float v_end,
               float upt, float downt, float startturnt, float fullturnt,
               u8 continue_flag, u8 follow_angle_flag);
void RunPoint(float x_want, float y_want, float angle_want,
              float v_start, float v_want, float v_end,
              float up_dis, float down_dis, float start_rotate_dis, float rotate_need,
              u8 continue_flag);
void Rotate(float angle_want, float v_w, u8 continue_next);
static void set_wheel(void);
#endif

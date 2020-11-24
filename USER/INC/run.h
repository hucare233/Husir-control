#ifndef __RUN_H_
#define __RUN_H_

#include "stm32f4xx.h"
#include "param.h"
#include "beep.h"
#include "pid.h"
#include "includes.h"
#include "motor.h"
#include "math.h"

/* Position */
extern volatile Position position_start;
extern volatile Position position_goal;
extern volatile Position position_want;
extern volatile Position lock_point;
extern volatile Position Radar;
extern volatile Position position_now;
extern volatile Position start_turn_point;
/* Velocity */
extern volatile Velocity velocity_now;
extern volatile Velocity velocity_start;
extern volatile Velocity velocity_goal;
extern volatile Velocity velocity_set;
extern volatile Velocity speed_max_limited;
extern volatile Velocity velocity_abs;
extern volatile WheelInfo Wheel[4];



void RunPoint(float x_want, float y_want, float angle_want,float v_start, float v_want, float v_end,float up_dis, float down_dis, float start_rotate_dis, float rotate_need,u8 continue_flag);
void Rotate(float angle_want, float v_w, u8 continue_next);

void RunPointSpeedUp(void);
void RunPointSpeedDown(void); 
void RunPointRotate(void);    
void SetPointSpeedBase(void); 

void calculate_wheel_speed(Velocity speed_wantset);
void SetChassisVelocity(Velocity volatile absolutespeed, float angle);
static void set_wheel(void);
#endif

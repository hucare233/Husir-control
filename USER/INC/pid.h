#ifndef __PID_H_
#define __PID_H_
#include "stm32f4xx.h"
#include "param.h"
#include "run.h"

#define bgroup1 1
#define bgroup2 2
#define bgroup3 3
/* 位置式 */
#define position_pid(pid) pid.kp *pid.err_now + pid.ki *pid.err_sum + pid.kd *(pid.err_now - pid.err_last)
#define position_pd(pid) pid.kp *pid.err_now + pid.kd *(pid.err_now - pid.err_last)
#define position_pi(pid) pid.kp *pid.err_now + pid.ki *pid.err_sum

/* 增量式 */
#define incremental_pid(pid) pid.kp *(pid.err_now - pid.err_last) + pid.ki *pid.err_now + pid.kd *(pid.err_now - 2 * pid.err_last + pid.err_before)
#define incremental_pd(pid) pid.kp *(pid.err_now - pid.err_last) + pid.kd *(pid.err_now - 2 * pid.err_last + pid.err_before)
#define incremental_pi(pid) pid.kp *(pid.err_now - pid.err_last) + pid.ki *pid.err_now

#define update_pid_para(pid, para) \
    {                              \
        pid.kp = para[0];          \
        pid.ki = para[1];          \
        pid.kd = para[2];          \
    }

extern unsigned char bezier_group;

extern volatile PidStruct run_pid;
extern volatile PidStruct lock_point_pid_x;
extern volatile PidStruct lock_point_pid_y;
extern volatile PidStruct angle_pid;

extern Coordinates vec_line;

extern float pid_para_run_point[3];
extern float pid_para_run_bezier[3];
extern float pid_para_angle[3];
extern float pid_para_lock_point_x[3];
extern float pid_para_lock_point_x1[3];
extern float pid_para_lock_point_x2[3];
extern float pid_para_lock_point_y[3];
extern float pid_para_lock_point_y1[3];
extern float pid_para_lock_point_y2[3];

void Pid_Act(void);
void PidInit(volatile PidStruct *pid_set, float *pid_para);
float PidOpreration(void);
void PidLockAngle(float lock_angle);
#endif

/********************************End of File************************************/

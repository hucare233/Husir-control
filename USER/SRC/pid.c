#include "pid.h"

volatile float Ek[3];
volatile float EkX[3];
volatile float EkY[3];
volatile float EkAngle[2];

volatile PidStruct run_pid = {0.0f};
volatile PidStruct lock_point_pid_x = {0.0f};
volatile PidStruct lock_point_pid_y = {0.0f};
volatile PidStruct angle_pid = {0.0f};

/* �˶�PID���� */
float pid_para_run_point[3] = {11.0f, 0.05f, 0.0000f};
float pid_para_run_bezier[3] = {5.0f, 0.01f, 0.0000f};
float pid_para_angle[3] = {20.0f, 0.10f, 0.01f};

/* ����PID���� */
float pid_para_lock_point_x[3] = {4.0f, 0.001f, 0.0000f};
float pid_para_lock_point_x1[3] = {4.8f, 0.0015f, 0.0001f};
float pid_para_lock_point_x2[3] = {3.9f, 0.0001f, 0.002f};
float pid_para_lock_point_y[3] = {4.0f, 0.001f, 0.000f};
float pid_para_lock_point_y1[3] = {4.7f, 0.0015f, 0.001f};
float pid_para_lock_point_y2[3] = {4.0f, 0.0f, 0.01f};

unsigned char bezier_group;

/** 
  * @brief  PID������ʼ��
  */
void PidInit(volatile PidStruct *pid_set, float *pid_para)
{
	pid_set->err_sum = 0;
	pid_set->err_before = 0;
	pid_set->err_last = 0;
	pid_set->err_now = 0;
	pid_set->out = 0;

	pid_set->kp = pid_para[0];
	pid_set->ki = pid_para[1];
	pid_set->kd = pid_para[2];
}

/** 
  * @brief	ֱ��PID
  */
void PidRunPoint(void)
{
	/* �Ӽ��١�ת�Ƕ� */
	if (TRUE == run.speed_up)
		RunPointSpeedUp();
	else
		velocity_set.V = run.speed_want;

	if (TRUE == run.speed_down)
		RunPointSpeedDown(); // ɲ�������⣬���ж��ﵽĿ�����ʱ��������ٶȲ�Ϊ0���ں�ʱ���ٶ�Ϊ0��ͬʱ���ٴ�˼���������������ɲ�������Ƿ�תɲ����������������ɲ����

	if (TRUE == run.rotate)
		RunPointRotate();

	/* ����pid֮ǰ�ĳ�ʼ�����ٶ�xy���� */
	SetPointSpeedBase();

	/* PID���� */
	if (pid_line_y == run.pid_line_type) //the K of line < 1
		run_pid.err_now = run.line_k * position_now.x + run.line_b - position_now.y;
	else
	{
		if (TRUE == run.no_line_K)
			run_pid.err_now = position_goal.x - position_now.x;
		else
			run_pid.err_now = position_now.y / run.line_k + run.line_b - position_now.x;
	}
	if (abs(run_pid.err_now) > 20) // ���ַ��룬��ֵ����TODO:
		run_pid.out += incremental_pi(run_pid);
	else
		run_pid.out += incremental_pid(run_pid);

	if (abs(run_pid.out) < 0.1f) // ��ֵ�����⣬����㣬���һ�¶��١�
		run_pid.out = 0;

	run_pid.err_before = run_pid.err_last;
	run_pid.err_last = run_pid.err_now;

	if (pid_line_x == run.pid_line_type)
		velocity_set.Vx += run_pid.out;
	else
		velocity_set.Vy += run_pid.out;
	velocity_set.V = sqrt(velocity_set.Vx * velocity_set.Vx + velocity_set.Vy * velocity_set.Vy); // ��pid����������ٶ������ȱ�������

	/* �ٶ����� */
	if (velocity_set.V > run.speed_max_limited)
	{
		velocity_set.Vx = run.speed_max_limited * velocity_set.Vx / velocity_set.V;
		velocity_set.Vy = run.speed_max_limited * velocity_set.Vy / velocity_set.V;
		velocity_set.V = run.speed_max_limited;
	}

	PidLockAngle(lock_point.angle); //lock the angle

	/* PID�������ٶ� */
	SetChassisVelocity(velocity_set, position_now.angle);
}

Coordinates vec_line;	//�滮�����ϵ�΢��ֱ������
Coordinates vec_actual; //ʵ��΢��ֱ������
Coordinates vec_error;	//ƫ������

/** 
  * @brief	����PID
  */
void PidLockPoint(Position volatile lock_position)
{
	lock_point_pid_x.err_now = lock_position.x - position_now.x;
	lock_point_pid_y.err_now = lock_position.y - position_now.y;
	angle_pid.err_now = lock_position.angle - position_now.angle;
	//PD����

	if (abs(lock_point_pid_x.err_now) < 0.1f)
		lock_point_pid_x.err_now = 0;
	if (abs(lock_point_pid_y.err_now) < 0.1f)
		lock_point_pid_y.err_now = 0;
	if (abs(angle_pid.err_now) < 0.1f)
		angle_pid.err_now = 0;

	if (abs(lock_point_pid_x.err_now) <= 5)
	{
		update_pid_para(lock_point_pid_x, pid_para_lock_point_x2);
		velocity_set.Vx = position_pid(lock_point_pid_x);
	}
	else if (abs(lock_point_pid_x.err_now) <= 20)
	{
		update_pid_para(lock_point_pid_x, pid_para_lock_point_x1);
		velocity_set.Vx = position_pid(lock_point_pid_x);
	}
	else
	{
		update_pid_para(lock_point_pid_x, pid_para_lock_point_x);
		velocity_set.Vx = position_pid(lock_point_pid_x);
	}

	if (abs(lock_point_pid_y.err_now) <= 5)
	{
		update_pid_para(lock_point_pid_y, pid_para_lock_point_y2);
		velocity_set.Vy = position_pid(lock_point_pid_y);
	}
	else if (abs(lock_point_pid_y.err_now) <= 20)
	{
		update_pid_para(lock_point_pid_y, pid_para_lock_point_y1);
		velocity_set.Vy = position_pid(lock_point_pid_y);
	}
	else
	{
		update_pid_para(lock_point_pid_y, pid_para_lock_point_y);
		velocity_set.Vy = position_pid(lock_point_pid_y);
	}

	velocity_set.Vw = position_pid(angle_pid);

	lock_point_pid_x.err_before = lock_point_pid_x.err_last;
	lock_point_pid_x.err_last = lock_point_pid_x.err_now;
	lock_point_pid_x.err_sum += lock_point_pid_x.err_now;
	lock_point_pid_y.err_before = lock_point_pid_y.err_last;
	lock_point_pid_y.err_last = lock_point_pid_y.err_now;
	lock_point_pid_y.err_sum += lock_point_pid_y.err_now;
	angle_pid.err_before = angle_pid.err_last;
	angle_pid.err_last = angle_pid.err_now;
	angle_pid.err_sum += angle_pid.err_now;

	//�޶��ٶ�
	velocity_set.V = sqrt(velocity_set.Vx * velocity_set.Vx + velocity_set.Vy * velocity_set.Vy);

	if (velocity_set.V > run.speed_max_limited)
	{
		velocity_set.Vx = velocity_set.Vx * run.speed_max_limited / velocity_set.V;
		velocity_set.Vy = velocity_set.Vy * run.speed_max_limited / velocity_set.V;
		velocity_set.V = run.speed_max_limited;
	}

	//�޶����ٶ�
	if (abs(velocity_set.Vw) > 130)
		velocity_set.Vw = sig(velocity_set.Vw) * 130;
	SetChassisVelocity(velocity_set, position_now.angle);
}
/**
  * @brief  ����PID
  */

void PidLockAngle(float lock_angle)
{
	angle_pid.err_now = lock_angle - position_now.angle;
	if (abs(angle_pid.err_now) < 0.1f)
		angle_pid.err_now = 0;

	velocity_set.Vw = position_pid(angle_pid);

	angle_pid.err_last = angle_pid.err_now;
	//�����ֱ���, FIXME:
	if (velocity_set.Vw > 130)
	{
		if (angle_pid.err_now < 0)
			angle_pid.err_sum += angle_pid.err_now;
	}
	else if (velocity_set.Vw < 130)
	{
		if (angle_pid.err_now > 0)
			angle_pid.err_sum += angle_pid.err_now;
	}
	else
		angle_pid.err_sum += angle_pid.err_now;

	//����޷�
	if (abs(velocity_set.Vw) > 130)
#include "stm32f4xx.h"                  // Device header
		velocity_set.Vw = sig(velocity_set.Vw) * 130;
}

/** 
  * @brief	����PID
  */
void Pid_Act(void)
{
	if (BRAKE != FlagPara.Mode_Run)
	{
		if (TRUE == run.pid_run)
		{
			switch (run.auto_type)
			{
			case RUNPOINT:
				PidRunPoint(); //line
				break;
			default:
				break;
			}
		}
		else if (TRUE == run.pid_point)
			PidLockPoint(lock_point);
	}
	else
	{
		Elmo_Stop(0,FALSE);
	}
}

//ֱ���ܵ㣬ɲ�����ܷ��򣨲��������ǣ���ԭ��ת


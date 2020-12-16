/*
 * @Descripttion: pid
 * @version: ��һ��
 * @Author: ���˵�
 * @Date: 2020-12-15 19:29:29
 * @LastEditors: ���˵�
 * @LastEditTime: 2020-12-16 10:20:26
 * @FilePath: \hu_sir-contorl\USER\SRC\pid.c
 */

#include "pid.h"

//PID�㷨:����ʽPID&���ٻ���&�����ֱ��͵ȵȣ���������������pid�Ż� TODO:

volatile float Ek[3];
volatile float EkX[3];
volatile float EkY[3];
volatile float EkAngle[2];

volatile PidStruct run_pid = {0.0f};
volatile PidStruct lock_point_pid_x = {0.0f};
volatile PidStruct lock_point_pid_y = {0.0f};
volatile PidStruct angle_pid = {0.0f};

/***************pid�������**************/
//pid para
float pid_para_run_point[3] = {11.0f, 0.05f, 0.0000f};
float pid_para_run_bezier[3] = {5.0f, 0.01f, 0.0000f};
float pid_para_angle[3] = {13.0f, 0.0f, 0.001f};

/* ����pid */
float pid_para_lock_point_x[3] = {4.0f, 0.001f, 0.0000f};
float pid_para_lock_point_x1[3] = {4.8f, 0.0015f, 0.0001f};
float pid_para_lock_point_x2[3] = {3.9f, 0.0001f, 0.002f};
float pid_para_lock_point_y[3] = {4.0f, 0.001f, 0.000f};
float pid_para_lock_point_y1[3] = {4.7f, 0.0015f, 0.001f};
float pid_para_lock_point_y2[3] = {4.0f, 0.0f, 0.01f};

unsigned char bezier_group;

/*** 
 * @brief  pid��ʼ��
 * @author ���˵�
 * @param  PidStruct
 * @return  void
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

/*** 
 * @brief  ֱ��PID������ʽ
 * @param  void
 * @return void
 * @author ���˵�
 */
void PidRunPoint(void)
{
	/* �Ӽ��١�ת�Ƕ� */
	if (True == run.speed_up)
		RunPointSpeedUp();
	else
		velocity_set.V = run.speed_want;

	if (True == run.speed_down)
		RunPointSpeedDown(); // ɲ�������⣬���ж��ﵽĿ�����ʱ��������ٶȲ�Ϊ0���ں�ʱ���ٶ�Ϊ0��ͬʱ���ٴ�˼���������������ɲ�������Ƿ�תɲ����������������ɲ����

	if (True == run.rotate)
		RunPointRotate();

	/* ����pid֮ǰ�ĳ�ʼ�����ٶ�xy���� */
	SetPointSpeedBase();

	/* PID���� */
	if (pid_line_y == run.pid_line_type) //the K of line < 1
		run_pid.err_now = run.line_k * position_now.x + run.line_b - position_now.y;
	else
	{
		if (True == run.no_line_K)
			run_pid.err_now = position_goal.x - position_now.x;
		else
			run_pid.err_now = position_now.y / run.line_k + run.line_b - position_now.x;
	}
	if (ABS(run_pid.err_now) > 20) // ���ַ��룬��ֵ����TODO:
		run_pid.out += incremental_pi(run_pid);
	else
		run_pid.out += incremental_pid(run_pid);

	if (ABS(run_pid.out) < 0.1f) // ��ֵ�����⣬����㣬���һ�¶��١�
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
/*** 
 * @brief  ����������PID
 * @param  
 * @return void
 * @author ���˵�
 */
void PidRunBezier(void)
{
	float temp_cos = 0.0f, temp_sin = 0.f, vec_line_mod = 0.0f, v_friction = 0.0f, CurveRPid = 0.0f;

	/* �Ӽ��١�ת�Ƕ� */
	if (True == run.speed_up) //������ٱ�־��Ч�����ü��ٳ���
		RunBezierSpeedUp();
	else
		velocity_set.V = run.speed_want;

	if (True == run.speed_down) //������ٱ�־��Ч�����ü��ٳ���
		RunBezierSpeedDown();

	if (True == run.rotate)
		RunBezierRotate();
	else if (True == run.follow_angle)
		RunBezierFollowAngle();

	/* ����pid֮ǰ�ĳ�ʼ�����ٶ�xy���� */
	velocity_set.Vx = velocity_set.V * temp_cos;
	velocity_set.Vy = velocity_set.V * temp_sin;

	/* �������������΢��ֱ���������� */
	vec_actual.x = position_now.x - bezier_point1.x; //����ʵ������
	vec_actual.y = position_now.y - bezier_point1.y;

	bezier_point1.t += (InnerProduct(vec_actual, vec_line) / InnerProduct(vec_line, vec_line)) * run.delta_t; //	//���������Ķ˵��Ӧ��tֵ TODO: ͨ���������᲻�ᳬ��delta_t���޸�delta_t��ȡֵ
	if (bezier_point1.t < 0)
		bezier_point1.t = 0;					//min
	else if (bezier_point1.t > 1 - run.delta_t) //max
		bezier_point1.t = 1 - run.delta_t;
	bezier_point2.t = bezier_point1.t + run.delta_t;

	CountBezierXYbyT(&BezierData, &bezier_point1); //��������
	CountBezierXYbyT(&BezierData, &bezier_point2);

	vec_error.x = position_now.x - bezier_point1.x; //���������������������
	vec_error.y = position_now.y - bezier_point1.y;
	vec_line.x = bezier_point2.x - bezier_point1.x;
	vec_line.y = bezier_point2.y - bezier_point1.y;

	//��Vec_Line��ת��Vec_Lineָ��X��������Vec_Errorͬʱ ˳ʱ�� ��ת��ͬ�ĽǶȣ������ʱ��Vec_Error��yֵ��Ϊ����ƫ��
	vec_line_mod = sqrt(vec_line.x * vec_line.x + vec_line.y * vec_line.y);
	temp_cos = vec_line.x / vec_line_mod; // ˳ʱ����ת����	[ cosa	sina ][x]		[ -sina	cosa ][y]
	temp_sin = vec_line.y / vec_line_mod;

	/* PID���� */
	run_pid.err_now = -(vec_error.y * temp_cos - vec_error.x * temp_sin); //��ת���Vec_line ��y����ӷ���=����ֵ-����ֵ��

	if (ABS(run_pid.err_now) < 0.5f)
		run_pid.err_now = 0.0f;

	if (ABS(run_pid.err_now) > 4) //���ƫ�����4cm,ȥ����������,���ַ��뷧ֵ
		run_pid.out += incremental_pd(run_pid);
	else //ƫ��С��10cm,���ϻ�������
		run_pid.out += incremental_pid(run_pid);

	if (ABS(run_pid.out) > 300) //����޷�,�����ٶ�������300cm/s��ô�󣿣���
		run_pid.out = get_sign(run_pid.out) * 300.0f;

	else if (ABS(run_pid.out) < 0.1f) //�������
		run_pid.out = 0;

	run_pid.err_before = run_pid.err_last;
	run_pid.err_last = run_pid.err_now;

	velocity_set.Vx = velocity_set.Vx - run_pid.out * temp_sin; //�����ٶ��뷨���ٶ��ں�
	velocity_set.Vy = velocity_set.Vy + run_pid.out * temp_cos;

	/* �ٶ����� */
	CountBezierCurvaturebyT(&BezierData, &bezier_point1); // �����ʷ����ٶ�

	CurveRPid = 1.0f / bezier_point1.curvature; // ���ʰ뾶
	if (CurveRPid > 250)						// TODO: ���������������ʰ뾶��ע���޸�
		CurveRPid = 250;

	v_friction = 100 * sqrt(CurveRPid * chassis.static_friction_coefficient * 9.7936f * 0.01f);	  //9.7936 Ϊ�人���������ٶ�  9.7985�����ݵģ�//��Ħ�����պ��ṩ��������������ٶ�
	velocity_set.V = sqrt(velocity_set.Vx * velocity_set.Vx + velocity_set.Vy * velocity_set.Vy); // ��pid����������ٶ������ȱ�������
	if (velocity_set.V > run.speed_max_limited)
	{
		velocity_set.Vx = run.speed_max_limited * velocity_set.Vx / velocity_set.V;
		velocity_set.Vy = run.speed_max_limited * velocity_set.Vy / velocity_set.V;
		velocity_set.V = run.speed_max_limited;
	}
	if (v_friction < velocity_set.V)
	{
		velocity_set.Vx = velocity_set.Vx * (v_friction / velocity_set.V);
		velocity_set.Vy = velocity_set.Vy * (v_friction / velocity_set.V);
		velocity_set.V = v_friction;
	}

	PidLockAngle(lock_point.angle); //lock the angle

	/* PID�������ٶ� */
	SetChassisVelocity(velocity_set, position_now.angle);
}

/*** 
 * @brief  ����PID
 * @param  lock_position
 * @return void
 * @author ���˵�
 */
void PidLockPoint(Position volatile lock_position)
{
	lock_point_pid_x.err_now = lock_position.x - position_now.x;
	lock_point_pid_y.err_now = lock_position.y - position_now.y;
	angle_pid.err_now = lock_position.angle - position_now.angle;
	//PD����

	if (ABS(lock_point_pid_x.err_now < 0.5f))
		lock_point_pid_x.err_now = 0;
	if (ABS(lock_point_pid_y.err_now < 0.5f))
		lock_point_pid_y.err_now = 0;
	if (ABS(angle_pid.err_now < 0.5f))
		angle_pid.err_now = 0;

	//TODO: duck ������ô�鷳������pid���Ҿ���û��Ҫ�����ԣ��ģ�
	if (ABS(lock_point_pid_x.err_now) <= 5)
	{
		update_pid_para(lock_point_pid_x, pid_para_lock_point_x2);
		velocity_set.Vx = incremental_pid(lock_point_pid_x);
	}
	else if (ABS(lock_point_pid_x.err_now) <= 20)
	{
		update_pid_para(lock_point_pid_x, pid_para_lock_point_x1);
		velocity_set.Vx = incremental_pid(lock_point_pid_x);
	}
	else
	{
		update_pid_para(lock_point_pid_x, pid_para_lock_point_x);
		velocity_set.Vx = incremental_pid(lock_point_pid_x);
	}

	if (ABS(lock_point_pid_y.err_now) <= 5)
	{
		update_pid_para(lock_point_pid_y, pid_para_lock_point_y2);
		velocity_set.Vx = incremental_pid(lock_point_pid_y);
	}
	else if (ABS(lock_point_pid_y.err_now) <= 20)
	{
		update_pid_para(lock_point_pid_y, pid_para_lock_point_y1);
		velocity_set.Vx = incremental_pid(lock_point_pid_y);
	}
	else
	{
		update_pid_para(lock_point_pid_y, pid_para_lock_point_y);
		velocity_set.Vx = incremental_pid(lock_point_pid_y);
	}

	velocity_set.Vw = incremental_pid(angle_pid);

	lock_point_pid_x.err_last = lock_point_pid_x.err_now;
	lock_point_pid_x.err_sum += lock_point_pid_x.err_now;
	lock_point_pid_y.err_last = lock_point_pid_y.err_now;
	lock_point_pid_y.err_sum += lock_point_pid_y.err_now;
	angle_pid.err_last = angle_pid.err_now;

	//�޶��ٶ�
	velocity_set.V = sqrt(velocity_set.Vx * velocity_set.Vx + velocity_set.Vy * velocity_set.Vy);
	if (velocity_set.V > run.speed_max_limited)
	{
		velocity_set.Vx = velocity_set.Vx * run.speed_max_limited / velocity_set.V;
		velocity_set.Vy = velocity_set.Vy * run.speed_max_limited / velocity_set.V;
		velocity_set.V = run.speed_max_limited;
	}

	//�޶����ٶ�
	if (ABS(velocity_set.Vw) > 130)
		velocity_set.Vw = get_sign(velocity_set.Vw) * 130;
	SetChassisVelocity(velocity_set, position_now.angle);
}
/**
  * @brief  ����PID
	* @brief  λ��ʽPID�ӿ����ֱ��� 
  */
/*** 
 * @brief  FIXME: λ��ʽ����ʽ�������ֱ��ͣ�
 * @param  
 * @return void
 * @author ���˵�
 */
void PidLockAngle(float lock_angle)
{
	angle_pid.err_now = lock_angle - position_now.angle;
	if (ABS(angle_pid.err_now) < 0.5f)
		angle_pid.err_now = 0;

	velocity_set.Vw = incremental_pid(angle_pid);

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
	if (ABS(velocity_set.Vw) > 130)
		velocity_set.Vw = get_sign(velocity_set.Vw) * 130;
}

/**
 * @author: ���˵�
 * @brief: pid���ú���
 */
void pid_act(void)
{
	if (breaking != motor.state)
	{
		if (True == run.pid_run)
		{
			switch (run.auto_type)
			{
			case RUNPOINT:
				PidRunPoint(); //line
				break;
			case RUNBEZIER:
				PidRunBezier();
				break;
			default:
				break;
			}
		}
		else if (True == run.pid_point)
			PidLockPoint(lock_point);
	}
}

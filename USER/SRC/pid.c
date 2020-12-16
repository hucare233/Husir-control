/*
 * @Descripttion: pid
 * @version: 第一版
 * @Author: 叮咚蛋
 * @Date: 2020-12-15 19:29:29
 * @LastEditors: 叮咚蛋
 * @LastEditTime: 2020-12-16 10:20:26
 * @FilePath: \hu_sir-contorl\USER\SRC\pid.c
 */

#include "pid.h"

//PID算法:增量式PID&变速积分&抗积分饱和等等，可以试试其他的pid优化 TODO:

volatile float Ek[3];
volatile float EkX[3];
volatile float EkY[3];
volatile float EkAngle[2];

volatile PidStruct run_pid = {0.0f};
volatile PidStruct lock_point_pid_x = {0.0f};
volatile PidStruct lock_point_pid_y = {0.0f};
volatile PidStruct angle_pid = {0.0f};

/***************pid各组参数**************/
//pid para
float pid_para_run_point[3] = {11.0f, 0.05f, 0.0000f};
float pid_para_run_bezier[3] = {5.0f, 0.01f, 0.0000f};
float pid_para_angle[3] = {13.0f, 0.0f, 0.001f};

/* 锁点pid */
float pid_para_lock_point_x[3] = {4.0f, 0.001f, 0.0000f};
float pid_para_lock_point_x1[3] = {4.8f, 0.0015f, 0.0001f};
float pid_para_lock_point_x2[3] = {3.9f, 0.0001f, 0.002f};
float pid_para_lock_point_y[3] = {4.0f, 0.001f, 0.000f};
float pid_para_lock_point_y1[3] = {4.7f, 0.0015f, 0.001f};
float pid_para_lock_point_y2[3] = {4.0f, 0.0f, 0.01f};

unsigned char bezier_group;

/*** 
 * @brief  pid初始化
 * @author 叮咚蛋
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
 * @brief  直线PID，增量式
 * @param  void
 * @return void
 * @author 叮咚蛋
 */
void PidRunPoint(void)
{
	/* 加减速、转角度 */
	if (True == run.speed_up)
		RunPointSpeedUp();
	else
		velocity_set.V = run.speed_want;

	if (True == run.speed_down)
		RunPointSpeedDown(); // 刹车有问题，在判定达到目标距离时，会存在速度不为0，在何时让速度为0，同时，再次思考，是锁驱动电机刹车，还是反转刹车，还是慢慢减速刹车。

	if (True == run.rotate)
		RunPointRotate();

	/* 设置pid之前的初始基底速度xy分量 */
	SetPointSpeedBase();

	/* PID计算 */
	if (pid_line_y == run.pid_line_type) //the K of line < 1
		run_pid.err_now = run.line_k * position_now.x + run.line_b - position_now.y;
	else
	{
		if (True == run.no_line_K)
			run_pid.err_now = position_goal.x - position_now.x;
		else
			run_pid.err_now = position_now.y / run.line_k + run.line_b - position_now.x;
	}
	if (ABS(run_pid.err_now) > 20) // 积分分离，阈值问题TODO:
		run_pid.out += incremental_pi(run_pid);
	else
		run_pid.out += incremental_pid(run_pid);

	if (ABS(run_pid.out) < 0.1f) // 阈值有问题，才这点，检测一下多少。
		run_pid.out = 0;

	run_pid.err_before = run_pid.err_last;
	run_pid.err_last = run_pid.err_now;

	if (pid_line_x == run.pid_line_type)
		velocity_set.Vx += run_pid.out;
	else
		velocity_set.Vy += run_pid.out;
	velocity_set.V = sqrt(velocity_set.Vx * velocity_set.Vx + velocity_set.Vy * velocity_set.Vy); // 在pid计算后，两个速度向量等比例限速

	/* 速度限制 */
	if (velocity_set.V > run.speed_max_limited)
	{
		velocity_set.Vx = run.speed_max_limited * velocity_set.Vx / velocity_set.V;
		velocity_set.Vy = run.speed_max_limited * velocity_set.Vy / velocity_set.V;
		velocity_set.V = run.speed_max_limited;
	}

	PidLockAngle(lock_point.angle); //lock the angle

	/* PID后给电机速度 */
	SetChassisVelocity(velocity_set, position_now.angle);
}

Coordinates vec_line;	//规划曲线上的微分直线向量
Coordinates vec_actual; //实际微分直线向量
Coordinates vec_error;	//偏差向量
/*** 
 * @brief  贝塞尔曲线PID
 * @param  
 * @return void
 * @author 叮咚蛋
 */
void PidRunBezier(void)
{
	float temp_cos = 0.0f, temp_sin = 0.f, vec_line_mod = 0.0f, v_friction = 0.0f, CurveRPid = 0.0f;

	/* 加减速、转角度 */
	if (True == run.speed_up) //如果加速标志有效，调用加速程序
		RunBezierSpeedUp();
	else
		velocity_set.V = run.speed_want;

	if (True == run.speed_down) //如果减速标志有效，调用加速程序
		RunBezierSpeedDown();

	if (True == run.rotate)
		RunBezierRotate();
	else if (True == run.follow_angle)
		RunBezierFollowAngle();

	/* 设置pid之前的初始基底速度xy分量 */
	velocity_set.Vx = velocity_set.V * temp_cos;
	velocity_set.Vy = velocity_set.V * temp_sin;

	/* 贝塞尔曲线相关微分直线向量计算 */
	vec_actual.x = position_now.x - bezier_point1.x; //计算实际向量
	vec_actual.y = position_now.y - bezier_point1.y;

	bezier_point1.t += (InnerProduct(vec_actual, vec_line) / InnerProduct(vec_line, vec_line)) * run.delta_t; //	//更新向量的端点对应的t值 TODO: 通过监测这里会不会超过delta_t来修改delta_t的取值
	if (bezier_point1.t < 0)
		bezier_point1.t = 0;					//min
	else if (bezier_point1.t > 1 - run.delta_t) //max
		bezier_point1.t = 1 - run.delta_t;
	bezier_point2.t = bezier_point1.t + run.delta_t;

	CountBezierXYbyT(&BezierData, &bezier_point1); //更新坐标
	CountBezierXYbyT(&BezierData, &bezier_point2);

	vec_error.x = position_now.x - bezier_point1.x; //更新曲线向量和误差向量
	vec_error.y = position_now.y - bezier_point1.y;
	vec_line.x = bezier_point2.x - bezier_point1.x;
	vec_line.y = bezier_point2.y - bezier_point1.y;

	//将Vec_Line旋转到Vec_Line指向X轴正方向，Vec_Error同时 顺时针 旋转相同的角度，计算此时的Vec_Error的y值即为法向偏差
	vec_line_mod = sqrt(vec_line.x * vec_line.x + vec_line.y * vec_line.y);
	temp_cos = vec_line.x / vec_line_mod; // 顺时针旋转矩阵	[ cosa	sina ][x]		[ -sina	cosa ][y]
	temp_sin = vec_line.y / vec_line_mod;

	/* PID计算 */
	run_pid.err_now = -(vec_error.y * temp_cos - vec_error.x * temp_sin); //旋转后的Vec_line 的y坐标加符号=期望值-测量值；

	if (ABS(run_pid.err_now) < 0.5f)
		run_pid.err_now = 0.0f;

	if (ABS(run_pid.err_now) > 4) //如果偏差大于4cm,去除积分作用,积分分离阀值
		run_pid.out += incremental_pd(run_pid);
	else //偏差小于10cm,加上积分作用
		run_pid.out += incremental_pid(run_pid);

	if (ABS(run_pid.out) > 300) //输出限幅,调节速度限制在300cm/s这么大？？？
		run_pid.out = get_sign(run_pid.out) * 300.0f;

	else if (ABS(run_pid.out) < 0.1f) //输出死区
		run_pid.out = 0;

	run_pid.err_before = run_pid.err_last;
	run_pid.err_last = run_pid.err_now;

	velocity_set.Vx = velocity_set.Vx - run_pid.out * temp_sin; //切向速度与法向速度融合
	velocity_set.Vy = velocity_set.Vy + run_pid.out * temp_cos;

	/* 速度限制 */
	CountBezierCurvaturebyT(&BezierData, &bezier_point1); // 按曲率分配速度

	CurveRPid = 1.0f / bezier_point1.curvature; // 曲率半径
	if (CurveRPid > 250)						// TODO: 这里限制死了曲率半径，注意修改
		CurveRPid = 250;

	v_friction = 100 * sqrt(CurveRPid * chassis.static_friction_coefficient * 9.7936f * 0.01f);	  //9.7936 为武汉的重力加速度  9.7985是柳州的，//由摩擦力刚好提供向心力的最大线速度
	velocity_set.V = sqrt(velocity_set.Vx * velocity_set.Vx + velocity_set.Vy * velocity_set.Vy); // 在pid计算后，两个速度向量等比例限速
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

	/* PID后给电机速度 */
	SetChassisVelocity(velocity_set, position_now.angle);
}

/*** 
 * @brief  锁点PID
 * @param  lock_position
 * @return void
 * @author 叮咚蛋
 */
void PidLockPoint(Position volatile lock_position)
{
	lock_point_pid_x.err_now = lock_position.x - position_now.x;
	lock_point_pid_y.err_now = lock_position.y - position_now.y;
	angle_pid.err_now = lock_position.angle - position_now.angle;
	//PD调节

	if (ABS(lock_point_pid_x.err_now < 0.5f))
		lock_point_pid_x.err_now = 0;
	if (ABS(lock_point_pid_y.err_now < 0.5f))
		lock_point_pid_y.err_now = 0;
	if (ABS(angle_pid.err_now < 0.5f))
		angle_pid.err_now = 0;

	//TODO: duck 不必这么麻烦，三组pid，我觉得没必要，测试，改，
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

	//限定速度
	velocity_set.V = sqrt(velocity_set.Vx * velocity_set.Vx + velocity_set.Vy * velocity_set.Vy);
	if (velocity_set.V > run.speed_max_limited)
	{
		velocity_set.Vx = velocity_set.Vx * run.speed_max_limited / velocity_set.V;
		velocity_set.Vy = velocity_set.Vy * run.speed_max_limited / velocity_set.V;
		velocity_set.V = run.speed_max_limited;
	}

	//限定角速度
	if (ABS(velocity_set.Vw) > 130)
		velocity_set.Vw = get_sign(velocity_set.Vw) * 130;
	SetChassisVelocity(velocity_set, position_now.angle);
}
/**
  * @brief  锁角PID
	* @brief  位置式PID加抗积分饱和 
  */
/*** 
 * @brief  FIXME: 位置式增量式，抗积分饱和？
 * @param  
 * @return void
 * @author 叮咚蛋
 */
void PidLockAngle(float lock_angle)
{
	angle_pid.err_now = lock_angle - position_now.angle;
	if (ABS(angle_pid.err_now) < 0.5f)
		angle_pid.err_now = 0;

	velocity_set.Vw = incremental_pid(angle_pid);

	angle_pid.err_last = angle_pid.err_now;
	//抗积分饱和, FIXME:
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

	//输出限幅
	if (ABS(velocity_set.Vw) > 130)
		velocity_set.Vw = get_sign(velocity_set.Vw) * 130;
}

/**
 * @author: 叮咚蛋
 * @brief: pid调用函数
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

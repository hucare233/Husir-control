/*
 * @Descripttion: run
 * @version: 第一版
 * @Author: 叮咚蛋
 * @Date: 2020-12-15 19:29:29
 * @LastEditors: 叮咚蛋
 * @LastEditTime: 2020-12-16 10:12:45
 * @FilePath: \hu_sir-contorl\USER\SRC\run.c
 */

#include "run.h"

volatile RunFlagStruct Run;

// float run.Proportion = 0.4; //缓冲比例系数 用于调节开始缓冲时的速度

//position structs
volatile Position position_now = {0.0, 0.0, 0.0};
volatile Position position_start = {0.0, 0.0, 0.0};
volatile Position position_goal = {0.0, 0.0, 0.0};
volatile Position position_want = {0.0, 0.0, 0.0};
volatile Position lock_point = {0.0, 0.0, 0.0};
volatile Position start_turn_point = {0.0, 0.0, 0.0};

//speed structs
volatile Velocity velocity_now = {0.0, 0.0, 0.0, 0.0};
volatile Velocity velocity_start = {0.0, 0.0, 0.0, 0.0};
volatile Velocity velocity_goal = {0.0, 0.0, 0.0, 0.0};
volatile Velocity velocity_set = {0.0, 0.0, 0.0, 0.0};
volatile Velocity velocity_abs = {0.0, 0.0, 0.0, 0.0};
volatile WheelInfo Wheel[4] = {{0.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 0.0f}};

volatile float wheel_angle_set[4] = {0.0f};

//Bezier参数
BezierStruct BezierData; //Bezier曲线相关参数

volatile BezierPoint bezier_point1;
volatile BezierPoint bezier_point2;

/*** 
 * @brief  直线跑点函数
 * @param  points 期望点坐标、角度
 * @param  Vstart_Vwant_Vstop 起步速度、最大期望速度、结束速度
 * @param  UpDis_DownDis 加减速距离
 * @param  StartTurnDis_FullTurnDis 开始转角度距离、停止旋转出距离
 * @param  continue_flag 是否接着下一个轨迹跑
 * @author 叮咚蛋
 */

void RunPoint(float x_want, float y_want, float angle_want,
			  float v_start, float v_want, float v_end,
			  float up_dis, float down_dis, float start_rotate_dis, float rotate_need_dis,
			  u8 continue_flag)
{
	//加入临界段 防止初始化被打断, // duck 不必，
	// OS_ENTER_CRITICAL();

	/* 计算跑车时间 */
	run_time.count_time = True;
	run_time.run_start = run_time.clk;

	/* flag 初始化 */
	if (v_want > v_start)
		run.speed_up = True;
	else
		run.speed_up = False;
	run.speed_up_over = !run.speed_up;

	if (v_want > v_end)
		run.speed_down = True;
	else
		run.speed_down = False;
	run.speed_down_over = True;

	if (ABS(lock_point.angle - angle_want) > 1.0f) // 上一次角度与期望角度比较
		run.rotate = True;
	else
		run.rotate = False;
	run.rotate_over = !run.rotate;

	run.continue_next = continue_flag;

	run.speed_down_mode_change = True; // 减速mode切换flag

	/* set parameters */
	run.speed_want = v_want; // TODO: 有问题，为啥要有匀速，一直加速然后减速不香么？电机顶不住？我觉得电机可以，
	run.speed_start = v_start;
	run.speed_end = v_end;

	position_start.x = position_now.x;
	position_start.y = position_now.y;
	position_start.angle = lock_point.angle;

	position_goal.x = x_want; //目标点
	position_goal.y = y_want;
	position_goal.angle = angle_want;

	run.up_dis = up_dis;	 //加速距离
	run.down_dis = down_dis; //减速距离
	run.goal_dis = CountDistance(position_start.x, position_start.y,
								 position_goal.x, position_goal.y); //移动总距离

	run.rotate_start = start_rotate_dis; //开始旋转的距离
	run.rotate_need = rotate_need_dis;	 //旋转所需的总距离

	/* error handling */
	if (run.rotate_start + run.rotate_need > run.goal_dis) //开始旋转时的距离与所需的距离之和大于总距离
	{
		run.rotate_start = run.goal_dis / 3.0f;
		run.rotate_need = run.goal_dis / 3.0f;
		Beep_Show(8);
		sprintf(user.error, "%s", "RotateParaErr");
	}
	if (run.up_dis + run.down_dis > run.goal_dis) //加减速距离之和大于总距离
	{
		run.up_dis = run.goal_dis / 3.0f;
		run.down_dis = run.goal_dis / 3.0f;
		Beep_Show(8);
		sprintf(user.error, "%s", "UpDwonParaErr");
	}

	/* 直线斜率截距计算 */
	if (ABS(position_goal.x - position_start.x) < 0.5f) //直线斜率过大，近似无斜率
	{
		run.no_line_K = True;			//直线无斜率
		run.pid_line_type = pid_line_x; //直线的PID的类型  以X方向的偏差做调节
	}
	else
	{
		run.no_line_K = False;
		run.line_k = (position_goal.y - position_start.y) / (position_goal.x - position_start.x); //直线斜率
		run.line_k_cos = 1 / sqrt(1 + run.line_k * run.line_k);									  //直线倾角的余弦
		if (ABS(run.line_k) < 1)																  //如果斜率小于1，以Y方向偏差做调节
		{
			run.pid_line_type = pid_line_y;
			run.line_b = position_start.y - position_start.x * run.line_k;
		}
		else //否则，以X方向的偏差做调节
		{
			run.pid_line_type = pid_line_x;
			run.line_b = position_start.x - position_start.y / run.line_k;
		}
	}

	/* set flag and parameters about pid*/
	run.pid_point = False; //关闭位置锁定

	velocity_set.V = run.speed_start; //把速度赋初值
	velocity_set.Vw = 0.0;
	lock_point.angle = position_start.angle; //起始姿态角不变

	run.auto_type = RUNPOINT;
	PidInit(&run_pid, pid_para_run_point); //PID初始化
	PidInit(&angle_pid, pid_para_angle);   //PID初始化
	ENABLE_ALL_MOTOR;					   //使能电机才能跑
	run.pid_run = True;					   //打开PID调节
	// OS_EXIT_CRITICAL();

	//计算期望坐标与当前坐标数量积大小,来判断是否跑到了期望坐标，是不是可以其他判断方式更好
	while (((position_goal.x - position_start.x) * (position_now.x - position_goal.x) +
			(position_goal.y - position_start.y) * (position_now.y - position_goal.y)) < -2.0f)
	{
		OSTimeDly(50);
	}

	if (run.continue_next == False)
	{
		lock_point = position_goal; // 期望坐标锁定
		run.pid_run = False;
		run.pid_point = True;							   // 切换pid到锁点
		PidInit(&lock_point_pid_x, pid_para_lock_point_x); //PID初始化
		PidInit(&lock_point_pid_y, pid_para_lock_point_y); //PID初始化
		PidInit(&angle_pid, pid_para_angle);			   //PID初始化

		run_time.count_time = False;
	}
	else
	{
		run.pid_run = False;
		run.pid_point = False;
	}
}

/**
 * @author: 叮咚蛋
 * @brief: 旋转
 * @param 同
 * @return void
 */

u8 relatspeedturn = {0};
void Rotate(float angle_want, float v_w, u8 continue_flag)
{
	run.rotate = True;
	run.rotate_over = False;

	position_goal.x = position_now.x;
	position_goal.y = position_now.y;
	position_goal.angle = angle_want;

	run.pid_point = False; //关闭位置锁定
	run.pid_run = False;   //关闭PID调节

	ENABLE_ALL_MOTOR; //使能电机才能跑

	velocity_set.Vw = v_w;
	velocity_set.Vx = 0.0f;
	velocity_set.Vy = 0.0f;

	while (ABS(position_goal.angle - position_now.angle) > 2.0f)
	{
		OSTimeDly(50);
		SetChassisVelocity(velocity_set, position_now.angle);
	}
	if (False == continue_flag)
	{
		lock_point = position_goal; // 期望坐标锁定
		run.pid_run = False;
		run.pid_point = True;							   // 切换pid到锁点
		PidInit(&lock_point_pid_x, pid_para_lock_point_x); //PID初始化
		PidInit(&lock_point_pid_y, pid_para_lock_point_y); //PID初始化
		PidInit(&angle_pid, pid_para_angle);			   //PID初始化
	}
	else
	{
		run.pid_run = False;
		run.pid_point = False;
	}
}

/*** 
 * @brief  计算参数方程系数
 * @param  bezier_para 贝塞尔参数，内含控制点与参数方程系数
 * @return void
 * @author 叮咚蛋
 */

void CountBezierCoefficient(BezierStruct *bezier_para)
{
	bezier_para->x_t[0] = bezier_para->P[0].x;																	 //x参数方程中的t的0次项系数
	bezier_para->x_t[1] = 3 * (bezier_para->P[1].x - bezier_para->P[0].x);										 //x参数方程中的t的1次项系数
	bezier_para->x_t[2] = 3 * (bezier_para->P[2].x - bezier_para->P[1].x) - bezier_para->x_t[1];				 //x参数方程中的t的2次项系数
	bezier_para->x_t[3] = bezier_para->P[3].x - bezier_para->P[0].x - bezier_para->x_t[2] - bezier_para->x_t[1]; //x参数方程中的t的3次项系数

	bezier_para->y_t[0] = bezier_para->P[0].y;																	 //Y参数方程中的t的0次项系数
	bezier_para->y_t[1] = 3 * (bezier_para->P[1].y - bezier_para->P[0].y);										 //Y参数方程中的t的1次项系数
	bezier_para->y_t[2] = 3 * (bezier_para->P[2].y - bezier_para->P[1].y) - bezier_para->y_t[1];				 //Y参数方程中的t的2次项系数
	bezier_para->y_t[3] = bezier_para->P[3].y - bezier_para->P[0].y - bezier_para->y_t[2] - bezier_para->y_t[1]; //Y参数方程中的t的3次项系数
}

/*** 
 * @brief  计算贝塞尔曲线坐标点
 * @param  bezier_para
 * @param  bezier_point 
 * @return void
 * @author 叮咚蛋
 */
void CountBezierXYbyT(BezierStruct volatile *bezier_para, BezierPoint volatile *bezier_point)
{
	float temp1, temp2, temp3;

	temp1 = bezier_point->t; //一次方
	temp2 = temp1 * temp1;	 //二次方
	temp3 = temp2 * temp1;	 //三次方

	bezier_point->x = bezier_para->x_t[3] * temp3 + bezier_para->x_t[2] * temp2 + bezier_para->x_t[1] * temp1 + bezier_para->x_t[0];
	bezier_point->y = bezier_para->y_t[3] * temp3 + bezier_para->y_t[2] * temp2 + bezier_para->y_t[1] * temp1 + bezier_para->y_t[0];
}

/*** 
 * @brief  计算贝塞尔曲线点曲率
 * @param  BezierStruct
 * @param  BezierPoint
 * @return void
 * @author 叮咚蛋
 */
void CountBezierCurvaturebyT(BezierStruct volatile *bezier_para, BezierPoint volatile *bezier_point)
{
	float temp1;
	float temp2;
	float Xdifferential1;
	float Xdifferential2;
	float Ydifferential1;
	float Ydifferential2;
	temp1 = bezier_point->t; //一次方
	temp2 = temp1 * temp1;	 //二次方

	Xdifferential1 = 3 * bezier_para->x_t[3] * temp2 + 2 * bezier_para->x_t[2] * temp1 + bezier_para->x_t[1];
	Ydifferential1 = 3 * bezier_para->y_t[3] * temp2 + 2 * bezier_para->y_t[2] * temp1 + bezier_para->y_t[1];

	Xdifferential2 = 6 * bezier_para->x_t[3] * temp1 + 2 * bezier_para->x_t[2];
	Ydifferential2 = 6 * bezier_para->y_t[3] * temp1 + 2 * bezier_para->y_t[2];
	bezier_point->curvature = ABS(Xdifferential1 * Ydifferential2 - Xdifferential2 * Ydifferential1) / ((Xdifferential1 * Xdifferential1 + Ydifferential1 * Ydifferential1) * sqrt(Xdifferential1 * Xdifferential1 + Ydifferential1 * Ydifferential1));
}

/*** 
 * @brief  跑贝塞尔
 * @param  
 * @return void
 * @author 叮咚蛋
 */
void RunBezier(float *points, float angle_want,
			   float v_start, float v_want, float v_end,
			   float upt, float downt, float startturnt, float fullturnt,
			   u8 continue_flag, u8 follow_angle_flag)
{
	//加入临界段 防止初始化被打断
	// OS_ENTER_CRITICAL();

	run_time.count_time = True;
	run_time.run_start = run_time.clk;

	/* flag 初始化 */
	if (v_want > v_start)
		run.speed_up = True;
	else
		run.speed_up = False;
	run.speed_up_over = !run.speed_up;

	if (v_want > v_end)
		run.speed_down = True;
	else
		run.speed_down = False;
	run.speed_down_over = !run.speed_down;

	if (ABS(lock_point.angle - angle_want) > 1.0f)
		run.rotate = True;
	else
		run.rotate = False;
	run.rotate_over = !run.rotate;

	run.continue_next = continue_flag;
	run.follow_angle = follow_angle_flag;

	run.speed_down_mode_change = True; // 减速mode切换flag

	/* set parameters */
	velocity_set.V = v_want;   //目标速度
	run.speed_start = v_start; //起始速度
	run.speed_end = v_end;	   //终止速度

	position_start.x = position_now.x; //起始点
	position_start.y = position_now.y;
	memcpy(BezierData.P, points, 8);	 // 轨迹控制点赋值
	CountBezierCoefficient(&BezierData); //根据4个控制点坐标计算Bezier参数方程系数

	//估算DeltaT TODO: 这个delta_t完全可以给大点， 可以试试增加采样点时间差，
	run.delta_t = (2.0f / (CountDistance(BezierData.P[0].x, BezierData.P[0].y,
										 BezierData.P[3].x, BezierData.P[3].y)));
	//初始化bezier_point1和bezier_point2
	bezier_point1.t = 0;
	bezier_point2.t = run.delta_t;
	CountBezierXYbyT(&BezierData, &bezier_point1); //根据bezier_point1中的t值计算对应的坐标
	CountBezierXYbyT(&BezierData, &bezier_point2); //根据bezier_point1中的t值计算对应的坐标

	//初始化向量Vec_Line
	vec_line.x = bezier_point2.x - bezier_point1.x;
	vec_line.y = bezier_point2.y - bezier_point1.y;

	position_start.angle = lock_point.angle; // 起始角度为上一次锁的角度。
	position_goal.x = BezierData.P[3].x;	 // 目标点
	position_goal.y = BezierData.P[3].y;
	position_goal.angle = angle_want; // 注意转角度与变角度跑的冲突。

	/* bezier time para */
	run.up_t = upt;
	run.down_t = downt;
	run.rotate_start = startturnt;
	run.rotate_need = fullturnt;

	// bezier_group = pid_class; // TODO: 修改不同曲线下的pid应该放在外部，手动修改

	/* error handling */
	if (run.up_t + run.down_t > 1)
	{
		run.up_t = 0.4f;
		run.down_t = 0.4f;
		Beep_Show(8);
		sprintf(user.error, "%s", "UpDwonParaErr");
		return;
	}
	if (run.rotate_start + run.rotate_need > 1)
	{
		run.rotate_start = 0.3f;
		run.rotate_need = 0.3f;
		Beep_Show(8);
		sprintf(user.error, "%s", "RotateParaErr");
		return;
	}
	if (run.rotate == True && run.follow_angle == True)
	{
		run.follow_angle = False;
		Beep_Show(8);
		sprintf(user.error, "%s", "RotateFollowErr");
		return;
	}

	/* set flag and parameters about pid*/
	run.pid_point = False; //关闭位置锁定
	run.pid_run = True;	   //打开PID调节
	run.auto_type = RUNBEZIER;
	PidInit(&run_pid, pid_para_run_bezier); //PID初始化
	PidInit(&angle_pid, pid_para_angle);	//PID初始化

	velocity_set.V = run.speed_start; //把速度赋初值
	velocity_set.Vw = 0.0;
	lock_point.angle = position_start.angle; //保持姿态角不变

	run.run_break = False; //该标志位用于强制退出当前run进程，转到下一个run进程
	ENABLE_ALL_MOTOR;	   //使能底盘电机

	//这里要好好研究一下
	// OS_EXIT_CRITICAL();
	while ((bezier_point1.t < (1 - run.delta_t)) && False == run.run_break)
	{
		OSTimeDly(50);
	}
	// TODO:以下语句只有上面的while判断完了才会执行，怎么会有goal != now的情况呢？注释了先 2020-08-01
	if (run.continue_next == False)
	{
		if (False == run.run_break) //如果是正常退出，
		{
			lock_point = position_goal; // 期望坐标锁定
		}
		else
		{
			//非正常退出，就锁当前点，当需要强制结束当前run进程，转到下一个run进程时使用
			lock_point.x = position_now.x;
			lock_point.y = position_now.y;
			lock_point.angle = position_now.angle;
		}
		run.pid_run = False;
		run.pid_point = True;							   // 切换pid到锁点
		PidInit(&lock_point_pid_x, pid_para_lock_point_x); //PID初始化
		PidInit(&lock_point_pid_y, pid_para_lock_point_y); //PID初始化
		PidInit(&angle_pid, pid_para_angle);			   //PID初始化

		run_time.count_time = False;
	}
	else
	{
		run.pid_run = False;
		run.pid_point = False;
	}
}

/*** 
 * @brief  直线跑点加速子程序
 * @param  void
 * @return void
 * @author 叮咚蛋
 */
void RunPointSpeedUp(void)
{
	float dis_temp = 0.0f;
	if (ABS(velocity_now.V - run.speed_want) > 10) //当前速度没有达到目标速度; FIXME: 阈值判定容易出问题，极有可能车已经跑过了这个速度，
	{
		dis_temp = CountDistance(position_start.x, position_start.y, position_now.x, position_now.y);
		if (dis_temp < run.up_dis) //行走距离小于加速距离，按以下算法进行加速 TODO: 必须研究一下怎么发挥最大摩擦提供的加速度
		{
			velocity_set.V = run.speed_start - (run.speed_start - run.speed_want) * sqrt(dis_temp / run.up_dis); //恒加速度  x=k * t * t   a = k‘
		}
		else //到达加速距离，不再加速了，但是此时速度可能并没有到达,直接赋速度
		{
			velocity_set.V = run.speed_want;
			run.speed_up = False;
			run.speed_up_over = True;
		}
	}
	else //速度已经达到了目标速度 ，可能此时还没走完加速距离
	{
		velocity_set.V = run.speed_want;
		run.speed_up = False;
		run.speed_up_over = True;
	}
}

/*** 
 * @brief  直线跑点减速子程序
 * @param  void
 * @return void
 * @author 叮咚蛋
 */
void RunPointSpeedDown(void) //好好研究运动学算法
{
	const float CubeDownDis = (1 - run.Proportion) * run.down_dis; //FIXME:
	float Cubetemp;
	float Cubev_start_brake;
	float dis_temp = CountDistance(position_now.x, position_now.y, position_goal.x, position_goal.y);

	if (dis_temp <= run.down_dis)
	{
		if (True == run.speed_down_over)
		{
			run_time.down_start = run_time.clk;
			run.v_start_brake = velocity_now.V;
			run.speed_down_over = False; // the flag is a temp flag here
		}

		if (False == run.speed_down_over)
		{
			switch (run.speed_down_mode)
			{
			case SQUAREROOT: //不同关系类型的减速曲线，
				velocity_set.V = run.speed_end - (run.speed_end - run.v_start_brake) * sqrt(dis_temp / run.down_dis);
				break;
			case LINE:
				velocity_set.V = run.speed_end - (run.speed_end - run.v_start_brake) * (dis_temp / run.down_dis);
				break;
			case SQUARE:
				velocity_set.V = run.speed_end - (run.speed_end - run.v_start_brake) * (dis_temp / run.down_dis) * (dis_temp / run.down_dis);
				break;
			case CUBE:
				velocity_set.V = run.speed_end - (run.speed_end - run.v_start_brake) * (dis_temp / run.down_dis) * (dis_temp / run.down_dis) * (dis_temp / run.down_dis);
				break;
			case CUBEBUFFER:
				Cubetemp = dis_temp - run.Proportion * run.down_dis;
				Cubev_start_brake = run.v_start_brake / (1 + (run.Proportion * run.down_dis / CubeDownDis) * (run.Proportion * run.down_dis / CubeDownDis) * (run.Proportion * run.down_dis / CubeDownDis));
				velocity_set.V = run.speed_end - (run.speed_end - Cubev_start_brake) * (Cubetemp / CubeDownDis) * (Cubetemp / CubeDownDis) * (Cubetemp / CubeDownDis) - (run.speed_end - Cubev_start_brake) * (run.Proportion * run.down_dis / CubeDownDis) * (run.Proportion * run.down_dis / CubeDownDis) * (run.Proportion * run.down_dis / CubeDownDis);
				break;
			default:
				velocity_set.V = run.speed_end - (run.speed_end - run.v_start_brake) * sqrt(dis_temp / run.down_dis);
				break;
			}

			if ((ABS(velocity_now.V) <= 50) && (run.speed_down_mode_change == True)) // FIXME: 准备把这个改了
			{
				run.speed_down_mode = SQUAREROOT;
				run.speed_down_mode_change = False;
				run.v_start_brake = ABS(velocity_now.V);
				run.down_dis = dis_temp;
			}
		}
	}
}
/*** 
 * @brief  跑点旋转子程序
 * @param  void
 * @return void
 * @author 叮咚蛋
 */
void RunPointRotate(void) //边直线行走边旋转的旋转子程序
{
	float temp = CountDistance(position_start.x, position_start.y, position_now.x, position_now.y);
	static u8 inner_init_flag = True;

	if (True == inner_init_flag && temp > run.rotate_start)
	{
		//到达旋转点，记录坐标
		start_turn_point.x = position_now.x;
		start_turn_point.y = position_now.y;
		inner_init_flag = False;
	}
	else if (temp > run.rotate_start + run.rotate_need)
	{
		inner_init_flag = True;

		run.rotate = False;
		run.rotate_over = True;

		lock_point.angle = position_goal.angle;
		return;
	}

	if (run.rotate_over == False) //按以下的算法进行旋转处理，跟位移呈线性关系
	{
		temp = CountDistance(position_now.x, position_now.y, start_turn_point.x, start_turn_point.y);

		lock_point.angle = position_start.angle + (position_goal.angle - position_start.angle) * temp / run.rotate_need;
	}
}

/*** 
 * @brief  赋值PID调节之前的原始速度xy分量
 * @param  void
 * @return void
 * @author 叮咚蛋
 */
void SetPointSpeedBase(void)
{
	if (True == run.no_line_K)
	{
		velocity_set.Vx = 0.0f;
		if (position_goal.y > position_start.y)
			velocity_set.Vy = velocity_set.V;
		else
			velocity_set.Vy = -velocity_set.V;
	}
	else
	{
		if (position_goal.x > position_start.x)
		{
			velocity_set.Vx = velocity_set.V * run.line_k_cos;
			velocity_set.Vy = velocity_set.V * run.line_k_cos * run.line_k;
		}
		else
		{
			velocity_set.Vx = -velocity_set.V * run.line_k_cos;
			velocity_set.Vy = -velocity_set.V * run.line_k_cos * run.line_k;
		}
	}
}

/*** 
 * @brief  Bezier加速子程序
 * @param  void
 * @return void
 * @author 叮咚蛋
 */
void RunBezierSpeedUp(void)
{
	if (ABS(velocity_set.V - velocity_set.V) > 10) //当前速度没有达到目标速度
	{
		if (bezier_point1.t < run.up_t) //行走距离小于加速距离，按以下算法进行加速
		{
			velocity_set.V = run.speed_start - (run.speed_start - velocity_set.V) * sqrt(bezier_point1.t / run.up_t);
		}
		else
		{
			velocity_set.V = velocity_set.V;
			run.speed_up = False;
			run.speed_up_over = True;
		}
	}
	else
	{
		velocity_set.V = velocity_set.V;
		run.speed_up = False;
		run.speed_up_over = True;
	}
}

/**
 * @author: 叮咚蛋
 * @brief: 贝塞尔减速子程序 
 * @param 
 * @return void
 */

void RunBezierSpeedDown(void)
{
	if (True == run.speed_up_over && (1 - bezier_point1.t) <= run.down_t)
	{
		run_time.down_start = run_time.clk;
		run.v_start_brake = velocity_now.V;
		run.speed_up_over = False; // the flag is a temp flag here
	}

	if (False == run.speed_down_over)
	{
		switch (run.speed_down_mode)
		{
		case SQUAREROOT:
			velocity_set.V = run.speed_end - (run.speed_end - run.v_start_brake) * sqrt((1 - bezier_point1.t) / run.down_t);
			break;
		case LINE:
			velocity_set.V = run.speed_end - (run.speed_end - run.v_start_brake) * (1 - bezier_point1.t) / run.down_t;
			break;
		case SQUARE:
			velocity_set.V = run.speed_end - (run.speed_end - run.v_start_brake) * (1 - bezier_point1.t) / run.down_t * (1 - bezier_point1.t) / run.down_t;
			break;
		case CUBE:
			velocity_set.V = run.speed_end - (run.speed_end - run.v_start_brake) * (1 - bezier_point1.t) / run.down_t * (1 - bezier_point1.t) / run.down_t * (1 - bezier_point1.t) / run.down_t;
			break;
		default:
			velocity_set.V = run.speed_end - (run.speed_end - run.v_start_brake) * sqrt((1 - bezier_point1.t) / run.down_t);

			break;
		}
		if (ABS(velocity_now.V) <= 50 && run.speed_down_mode_change == True)
		{
			run.speed_down_mode = SQUAREROOT;
			run.speed_down_mode_change = False;
			run.v_start_brake = ABS(velocity_now.V);
			run.down_t = 1 - bezier_point1.t;
		}
	}
}

/*** 
 * @brief  贝塞尔旋转子程序
 * @param  void
 * @return void
 * @author 叮咚蛋
 */
void RunBezierRotate(void)
{
	static float t_start_rotate_temp = 0.0;
	static u8 inner_init_flag = True;

	if (True == inner_init_flag && (bezier_point1.t > run.rotate_start))
	{
		//到达旋转点，记录t值
		t_start_rotate_temp = bezier_point1.t;
		inner_init_flag = False;
	}
	else if (bezier_point1.t > run.rotate_start + run.rotate_need) //当前t大于转动结束时的t
	{
		inner_init_flag = True;

		run.rotate = False;
		run.rotate_over = True;

		lock_point.angle = position_goal.angle; //lock the goal angle
		return;
	}
	if (run.rotate_over == False) //按以下算法进行旋转处理，跟t呈线性关系
	{
		lock_point.angle = position_start.angle + (bezier_point1.t - t_start_rotate_temp) * (position_goal.angle - position_start.angle) / run.rotate_need;
	}
}
/*** 
 * @brief  变角度跑曲线，注意atan的返回值的范围[-pi/2, pi/2]，然后直角坐标系下0度需要转换到偏航角坐标系下，逆正顺负，y+方向为0度。
 * @param  void
 * @return void
 * @author 叮咚蛋
 */
void RunBezierFollowAngle(void)
{
	float temp_x = 0.0f, temp_y = 0.0f;

	temp_x = (bezier_point2.x - bezier_point1.x);
	temp_y = (bezier_point2.y - bezier_point1.y);

	if (temp_x > -0.5f && temp_x < 0.5f) // 阈值
	{
		if (temp_y > 0)
			run.path_angle = 0.0f;
		else if (temp_y < 0)
			run.path_angle = 180.0f;
	}
	else
	{
		run.path_angle = atan(temp_y / temp_x);
		if (temp_y > 0)
			run.path_angle = (run.path_angle >= 0 ? (-PI / 2 + run.path_angle) : (PI / 2 + run.path_angle));
		else if (temp_x < 0)
			run.path_angle += PI / 2;
		else
			run.path_angle -= PI / 2;

		run.path_angle = run.path_angle * 180.0f / PI;
	}
	lock_point.angle = run.path_angle;
}

///**
//  * @brief   这谁瞎鸡儿写的anti函数，哦哦，这是防抱死制动 TODO:
//  */
//void Antilockbrake()
//{
//    static int i=0;
//    static int j=0;
//    float temp=CountDistance(position_now.x, position_now.y, position_goal.x, position_goal.y) ;
//    if(run.speed_down_over == False &&(temp<= run.down_dis))
//    {
//        //存储刚开始减速时的坐标点与速度值

//        v_start_brake = velocity_set.V;
//        run.speed_down_over = 1;
//        run.speed_up = False; 			//关闭加速
//    }
//    if(run.speed_down_over == 1) //按以下算法进行刹车
//    {
//        if(i%100==0)
//        {
//            j=50;
//        }
//        if(j>0)
//        {
////            Elmo_StopMotor(ID_Elmo_Motor1);
////            Elmo_StopMotor(ID_Elmo_Motor2);
////            Elmo_StopMotor(ID_Elmo_Motor3);

//            j--;
//        }
//        else
//        {
////            ENABLE_ALL_MOTOR;
//            velocity_set.V	=	0;
//        }
//    }
//}

/**************************************************************************************************************************************/
/*******************************************************  计算轮子转速  **************************************************************/
/**************************************************************************************************************************************/

/*** 
 * @brief 判断舵象限
 * @author 叮咚蛋
 * @param  
 * @return  
 * @Date 2020-09-04 21:46:45
 */
void QuadrantIs(void)
{
	int i = 0;
	for (i = 0; i < 4; i++)
	{
		if (Wheel[i].Vx >= 0 && Wheel[i].Vy >= 0) //含有x+ y+
			Wheel[i].quadrant = 1;
		else if (Wheel[i].Vx <= 0 && Wheel[i].Vy > 0) //含有x-
			Wheel[i].quadrant = 2;
		else if (Wheel[i].Vx <= 0 && Wheel[i].Vy < 0) //含有y-
			Wheel[i].quadrant = 3;
		else
			Wheel[i].quadrant = 4;
	}
}

/*** 
 * @brief 根据想要达到的绝对速度来计算相对速度
 * @author 叮咚蛋
 * @param  :世界坐标系, angle
 * @return  
 * @Date 2020-09-04 21:49:37
 */
void SetChassisVelocity(Velocity volatile absolutespeed, float angle)
{
	Velocity relatspeed = {0};
	float Sin_a = sin(angle * PI / 180.f);
	float Cos_a = cos(angle * PI / 180.f);

	relatspeed.Vx = absolutespeed.Vx * Cos_a + absolutespeed.Vy * Sin_a;
	relatspeed.Vy = absolutespeed.Vy * Cos_a - absolutespeed.Vx * Sin_a;
	relatspeed.Vw = absolutespeed.Vw;

	calculate_wheel_speed(relatspeed);
}

/*** 
 * @brief  根据相对速度来设置轮子速度
 * @author 叮咚蛋
 * @param :想要设置的速度 speed_wantset
 * @return void
 */

void calculate_wheel_speed(Velocity speed_wantset)
{

	/* 以下全部解算没有测试，暂时放在这，留下这个意思 */
#ifdef OMNI_3 //全向等边三轮
	float WheelSpeed[3];
	WheelSpeed[0] = (-speed_wantset.Vx - speed_wantset.Vw * Radius * PI / 180.f);
	WheelSpeed[1] = (cos_60 * speed_wantset.Vx - cos_30 * speed_wantset.Vy - speed_wantset.Vw * Radius * PI / 180.f);
	WheelSpeed[2] = (cos_60 * speed_wantset.Vx + cos_30 * speed_wantset.Vy - speed_wantset.Vw * Radius * PI / 180.f);
#endif

#ifdef OMNI_4a //全向正方形四轮
	float WheelSpeed[4];
	WheelSpeed[0] = (-cos_45 * speed_wantset.Vx + cos_45 * speed_wantset.Vy + speed_wantset.Vw * Radius * PI / 180.f);
	WheelSpeed[1] = (cos_45 * speed_wantset.Vx + cos_45 * speed_wantset.Vy + speed_wantset.Vw * Radius * PI / 180.f);
	WheelSpeed[2] = (-cos_45 * speed_wantset.Vx + cos_45 * speed_wantset.Vy + speed_wantset.Vw * Radius * PI / 180.f);
	WheelSpeed[3] = (cos_45 * speed_wantset.Vx + cos_45 * speed_wantset.Vy + speed_wantset.Vw * Radius * PI / 180.f);
#endif
#ifdef OMNI_4 //全向正方形四轮

#endif

#ifdef OMNI_4b //全向长方形四轮
	float WheelSpeed[4];
	float m = 30, n = 20;
	WheelSpeed[0] = (-cos_45 * speed_wantset.Vx + cos_45 * speed_wantset.Vy + (m + n) * speed_wantset.Vw * Radius * PI / 180.f);
	WheelSpeed[1] = (-cos_45 * speed_wantset.Vx - cos_45 * speed_wantset.Vy + (-m + n) * speed_wantset.Vw * Radius * PI / 180.f);
	WheelSpeed[2] = (cos_45 * speed_wantset.Vx - cos_45 * speed_wantset.Vy + (-m + -n) * speed_wantset.Vw * Radius * PI / 180.f);
	WheelSpeed[3] = (cos_45 * speed_wantset.Vx + cos_45 * speed_wantset.Vy + (m + -n) * speed_wantset.Vw * Radius * PI / 180.f);
#endif

#ifdef MECANUM //麦克纳姆轮
	float WheelSpeed[4];
	float m = 30, n = 20;
	WheelSpeed[0] = (-1 * speed_wantset.Vx + (-1) * speed_wantset.Vy + (m + n) * speed_wantset.Vw * Radius * PI / 180.f);
	WheelSpeed[1] = (-1 * speed_wantset.Vx + (-1) * speed_wantset.Vy + (m + n) * speed_wantset.Vw * Radius * PI / 180.f);
	WheelSpeed[2] = (1 * speed_wantset.Vx + (1) * speed_wantset.Vy + (m + n) * speed_wantset.Vw * Radius * PI / 180.f);
	WheelSpeed[3] = (1 * speed_wantset.Vx + (1) * speed_wantset.Vy + (m + n) * speed_wantset.Vw * Radius * PI / 180.f);
#endif

#ifdef THREE_STEER

	// float WheelSpeed[4];
	int i = 0;
	Wheel[0].Vx = speed_wantset.Vx - speed_wantset.Vw * chassis.width_half * 4 / 3 * PI / 180.f; // 单位有问题，？试一试旋转，
	Wheel[0].Vy = speed_wantset.Vy;
	Wheel[0].V = sqrt(Wheel[0].Vx * Wheel[0].Vx + Wheel[0].Vy * Wheel[0].Vy);

	Wheel[1].Vx = speed_wantset.Vx + speed_wantset.Vw * chassis.width_half * 2 / 3 * PI / 180.f;
	Wheel[1].Vy = speed_wantset.Vy - speed_wantset.Vw * chassis.length_half * PI / 180.f;
	Wheel[1].V = sqrt(Wheel[1].Vx * Wheel[1].Vx + Wheel[1].Vy * Wheel[1].Vy);

	Wheel[2].Vx = speed_wantset.Vx + speed_wantset.Vw * chassis.width_half * 2 / 3 * PI / 180.f;
	Wheel[2].Vy = speed_wantset.Vy + speed_wantset.Vw * chassis.length_half * PI / 180.f;
	Wheel[2].V = sqrt(Wheel[2].Vx * Wheel[2].Vx + Wheel[2].Vy * Wheel[2].Vy);

	for (i = 0; i < 3; i++)
	{
		if (Wheel[i].V > -1.0f && Wheel[i].V < 1.0f)
		{
			Wheel[i].Angle = Wheel[i].Angle; //保持上次位置不变
			Wheel[i].V = 0;
		}
		else
		{
			Wheel[i].Angle = PI / 2 - acos(Wheel[i].Vx / Wheel[i].V); //?aê2?′òaè?ò???￡?2??±?ó??angle??￡?￡?￡?

			if ((Wheel[i].Vy < 0) && (Wheel[i].Vx <= 0)) //μúèy?ó?T?ò??y?o?á
			{
				Wheel[i].Angle = -PI - Wheel[i].Angle;
			}
			else
			{
				if ((Wheel[i].Vy) < 0 && (Wheel[i].Vx > 0)) //μú???ó?T
					Wheel[i].Angle = PI - Wheel[i].Angle;
			}

			//			if(Wheel[i].Vy < 0){
			//				Wheel[i].Angle = (Wheel[i].Vx <= 0 ? -PI : PI) - Wheel[i].Angle;
			//			}
		}

		if ((Wheel[i].Angle - wheel_angle_set[i]) < (-PI / 2.0f)) //è?1?×aμ????èì?′ó
		{
			Wheel[i].Angle += PI;
			Wheel[i].V = -Wheel[i].V;
		}
		else if ((Wheel[i].Angle - wheel_angle_set[i]) > (PI / 2.0f)) //è?1?×aμ????èì?′ó
		{
			Wheel[i].Angle -= PI;
			Wheel[i].V = -Wheel[i].V;
		}

		wheel_angle_set[i] = Wheel[i].Angle;
	}

#endif

#ifdef STEER //舵轮
	// float WheelSpeed[4];
	int i = 0;
	Wheel[0].Vx = speed_wantset.Vx - speed_wantset.Vw * chassis.width_half * PI / 180.f;
	Wheel[0].Vy = speed_wantset.Vy + speed_wantset.Vw * chassis.length_half * PI / 180.f;
	Wheel[0].V = sqrt(Wheel[0].Vx * Wheel[0].Vx + Wheel[0].Vy * Wheel[0].Vy);

	Wheel[1].Vx = speed_wantset.Vx - speed_wantset.Vw * chassis.width_half * PI / 180.f;
	Wheel[1].Vy = speed_wantset.Vy - speed_wantset.Vw * chassis.length_half * PI / 180.f;
	Wheel[1].V = sqrt(Wheel[1].Vx * Wheel[1].Vx + Wheel[1].Vy * Wheel[1].Vy);

	Wheel[2].Vx = speed_wantset.Vx + speed_wantset.Vw * chassis.width_half * PI / 180.f;
	Wheel[2].Vy = speed_wantset.Vy - speed_wantset.Vw * chassis.length_half * PI / 180.f;
	Wheel[2].V = sqrt(Wheel[2].Vx * Wheel[2].Vx + Wheel[2].Vy * Wheel[2].Vy);

	Wheel[3].Vx = speed_wantset.Vx + speed_wantset.Vw * chassis.width_half * PI / 180.f;
	Wheel[3].Vy = speed_wantset.Vy + speed_wantset.Vw * chassis.length_half * PI / 180.f;
	Wheel[3].V = sqrt(Wheel[3].Vx * Wheel[3].Vx + Wheel[3].Vy * Wheel[3].Vy);

	QuadrantIs(); //判断象限

	for (i = 0; i < 4; i++)
	{
		if (Wheel[i].V > -1.0f && Wheel[i].V < 1.0f) // 这个阈值，速度=0时，只有减速停下时，存在这种情况，感觉这一个判断duck不必，待删除
		{
			Wheel[i].Angle = Wheel[i].Angle; //保持上次位置不变
			Wheel[i].V = 0;
		}
		else
		{
			Wheel[i].Angle = PI / 2 - acos(Wheel[i].Vx / Wheel[i].V); //

			if ((Wheel[i].Vy < 0) && (Wheel[i].Vx <= 0)) //
			{
				Wheel[i].Angle = -PI - Wheel[i].Angle;
			}
			else
			{
				if ((Wheel[i].Vy) < 0 && (Wheel[i].Vx > 0)) //
					Wheel[i].Angle = PI - Wheel[i].Angle;
			}
		}

		if ((Wheel[i].Angle - wheel_angle_set[i]) < (-PI / 2.0f)) //
		{
			Wheel[i].Angle += PI;
			Wheel[i].V = -Wheel[i].V;
		}
		else if ((Wheel[i].Angle - wheel_angle_set[i]) > (PI / 2.0f)) //
		{
			Wheel[i].Angle -= PI;
			Wheel[i].V = -Wheel[i].V;
		}

		wheel_angle_set[i] = Wheel[i].Angle;

		// 待测试
		/* if (0 == queue_flag.LockPointOn) //未开锁点 正逆可转180°
		{
			if (Wheel[i].V > -1.0f && Wheel[i].V < 1.0f)
			{
				Wheel[i].Angle = WheelAngleSet[i]; //保持上次的位置不变
				Wheel[i].V = 0;
			}
			else
			{
				Wheel[i].Angle = PI / 2 - acos(Wheel[i].Vx / Wheel[i].V);

				if ((Wheel[i].Vy < 0) && (Wheel[i].Vx <= 0))
				{
					//第三象限或者y负轴
					Wheel[i].Angle = -PI - Wheel[i].Angle;
				}
				else
				{
					if ((Wheel[i].Vy) < 0 && (Wheel[i].Vx > 0)) //第四象限
						Wheel[i].Angle = PI - Wheel[i].Angle;
				}
			}
			if ((Wheel[i].Angle - WheelAngleSet[i]) < (-PI / 2.0f)) //如果转的角度太大
			{
				Wheel[i].Angle += PI;
				Wheel[i].V = -Wheel[i].V;
			}
			else if ((Wheel[i].Angle - WheelAngleSet[i]) > (PI / 2.0f)) //如果转的角度太大
			{
				Wheel[i].Angle -= PI;
				Wheel[i].V = -Wheel[i].V;
			}

			WheelAngleSet[i] = Wheel[i].Angle; //记录下此次的角度
		}
		else //锁点 只在一二象限转
		{
			if (Wheel[i].V > -1.0f && Wheel[i].V < 1.0f)
			{
				Wheel[i].Angle = WheelAngleSet[i]; //保持上次的位置不变
				Wheel[i].V = 0;
			}
			else
			{
				Wheel[i].Angle = PI / 2 - acos(Wheel[i].Vx / Wheel[i].V);
				if (3 == Quadrant[i]) //第三象限 含y-
				{
					Wheel[i].Angle = -Wheel[i].Angle; //转到第一象限对应角度 轮子反向
					Wheel[i].V = -Wheel[i].V;
				}
				else if (4 == Quadrant[i])
				{
					Wheel[i].Angle = -Wheel[i].Angle; //转到第二象限对应角度 轮子反向
					Wheel[i].V = -Wheel[i].V;
				}
			}
		} */
	}
#endif

	set_wheel();
}

extern u8 FlagRunPoint7;
float angleLast[4] = {0};
u8 anledd = 3;

/**
 * @author: 叮咚蛋
 * @brief: 设置轮子速度
 * @param  void
 * @return void
 */
static void set_wheel()
{
	if (enable == motor.state)
	{
#ifdef STEER
		turn_motor_4PAx30(30 * wheel_angle_set[0] * 180 / PI, 30 * wheel_angle_set[1] * 180 / PI, //单位是角度
						  30 * wheel_angle_set[2] * 180 / PI, 30 * wheel_angle_set[3] * 180 / PI, 0);

		Elmo_Setspeed(1, Wheel[0].V, 0); //这是转速啊，
		Elmo_Setspeed(2, Wheel[1].V, 0);
		Elmo_Setspeed(3, Wheel[2].V, 0);
		Elmo_Setspeed(4, Wheel[3].V, 0);

#endif

#ifdef THREE_STEER
		turn_motor_4PAx30(30 * wheel_angle_set[0] * 180 / PI, 30 * wheel_angle_set[1] * 180 / PI, //单位是角度
						  30 * wheel_angle_set[2] * 180 / PI, 30 * wheel_angle_set[3] * 180 / PI, 0);

		Elmo_Setspeed(1, Wheel[0].V, 0); //这是转速啊，
		Elmo_Setspeed(2, Wheel[1].V, 0);
		Elmo_Setspeed(3, Wheel[2].V, 0);

#endif
		Elmo_Begin(0, 0);
	}
}

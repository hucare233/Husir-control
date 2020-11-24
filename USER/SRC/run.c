#include "run.h"

/* Position */
volatile Position position_now = {0.0, 0.0, 0.0};
volatile Position position_start = {0.0, 0.0, 0.0};
volatile Position position_goal = {0.0, 0.0, 0.0};
volatile Position position_want = {0.0, 0.0, 0.0};
volatile Position lock_point = {0.0, 0.0, 0.0};
volatile Position start_turn_point = {0.0, 0.0, 0.0};
/* Velocity */
volatile Velocity velocity_now = {0.0, 0.0, 0.0, 0.0};
volatile Velocity velocity_start = {0.0, 0.0, 0.0, 0.0};
volatile Velocity velocity_goal = {0.0, 0.0, 0.0, 0.0};
volatile Velocity velocity_set = {0.0, 0.0, 0.0, 0.0};
volatile Velocity velocity_abs = {0.0, 0.0, 0.0, 0.0};
volatile WheelInfo Wheel[4] = {{0.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 0.0f}};

volatile float wheel_angle_set[4] = {0.0f};

/**
	* @brief  直线跑点函数
	* @param  points 期望点坐标、角度
	* @param  Vstart_Vwant_Vstop 起步速度、最大期望速度、结束速度
	* @param  UpDis_DownDis 加减速距离
	* @param  StartTurnDis_FullTurnDis 开始转角度距离、停止旋转出距离
	* @param  continue_flag 是否接着下一个轨迹跑
	*/
void RunPoint(float x_want, float y_want, float angle_want,
			  float v_start, float v_want, float v_end,
			  float up_dis, float down_dis, float start_rotate_dis, float rotate_need_dis,
			  u8 continue_flag)
{
	/* 计算跑车时间 */
	run_time.count_time = TRUE;
	run_time.run_start = run_time.clk;

	/* flag 初始化 */
	if (v_want > v_start)
		run.speed_up = TRUE;
	else
		run.speed_up = FALSE;
	run.speed_up_over = !run.speed_up;

	if (v_want > v_end)
		run.speed_down = TRUE;
	else
		run.speed_down = FALSE;
	run.speed_down_over = TRUE;

	if (abs(lock_point.angle - angle_want) > 1.0f) // 上一次角度与期望角度比较
		run.rotate = TRUE;
	else
		run.rotate = FALSE;
	run.rotate_over = !run.rotate;

	run.continue_next = continue_flag;

	run.speed_down_mode_change = TRUE; // 减速mode切换flag

	/* set parameters */
	run.speed_want = v_want; 
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
	}
	if (run.up_dis + run.down_dis > run.goal_dis) //加减速距离之和大于总距离
	{
		run.up_dis = run.goal_dis / 3.0f;
		run.down_dis = run.goal_dis / 3.0f;
	}

	/* 直线斜率截距计算 */
	if (abs(position_goal.x - position_start.x) < 0.5f) //直线斜率过大，近似无斜率
	{
		run.no_line_K = TRUE;			//直线无斜率
		run.pid_line_type = pid_line_x; //直线的PID的类型  以X方向的偏差做调节
	}
	else
	{
		run.no_line_K = FALSE;
		run.line_k = (position_goal.y - position_start.y) / (position_goal.x - position_start.x); //直线斜率
		run.line_k_cos = 1 / sqrt(1 + run.line_k * run.line_k);									  //直线倾角的余弦
		if (abs(run.line_k) < 1)																  //如果斜率小于1，以Y方向偏差做调节
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

	run.pid_point = FALSE; //关闭位置锁定

	velocity_set.V = run.speed_start; //把速度赋初值
	velocity_set.Vw = 0.0;
	lock_point.angle = position_start.angle;

	run.auto_type = RUNPOINT;
	PidInit(&run_pid, pid_para_run_point); //PID初始化
	PidInit(&angle_pid, pid_para_angle);   //PID初始化
	ENABLE_ALL_MOTOR();					   //使能电机才能跑
	run.pid_run = TRUE;					   //打开PID调节
	// OS_EXIT_CRITICAL();

	//计算期望坐标与当前坐标数量积大小,来判断是否跑到了期望坐标，是不是可以其他判断方式更好,反应出的物理意义是啥？
	while (((position_goal.x - position_start.x) * (position_now.x - position_goal.x) + (position_goal.y - position_start.y) * (position_now.y - position_goal.y)) < -2.0f)
		OSTimeDly(50);

	if (run.continue_next == FALSE)
	{
		lock_point = position_goal; // 期望坐标锁定
		run.pid_run = FALSE;
		run.pid_point = TRUE;							   // 切换pid到锁点
		PidInit(&lock_point_pid_x, pid_para_lock_point_x); //PID初始化
		PidInit(&lock_point_pid_y, pid_para_lock_point_y); //PID初始化
		PidInit(&angle_pid, pid_para_angle);			   //PID初始化

		run_time.count_time = FALSE;
	}
	else
	{
		run.pid_run = FALSE;
		run.pid_point = FALSE;
	}
}

/** 
  * @brief	原地旋转
  */
void Rotate(float angle_want, float v_w, u8 continue_flag)
{
	run.rotate = TRUE;
	run.rotate_over = FALSE;

	position_goal.x = position_now.x;
	position_goal.y = position_now.y;
	position_goal.angle = angle_want;

	run.pid_point = FALSE; //关闭位置锁定
	run.pid_run = FALSE;   //关闭PID调节

	ENABLE_ALL_MOTOR(); //使能电机才能跑

	velocity_set.Vw = v_w;
	velocity_set.Vx = 0.0f;
	velocity_set.Vy = 0.0f;

	while (abs(position_goal.angle - position_now.angle) > 2.0f)
	{
		OSTimeDly(50);
		SetChassisVelocity(velocity_set, position_now.angle);
	}
	if (FALSE == continue_flag)
	{
		lock_point = position_goal; // 期望坐标锁定
		run.pid_run = FALSE;
		run.pid_point = TRUE;							   // 切换pid到锁点
		PidInit(&lock_point_pid_x, pid_para_lock_point_x); //PID初始化
		PidInit(&lock_point_pid_y, pid_para_lock_point_y); //PID初始化
		PidInit(&angle_pid, pid_para_angle);			   //PID初始化
	}
	else
	{
		run.pid_run = FALSE;
		run.pid_point = FALSE;
	}
}
/** 
  * @brief	直线跑点加速子程序
  */
void RunPointSpeedUp(void)
{
	float dis_temp;
	if (abs(velocity_now.V - run.speed_want) > 10) //当前速度没有达到目标速度; FIXME: 阈值判定容易出问题，极有可能车已经跑过了这个速度，
	{
		dis_temp = CountDistance(position_start.x, position_start.y, position_now.x, position_now.y);
		if (dis_temp < run.up_dis) //行走距离小于加速距离，按以下算法进行加速 TODO: 必须研究一下怎么发挥最大摩擦提供的加速度
		{
			velocity_set.V = run.speed_start - (run.speed_start - run.speed_want) * sqrt(dis_temp / run.up_dis); //恒加速度  x=k * t * t   a = k‘
		}
		else //到达加速距离，不再加速了，但是此时速度可能并没有到达,直接赋速度
		{
			velocity_set.V = run.speed_want;
			run.speed_up = FALSE;
			run.speed_up_over = TRUE;
		}
	}
	else //速度已经达到了目标速度 ，可能此时还没走完加速距离
	{
		velocity_set.V = run.speed_want;
		run.speed_up = FALSE;
		run.speed_up_over = TRUE;
	}
}

/**
	* @brief  直线跑点减速子程序
	*/
void RunPointSpeedDown(void) //好好研究运动学算法
{
	const float CubeDownDis = (1 - run.Proportion) * run.down_dis; //FIXME:
	float Cubetemp;
	float Cubev_start_brake;
	float dis_temp = CountDistance(position_now.x, position_now.y, position_goal.x, position_goal.y);

	if (dis_temp <= run.down_dis)
	{
		if (TRUE == run.speed_down_over)
		{
			run_time.down_start = run_time.clk;
			run.v_start_brake = velocity_now.V;
			run.speed_down_over = FALSE; // the flag is a temp flag here
		}

		if (FALSE == run.speed_down_over)
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

			if ((abs(velocity_now.V) <= 50) && (run.speed_down_mode_change == TRUE)) // FIXME: 准备把这个改了
			{
				run.speed_down_mode = SQUAREROOT;
				run.speed_down_mode_change = FALSE;
				run.v_start_brake = abs(velocity_now.V);
				run.down_dis = dis_temp;
			}
		}
	}
}

/** 
	* @brief  跑点旋转子程序
	*/
void RunPointRotate(void) //边直线行走边旋转的旋转子程序
{
	float temp = CountDistance(position_start.x, position_start.y, position_now.x, position_now.y);
	static u8 inner_init_flag = TRUE;

	if (TRUE == inner_init_flag && temp > run.rotate_start)
	{
		//到达旋转点，记录坐标
		start_turn_point.x = position_now.x;
		start_turn_point.y = position_now.y;
		inner_init_flag = FALSE;
	}
	else if (temp > run.rotate_start + run.rotate_need)
	{
		inner_init_flag = TRUE;

		run.rotate = FALSE;
		run.rotate_over = TRUE;

		lock_point.angle = position_goal.angle;
		return;
	}

	if (temp > run.rotate_start && FALSE == run.rotate_over) //按以下的算法进行旋转处理，跟位移呈线性关系
	{
		temp = CountDistance(start_turn_point.x, start_turn_point.y, position_now.x, position_now.y);
		lock_point.angle = position_start.angle + (position_goal.angle - position_start.angle) * temp / run.rotate_need;
	}
}

/** 
	* @brief  赋值PID调节之前的原始速度xy分量
	*/
void SetPointSpeedBase(void)
{
	if (TRUE == run.no_line_K)
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


/*********************** 计算轮子转速 ***********************/

/**
	* @brief 判断舵象限
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

/** 
	* @brief 根据想要达到的绝对速度来计算相对速度
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

/** 
	* @brief  根据相对速度来设置轮子速度
	*/
void calculate_wheel_speed(Velocity speed_wantset)
{
	//全向正方形四轮
	Wheel[0].V = (-cos_45 * speed_wantset.Vx + cos_45 * speed_wantset.Vy + speed_wantset.Vw * chassis.omni_wheel2center * PI / 180.f);
	Wheel[1].V = (-cos_45 * speed_wantset.Vx - cos_45 * speed_wantset.Vy + speed_wantset.Vw * chassis.omni_wheel2center * PI / 180.f);
	Wheel[2].V = (cos_45 * speed_wantset.Vx - cos_45 * speed_wantset.Vy + speed_wantset.Vw * chassis.omni_wheel2center * PI / 180.f);
	Wheel[3].V = (cos_45 * speed_wantset.Vx + cos_45 * speed_wantset.Vy + speed_wantset.Vw * chassis.omni_wheel2center * PI / 180.f);

	set_wheel();
}

/** 
	* @brief  输出转速
	*/
static void set_wheel()
{
	if (ENABLE == FlagPara.Mode_Run)
	{
		Elmo_C_Speed(1, FALSE, Wheel[0].V); 
		Elmo_C_Speed(2, FALSE, Wheel[1].V);
		Elmo_C_Speed(3, FALSE, Wheel[2].V);
		Elmo_C_Speed(4, FALSE, Wheel[3].V);
	}
}



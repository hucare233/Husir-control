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
	* @brief  ֱ���ܵ㺯��
	* @param  points ���������ꡢ�Ƕ�
	* @param  Vstart_Vwant_Vstop ���ٶȡ���������ٶȡ������ٶ�
	* @param  UpDis_DownDis �Ӽ��پ���
	* @param  StartTurnDis_FullTurnDis ��ʼת�ǶȾ��롢ֹͣ��ת������
	* @param  continue_flag �Ƿ������һ���켣��
	*/
void RunPoint(float x_want, float y_want, float angle_want,
			  float v_start, float v_want, float v_end,
			  float up_dis, float down_dis, float start_rotate_dis, float rotate_need_dis,
			  u8 continue_flag)
{
	/* �����ܳ�ʱ�� */
	run_time.count_time = TRUE;
	run_time.run_start = run_time.clk;

	/* flag ��ʼ�� */
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

	if (abs(lock_point.angle - angle_want) > 1.0f) // ��һ�νǶ��������ǶȱȽ�
		run.rotate = TRUE;
	else
		run.rotate = FALSE;
	run.rotate_over = !run.rotate;

	run.continue_next = continue_flag;

	run.speed_down_mode_change = TRUE; // ����mode�л�flag

	/* set parameters */
	run.speed_want = v_want; 
	run.speed_start = v_start;
	run.speed_end = v_end;

	position_start.x = position_now.x;
	position_start.y = position_now.y;
	position_start.angle = lock_point.angle;

	position_goal.x = x_want; //Ŀ���
	position_goal.y = y_want;
	position_goal.angle = angle_want;

	run.up_dis = up_dis;	 //���پ���
	run.down_dis = down_dis; //���پ���
	run.goal_dis = CountDistance(position_start.x, position_start.y,
								 position_goal.x, position_goal.y); //�ƶ��ܾ���

	run.rotate_start = start_rotate_dis; //��ʼ��ת�ľ���
	run.rotate_need = rotate_need_dis;	 //��ת������ܾ���

	/* error handling */
	if (run.rotate_start + run.rotate_need > run.goal_dis) //��ʼ��תʱ�ľ���������ľ���֮�ʹ����ܾ���
	{
		run.rotate_start = run.goal_dis / 3.0f;
		run.rotate_need = run.goal_dis / 3.0f;
	}
	if (run.up_dis + run.down_dis > run.goal_dis) //�Ӽ��پ���֮�ʹ����ܾ���
	{
		run.up_dis = run.goal_dis / 3.0f;
		run.down_dis = run.goal_dis / 3.0f;
	}

	/* ֱ��б�ʽؾ���� */
	if (abs(position_goal.x - position_start.x) < 0.5f) //ֱ��б�ʹ��󣬽�����б��
	{
		run.no_line_K = TRUE;			//ֱ����б��
		run.pid_line_type = pid_line_x; //ֱ�ߵ�PID������  ��X�����ƫ��������
	}
	else
	{
		run.no_line_K = FALSE;
		run.line_k = (position_goal.y - position_start.y) / (position_goal.x - position_start.x); //ֱ��б��
		run.line_k_cos = 1 / sqrt(1 + run.line_k * run.line_k);									  //ֱ����ǵ�����
		if (abs(run.line_k) < 1)																  //���б��С��1����Y����ƫ��������
		{
			run.pid_line_type = pid_line_y;
			run.line_b = position_start.y - position_start.x * run.line_k;
		}
		else //������X�����ƫ��������
		{
			run.pid_line_type = pid_line_x;
			run.line_b = position_start.x - position_start.y / run.line_k;
		}
	}

	run.pid_point = FALSE; //�ر�λ������

	velocity_set.V = run.speed_start; //���ٶȸ���ֵ
	velocity_set.Vw = 0.0;
	lock_point.angle = position_start.angle;

	run.auto_type = RUNPOINT;
	PidInit(&run_pid, pid_para_run_point); //PID��ʼ��
	PidInit(&angle_pid, pid_para_angle);   //PID��ʼ��
	ENABLE_ALL_MOTOR();					   //ʹ�ܵ��������
	run.pid_run = TRUE;					   //��PID����
	// OS_EXIT_CRITICAL();

	//�������������뵱ǰ������������С,���ж��Ƿ��ܵ����������꣬�ǲ��ǿ��������жϷ�ʽ����,��Ӧ��������������ɶ��
	while (((position_goal.x - position_start.x) * (position_now.x - position_goal.x) + (position_goal.y - position_start.y) * (position_now.y - position_goal.y)) < -2.0f)
		OSTimeDly(50);

	if (run.continue_next == FALSE)
	{
		lock_point = position_goal; // ������������
		run.pid_run = FALSE;
		run.pid_point = TRUE;							   // �л�pid������
		PidInit(&lock_point_pid_x, pid_para_lock_point_x); //PID��ʼ��
		PidInit(&lock_point_pid_y, pid_para_lock_point_y); //PID��ʼ��
		PidInit(&angle_pid, pid_para_angle);			   //PID��ʼ��

		run_time.count_time = FALSE;
	}
	else
	{
		run.pid_run = FALSE;
		run.pid_point = FALSE;
	}
}

/** 
  * @brief	ԭ����ת
  */
void Rotate(float angle_want, float v_w, u8 continue_flag)
{
	run.rotate = TRUE;
	run.rotate_over = FALSE;

	position_goal.x = position_now.x;
	position_goal.y = position_now.y;
	position_goal.angle = angle_want;

	run.pid_point = FALSE; //�ر�λ������
	run.pid_run = FALSE;   //�ر�PID����

	ENABLE_ALL_MOTOR(); //ʹ�ܵ��������

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
		lock_point = position_goal; // ������������
		run.pid_run = FALSE;
		run.pid_point = TRUE;							   // �л�pid������
		PidInit(&lock_point_pid_x, pid_para_lock_point_x); //PID��ʼ��
		PidInit(&lock_point_pid_y, pid_para_lock_point_y); //PID��ʼ��
		PidInit(&angle_pid, pid_para_angle);			   //PID��ʼ��
	}
	else
	{
		run.pid_run = FALSE;
		run.pid_point = FALSE;
	}
}
/** 
  * @brief	ֱ���ܵ�����ӳ���
  */
void RunPointSpeedUp(void)
{
	float dis_temp;
	if (abs(velocity_now.V - run.speed_want) > 10) //��ǰ�ٶ�û�дﵽĿ���ٶ�; FIXME: ��ֵ�ж����׳����⣬���п��ܳ��Ѿ��ܹ�������ٶȣ�
	{
		dis_temp = CountDistance(position_start.x, position_start.y, position_now.x, position_now.y);
		if (dis_temp < run.up_dis) //���߾���С�ڼ��پ��룬�������㷨���м��� TODO: �����о�һ����ô�������Ħ���ṩ�ļ��ٶ�
		{
			velocity_set.V = run.speed_start - (run.speed_start - run.speed_want) * sqrt(dis_temp / run.up_dis); //����ٶ�  x=k * t * t   a = k��
		}
		else //������پ��룬���ټ����ˣ����Ǵ�ʱ�ٶȿ��ܲ�û�е���,ֱ�Ӹ��ٶ�
		{
			velocity_set.V = run.speed_want;
			run.speed_up = FALSE;
			run.speed_up_over = TRUE;
		}
	}
	else //�ٶ��Ѿ��ﵽ��Ŀ���ٶ� �����ܴ�ʱ��û������پ���
	{
		velocity_set.V = run.speed_want;
		run.speed_up = FALSE;
		run.speed_up_over = TRUE;
	}
}

/**
	* @brief  ֱ���ܵ�����ӳ���
	*/
void RunPointSpeedDown(void) //�ú��о��˶�ѧ�㷨
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
			case SQUAREROOT: //��ͬ��ϵ���͵ļ������ߣ�
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

			if ((abs(velocity_now.V) <= 50) && (run.speed_down_mode_change == TRUE)) // FIXME: ׼�����������
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
	* @brief  �ܵ���ת�ӳ���
	*/
void RunPointRotate(void) //��ֱ�����߱���ת����ת�ӳ���
{
	float temp = CountDistance(position_start.x, position_start.y, position_now.x, position_now.y);
	static u8 inner_init_flag = TRUE;

	if (TRUE == inner_init_flag && temp > run.rotate_start)
	{
		//������ת�㣬��¼����
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

	if (temp > run.rotate_start && FALSE == run.rotate_over) //�����µ��㷨������ת������λ�Ƴ����Թ�ϵ
	{
		temp = CountDistance(start_turn_point.x, start_turn_point.y, position_now.x, position_now.y);
		lock_point.angle = position_start.angle + (position_goal.angle - position_start.angle) * temp / run.rotate_need;
	}
}

/** 
	* @brief  ��ֵPID����֮ǰ��ԭʼ�ٶ�xy����
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


/*********************** ��������ת�� ***********************/

/**
	* @brief �ж϶�����
	*/
void QuadrantIs(void)
{
	int i = 0;
	for (i = 0; i < 4; i++)
	{
		if (Wheel[i].Vx >= 0 && Wheel[i].Vy >= 0) //����x+ y+
			Wheel[i].quadrant = 1;
		else if (Wheel[i].Vx <= 0 && Wheel[i].Vy > 0) //����x-
			Wheel[i].quadrant = 2;
		else if (Wheel[i].Vx <= 0 && Wheel[i].Vy < 0) //����y-
			Wheel[i].quadrant = 3;
		else
			Wheel[i].quadrant = 4;
	}
}

/** 
	* @brief ������Ҫ�ﵽ�ľ����ٶ�����������ٶ�
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
	* @brief  ��������ٶ������������ٶ�
	*/
void calculate_wheel_speed(Velocity speed_wantset)
{
	//ȫ������������
	Wheel[0].V = (-cos_45 * speed_wantset.Vx + cos_45 * speed_wantset.Vy + speed_wantset.Vw * chassis.omni_wheel2center * PI / 180.f);
	Wheel[1].V = (-cos_45 * speed_wantset.Vx - cos_45 * speed_wantset.Vy + speed_wantset.Vw * chassis.omni_wheel2center * PI / 180.f);
	Wheel[2].V = (cos_45 * speed_wantset.Vx - cos_45 * speed_wantset.Vy + speed_wantset.Vw * chassis.omni_wheel2center * PI / 180.f);
	Wheel[3].V = (cos_45 * speed_wantset.Vx + cos_45 * speed_wantset.Vy + speed_wantset.Vw * chassis.omni_wheel2center * PI / 180.f);

	set_wheel();
}

/** 
	* @brief  ���ת��
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



/*
 * @Descripttion: run
 * @version: ��һ��
 * @Author: ���˵�
 * @Date: 2020-12-15 19:29:29
 * @LastEditors: ���˵�
 * @LastEditTime: 2020-12-16 10:12:45
 * @FilePath: \hu_sir-contorl\USER\SRC\run.c
 */

#include "run.h"

volatile RunFlagStruct Run;

// float run.Proportion = 0.4; //�������ϵ�� ���ڵ��ڿ�ʼ����ʱ���ٶ�

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

//Bezier����
BezierStruct BezierData; //Bezier������ز���

volatile BezierPoint bezier_point1;
volatile BezierPoint bezier_point2;

/*** 
 * @brief  ֱ���ܵ㺯��
 * @param  points ���������ꡢ�Ƕ�
 * @param  Vstart_Vwant_Vstop ���ٶȡ���������ٶȡ������ٶ�
 * @param  UpDis_DownDis �Ӽ��پ���
 * @param  StartTurnDis_FullTurnDis ��ʼת�ǶȾ��롢ֹͣ��ת������
 * @param  continue_flag �Ƿ������һ���켣��
 * @author ���˵�
 */

void RunPoint(float x_want, float y_want, float angle_want,
			  float v_start, float v_want, float v_end,
			  float up_dis, float down_dis, float start_rotate_dis, float rotate_need_dis,
			  u8 continue_flag)
{
	//�����ٽ�� ��ֹ��ʼ�������, // duck ���أ�
	// OS_ENTER_CRITICAL();

	/* �����ܳ�ʱ�� */
	run_time.count_time = True;
	run_time.run_start = run_time.clk;

	/* flag ��ʼ�� */
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

	if (ABS(lock_point.angle - angle_want) > 1.0f) // ��һ�νǶ��������ǶȱȽ�
		run.rotate = True;
	else
		run.rotate = False;
	run.rotate_over = !run.rotate;

	run.continue_next = continue_flag;

	run.speed_down_mode_change = True; // ����mode�л�flag

	/* set parameters */
	run.speed_want = v_want; // TODO: �����⣬ΪɶҪ�����٣�һֱ����Ȼ����ٲ���ô���������ס���Ҿ��õ�����ԣ�
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
		Beep_Show(8);
		sprintf(user.error, "%s", "RotateParaErr");
	}
	if (run.up_dis + run.down_dis > run.goal_dis) //�Ӽ��پ���֮�ʹ����ܾ���
	{
		run.up_dis = run.goal_dis / 3.0f;
		run.down_dis = run.goal_dis / 3.0f;
		Beep_Show(8);
		sprintf(user.error, "%s", "UpDwonParaErr");
	}

	/* ֱ��б�ʽؾ���� */
	if (ABS(position_goal.x - position_start.x) < 0.5f) //ֱ��б�ʹ��󣬽�����б��
	{
		run.no_line_K = True;			//ֱ����б��
		run.pid_line_type = pid_line_x; //ֱ�ߵ�PID������  ��X�����ƫ��������
	}
	else
	{
		run.no_line_K = False;
		run.line_k = (position_goal.y - position_start.y) / (position_goal.x - position_start.x); //ֱ��б��
		run.line_k_cos = 1 / sqrt(1 + run.line_k * run.line_k);									  //ֱ����ǵ�����
		if (ABS(run.line_k) < 1)																  //���б��С��1����Y����ƫ��������
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

	/* set flag and parameters about pid*/
	run.pid_point = False; //�ر�λ������

	velocity_set.V = run.speed_start; //���ٶȸ���ֵ
	velocity_set.Vw = 0.0;
	lock_point.angle = position_start.angle; //��ʼ��̬�ǲ���

	run.auto_type = RUNPOINT;
	PidInit(&run_pid, pid_para_run_point); //PID��ʼ��
	PidInit(&angle_pid, pid_para_angle);   //PID��ʼ��
	ENABLE_ALL_MOTOR;					   //ʹ�ܵ��������
	run.pid_run = True;					   //��PID����
	// OS_EXIT_CRITICAL();

	//�������������뵱ǰ������������С,���ж��Ƿ��ܵ����������꣬�ǲ��ǿ��������жϷ�ʽ����
	while (((position_goal.x - position_start.x) * (position_now.x - position_goal.x) +
			(position_goal.y - position_start.y) * (position_now.y - position_goal.y)) < -2.0f)
	{
		OSTimeDly(50);
	}

	if (run.continue_next == False)
	{
		lock_point = position_goal; // ������������
		run.pid_run = False;
		run.pid_point = True;							   // �л�pid������
		PidInit(&lock_point_pid_x, pid_para_lock_point_x); //PID��ʼ��
		PidInit(&lock_point_pid_y, pid_para_lock_point_y); //PID��ʼ��
		PidInit(&angle_pid, pid_para_angle);			   //PID��ʼ��

		run_time.count_time = False;
	}
	else
	{
		run.pid_run = False;
		run.pid_point = False;
	}
}

/**
 * @author: ���˵�
 * @brief: ��ת
 * @param ͬ
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

	run.pid_point = False; //�ر�λ������
	run.pid_run = False;   //�ر�PID����

	ENABLE_ALL_MOTOR; //ʹ�ܵ��������

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
		lock_point = position_goal; // ������������
		run.pid_run = False;
		run.pid_point = True;							   // �л�pid������
		PidInit(&lock_point_pid_x, pid_para_lock_point_x); //PID��ʼ��
		PidInit(&lock_point_pid_y, pid_para_lock_point_y); //PID��ʼ��
		PidInit(&angle_pid, pid_para_angle);			   //PID��ʼ��
	}
	else
	{
		run.pid_run = False;
		run.pid_point = False;
	}
}

/*** 
 * @brief  �����������ϵ��
 * @param  bezier_para �������������ں����Ƶ����������ϵ��
 * @return void
 * @author ���˵�
 */

void CountBezierCoefficient(BezierStruct *bezier_para)
{
	bezier_para->x_t[0] = bezier_para->P[0].x;																	 //x���������е�t��0����ϵ��
	bezier_para->x_t[1] = 3 * (bezier_para->P[1].x - bezier_para->P[0].x);										 //x���������е�t��1����ϵ��
	bezier_para->x_t[2] = 3 * (bezier_para->P[2].x - bezier_para->P[1].x) - bezier_para->x_t[1];				 //x���������е�t��2����ϵ��
	bezier_para->x_t[3] = bezier_para->P[3].x - bezier_para->P[0].x - bezier_para->x_t[2] - bezier_para->x_t[1]; //x���������е�t��3����ϵ��

	bezier_para->y_t[0] = bezier_para->P[0].y;																	 //Y���������е�t��0����ϵ��
	bezier_para->y_t[1] = 3 * (bezier_para->P[1].y - bezier_para->P[0].y);										 //Y���������е�t��1����ϵ��
	bezier_para->y_t[2] = 3 * (bezier_para->P[2].y - bezier_para->P[1].y) - bezier_para->y_t[1];				 //Y���������е�t��2����ϵ��
	bezier_para->y_t[3] = bezier_para->P[3].y - bezier_para->P[0].y - bezier_para->y_t[2] - bezier_para->y_t[1]; //Y���������е�t��3����ϵ��
}

/*** 
 * @brief  ���㱴�������������
 * @param  bezier_para
 * @param  bezier_point 
 * @return void
 * @author ���˵�
 */
void CountBezierXYbyT(BezierStruct volatile *bezier_para, BezierPoint volatile *bezier_point)
{
	float temp1, temp2, temp3;

	temp1 = bezier_point->t; //һ�η�
	temp2 = temp1 * temp1;	 //���η�
	temp3 = temp2 * temp1;	 //���η�

	bezier_point->x = bezier_para->x_t[3] * temp3 + bezier_para->x_t[2] * temp2 + bezier_para->x_t[1] * temp1 + bezier_para->x_t[0];
	bezier_point->y = bezier_para->y_t[3] * temp3 + bezier_para->y_t[2] * temp2 + bezier_para->y_t[1] * temp1 + bezier_para->y_t[0];
}

/*** 
 * @brief  ���㱴�������ߵ�����
 * @param  BezierStruct
 * @param  BezierPoint
 * @return void
 * @author ���˵�
 */
void CountBezierCurvaturebyT(BezierStruct volatile *bezier_para, BezierPoint volatile *bezier_point)
{
	float temp1;
	float temp2;
	float Xdifferential1;
	float Xdifferential2;
	float Ydifferential1;
	float Ydifferential2;
	temp1 = bezier_point->t; //һ�η�
	temp2 = temp1 * temp1;	 //���η�

	Xdifferential1 = 3 * bezier_para->x_t[3] * temp2 + 2 * bezier_para->x_t[2] * temp1 + bezier_para->x_t[1];
	Ydifferential1 = 3 * bezier_para->y_t[3] * temp2 + 2 * bezier_para->y_t[2] * temp1 + bezier_para->y_t[1];

	Xdifferential2 = 6 * bezier_para->x_t[3] * temp1 + 2 * bezier_para->x_t[2];
	Ydifferential2 = 6 * bezier_para->y_t[3] * temp1 + 2 * bezier_para->y_t[2];
	bezier_point->curvature = ABS(Xdifferential1 * Ydifferential2 - Xdifferential2 * Ydifferential1) / ((Xdifferential1 * Xdifferential1 + Ydifferential1 * Ydifferential1) * sqrt(Xdifferential1 * Xdifferential1 + Ydifferential1 * Ydifferential1));
}

/*** 
 * @brief  �ܱ�����
 * @param  
 * @return void
 * @author ���˵�
 */
void RunBezier(float *points, float angle_want,
			   float v_start, float v_want, float v_end,
			   float upt, float downt, float startturnt, float fullturnt,
			   u8 continue_flag, u8 follow_angle_flag)
{
	//�����ٽ�� ��ֹ��ʼ�������
	// OS_ENTER_CRITICAL();

	run_time.count_time = True;
	run_time.run_start = run_time.clk;

	/* flag ��ʼ�� */
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

	run.speed_down_mode_change = True; // ����mode�л�flag

	/* set parameters */
	velocity_set.V = v_want;   //Ŀ���ٶ�
	run.speed_start = v_start; //��ʼ�ٶ�
	run.speed_end = v_end;	   //��ֹ�ٶ�

	position_start.x = position_now.x; //��ʼ��
	position_start.y = position_now.y;
	memcpy(BezierData.P, points, 8);	 // �켣���Ƶ㸳ֵ
	CountBezierCoefficient(&BezierData); //����4�����Ƶ��������Bezier��������ϵ��

	//����DeltaT TODO: ���delta_t��ȫ���Ը���㣬 �����������Ӳ�����ʱ��
	run.delta_t = (2.0f / (CountDistance(BezierData.P[0].x, BezierData.P[0].y,
										 BezierData.P[3].x, BezierData.P[3].y)));
	//��ʼ��bezier_point1��bezier_point2
	bezier_point1.t = 0;
	bezier_point2.t = run.delta_t;
	CountBezierXYbyT(&BezierData, &bezier_point1); //����bezier_point1�е�tֵ�����Ӧ������
	CountBezierXYbyT(&BezierData, &bezier_point2); //����bezier_point1�е�tֵ�����Ӧ������

	//��ʼ������Vec_Line
	vec_line.x = bezier_point2.x - bezier_point1.x;
	vec_line.y = bezier_point2.y - bezier_point1.y;

	position_start.angle = lock_point.angle; // ��ʼ�Ƕ�Ϊ��һ�����ĽǶȡ�
	position_goal.x = BezierData.P[3].x;	 // Ŀ���
	position_goal.y = BezierData.P[3].y;
	position_goal.angle = angle_want; // ע��ת�Ƕ����Ƕ��ܵĳ�ͻ��

	/* bezier time para */
	run.up_t = upt;
	run.down_t = downt;
	run.rotate_start = startturnt;
	run.rotate_need = fullturnt;

	// bezier_group = pid_class; // TODO: �޸Ĳ�ͬ�����µ�pidӦ�÷����ⲿ���ֶ��޸�

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
	run.pid_point = False; //�ر�λ������
	run.pid_run = True;	   //��PID����
	run.auto_type = RUNBEZIER;
	PidInit(&run_pid, pid_para_run_bezier); //PID��ʼ��
	PidInit(&angle_pid, pid_para_angle);	//PID��ʼ��

	velocity_set.V = run.speed_start; //���ٶȸ���ֵ
	velocity_set.Vw = 0.0;
	lock_point.angle = position_start.angle; //������̬�ǲ���

	run.run_break = False; //�ñ�־λ����ǿ���˳���ǰrun���̣�ת����һ��run����
	ENABLE_ALL_MOTOR;	   //ʹ�ܵ��̵��

	//����Ҫ�ú��о�һ��
	// OS_EXIT_CRITICAL();
	while ((bezier_point1.t < (1 - run.delta_t)) && False == run.run_break)
	{
		OSTimeDly(50);
	}
	// TODO:�������ֻ�������while�ж����˲Ż�ִ�У���ô����goal != now������أ�ע������ 2020-08-01
	if (run.continue_next == False)
	{
		if (False == run.run_break) //����������˳���
		{
			lock_point = position_goal; // ������������
		}
		else
		{
			//�������˳���������ǰ�㣬����Ҫǿ�ƽ�����ǰrun���̣�ת����һ��run����ʱʹ��
			lock_point.x = position_now.x;
			lock_point.y = position_now.y;
			lock_point.angle = position_now.angle;
		}
		run.pid_run = False;
		run.pid_point = True;							   // �л�pid������
		PidInit(&lock_point_pid_x, pid_para_lock_point_x); //PID��ʼ��
		PidInit(&lock_point_pid_y, pid_para_lock_point_y); //PID��ʼ��
		PidInit(&angle_pid, pid_para_angle);			   //PID��ʼ��

		run_time.count_time = False;
	}
	else
	{
		run.pid_run = False;
		run.pid_point = False;
	}
}

/*** 
 * @brief  ֱ���ܵ�����ӳ���
 * @param  void
 * @return void
 * @author ���˵�
 */
void RunPointSpeedUp(void)
{
	float dis_temp = 0.0f;
	if (ABS(velocity_now.V - run.speed_want) > 10) //��ǰ�ٶ�û�дﵽĿ���ٶ�; FIXME: ��ֵ�ж����׳����⣬���п��ܳ��Ѿ��ܹ�������ٶȣ�
	{
		dis_temp = CountDistance(position_start.x, position_start.y, position_now.x, position_now.y);
		if (dis_temp < run.up_dis) //���߾���С�ڼ��پ��룬�������㷨���м��� TODO: �����о�һ����ô�������Ħ���ṩ�ļ��ٶ�
		{
			velocity_set.V = run.speed_start - (run.speed_start - run.speed_want) * sqrt(dis_temp / run.up_dis); //����ٶ�  x=k * t * t   a = k��
		}
		else //������پ��룬���ټ����ˣ����Ǵ�ʱ�ٶȿ��ܲ�û�е���,ֱ�Ӹ��ٶ�
		{
			velocity_set.V = run.speed_want;
			run.speed_up = False;
			run.speed_up_over = True;
		}
	}
	else //�ٶ��Ѿ��ﵽ��Ŀ���ٶ� �����ܴ�ʱ��û������پ���
	{
		velocity_set.V = run.speed_want;
		run.speed_up = False;
		run.speed_up_over = True;
	}
}

/*** 
 * @brief  ֱ���ܵ�����ӳ���
 * @param  void
 * @return void
 * @author ���˵�
 */
void RunPointSpeedDown(void) //�ú��о��˶�ѧ�㷨
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

			if ((ABS(velocity_now.V) <= 50) && (run.speed_down_mode_change == True)) // FIXME: ׼�����������
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
 * @brief  �ܵ���ת�ӳ���
 * @param  void
 * @return void
 * @author ���˵�
 */
void RunPointRotate(void) //��ֱ�����߱���ת����ת�ӳ���
{
	float temp = CountDistance(position_start.x, position_start.y, position_now.x, position_now.y);
	static u8 inner_init_flag = True;

	if (True == inner_init_flag && temp > run.rotate_start)
	{
		//������ת�㣬��¼����
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

	if (run.rotate_over == False) //�����µ��㷨������ת������λ�Ƴ����Թ�ϵ
	{
		temp = CountDistance(position_now.x, position_now.y, start_turn_point.x, start_turn_point.y);

		lock_point.angle = position_start.angle + (position_goal.angle - position_start.angle) * temp / run.rotate_need;
	}
}

/*** 
 * @brief  ��ֵPID����֮ǰ��ԭʼ�ٶ�xy����
 * @param  void
 * @return void
 * @author ���˵�
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
 * @brief  Bezier�����ӳ���
 * @param  void
 * @return void
 * @author ���˵�
 */
void RunBezierSpeedUp(void)
{
	if (ABS(velocity_set.V - velocity_set.V) > 10) //��ǰ�ٶ�û�дﵽĿ���ٶ�
	{
		if (bezier_point1.t < run.up_t) //���߾���С�ڼ��پ��룬�������㷨���м���
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
 * @author: ���˵�
 * @brief: �����������ӳ��� 
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
 * @brief  ��������ת�ӳ���
 * @param  void
 * @return void
 * @author ���˵�
 */
void RunBezierRotate(void)
{
	static float t_start_rotate_temp = 0.0;
	static u8 inner_init_flag = True;

	if (True == inner_init_flag && (bezier_point1.t > run.rotate_start))
	{
		//������ת�㣬��¼tֵ
		t_start_rotate_temp = bezier_point1.t;
		inner_init_flag = False;
	}
	else if (bezier_point1.t > run.rotate_start + run.rotate_need) //��ǰt����ת������ʱ��t
	{
		inner_init_flag = True;

		run.rotate = False;
		run.rotate_over = True;

		lock_point.angle = position_goal.angle; //lock the goal angle
		return;
	}
	if (run.rotate_over == False) //�������㷨������ת������t�����Թ�ϵ
	{
		lock_point.angle = position_start.angle + (bezier_point1.t - t_start_rotate_temp) * (position_goal.angle - position_start.angle) / run.rotate_need;
	}
}
/*** 
 * @brief  ��Ƕ������ߣ�ע��atan�ķ���ֵ�ķ�Χ[-pi/2, pi/2]��Ȼ��ֱ������ϵ��0����Ҫת����ƫ��������ϵ�£�����˳����y+����Ϊ0�ȡ�
 * @param  void
 * @return void
 * @author ���˵�
 */
void RunBezierFollowAngle(void)
{
	float temp_x = 0.0f, temp_y = 0.0f;

	temp_x = (bezier_point2.x - bezier_point1.x);
	temp_y = (bezier_point2.y - bezier_point1.y);

	if (temp_x > -0.5f && temp_x < 0.5f) // ��ֵ
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
//  * @brief   ��˭Ϲ����д��anti������ŶŶ�����Ƿ������ƶ� TODO:
//  */
//void Antilockbrake()
//{
//    static int i=0;
//    static int j=0;
//    float temp=CountDistance(position_now.x, position_now.y, position_goal.x, position_goal.y) ;
//    if(run.speed_down_over == False &&(temp<= run.down_dis))
//    {
//        //�洢�տ�ʼ����ʱ����������ٶ�ֵ

//        v_start_brake = velocity_set.V;
//        run.speed_down_over = 1;
//        run.speed_up = False; 			//�رռ���
//    }
//    if(run.speed_down_over == 1) //�������㷨����ɲ��
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
/*******************************************************  ��������ת��  **************************************************************/
/**************************************************************************************************************************************/

/*** 
 * @brief �ж϶�����
 * @author ���˵�
 * @param  
 * @return  
 * @Date 2020-09-04 21:46:45
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

/*** 
 * @brief ������Ҫ�ﵽ�ľ����ٶ�����������ٶ�
 * @author ���˵�
 * @param  :��������ϵ, angle
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
 * @brief  ��������ٶ������������ٶ�
 * @author ���˵�
 * @param :��Ҫ���õ��ٶ� speed_wantset
 * @return void
 */

void calculate_wheel_speed(Velocity speed_wantset)
{

	/* ����ȫ������û�в��ԣ���ʱ�����⣬���������˼ */
#ifdef OMNI_3 //ȫ��ȱ�����
	float WheelSpeed[3];
	WheelSpeed[0] = (-speed_wantset.Vx - speed_wantset.Vw * Radius * PI / 180.f);
	WheelSpeed[1] = (cos_60 * speed_wantset.Vx - cos_30 * speed_wantset.Vy - speed_wantset.Vw * Radius * PI / 180.f);
	WheelSpeed[2] = (cos_60 * speed_wantset.Vx + cos_30 * speed_wantset.Vy - speed_wantset.Vw * Radius * PI / 180.f);
#endif

#ifdef OMNI_4a //ȫ������������
	float WheelSpeed[4];
	WheelSpeed[0] = (-cos_45 * speed_wantset.Vx + cos_45 * speed_wantset.Vy + speed_wantset.Vw * Radius * PI / 180.f);
	WheelSpeed[1] = (cos_45 * speed_wantset.Vx + cos_45 * speed_wantset.Vy + speed_wantset.Vw * Radius * PI / 180.f);
	WheelSpeed[2] = (-cos_45 * speed_wantset.Vx + cos_45 * speed_wantset.Vy + speed_wantset.Vw * Radius * PI / 180.f);
	WheelSpeed[3] = (cos_45 * speed_wantset.Vx + cos_45 * speed_wantset.Vy + speed_wantset.Vw * Radius * PI / 180.f);
#endif
#ifdef OMNI_4 //ȫ������������

#endif

#ifdef OMNI_4b //ȫ�򳤷�������
	float WheelSpeed[4];
	float m = 30, n = 20;
	WheelSpeed[0] = (-cos_45 * speed_wantset.Vx + cos_45 * speed_wantset.Vy + (m + n) * speed_wantset.Vw * Radius * PI / 180.f);
	WheelSpeed[1] = (-cos_45 * speed_wantset.Vx - cos_45 * speed_wantset.Vy + (-m + n) * speed_wantset.Vw * Radius * PI / 180.f);
	WheelSpeed[2] = (cos_45 * speed_wantset.Vx - cos_45 * speed_wantset.Vy + (-m + -n) * speed_wantset.Vw * Radius * PI / 180.f);
	WheelSpeed[3] = (cos_45 * speed_wantset.Vx + cos_45 * speed_wantset.Vy + (m + -n) * speed_wantset.Vw * Radius * PI / 180.f);
#endif

#ifdef MECANUM //�����ķ��
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
	Wheel[0].Vx = speed_wantset.Vx - speed_wantset.Vw * chassis.width_half * 4 / 3 * PI / 180.f; // ��λ�����⣬����һ����ת��
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
			Wheel[i].Angle = Wheel[i].Angle; //�����ϴ�λ�ò���
			Wheel[i].V = 0;
		}
		else
		{
			Wheel[i].Angle = PI / 2 - acos(Wheel[i].Vx / Wheel[i].V); //?a��2?�䨰a��?��???��?2??��?��??angle??��?��?��?

			if ((Wheel[i].Vy < 0) && (Wheel[i].Vx <= 0)) //�̨���y?��?T?��??y?o?��
			{
				Wheel[i].Angle = -PI - Wheel[i].Angle;
			}
			else
			{
				if ((Wheel[i].Vy) < 0 && (Wheel[i].Vx > 0)) //�̨�???��?T
					Wheel[i].Angle = PI - Wheel[i].Angle;
			}

			//			if(Wheel[i].Vy < 0){
			//				Wheel[i].Angle = (Wheel[i].Vx <= 0 ? -PI : PI) - Wheel[i].Angle;
			//			}
		}

		if ((Wheel[i].Angle - wheel_angle_set[i]) < (-PI / 2.0f)) //��?1?��a��????����?�䨮
		{
			Wheel[i].Angle += PI;
			Wheel[i].V = -Wheel[i].V;
		}
		else if ((Wheel[i].Angle - wheel_angle_set[i]) > (PI / 2.0f)) //��?1?��a��????����?�䨮
		{
			Wheel[i].Angle -= PI;
			Wheel[i].V = -Wheel[i].V;
		}

		wheel_angle_set[i] = Wheel[i].Angle;
	}

#endif

#ifdef STEER //����
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

	QuadrantIs(); //�ж�����

	for (i = 0; i < 4; i++)
	{
		if (Wheel[i].V > -1.0f && Wheel[i].V < 1.0f) // �����ֵ���ٶ�=0ʱ��ֻ�м���ͣ��ʱ����������������о���һ���ж�duck���أ���ɾ��
		{
			Wheel[i].Angle = Wheel[i].Angle; //�����ϴ�λ�ò���
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

		// ������
		/* if (0 == queue_flag.LockPointOn) //δ������ �����ת180��
		{
			if (Wheel[i].V > -1.0f && Wheel[i].V < 1.0f)
			{
				Wheel[i].Angle = WheelAngleSet[i]; //�����ϴε�λ�ò���
				Wheel[i].V = 0;
			}
			else
			{
				Wheel[i].Angle = PI / 2 - acos(Wheel[i].Vx / Wheel[i].V);

				if ((Wheel[i].Vy < 0) && (Wheel[i].Vx <= 0))
				{
					//�������޻���y����
					Wheel[i].Angle = -PI - Wheel[i].Angle;
				}
				else
				{
					if ((Wheel[i].Vy) < 0 && (Wheel[i].Vx > 0)) //��������
						Wheel[i].Angle = PI - Wheel[i].Angle;
				}
			}
			if ((Wheel[i].Angle - WheelAngleSet[i]) < (-PI / 2.0f)) //���ת�ĽǶ�̫��
			{
				Wheel[i].Angle += PI;
				Wheel[i].V = -Wheel[i].V;
			}
			else if ((Wheel[i].Angle - WheelAngleSet[i]) > (PI / 2.0f)) //���ת�ĽǶ�̫��
			{
				Wheel[i].Angle -= PI;
				Wheel[i].V = -Wheel[i].V;
			}

			WheelAngleSet[i] = Wheel[i].Angle; //��¼�´˴εĽǶ�
		}
		else //���� ֻ��һ������ת
		{
			if (Wheel[i].V > -1.0f && Wheel[i].V < 1.0f)
			{
				Wheel[i].Angle = WheelAngleSet[i]; //�����ϴε�λ�ò���
				Wheel[i].V = 0;
			}
			else
			{
				Wheel[i].Angle = PI / 2 - acos(Wheel[i].Vx / Wheel[i].V);
				if (3 == Quadrant[i]) //�������� ��y-
				{
					Wheel[i].Angle = -Wheel[i].Angle; //ת����һ���޶�Ӧ�Ƕ� ���ӷ���
					Wheel[i].V = -Wheel[i].V;
				}
				else if (4 == Quadrant[i])
				{
					Wheel[i].Angle = -Wheel[i].Angle; //ת���ڶ����޶�Ӧ�Ƕ� ���ӷ���
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
 * @author: ���˵�
 * @brief: ���������ٶ�
 * @param  void
 * @return void
 */
static void set_wheel()
{
	if (enable == motor.state)
	{
#ifdef STEER
		turn_motor_4PAx30(30 * wheel_angle_set[0] * 180 / PI, 30 * wheel_angle_set[1] * 180 / PI, //��λ�ǽǶ�
						  30 * wheel_angle_set[2] * 180 / PI, 30 * wheel_angle_set[3] * 180 / PI, 0);

		Elmo_Setspeed(1, Wheel[0].V, 0); //����ת�ٰ���
		Elmo_Setspeed(2, Wheel[1].V, 0);
		Elmo_Setspeed(3, Wheel[2].V, 0);
		Elmo_Setspeed(4, Wheel[3].V, 0);

#endif

#ifdef THREE_STEER
		turn_motor_4PAx30(30 * wheel_angle_set[0] * 180 / PI, 30 * wheel_angle_set[1] * 180 / PI, //��λ�ǽǶ�
						  30 * wheel_angle_set[2] * 180 / PI, 30 * wheel_angle_set[3] * 180 / PI, 0);

		Elmo_Setspeed(1, Wheel[0].V, 0); //����ת�ٰ���
		Elmo_Setspeed(2, Wheel[1].V, 0);
		Elmo_Setspeed(3, Wheel[2].V, 0);

#endif
		Elmo_Begin(0, 0);
	}
}

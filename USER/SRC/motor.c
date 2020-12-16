/*** 
 * @Date          2020-07-25 10:35:59
 * @LastEditors   ���˵�
 * @LastEditTime  2020-09-25 16:55:44
 * @FilePath      \Project\User\src\motor.c
 */
#include "motor.h"

void EncodeFloatData(float *f, u8 *buff)
{
	u16 f_c[2];
	*(float *)f_c = *f;
	buff[0] = (f_c[0] >> 0 & 0x00ff);
	buff[1] = ((f_c[0] >> 8) & 0x00ff);
	buff[2] = (f_c[1] >> 0 & 0x00ff);
	buff[3] = ((f_c[1] >> 8) & 0x00ff);
}

/*** 
 * @brief  enalbe the CAN BUS of motor drivers
 * @param  InConGrpFlag put the CAN message in the control group
 * @return void
 * @author δ����
 * @CreatedTime 2020-09-23 21:13:10
 */
void Elmo_Ele(u8 InConGrpFlag)
{
	if (Rear2 == can2_sendqueue.Front)
	{
		queue_flag.can2_queue_full++;
		return;
	}
	else
	{
		can2_sendqueue.node[can2_sendqueue.Rear].Id = 0X00;
		can2_sendqueue.node[can2_sendqueue.Rear].DLC = 0X08;
		can2_sendqueue.node[can2_sendqueue.Rear].Data[0] = 1;
		can2_sendqueue.node[can2_sendqueue.Rear].Data[1] = 0;
		can2_sendqueue.node[can2_sendqueue.Rear].Data[2] = 0;
		can2_sendqueue.node[can2_sendqueue.Rear].Data[3] = 0;
		can2_sendqueue.node[can2_sendqueue.Rear].Data[4] = 0;
		can2_sendqueue.node[can2_sendqueue.Rear].Data[5] = 0;
		can2_sendqueue.node[can2_sendqueue.Rear].Data[6] = 0;
		can2_sendqueue.node[can2_sendqueue.Rear].Data[7] = 0;
		can2_sendqueue.node[can2_sendqueue.Rear].InConGrpFlag = InConGrpFlag;
	}
	can2_sendqueue.Rear = Rear2;
}

/**
  * @brief ʹ��Elmo���
  */
void Elmo_SetParameter(u8 ID, u8 fst, u8 snd, u8 index, u8 value, u8 InConGrpFlag)
{
	s32 a = value;
	if (Rear2 == can2_sendqueue.Front)
	{
		queue_flag.can2_queue_full++;
		return;
	}
	else
	{
		can2_sendqueue.node[can2_sendqueue.Rear].Id = 0x300 + ID;
		can2_sendqueue.node[can2_sendqueue.Rear].DLC = 0X08;
		can2_sendqueue.node[can2_sendqueue.Rear].Data[0] = fst;
		can2_sendqueue.node[can2_sendqueue.Rear].Data[1] = snd;
		can2_sendqueue.node[can2_sendqueue.Rear].Data[2] = index;
		can2_sendqueue.node[can2_sendqueue.Rear].Data[3] = 0;
		EncodeS32To4ByteNone(&a, &can2_sendqueue.node[can2_sendqueue.Rear].Data[4]);
		can2_sendqueue.node[can2_sendqueue.Rear].InConGrpFlag = InConGrpFlag;
	}
	can2_sendqueue.Rear = Rear2;
}

/*** 
 * @brief ��ȡElmo����ٶ�
 * @author ���˵�
 */
void Elmo_GetVX(u8 InConGrpFlag)
{
	if (Rear2 == can2_sendqueue.Front)
	{
		queue_flag.can2_queue_full++;
		return;
	}
	else
	{
		can2_sendqueue.node[can2_sendqueue.Rear].Id = 0X300;
		can2_sendqueue.node[can2_sendqueue.Rear].DLC = 0X04;
		can2_sendqueue.node[can2_sendqueue.Rear].Data[0] = 'V';
		can2_sendqueue.node[can2_sendqueue.Rear].Data[1] = 'X';
		can2_sendqueue.node[can2_sendqueue.Rear].Data[2] = 0;
		can2_sendqueue.node[can2_sendqueue.Rear].Data[3] = 0;
		can2_sendqueue.node[can2_sendqueue.Rear].InConGrpFlag = InConGrpFlag;
	}
	can2_sendqueue.Rear = Rear2;
}

/**
  * @brief  λ��ģʽ������λ�ã�����4�����̽Ƕ�,�Ŵ�30�� TODO: ȷ������Ƕ�Ҫ��Ҫ�����ٱ�
  */
void turn_motor_4PAx30(float FrontAngle1, float FrontAngle2, float FrontAngle3, float FrontAngle4, u8 InConGrpFlag)
{
	s16 A1, A2, A3, A4;
	if (Rear2 == can2_sendqueue.Front)
	{
		queue_flag.can2_queue_full++;
		return;
	}
	else
	{
		A1 = FrontAngle1;
		A2 = FrontAngle2;
		A3 = FrontAngle3;
		A4 = FrontAngle4;

		can2_sendqueue.node[can2_sendqueue.Rear].Id = 0x320;
		can2_sendqueue.node[can2_sendqueue.Rear].DLC = 0X08;
		EncodeS16To2ByteNone(&A1, &can2_sendqueue.node[can2_sendqueue.Rear].Data[0]);
		EncodeS16To2ByteNone(&A2, &can2_sendqueue.node[can2_sendqueue.Rear].Data[2]);
		EncodeS16To2ByteNone(&A3, &can2_sendqueue.node[can2_sendqueue.Rear].Data[4]);
		EncodeS16To2ByteNone(&A4, &can2_sendqueue.node[can2_sendqueue.Rear].Data[6]);

		can2_sendqueue.node[can2_sendqueue.Rear].InConGrpFlag = InConGrpFlag;
	}
	can2_sendqueue.Rear = Rear2;
}

/**
  * @brief  λ��ģʽ������λ�ã����õ���3508��ǰ�Ƕ�,�Ŵ�30��
  */
void turn_motor_singlePAx30(u8 ID, u8 WheelNum, float FrontAngle, u8 InConGrpFlag)
{
	s16 A;
	if (Rear2 == can2_sendqueue.Front)
	{
		queue_flag.can2_queue_full++;
		return;
	}
	else
	{
		A = FrontAngle * 30 * motor.reduction;
		can2_sendqueue.node[can2_sendqueue.Rear].Id = 0x300 + ID;
		can2_sendqueue.node[can2_sendqueue.Rear].DLC = 0X05;
		can2_sendqueue.node[can2_sendqueue.Rear].Data[0] = 'C';
		can2_sendqueue.node[can2_sendqueue.Rear].Data[1] = 'H';
		can2_sendqueue.node[can2_sendqueue.Rear].Data[2] = WheelNum;
		EncodeS16To2ByteNone(&A, &can2_sendqueue.node[can2_sendqueue.Rear].Data[3]);
		can2_sendqueue.node[can2_sendqueue.Rear].InConGrpFlag = InConGrpFlag;
	}
	can2_sendqueue.Rear = Rear2;
}

/**
  * @brief  ����3508����pid ��
  */
void C3508_PidHard(u8 ID, u8 InConGrpFlag)
{
	if (Rear2 == can2_sendqueue.Front)
	{
		queue_flag.can2_queue_full++;
		return;
	}
	else
	{
		can2_sendqueue.node[can2_sendqueue.Rear].Id = 0x300 + ID;
		can2_sendqueue.node[can2_sendqueue.Rear].DLC = 0X4;
		can2_sendqueue.node[can2_sendqueue.Rear].Data[0] = 'P';
		can2_sendqueue.node[can2_sendqueue.Rear].Data[1] = 'I';
		can2_sendqueue.node[can2_sendqueue.Rear].Data[2] = 'D';
		can2_sendqueue.node[can2_sendqueue.Rear].Data[3] = 'G';
		can2_sendqueue.node[can2_sendqueue.Rear].InConGrpFlag = InConGrpFlag;
	}
	can2_sendqueue.Rear = Rear2;
}

/**
  * @brief  3508��ͣ
  */
void turn_motor_ST(u8 ID, u8 InConGrpFlag)
{
	if (Rear2 == can2_sendqueue.Front)
	{
		queue_flag.can2_queue_full++;
		return;
	}
	else
	{
		can2_sendqueue.node[can2_sendqueue.Rear].Id = 0x300 + ID;
		can2_sendqueue.node[can2_sendqueue.Rear].DLC = 0X02;
		can2_sendqueue.node[can2_sendqueue.Rear].Data[0] = 'S';
		can2_sendqueue.node[can2_sendqueue.Rear].Data[1] = 'T';
		can2_sendqueue.node[can2_sendqueue.Rear].InConGrpFlag = InConGrpFlag;
	}
	can2_sendqueue.Rear = Rear2;
}

/**
  * @brief  3508Ѱ��
  */
void turn_motor_SZ(u8 ID, short int Speed, u8 InConGrpFlag)
{
	if (Rear2 == can2_sendqueue.Front)
	{
		queue_flag.can2_queue_full++;
		return;
	}
	else
	{
		can2_sendqueue.node[can2_sendqueue.Rear].Id = 0x300 + ID;
		can2_sendqueue.node[can2_sendqueue.Rear].DLC = 0X04;
		can2_sendqueue.node[can2_sendqueue.Rear].Data[0] = 'S';
		can2_sendqueue.node[can2_sendqueue.Rear].Data[1] = 'Z';
		EncodeS16To2ByteNone(&Speed, &can2_sendqueue.node[can2_sendqueue.Rear].Data[2]);
		can2_sendqueue.node[can2_sendqueue.Rear].InConGrpFlag = InConGrpFlag;
	}
	can2_sendqueue.Rear = Rear2;
}

/**
  * @brief  ������λ��ģʽ�ٶ�
  */
void turn_motor_sp(u8 ID, u16 Speed, u8 InConGrpFlag)
{
	if (Rear2 == can2_sendqueue.Front)
	{
		queue_flag.can2_queue_full++;
		return;
	}
	else
	{
		can2_sendqueue.node[can2_sendqueue.Rear].Id = 0x300 + ID;
		can2_sendqueue.node[can2_sendqueue.Rear].DLC = 0X04;
		can2_sendqueue.node[can2_sendqueue.Rear].Data[0] = 'S';
		can2_sendqueue.node[can2_sendqueue.Rear].Data[1] = 'P';
		can2_sendqueue.node[can2_sendqueue.Rear].Data[2] = (u8)(Speed & 0xff);
		can2_sendqueue.node[can2_sendqueue.Rear].Data[3] = (u8)(Speed >> 8);

		//		EncodeS16To2ByteNone(&Speed,&can2_sendqueue.node[can2_sendqueue.Rear].Data[2]);
		can2_sendqueue.node[can2_sendqueue.Rear].InConGrpFlag = InConGrpFlag;
	}
	can2_sendqueue.Rear = Rear2;
}

/**
  * @brief  3508ʹ����ʧ��
  */
void turn_motor_mo(u8 ID, u8 Status, u8 InConGrpFlag)
{
	if (Rear2 == can2_sendqueue.Front)
	{
		queue_flag.can2_queue_full++;
		return;
	}
	else
	{
		can2_sendqueue.node[can2_sendqueue.Rear].Id = 0x300 + ID;
		can2_sendqueue.node[can2_sendqueue.Rear].DLC = 0X03;
		can2_sendqueue.node[can2_sendqueue.Rear].Data[0] = 'M';
		can2_sendqueue.node[can2_sendqueue.Rear].Data[1] = 'O';
		can2_sendqueue.node[can2_sendqueue.Rear].Data[2] = Status;
		can2_sendqueue.node[can2_sendqueue.Rear].InConGrpFlag = InConGrpFlag;
	}
	can2_sendqueue.Rear = Rear2;
}
/**
  * @brief  3508�Լ�
  */
void C3508_Check(u8 ID, u8 InConGrpFlag)
{
	if (Rear2 == can2_sendqueue.Front)
	{
		queue_flag.can2_queue_full++;
		return;
	}
	else
	{
		can2_sendqueue.node[can2_sendqueue.Rear].Id = 0x300 + ID;
		can2_sendqueue.node[can2_sendqueue.Rear].DLC = 0X04;
		can2_sendqueue.node[can2_sendqueue.Rear].Data[0] = 'C';
		can2_sendqueue.node[can2_sendqueue.Rear].Data[1] = 'W';
		can2_sendqueue.node[can2_sendqueue.Rear].Data[2] = 'H';
		can2_sendqueue.node[can2_sendqueue.Rear].Data[3] = 'U';
		can2_sendqueue.node[can2_sendqueue.Rear].InConGrpFlag = InConGrpFlag;
	}
	can2_sendqueue.Rear = Rear2;
}
/**
  * @brief  ��ѯ����3508��һЩ״̬��־λ
  */
void turn_motor_ask(u8 ID, u8 InConGrpFlag)
{
	if (Rear2 == can2_sendqueue.Front)
	{
		queue_flag.can2_queue_full++;
		return;
	}
	else
	{
		can2_sendqueue.node[can2_sendqueue.Rear].Id = 0x300 + ID;
		can2_sendqueue.node[can2_sendqueue.Rear].DLC = 0X03;
		can2_sendqueue.node[can2_sendqueue.Rear].Data[0] = 'A';
		can2_sendqueue.node[can2_sendqueue.Rear].Data[1] = 'S';
		can2_sendqueue.node[can2_sendqueue.Rear].Data[2] = 'K';
		can2_sendqueue.node[can2_sendqueue.Rear].InConGrpFlag = InConGrpFlag;
	}
	can2_sendqueue.Rear = Rear2;
}
void arm_motor_init(void)
{
	turn_motor_sp(5, 9000, 1);
	OSTimeDly(100);
	turn_motor_sp(6, 9000, 1);
	OSTimeDly(100);
	C3508_PidHard(5, 1);
	OSTimeDly(100);
	C3508_PidHard(6, 1);
	OSTimeDly(100);
}
/**
  * @brief  ����Elmo�����˶��ٶ�
 **/

void Elmo_Setspeed(u8 ID, s32 speed, u8 InConGrpFlag)
{

	speed = speed * chassis.K_V_motor * motor.encoder_resolution / 15.0f;

	if (Rear2 == can2_sendqueue.Front)
	{
		queue_flag.can2_queue_full++;
		return;
	}
	else
	{
		can2_sendqueue.node[can2_sendqueue.Rear].Id = 0x300 + ID;
		can2_sendqueue.node[can2_sendqueue.Rear].DLC = 0X08;
		can2_sendqueue.node[can2_sendqueue.Rear].Data[0] = 'J'; //4A
		can2_sendqueue.node[can2_sendqueue.Rear].Data[1] = 'V'; //56
		can2_sendqueue.node[can2_sendqueue.Rear].Data[2] = 0;
		can2_sendqueue.node[can2_sendqueue.Rear].Data[3] = 0;
		EncodeS32To4ByteNone(&speed, &can2_sendqueue.node[can2_sendqueue.Rear].Data[4]);
		can2_sendqueue.node[can2_sendqueue.Rear].InConGrpFlag = InConGrpFlag;
	}
	can2_sendqueue.Rear = Rear2;
}
/**
  * @brief  ����Elmo���ٶ�
  */
void Elmo_SetAC(u8 ID, s32 ac)
{

	if (Rear2 == can2_sendqueue.Front)
	{
		queue_flag.can2_queue_full++;
		return;
	}
	else
	{
		can2_sendqueue.node[can2_sendqueue.Rear].Id = 0x300 + ID;
		can2_sendqueue.node[can2_sendqueue.Rear].DLC = 0X08;
		can2_sendqueue.node[can2_sendqueue.Rear].Data[0] = 'A';
		can2_sendqueue.node[can2_sendqueue.Rear].Data[1] = 'C';
		can2_sendqueue.node[can2_sendqueue.Rear].Data[2] = 0;
		can2_sendqueue.node[can2_sendqueue.Rear].Data[3] = 0;
		EncodeS32Data(&ac, &can2_sendqueue.node[can2_sendqueue.Rear].Data[4]);
	}
	can2_sendqueue.Rear = Rear2;
}
/**
  * @brief  ��ѯElmo��λ��
  */
void AskElmo_MasterPosition(u8 ID)
{

	if (Rear2 == can2_sendqueue.Front)
	{
		queue_flag.can2_queue_full++;
		return;
	}
	else
	{
		can2_sendqueue.node[can2_sendqueue.Rear].Id = 0x300 + ID;
		can2_sendqueue.node[can2_sendqueue.Rear].DLC = 0X04;
		can2_sendqueue.node[can2_sendqueue.Rear].Data[0] = 'P';
		can2_sendqueue.node[can2_sendqueue.Rear].Data[1] = 'X';
		can2_sendqueue.node[can2_sendqueue.Rear].Data[2] = 0;
		can2_sendqueue.node[can2_sendqueue.Rear].Data[3] = 0;
		//				EncodeS32Data(&pulse, &can2_sendqueue.node[can2_sendqueue.Rear].Data[4]);
	}
	can2_sendqueue.Rear = Rear2;
}
/**
  * @brief  ����Elmo��λ��
  */
void Elmo_MasterPosition(u8 ID, s32 pulse)
{

	if (Rear2 == can2_sendqueue.Front)
	{
		queue_flag.can2_queue_full++;
		return;
	}
	else
	{
		can2_sendqueue.node[can2_sendqueue.Rear].Id = 0x300 + ID;
		can2_sendqueue.node[can2_sendqueue.Rear].DLC = 0X08;
		can2_sendqueue.node[can2_sendqueue.Rear].Data[0] = 'P';
		can2_sendqueue.node[can2_sendqueue.Rear].Data[1] = 'X';
		can2_sendqueue.node[can2_sendqueue.Rear].Data[2] = 0;
		can2_sendqueue.node[can2_sendqueue.Rear].Data[3] = 0;
		EncodeS32Data(&pulse, &can2_sendqueue.node[can2_sendqueue.Rear].Data[4]);
	}
	can2_sendqueue.Rear = Rear2;
}
/**
  * @brief  Elmo����λ��
  */
void Elmo_AbsolutePosition(u8 ID, s32 pulse)
{
	pulse += ElmoPulse;
	if (Rear2 == can2_sendqueue.Front)
	{
		queue_flag.can2_queue_full++;
		return;
	}
	else
	{
		can2_sendqueue.node[can2_sendqueue.Rear].Id = 0x300 + ID;
		can2_sendqueue.node[can2_sendqueue.Rear].DLC = 0X08;
		can2_sendqueue.node[can2_sendqueue.Rear].Data[0] = 'P';
		can2_sendqueue.node[can2_sendqueue.Rear].Data[1] = 'A';
		can2_sendqueue.node[can2_sendqueue.Rear].Data[2] = 0;
		can2_sendqueue.node[can2_sendqueue.Rear].Data[3] = 0;
		EncodeS32Data(&pulse, &can2_sendqueue.node[can2_sendqueue.Rear].Data[4]);
	}
	can2_sendqueue.Rear = Rear2;
}
/**
  * @brief ʹ��Elmo���
  */
void Elmo_StartMotor(u8 ID, u8 InConGrpFlag)
{
	s32 a = 1;
	if (Rear2 == can2_sendqueue.Front)
	{
		queue_flag.can2_queue_full++;
		return;
	}
	else
	{
		can2_sendqueue.node[can2_sendqueue.Rear].Id = 0x300 + ID;
		can2_sendqueue.node[can2_sendqueue.Rear].DLC = 0X08;
		can2_sendqueue.node[can2_sendqueue.Rear].Data[0] = 'M';
		can2_sendqueue.node[can2_sendqueue.Rear].Data[1] = 'O';
		can2_sendqueue.node[can2_sendqueue.Rear].Data[2] = 0;
		can2_sendqueue.node[can2_sendqueue.Rear].Data[3] = 0;
		EncodeS32Data(&a, &can2_sendqueue.node[can2_sendqueue.Rear].Data[4]);
		can2_sendqueue.node[can2_sendqueue.Rear].InConGrpFlag = InConGrpFlag;
	}
	can2_sendqueue.Rear = Rear2;
}
/**
  * @brief ELMOѡ��ģʽ
  */
void Elmo_SelectMode(u8 ID, s32 Mode)
{
	if (Rear2 == can2_sendqueue.Front)
	{
		queue_flag.can2_queue_full++;
		return;
	}
	else
	{
		can2_sendqueue.node[can2_sendqueue.Rear].Id = 0x300 + ID;
		can2_sendqueue.node[can2_sendqueue.Rear].DLC = 0X08;
		can2_sendqueue.node[can2_sendqueue.Rear].Data[0] = 'U';
		can2_sendqueue.node[can2_sendqueue.Rear].Data[1] = 'M';
		can2_sendqueue.node[can2_sendqueue.Rear].Data[2] = 0;
		can2_sendqueue.node[can2_sendqueue.Rear].Data[3] = 0;
		EncodeS32Data(&Mode, &can2_sendqueue.node[can2_sendqueue.Rear].Data[4]);
	}
	can2_sendqueue.Rear = Rear2;
}
/**
  * @brief ELMO��Ե��ٶ�
  */
void Elmo_PTP_Speed(u8 ID, s32 speed)
{
	speed = speed * 200 / 3;
	if (Rear2 == can2_sendqueue.Front)
	{
		queue_flag.can2_queue_full++;
		return;
	}
	else
	{
		can2_sendqueue.node[can2_sendqueue.Rear].Id = 0x300 + ID;
		can2_sendqueue.node[can2_sendqueue.Rear].DLC = 0X08;
		can2_sendqueue.node[can2_sendqueue.Rear].Data[0] = 'S';
		can2_sendqueue.node[can2_sendqueue.Rear].Data[1] = 'P';
		can2_sendqueue.node[can2_sendqueue.Rear].Data[2] = 0;
		can2_sendqueue.node[can2_sendqueue.Rear].Data[3] = 0;
		EncodeS32Data(&speed, &can2_sendqueue.node[can2_sendqueue.Rear].Data[4]);
	}
	can2_sendqueue.Rear = Rear2;
}
/**
  * @brief  �ͷ�Elmo���
  */
void Elmo_RelieveMotor(u8 ID, u8 InConGrpFlag)
{
	s32 a = 0;
	if (Rear2 == can2_sendqueue.Front)
	{
		queue_flag.can2_queue_full++;
		return;
	}
	else
	{
		can2_sendqueue.node[can2_sendqueue.Rear].Id = 0x300 + ID;
		can2_sendqueue.node[can2_sendqueue.Rear].DLC = 0X08;
		can2_sendqueue.node[can2_sendqueue.Rear].Data[0] = 'M';
		can2_sendqueue.node[can2_sendqueue.Rear].Data[1] = 'O';
		can2_sendqueue.node[can2_sendqueue.Rear].Data[2] = 0;
		can2_sendqueue.node[can2_sendqueue.Rear].Data[3] = 0;
		EncodeS32Data(&a, &can2_sendqueue.node[can2_sendqueue.Rear].Data[4]);
	}
	can2_sendqueue.Rear = Rear2;
}
/**
  * @brief  �ƶ�Elmo���
  */
void Elmo_StopMotor(u8 ID, u8 InConGrpFlag)
{
	if (Rear2 == can2_sendqueue.Front)
	{
		queue_flag.can2_queue_full++;
		return;
	}
	else
	{
		can2_sendqueue.node[can2_sendqueue.Rear].Id = 0x300 + ID;
		can2_sendqueue.node[can2_sendqueue.Rear].DLC = 0X04;
		can2_sendqueue.node[can2_sendqueue.Rear].Data[0] = 'S';
		can2_sendqueue.node[can2_sendqueue.Rear].Data[1] = 'T';
		can2_sendqueue.node[can2_sendqueue.Rear].Data[2] = 0;
		can2_sendqueue.node[can2_sendqueue.Rear].Data[3] = 0;
		can2_sendqueue.node[can2_sendqueue.Rear].InConGrpFlag = InConGrpFlag;
	}
	can2_sendqueue.Rear = Rear2;
}
/**
  * @brief  ִ��Elmoָ��
  */
void Elmo_Begin(u8 ID, u8 InConGrpFlag)
{
	if (Rear2 == can2_sendqueue.Front)
	{
		queue_flag.can2_queue_full++;
		return;
	}
	else
	{
		can2_sendqueue.node[can2_sendqueue.Rear].Id = 0x300 + ID;
		can2_sendqueue.node[can2_sendqueue.Rear].DLC = 0X04;
		can2_sendqueue.node[can2_sendqueue.Rear].Data[0] = 'B';
		can2_sendqueue.node[can2_sendqueue.Rear].Data[1] = 'G';
		can2_sendqueue.node[can2_sendqueue.Rear].Data[2] = 0;
		can2_sendqueue.node[can2_sendqueue.Rear].Data[3] = 0;
		can2_sendqueue.node[can2_sendqueue.Rear].InConGrpFlag = InConGrpFlag;
	}
	can2_sendqueue.Rear = Rear2;
}
/**
  * @brief  ��ȡElmo�������
  */
void Elmo_GetError(void)
{
	if (Rear2 == can2_sendqueue.Front)
	{
		queue_flag.can2_queue_full++;
		return;
	}
	else
	{
		can2_sendqueue.node[can2_sendqueue.Rear].Id = 0X300;
		can2_sendqueue.node[can2_sendqueue.Rear].DLC = 0X08;
		can2_sendqueue.node[can2_sendqueue.Rear].Data[0] = 'E';
		can2_sendqueue.node[can2_sendqueue.Rear].Data[1] = 'C';
		can2_sendqueue.node[can2_sendqueue.Rear].Data[2] = 0;
		can2_sendqueue.node[can2_sendqueue.Rear].Data[3] = 0X40;
		can2_sendqueue.node[can2_sendqueue.Rear].Data[4] = 0;
		can2_sendqueue.node[can2_sendqueue.Rear].Data[5] = 0;
		can2_sendqueue.node[can2_sendqueue.Rear].Data[6] = 0;
		can2_sendqueue.node[can2_sendqueue.Rear].Data[7] = 0;
	}
	can2_sendqueue.Rear = Rear2;
}

/**
  * @brief  ��ȡElmo����й�����
  */
void Elmo_GetIQA(u8 ID)
{
	if (Rear2 == can2_sendqueue.Front)
	{
		queue_flag.can2_queue_full++;
		return;
	}
	else
	{
		can2_sendqueue.node[can2_sendqueue.Rear].Id = 0x300 + ID;
		can2_sendqueue.node[can2_sendqueue.Rear].DLC = 0X04;
		can2_sendqueue.node[can2_sendqueue.Rear].Data[0] = 'I';
		can2_sendqueue.node[can2_sendqueue.Rear].Data[1] = 'Q';
		can2_sendqueue.node[can2_sendqueue.Rear].Data[2] = 0;
		can2_sendqueue.node[can2_sendqueue.Rear].Data[3] = 0;
	}
	can2_sendqueue.Rear = Rear2;
}
/**
  * @brief  ��ȡElmo����޹�����
  */
void Elmo_GetIDA(u8 ID)
{
	if (Rear2 == can2_sendqueue.Front)
	{
		queue_flag.can2_queue_full++;
		return;
	}
	else
	{
		can2_sendqueue.node[can2_sendqueue.Rear].Id = 0x300 + ID;
		can2_sendqueue.node[can2_sendqueue.Rear].DLC = 0X04;
		can2_sendqueue.node[can2_sendqueue.Rear].Data[0] = 'I';
		can2_sendqueue.node[can2_sendqueue.Rear].Data[1] = 'D';
		can2_sendqueue.node[can2_sendqueue.Rear].Data[2] = 0;
		can2_sendqueue.node[can2_sendqueue.Rear].Data[3] = 0;
	}
	can2_sendqueue.Rear = Rear2;
}
/**
  * @brief  ����elmo������
  */
void Elmo_SetTC(u8 ID, float current)
{
	if (Rear2 == can2_sendqueue.Front)
	{
		queue_flag.can2_queue_full++;
		return;
	}
	else
	{
		can2_sendqueue.node[can2_sendqueue.Rear].Id = 0X300 + ID;
		can2_sendqueue.node[can2_sendqueue.Rear].DLC = 0X08;
		can2_sendqueue.node[can2_sendqueue.Rear].Data[0] = 'T';
		can2_sendqueue.node[can2_sendqueue.Rear].Data[1] = 'C';
		can2_sendqueue.node[can2_sendqueue.Rear].Data[2] = 0;
		can2_sendqueue.node[can2_sendqueue.Rear].Data[3] = 0x80;
		EncodeFloatData(&current, &can2_sendqueue.node[can2_sendqueue.Rear].Data[4]);
	}
	can2_sendqueue.Rear = Rear2;
}

void __ELMOSETPX(char *y, u8 ID)
{
	s32 position = 0;
	if (Rear2 == can2_sendqueue.Front)
	{
		queue_flag.can2_queue_full++;
		return;
	}
	else
	{
		can2_sendqueue.node[can2_sendqueue.Rear].Id = 0x300 + ID;
		if (y[0] == '-')
			can2_sendqueue.node[can2_sendqueue.Rear].DLC = 0X04;
		else
		{
			sscanf(y, "%d", &position);
			can2_sendqueue.node[can2_sendqueue.Rear].DLC = 0X08;
			EncodeS32Data(&position, &can2_sendqueue.node[can2_sendqueue.Rear].Data[4]);
		}

		can2_sendqueue.node[can2_sendqueue.Rear].Data[0] = 'P';
		can2_sendqueue.node[can2_sendqueue.Rear].Data[1] = 'X';
		can2_sendqueue.node[can2_sendqueue.Rear].Data[2] = 0;
		can2_sendqueue.node[can2_sendqueue.Rear].Data[3] = 0;
	}
	can2_sendqueue.Rear = Rear2;
}
/*** 
 * @brief ת����������ʼ��
 * @author δ����
 * @Date 2020-08-29 14:47:48
 */
void turn_motor_init(void)
{
	turn_motor_sp(5, 19500, 1);
	// OSTimeDly(100);
	turn_motor_sp(6, 19500, 1);
	// OSTimeDly(100);
	turn_motor_sp(7, 19500, 1);
	// OSTimeDly(100);
	turn_motor_sp(8, 19500, 1);
	// OSTimeDly(100);
}

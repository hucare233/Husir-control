/*** 
 * @Date: 2020-12-03 01:29:51
 * @LastEditors   叮咚蛋
 * @LastEditTime  2020-09-09 19:10:29
 * @FilePath      \Project\User\src\Master_para.c
 */

#include "param.h"

/* Struct */
UserFlagStruct user;
u8 USART1_DMA_SendBuf[USART_SENDQUEUESIZE];
u8 USART2_DMA_SendBuf[USART_SENDQUEUESIZE];
u8 USART3_DMA_SendBuf[USART_SENDQUEUESIZE];
u8 UART4_DMA_SendBuf[USART_SENDQUEUESIZE];
u8 UART5_DMA_SendBuf[USART_SENDQUEUESIZE];
volatile RunFlagStruct run;
volatile LcdStruct lcd;
volatile TimeStruct run_time;
volatile DebugPosiStruct debug_posi;
volatile GyroscopeFlagStruct gyroscope;
volatile CameraFlagStruct camera;
volatile MechanismFlagStruct mechanism;
volatile MotorFlagStruct motor;
volatile ChassisStruct chassis;

volatile UsartDmaFlagStruct usart_dma = {
	True,
	True,
	True,
	True,
	True,
};

/* ucosii */
OS_CPU_SR cpu_sr = 0u;

OS_EVENT *RUN;
OS_EVENT *RDY;
OS_EVENT *BALL;
OS_EVENT *SHUT;

OS_FLAG_GRP *FlagCan1Check;
OS_FLAG_GRP *FlagCan2Check;

INT8U *ErrorRUN;
INT8U *ErrorRDY;
INT8U *ErrorBALL;
INT8U *ErrorSHUT;
INT8U *ErrorCan1;
INT8U *ErrorCan2;

void para_init()
{
	char temp[15] = "Well!!!";

	/* init motor struct */
	motor.state = disable;
	motor.encoder_resolution = 1000; // EC 30: 1000   U10: 4096
	motor.reduction = 18.7778f;		 // EC 30: 169/9 = 18.7778  U10: 1

	/* init chassis physical para */
	chassis.width_half = 15.5f;
	chassis.length_half = 15.5f;
	// chassis.omni_wheel2center = 30.0f;
	chassis.WheelD = 15.0f;
	chassis.reduction_turn = 12.0f; // FIXME: 一定要统一协议啊，吐血
	chassis.K_V_motor = motor.reduction * 60.0f / (PI * chassis.WheelD);
	chassis.static_friction_coefficient = 1.0f;
	chassis.dynamic_friction_coefficient = 0.9f;

	/* init user struct */
	user.identity = Other;
	user.area_side = red_area;
	user.area_side_last = red_area;
	user.run_mode = auto_mode;
	user.debug_mode = debug_point;
	strcpy(user.error, temp);
	run.speed_max_limited = 100.f; // user should set or reset the limited speed befor your run_task to ensure your and robot's serity

	/* init run_time flag */
	run_time.count_time = False;

	/* init lcd struct */
	lcd.screen_id = 0;
	lcd.show_area = True;
	lcd.show_speed = False;
	lcd.switch_screen = False;
	lcd.read_id = True;

	/* init debug_posi struct */
	debug_posi.xyr = y_dir;
	debug_posi.speed_max_limited = 100;
	debug_posi.dis_up = 100;
	debug_posi.dis_down = 100;
	debug_posi.points_zero[0] = 0;
	debug_posi.points_zero[1] = 0;
	debug_posi.points_zero[2] = 0.0f;
	debug_posi.points[x_dir][0] = 500;
	debug_posi.points[x_dir][1] = 0;
	debug_posi.points[x_dir][2] = 0.0f;
	debug_posi.points[y_dir][0] = 0;
	debug_posi.points[y_dir][1] = 500;
	debug_posi.points[y_dir][2] = 0.0f;
	debug_posi.speed_rotate = 0;
	debug_posi.speed_start = 50;

	/* init gyroscope struct */
	gyroscope.init_success = False;
	gyroscope.state = Error;
	gyroscope.angle_err = 0;

	/* init camera struct */
	camera.on_off = False;
	camera.read_data = False;

	/* init usart_dma flag */
	usart_dma.Usart2DmaSendFinish = True;

	/* init mechanism struct */
	// mechanism.balabalabala
}

#ifndef __CAN1_H
#define __CAN1_H

#include "stm32f4xx.h"
#include "stm32f4xx_can.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "param.h"
#include "run.h"

void CAN1_Configuration(void);
void gyroscopes_whu(u8 waitflag);

#endif

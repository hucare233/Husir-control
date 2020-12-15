#ifndef __CAN2_H
#define __CAN2_H

#include "stm32f4xx.h"
#include "stm32f4xx_can.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "param.h"

void CAN2_Configuration(void);
void DecodeFloatData(float* f, unsigned char* buff);
void Check(u8 ind_1,u8 ind_2);
	
#endif

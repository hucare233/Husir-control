#ifndef __USART2_H
#define __USART2_H

#include "stdlib.h"
#include "stm32f4xx.h"
#include "stm32f4xx_usart.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_dma.h"
#include "param.h"
#include "motor.h"
#include "can1.h"

void USART2_Configuration(void);

#endif

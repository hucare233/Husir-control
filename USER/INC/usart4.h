#ifndef __USART4_H
#define __USART4_H

#include "stdlib.h"
#include "stm32f4xx.h"
#include "stm32f4xx_usart.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_dma.h"
#include "param.h"

void USART4_Configuration(void);
void USART4_sendData(u8 *a, u8 count);

#endif

#ifndef _USART5_H
#define _USART5_H

#include "stm32f4xx.h"
#include "stm32f4xx_usart.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_dma.h"
#include "param.h"

void USART5_Configuration(void);
void Communication_With_TR(void);

#endif

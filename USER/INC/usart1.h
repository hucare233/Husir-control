#ifndef __USART1_H
#define __USART1_H

#include "stm32f4xx.h"
#include "stm32f4xx_usart.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_dma.h"
#include "queue.h"
#include "param.h"

extern OS_EVENT *Cylinder;
extern char str_ElectorMagnet[8];
extern char str_Cylinder[8];
extern u32 Delay_time;
void USART1_Configuration(void);
void USART1_sendData(u8 *a, u8 count);
void usart_txhander(void);

#endif

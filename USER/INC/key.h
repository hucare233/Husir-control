#ifndef __KEY_H
#define __KEY_H

#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "delay.h"

#define Matrixkey  Check_Matrixkey() //¾ØÕó¼üÅÌ
#define Dialkey		 Check_Dialkey()   //²¦Âë¿ª¹Ø
 
/********* Matrixkey **********/
#define S1           				 0x11
#define S2        					 0x12
#define S3        					 0x14
#define S4          				 0x18
#define S5            			 0x21
#define S6           				 0x22
#define S7        					 0x24
#define S8       						 0x28
#define S9          				 0x41
#define S10            			 0x42
#define S11           			 0x44
#define S12        					 0x48
#define S13        					 0x81
#define S14         				 0x82
#define S15            	 		 0x84
#define S16            	 		 0x88
/*********************************/

extern u8 MatrixkeyNumber;
extern u8 DialkeyNumber;
void KEY_Configuration(void);
u8 Check_Matrixkey(void);
u8 Check_Dialkey(void);

#endif

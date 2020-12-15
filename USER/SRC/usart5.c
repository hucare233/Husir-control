#include "usart5.h"

uint8_t   aTxBuffer_UART5[51]={0};//一次传输数据量不要大于52字节

void USART5_Configuration()
{
	USART_InitTypeDef USART_InitStructure;
	GPIO_InitTypeDef  GPIO_InitStructure;
	NVIC_InitTypeDef  NVIC_InitStructure;
	DMA_InitTypeDef 	DMA_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC|RCC_AHB1Periph_GPIOD, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE);

	GPIO_PinAFConfig(GPIOC, GPIO_PinSource12, GPIO_AF_UART5);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource2, GPIO_AF_UART5);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(UART5, &USART_InitStructure);
	USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);          //接受中断

	//配置NVIC
	NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 7;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	//使能USART
	DMA_DeInit (DMA1_Stream7);
	DMA_InitStructure.DMA_Channel = DMA_Channel_4;
	DMA_InitStructure.DMA_BufferSize = 0;
	DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)(aTxBuffer_UART5);
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&UART5->DR);
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;//以下为f4特有
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	DMA_Init (DMA1_Stream7, &DMA_InitStructure);
	
	DMA_ClearFlag (DMA1_Stream7, DMA_IT_TCIF7);					//清除中断标志
	DMA_ClearITPendingBit (DMA1_Stream7, DMA_IT_TCIF7);
	DMA_ITConfig (DMA1_Stream7, DMA_IT_TC,ENABLE);//关闭了DMA传输完成中断，导致数据无法传输。10/14/2018
	
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream7_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 6;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init (&NVIC_InitStructure);
	
	USART_DMACmd (UART5, USART_DMAReq_Tx, ENABLE);
	USART_Cmd(UART5, ENABLE);
}


u8 USART5_RX_BUF[32];     //接收缓冲,最大32个字节.
u8 USART5_RX_STA = 0;     //接收状态标记
short int TR_Data_Updata_Time = 0;//接收到TR数据的次数
void UART5_IRQHandler (void)
{
  u8 temp;
	
	if(USART_GetITStatus (UART5, USART_IT_ORE_RX) != RESET)//过载重置接受数组
	{
	 temp = USART_ReceiveData(UART5);
	 USART5_RX_STA = 0;
	 memset(USART5_RX_BUF, 0, sizeof(USART5_RX_BUF));
	}
	
	else if(USART_GetITStatus (UART5, USART_IT_RXNE) != RESET) //接收寄存器非空
  {
		USART_ClearFlag(UART5, USART_FLAG_RXNE);				//USART_FLAG_RXNE        //清除中断标志
		USART_ClearITPendingBit (UART5, USART_IT_RXNE);
		
    temp = USART_ReceiveData(UART5);
    if((USART5_RX_STA & 0x40) != 0) //接收已经开始
    {
			if((USART5_RX_STA & 0x80) == 0) //接收未完成
      {
				if(temp == 0x0a)
					USART5_RX_STA |= 0x80;  //接收完成了
				
        else //还没收到*
        {
          USART5_RX_BUF[USART5_RX_STA & 0X3F] = temp;
          USART5_RX_STA++;
					
          if((USART5_RX_STA & 0X3F) > 32)
						USART5_RX_STA = 0; //接收数据错误，重新开始接收
        }
      }
    }
    else if(temp == '#')    //接收到包头
      USART5_RX_STA |= 0x40;

    if((USART5_RX_STA & 0x80) != 0)     //接收已经完成，立即处理命令
    {
			USART5_RX_STA = 0;
//			TR_PositionNow_x = (double)((s16)(((u16)(USART5_RX_BUF[1]) << 8) | USART5_RX_BUF[0])) * 0.1;
//			TR_PositionNow_y = (double)((s16)(((u16)(USART5_RX_BUF[3]) << 8) | USART5_RX_BUF[2])) * 0.1;
//			TR_Process_Sequence =USART5_RX_BUF[4];
//			if(1 == TR_Process_Sequence)
//			{
//				BallLeftOrRight=0;
//			}
			TR_Data_Updata_Time++;
    }
	}
}

void DMA1_Stream7_IRQHandler (void)               //数据传输完成，产生中断，检查是否还有没有传输的数据，继续传输
{
	if(DMA_GetITStatus (DMA1_Stream7, DMA_IT_TCIF7) == SET)
	{
		DMA_ClearFlag (DMA1_Stream7, DMA_IT_TCIF7);					//清除中断标志
		DMA_ClearITPendingBit (DMA1_Stream7, DMA_IT_TCIF7);
		FlagPara.Usart5DmaSendFinish= 0;
	}
}

void USART5_sendData (u8 *a, u8 count)
{
	if(0 == FlagPara.Usart5DmaSendFinish)//若上一次传输未完成，则舍弃本次传输
	{
	  memcpy (&aTxBuffer_UART5[0], a, count);
		DMA_SetCurrDataCounter (DMA1_Stream7, count);
		DMA_Cmd (DMA1_Stream7, ENABLE);
	  FlagPara.Usart5DmaSendFinish = 1;
	}
}

void Communication_With_TR(void)//向PR发送TR的坐标和流程序列
{
////BallLeftOrRight 1球偏右 TR往Y正走     -1球偏左 TR往Y负走
//	u8 Information_For_TR[6];
//	Information_For_TR[0] = '#';
//	Information_For_TR[1] = (uint8_t)((int16_t)(BallLeftOrRight ));
//	Information_For_TR[2] = (uint8_t)((int16_t)(BallLeftOrRight ) >> 8);
//	Information_For_TR[3] = (uint8_t)((int16_t)(PR_Process_Sequence));
//	Information_For_TR[4] = (uint8_t)((int16_t)(PR_Process_Sequence) >> 8);
//	Information_For_TR[5] = 0x0a;
//	USART5_sendData(Information_For_TR, 6);
}

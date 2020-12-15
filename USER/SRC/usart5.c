#include "usart5.h"

uint8_t   aTxBuffer_UART5[51]={0};//һ�δ�����������Ҫ����52�ֽ�

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
	USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);          //�����ж�

	//����NVIC
	NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 7;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	//ʹ��USART
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
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;//����Ϊf4����
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	DMA_Init (DMA1_Stream7, &DMA_InitStructure);
	
	DMA_ClearFlag (DMA1_Stream7, DMA_IT_TCIF7);					//����жϱ�־
	DMA_ClearITPendingBit (DMA1_Stream7, DMA_IT_TCIF7);
	DMA_ITConfig (DMA1_Stream7, DMA_IT_TC,ENABLE);//�ر���DMA��������жϣ����������޷����䡣10/14/2018
	
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream7_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 6;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init (&NVIC_InitStructure);
	
	USART_DMACmd (UART5, USART_DMAReq_Tx, ENABLE);
	USART_Cmd(UART5, ENABLE);
}


u8 USART5_RX_BUF[32];     //���ջ���,���32���ֽ�.
u8 USART5_RX_STA = 0;     //����״̬���
short int TR_Data_Updata_Time = 0;//���յ�TR���ݵĴ���
void UART5_IRQHandler (void)
{
  u8 temp;
	
	if(USART_GetITStatus (UART5, USART_IT_ORE_RX) != RESET)//�������ý�������
	{
	 temp = USART_ReceiveData(UART5);
	 USART5_RX_STA = 0;
	 memset(USART5_RX_BUF, 0, sizeof(USART5_RX_BUF));
	}
	
	else if(USART_GetITStatus (UART5, USART_IT_RXNE) != RESET) //���ռĴ����ǿ�
  {
		USART_ClearFlag(UART5, USART_FLAG_RXNE);				//USART_FLAG_RXNE        //����жϱ�־
		USART_ClearITPendingBit (UART5, USART_IT_RXNE);
		
    temp = USART_ReceiveData(UART5);
    if((USART5_RX_STA & 0x40) != 0) //�����Ѿ���ʼ
    {
			if((USART5_RX_STA & 0x80) == 0) //����δ���
      {
				if(temp == 0x0a)
					USART5_RX_STA |= 0x80;  //���������
				
        else //��û�յ�*
        {
          USART5_RX_BUF[USART5_RX_STA & 0X3F] = temp;
          USART5_RX_STA++;
					
          if((USART5_RX_STA & 0X3F) > 32)
						USART5_RX_STA = 0; //�������ݴ������¿�ʼ����
        }
      }
    }
    else if(temp == '#')    //���յ���ͷ
      USART5_RX_STA |= 0x40;

    if((USART5_RX_STA & 0x80) != 0)     //�����Ѿ���ɣ�������������
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

void DMA1_Stream7_IRQHandler (void)               //���ݴ�����ɣ������жϣ�����Ƿ���û�д�������ݣ���������
{
	if(DMA_GetITStatus (DMA1_Stream7, DMA_IT_TCIF7) == SET)
	{
		DMA_ClearFlag (DMA1_Stream7, DMA_IT_TCIF7);					//����жϱ�־
		DMA_ClearITPendingBit (DMA1_Stream7, DMA_IT_TCIF7);
		FlagPara.Usart5DmaSendFinish= 0;
	}
}

void USART5_sendData (u8 *a, u8 count)
{
	if(0 == FlagPara.Usart5DmaSendFinish)//����һ�δ���δ��ɣ����������δ���
	{
	  memcpy (&aTxBuffer_UART5[0], a, count);
		DMA_SetCurrDataCounter (DMA1_Stream7, count);
		DMA_Cmd (DMA1_Stream7, ENABLE);
	  FlagPara.Usart5DmaSendFinish = 1;
	}
}

void Communication_With_TR(void)//��PR����TR���������������
{
////BallLeftOrRight 1��ƫ�� TR��Y����     -1��ƫ�� TR��Y����
//	u8 Information_For_TR[6];
//	Information_For_TR[0] = '#';
//	Information_For_TR[1] = (uint8_t)((int16_t)(BallLeftOrRight ));
//	Information_For_TR[2] = (uint8_t)((int16_t)(BallLeftOrRight ) >> 8);
//	Information_For_TR[3] = (uint8_t)((int16_t)(PR_Process_Sequence));
//	Information_For_TR[4] = (uint8_t)((int16_t)(PR_Process_Sequence) >> 8);
//	Information_For_TR[5] = 0x0a;
//	USART5_sendData(Information_For_TR, 6);
}

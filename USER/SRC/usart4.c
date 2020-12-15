#include "usart4.h"

static uint8_t   aTxBuffer_UART4[51] = {0};//һ�δ�����������Ҫ����52�ֽ�

void USART4_Configuration ()
{
	USART_InitTypeDef USART_InitStructure;
	GPIO_InitTypeDef  GPIO_InitStructure;
	NVIC_InitTypeDef  NVIC_InitStructure;
	DMA_InitTypeDef 	DMA_InitStructure;

	RCC_AHB1PeriphClockCmd (RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_APB1PeriphClockCmd (RCC_APB1Periph_UART4, ENABLE);
	RCC_AHB1PeriphClockCmd (RCC_AHB1Periph_DMA1, ENABLE);

	GPIO_PinAFConfig (GPIOA, GPIO_PinSource0, GPIO_AF_UART4);
	GPIO_PinAFConfig (GPIOA, GPIO_PinSource1, GPIO_AF_UART4);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init (GPIOA, &GPIO_InitStructure);

	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init (UART4, &USART_InitStructure);
	USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);          //�����ж�
	NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	DMA_DeInit (DMA1_Stream4);
	DMA_InitStructure.DMA_Channel = DMA_Channel_4;
	DMA_InitStructure.DMA_BufferSize = 0;
	DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)(aTxBuffer_UART4);
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&UART4->DR);
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
	DMA_Init (DMA1_Stream4, &DMA_InitStructure);
	
	DMA_ClearFlag (DMA1_Stream4, DMA_IT_TCIF4);					//����жϱ�־
	DMA_ClearITPendingBit (DMA1_Stream4, DMA_IT_TCIF4);
	DMA_ITConfig (DMA1_Stream4, DMA_IT_TC,ENABLE);//�ر���DMA��������жϣ����������޷����䡣10/14/2018
	
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 7;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init (&NVIC_InitStructure);

	USART_DMACmd (UART4, USART_DMAReq_Tx, ENABLE);
	USART_Cmd (UART4, ENABLE);
}

void USART4_sendData (u8 *a, u8 count)
{
	if(0 == FlagPara.Usart4DmaSendFinish)//����һ�δ���δ��ɣ����������δ���
	{
	  memcpy (&aTxBuffer_UART4[0], a, count);
		DMA_SetCurrDataCounter (DMA1_Stream4, count);
		DMA_Cmd (DMA1_Stream4, ENABLE);
	  FlagPara.Usart4DmaSendFinish = 1;
	}
}

u8 USART4_RX_BUF[32];     //���ջ���,���32���ֽ�.
u8 USART4_RX_STA = 0;     //����״̬���
int Count_times = 0;

float Camera_Dispose[2][10] = {0};
u8 temp;
void UART4_IRQHandler (void)
{
	if(USART_GetITStatus (UART4, USART_IT_ORE_RX) != RESET)//�������ý�������
	{
	 temp = USART_ReceiveData(UART4);
	 USART4_RX_STA = 0;
	 memset(USART4_RX_BUF, 0, sizeof(USART4_RX_BUF));
	}
	
	else if(USART_GetITStatus (UART4, USART_IT_RXNE) != RESET) //���ռĴ����ǿ�
  {
		USART_ClearFlag(UART4, USART_FLAG_RXNE);				//USART_FLAG_RXNE        //����жϱ�־
		USART_ClearITPendingBit (UART4, USART_IT_RXNE);
		temp = USART_ReceiveData(UART4);
    if((USART4_RX_STA & 0x40) != 0) //�����Ѿ���ʼ
    {
			if((USART4_RX_STA & 0x80) == 0) //����δ���
      {
				if(temp == 0x0a)
					USART4_RX_STA |= 0x80;  //���������
        else //��û�յ�*
        {
          USART4_RX_BUF[USART4_RX_STA & 0X3F] = temp;
          USART4_RX_STA++;
          if((USART4_RX_STA & 0X3F) > 32)
						USART4_RX_STA = 0; //�������ݴ������¿�ʼ����
        }
      }
    }
    else if(temp == '#')    //���յ���ͷ
      USART4_RX_STA |= 0x40;

    if((USART4_RX_STA & 0x80) != 0)     //�����Ѿ���ɣ�������������
		{
//			Camera_Position[0] = ((short int)(USART4_RX_BUF[1] << 8 | USART4_RX_BUF[0])) / 10.0 ;			
//			Camera_Position[1] = ((short int)(USART4_RX_BUF[3] << 8 | USART4_RX_BUF[2])) / 10.0 -22;
////			Camera_Position[1] = 516.5/cos(-PI/180.0f*AngelTurnNow/300.0f);		
//			Camera_Position[1] = 475/cos(-PI/180.0f*AngelTurnNow/300.0f);						
////		    Camera_Position[2] = sqrt(Camera_Position[0] * Camera_Position[0] + Camera_Position[1] * Camera_Position[1]);
//			//X
//			Camera_Dispose[0][0] = cos(-PI/180.0f*AngelTurnNow/300.0f)*Camera_Position[0]-sin(-PI/180.0f*AngelTurnNow/300.0f)*Camera_Position[1]+PositionNow.x;	//����������
//			//Y
//			Camera_Dispose[0][1] = sin(-PI/180.0f*AngelTurnNow/300.0f)*Camera_Position[0]+cos(-PI/180.0f*AngelTurnNow/300.0f)*Camera_Position[1]+PositionNow.y;
//			//ת����TR��������ϵ
//			PositionX_Ball = -Camera_Dispose[0][1]+410;	
//			PositionY_Ball =  Camera_Dispose[0][0]+1000-15;
//			if((PositionY_Ball-TR_PositionNow_y)>=10&&(PositionY_Ball-TR_PositionNow_y)<=40)
//				BallLeftOrRight=1;	//��
//			else if((PositionY_Ball-TR_PositionNow_y)<=-10&&(PositionY_Ball-TR_PositionNow_y)>=-50)
//				BallLeftOrRight=-1;	//��
//			else
//				BallLeftOrRight=0;	//λ�ò���			
//			Camera_Data_Receieve_Flag = 0;
//			USART4_RX_STA = 0;
		}
	}
}

void DMA1_Stream4_IRQHandler (void)               //���ݴ�����ɣ������жϣ�����Ƿ���û�д�������ݣ���������
{
	if(DMA_GetITStatus (DMA1_Stream4, DMA_IT_TCIF4) == SET)
	{
		DMA_ClearFlag (DMA1_Stream4, DMA_IT_TCIF4);					//����жϱ�־
		DMA_ClearITPendingBit (DMA1_Stream4, DMA_IT_TCIF4);
		FlagPara.Usart4DmaSendFinish = 0;
	}
}


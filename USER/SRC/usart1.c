#include "usart1.h"

/** 
  * @brief  ��������ʾ����
  */
static uint8_t aTxBuffer_UART1[51]={0};//һ�δ�����������Ҫ����52�ֽ�

void USART1_Configuration()
{
	USART_InitTypeDef USART_InitStructure;
	GPIO_InitTypeDef  GPIO_InitStructure;
	NVIC_InitTypeDef  NVIC_InitStructure;
	DMA_InitTypeDef 	DMA_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);

	GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStructure);
	
//�����жϿ������³��������ж��� �޷�ִ�� 20180529 ����������ʾ����Ҳ����Ҫ�����ж�
//	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);          //�����ж�
//	USART_ClearFlag (USART1,USART_IT_RXNE);			
//	USART_ClearITPendingBit(USART1,USART_IT_RXNE);
//	
//	//����NVIC
//	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_Init(&NVIC_InitStructure);
	
	DMA_DeInit(DMA2_Stream7);
	DMA_InitStructure.DMA_Channel =DMA_Channel_4;
	DMA_InitStructure.DMA_BufferSize =0;
	DMA_InitStructure.DMA_DIR =DMA_DIR_MemoryToPeripheral;
	DMA_InitStructure.DMA_Memory0BaseAddr =(uint32_t)(aTxBuffer_UART1);
	DMA_InitStructure.DMA_PeripheralBaseAddr =(uint32_t)(&USART1->DR);
	DMA_InitStructure.DMA_MemoryInc =DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralInc =DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryDataSize =DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_PeripheralDataSize =DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_Mode=DMA_Mode_Normal;
	DMA_InitStructure.DMA_Priority =DMA_Priority_VeryHigh;//����Ϊf4����
	DMA_InitStructure.DMA_FIFOMode =DMA_FIFOMode_Disable;
	DMA_InitStructure.DMA_FIFOThreshold =DMA_FIFOThreshold_HalfFull;
	DMA_InitStructure.DMA_PeripheralBurst =DMA_PeripheralBurst_Single;
	DMA_InitStructure.DMA_MemoryBurst =DMA_MemoryBurst_Single;
	DMA_Init(DMA2_Stream7,&DMA_InitStructure);
	
	DMA_ClearFlag (DMA2_Stream7,DMA_IT_TCIF7);					//����жϱ�־
	DMA_ClearITPendingBit(DMA2_Stream7,DMA_IT_TCIF7);
	DMA_ITConfig(DMA2_Stream7,DMA_IT_TC,ENABLE);//�ر���DMA��������жϣ����������޷����䡣10/14/2018
	
	NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream7_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 5;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	USART_DMACmd(USART1,USART_DMAReq_Tx,ENABLE);
	USART_Cmd(USART1, ENABLE);
}

void USART1_sendData(u8 *a,u8 count)
{
	if(FlagPara.Usart1DmaSendFinish==0)//����һ�δ���δ��ɣ����������δ���
	{
	  memcpy(&aTxBuffer_UART1[0],a,count);
		DMA_SetCurrDataCounter(DMA2_Stream7,count);
		DMA_Cmd(DMA2_Stream7, ENABLE);
	  FlagPara.Usart1DmaSendFinish=1;
	}
}

void DMA2_Stream7_IRQHandler(void)               //���ݴ�����ɣ������жϣ�����Ƿ���û�д�������ݣ���������
{
	if(DMA_GetITStatus(DMA2_Stream7,DMA_IT_TCIF7)==SET)
	{
		DMA_ClearFlag (DMA2_Stream7,DMA_IT_TCIF7);					//����жϱ�־
		DMA_ClearITPendingBit(DMA2_Stream7,DMA_IT_TCIF7);
		FlagPara.Usart1DmaSendFinish=0;
	}
}

u8 USART1_RX_BUF[32];     //���ջ���,���32���ֽ�.
u8 USART1_RX_STA = 0;     //����״̬���

/*
	 ȡ���˴��ڵĽ����жϣ��Ժ���Ҫ�����ټӻ��� 
*/
void USART1_IRQHandler(void)
{
	u8 temp;
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) //���ռĴ����ǿ�
	{
		USART_ClearFlag(USART1, USART_IT_RXNE); 				//USART_FLAG_RXNE        //����жϱ�־
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);

		temp = USART_ReceiveData(USART1);
		if((USART1_RX_STA & 0x40) != 0) //�����Ѿ���ʼ
		{
			if((USART1_RX_STA & 0x80) == 0) //����δ���
			{
				if(temp == 13)
					USART1_RX_STA |=0x80;  //���������
				else //��û�յ�*
				{
					USART1_RX_BUF[USART1_RX_STA & 0X3F] = temp;
					USART1_RX_STA++;
					if((USART1_RX_STA & 0X3F) > 32)
						USART1_RX_STA = 0; //�������ݴ������¿�ʼ����
				}
			}
		}
		else if(temp == '#')    //���յ���ͷ
			USART1_RX_STA |= 0x40;

		if((USART1_RX_STA & 0x80) != 0)     //�����Ѿ���ɣ�������������
		{	
			USART1_RX_STA = 0;
		}
	}
}

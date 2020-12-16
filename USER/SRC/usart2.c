/*
 * @Descripttion: ������
 * @version: ��һ��
 * @Author: ���˵�
 * @Date: 2020-12-15 19:29:29
 * @LastEditors: ���˵�
 * @LastEditTime: 2020-12-16 10:10:17
 * @FilePath: \hu_sir-contorl\USER\SRC\usart2.c
 */
#include "usart2.h"
void USART2_Configuration()
{
	USART_InitTypeDef USART_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	DMA_InitTypeDef DMA_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);

	GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
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
	USART_Init(USART2, &USART_InitStructure);
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE); //�����ж�

	//����NVIC
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 4;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	DMA_DeInit(DMA1_Stream6);
	DMA_InitStructure.DMA_Channel = DMA_Channel_4;
	DMA_InitStructure.DMA_BufferSize = 0;
	DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)(USART2_DMA_SendBuf);
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&USART2->DR);
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh; //����Ϊf4����
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	DMA_Init(DMA1_Stream6, &DMA_InitStructure);

	DMA_ClearFlag(DMA1_Stream6, DMA_IT_TCIF6); //����жϱ�־
	DMA_ClearITPendingBit(DMA1_Stream6, DMA_IT_TCIF6);
	DMA_ITConfig(DMA1_Stream6, DMA_IT_TC, ENABLE); //�ر���DMA��������жϣ����������޷����䡣10/14/2018

	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream6_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 7;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	USART_DMACmd(USART2, USART_DMAReq_Tx, ENABLE);

	//ʹ��USART
	USART_Cmd(USART2, ENABLE);
}

void DMA1_Stream6_IRQHandler(void) //���ݴ�����ɣ������жϣ�����Ƿ���û�д�������ݣ���������
{
	if (DMA_GetITStatus(DMA1_Stream6, DMA_IT_TCIF6) == SET)
	{
		DMA_ClearFlag(DMA1_Stream6, DMA_IT_TCIF6); //����жϱ�־
		DMA_ClearITPendingBit(DMA1_Stream6, DMA_IT_TCIF6);
		usart_dma.Usart2DmaSendFinish = TRUE;
	}
}

static u8 USART2_RX_BUF[60]; //���ջ���,���32���ֽ�.
static u8 USART2_RX_STA = 0; //����״̬���

//��ͷ 0xee ��β0xff
void USART2_IRQHandler(void)
{
	u8 temp = 0;
	if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) //���ռĴ����ǿ�
	{
		USART_ClearFlag(USART2, USART_IT_RXNE);
		USART_ClearITPendingBit(USART2, USART_IT_RXNE);
		temp = USART_ReceiveData(USART2);
		if ((USART2_RX_STA & 0x40) != 0) //�����Ѿ���ʼ
		{
			if ((USART2_RX_STA & 0x80) == 0) //����δ���
			{
				if (temp == 0xff)
					USART2_RX_STA |= 0x80; //���������
				else					   //��û�յ�
				{
					USART2_RX_BUF[USART2_RX_STA & 0X3F] = temp;
					USART2_RX_STA++;
					if ((USART2_RX_STA & 0X3F) > 40)
						USART2_RX_STA = 0; //�������ݴ������¿�ʼ����
				}
			}
		}
		else if (temp == 0xee) //���յ���ͷ
			USART2_RX_STA |= 0x40;

		//������ɴ�������
		if (((USART2_RX_STA & 0x80) != 0))
		{
			if (USART2_RX_BUF[0] == 0x12) //��ʵ�������
			{
				switch (USART2_RX_BUF[1])
				{
				case 0x00:

					break;
				case 0x01:

					break;
				case 0x02:

					break;
				case 0x03:

					break;
				case 0x04:
					break;
				case 0x05:

					break;
				case 0x06:
					break;
				case 0x07:
					break;
				case 0x08:
					break;
				case 0x09:
					break;
				case 0x0A:
					break;
				case 0x0B:
					break;

				default:
					break;
				}
			}
			else //��Ļ����
			{
				switch (USART2_RX_BUF[3])
				{

				default:
					break;
				}
			}
			USART2_RX_STA = 0;
			memset(USART2_RX_BUF, 0, sizeof(USART2_RX_BUF));
		}
	}
}

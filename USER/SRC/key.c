#include "key.h"

u8 MatrixkeyNumber = 0;
u8 DialkeyNumber = 0;
void KEY_Configuration()
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC->AHB1ENR |= RCC_AHB1Periph_GPIOG;
	RCC->AHB1ENR |= RCC_AHB1Periph_GPIOF;
	RCC->AHB1ENR |= RCC_AHB1Periph_GPIOC;

	/* ���뿪�� */
	GPIOF->MODER &= ~(0X0 << 8);
	GPIOF->MODER |= 0X0 << 8;				//����ģʽ
	GPIOF->OSPEEDR &= (uint32_t)0XFFFFF0FF; //2MHz
	GPIOF->PUPDR &= (uint32_t)0XFFFFF0FF;
	GPIOF->PUPDR |= (uint32_t)0X00000500; //����

	GPIOC->MODER &= (uint32_t)0XFFFFFFF0;	//����ģʽ
	GPIOC->OSPEEDR &= (uint32_t)0XFFFFFFF0; //2MHz
	GPIOC->PUPDR &= (uint32_t)0XFFFFFFF0;
	GPIOC->PUPDR |= (uint32_t)0X00000005; //����

	/* ������� */

	//���� �������
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12;
	GPIO_Init(GPIOF, &GPIO_InitStructure);

	//���� ��������
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_Init(GPIOG, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_Init(GPIOF, &GPIO_InitStructure);

	//��ͣ����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_SetBits(GPIOB, GPIO_Pin_1);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_SetBits(GPIOB, GPIO_Pin_1);

	//����������
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_SetBits(GPIOC, GPIO_Pin_9);
}
u8 Check_Matrixkey(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	u8 cord_h = 0XFF, cord_l = 0XFF; //hΪ���� lΪ����
	u8 Val = 0xFF;

	//���� �������
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12;
	GPIO_Init(GPIOF, &GPIO_InitStructure);

	//���� ��������
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_Init(GPIOG, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_Init(GPIOF, &GPIO_InitStructure);

	//�������ȫ������Ϊ0
	GPIO_WriteBit(GPIOC, GPIO_Pin_4 | GPIO_Pin_5, Bit_RESET);
	GPIO_WriteBit(GPIOF, GPIO_Pin_11 | GPIO_Pin_12, Bit_RESET);
	Delay_us(1);

	//��������ֵ
	cord_l &= (u8)((GPIO_ReadInputDataBit(GPIOF, GPIO_Pin_13) << 0) | (GPIO_ReadInputDataBit(GPIOF, GPIO_Pin_14) << 1) |
				   (GPIO_ReadInputDataBit(GPIOF, GPIO_Pin_15) << 2) | (GPIO_ReadInputDataBit(GPIOG, GPIO_Pin_0) << 3));
	if (cord_l != 0X0F)
	{
		Delay_ms(10); //ȥ��
		cord_l &= (u8)((GPIO_ReadInputDataBit(GPIOF, GPIO_Pin_13) << 0) | (GPIO_ReadInputDataBit(GPIOF, GPIO_Pin_14) << 1) |
					   (GPIO_ReadInputDataBit(GPIOF, GPIO_Pin_15) << 2) | (GPIO_ReadInputDataBit(GPIOG, GPIO_Pin_0) << 3));
		if (cord_l != 0X0F)
		{
			//���� �������
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
			GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;

			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
			GPIO_Init(GPIOG, &GPIO_InitStructure);

			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
			GPIO_Init(GPIOF, &GPIO_InitStructure);

			//���� ��������
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
			GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;

			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
			GPIO_Init(GPIOC, &GPIO_InitStructure);

			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12;
			GPIO_Init(GPIOF, &GPIO_InitStructure);

			//�������ȫ������Ϊ0
			GPIO_WriteBit(GPIOG, GPIO_Pin_0, Bit_RESET);
			GPIO_WriteBit(GPIOF, GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15, Bit_RESET);
			Delay_ms(2);
			//��������ֵ
			cord_h &= (u8)((GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_4) << 3) | (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_5) << 2) |
						   (GPIO_ReadInputDataBit(GPIOF, GPIO_Pin_11) << 1) | (GPIO_ReadInputDataBit(GPIOF, GPIO_Pin_12) << 0));

			Val = ~(cord_h << 4 | cord_l);
			return Val;
		}
	}
	return ~Val;
}

u8 Check_Dialkey(void)
{
	return ((GPIO_ReadInputDataBit(GPIOF, GPIO_Pin_4) << 0) | (GPIO_ReadInputDataBit(GPIOF, GPIO_Pin_5) << 1) |
			(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_0) << 2));
}

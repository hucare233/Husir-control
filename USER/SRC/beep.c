#include "beep.h"

/** 
  * @brief  BB响初始化函数
  */
void Beep_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;

	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;

	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;

	GPIO_Init(GPIOA, &GPIO_InitStructure);

	BEEP_OFF;
}

void Beep_Show(u8 num)
{
	for (int i = 0; i < num; i++)
	{
		BEEP_ON;
		Delay_ms(200);
		BEEP_OFF;
		Delay_ms(200);
	}
}

//控制发声
void Sound(u16 frq)
{
        u32 time;
        if(frq != 1000)//休止符
        {
                time = 500000/((u32)frq);
                BEEP_ON;
                Delay_us(time);
                BEEP_OFF;
                Delay_us(time);
        }else
                Delay_us(1000);
}

//生日歌
void play_Music_1(void)
{
        //音谱         低1  2   3   4   5   6   7  中1  2   3   4   5   6   7  高1    2   3     4    5    6    7  不发音
        uc16 tone[] = {262,294,330,349,392,440,494,523,587,659,698,784,880,988,1047,1175,1319,1397,1568,1760,1967,1000};//音频数据表
        //编号          0   1   2   3   4   5   6   7   8   9   10  11  12  13  14   15   16    17   18   19   20  21
        
        //音谱
        u8 music[]={4,4,5,4,
                                                        7,6,21,
                                                        4,4,5,4,
                                                  8,7,21,
                                                        4,4,11,9,
                                                        7,6,5,21,
                                                        3,3,9,7,
                                                        8,7,21,
                                                        };
        //节拍
        u8 time[] = {2,2,4,4,        //时间--2代表半拍(100ms) 4代表一拍(200ms) 8代表两拍(400ms)
                                                         4,4,4, 
                                                         2,2,4,4,
                                                         4,4,4,
                                                         2,2,4,4,
                                                         4,4,4,4,
                                                         2,2,4,4,
                                                         4,4,4,
               };        
        u32 delayShow;
        u16 i,j;
        delayShow = 10;//控制播放快慢
        for(i=0;i<sizeof(music)/sizeof(music[0]);i++)//放歌
        {
                for(j=0;j<((u16)time[i])*tone[music[i]]/delayShow;j++)
                {
                        Sound((u32)tone[music[i]]);
                }        
        }
}

#include "visual_scope.h"

//   两个示波器
unsigned short CRC_CHECK(unsigned char * Buf, unsigned char CRC_CNT)
{
    unsigned short CRC_Temp;
    unsigned char i, j;
    CRC_Temp = 0xffff;

    for (i = 0; i < CRC_CNT; i++)
    {
        CRC_Temp ^= Buf[i];
        for (j = 0; j < 8; j++)
        {
            if (CRC_Temp & 0x01)
                CRC_Temp = (CRC_Temp >> 1 ) ^ 0xa001;
            else
                CRC_Temp = CRC_Temp >> 1;
        }
    }
    return(CRC_Temp);
}



/*虚拟示波器的输出函数*/
 void VS4Channal_Send(int16_t n_dataCH1, int16_t n_dataCH2, int16_t n_dataCH3, int16_t n_dataCH4)
{      
//    unsigned char ii = 0;
    unsigned char SendData[10];
    unsigned short CRC16;   
    SendData[1] = (n_dataCH1 & 0xFF00) >> 8;
		
    SendData[0] = (n_dataCH1 & 0x00FF);
    SendData[3] = (n_dataCH2 & 0xFF00) >> 8;
    SendData[2] = (n_dataCH2 & 0x00FF);
    SendData[5] = (n_dataCH3 & 0xFF00) >> 8;
    SendData[4] = (n_dataCH3 & 0x00FF);
    SendData[7] = (n_dataCH4 & 0xFF00) >> 8;
    SendData[6] = (n_dataCH4 & 0x00FF);
  
    CRC16 = CRC_CHECK(SendData, 8);
    SendData[8] = CRC16 % 256;
    SendData[9] = CRC16 / 256;

		USART1_sendData(SendData,10);

 
}
void VS20Channal_Send (int16_t n_dataCH1, int16_t n_dataCH2, int16_t n_dataCH3, int16_t n_dataCH4,
									int16_t n_dataCH5, int16_t n_dataCH6, int16_t n_dataCH7, int16_t n_dataCH8,
									int16_t n_dataCH9, int16_t n_dataCH10, int16_t n_dataCH11, int16_t n_dataCH12,
									int16_t n_dataCH13, int16_t n_dataCH14, int16_t n_dataCH15, int16_t n_dataCH16,
									int16_t n_dataCH17, int16_t n_dataCH18, int16_t n_dataCH19, int16_t n_dataCH20,u8 Num)
{
	u8 i=0;
	int8_t sum=0;
  unsigned char SendData[45];
	if(Num>20)
		Num=20;
  SendData[0] = 0xAA;
  SendData[1] = 0xAA;
	SendData[2] = 0xF1;
	SendData[3] = Num*2;

	SendData[4] = (n_dataCH1 & 0xFF00) >> 8;	
  SendData[5] = (n_dataCH1 & 0x00FF);
	SendData[6] = (n_dataCH2 & 0xFF00) >> 8;
	SendData[7] = (n_dataCH2 & 0x00FF);
	SendData[8] = (n_dataCH3 & 0xFF00) >> 8;
	SendData[9] = (n_dataCH3 & 0x00FF);
	SendData[10] = (n_dataCH4 & 0xFF00) >> 8;
	SendData[11] = (n_dataCH4 & 0x00FF);
	
	SendData[12] = (n_dataCH5 & 0xFF00) >> 8;	
  SendData[13] = (n_dataCH5 & 0x00FF);
	SendData[14] = (n_dataCH6 & 0xFF00) >> 8;
	SendData[15] = (n_dataCH6 & 0x00FF);
	SendData[16] = (n_dataCH7 & 0xFF00) >> 8;
	SendData[17] = (n_dataCH7 & 0x00FF);
	SendData[18] = (n_dataCH8 & 0xFF00) >> 8;
	SendData[19] = (n_dataCH8 & 0x00FF);
	
	SendData[20] = (n_dataCH9 & 0xFF00) >> 8;	
  SendData[21] = (n_dataCH9 & 0x00FF);
	SendData[22] = (n_dataCH10 & 0xFF00) >> 8;
	SendData[23] = (n_dataCH10 & 0x00FF);
	SendData[24] = (n_dataCH11 & 0xFF00) >> 8;
	SendData[25] = (n_dataCH11 & 0x00FF);
	SendData[26] = (n_dataCH12 & 0xFF00) >> 8;
	SendData[27] = (n_dataCH12 & 0x00FF);
	
	SendData[28] = (n_dataCH13 & 0xFF00) >> 8;	
  SendData[29] = (n_dataCH13 & 0x00FF);
	SendData[30] = (n_dataCH14 & 0xFF00) >> 8;
	SendData[31] = (n_dataCH14 & 0x00FF);
	SendData[32] = (n_dataCH15 & 0xFF00) >> 8;
	SendData[33] = (n_dataCH15 & 0x00FF);
	SendData[34] = (n_dataCH16 & 0xFF00) >> 8;
	SendData[35] = (n_dataCH16 & 0x00FF);
	
	SendData[36] = (n_dataCH17 & 0xFF00) >> 8;	
  SendData[37] = (n_dataCH17 & 0x00FF);
	SendData[38] = (n_dataCH18 & 0xFF00) >> 8;
	SendData[39] = (n_dataCH18 & 0x00FF);
	SendData[40] = (n_dataCH19 & 0xFF00) >> 8;
	SendData[41] = (n_dataCH19 & 0x00FF);
	SendData[42] = (n_dataCH20 & 0xFF00) >> 8;
	SendData[43] = (n_dataCH20 & 0x00FF);
	

	while(i<Num*2+4)
	{		
		sum+=SendData[i];
		i++;
		
	}
	SendData[i]=sum;
  USART1_sendData(SendData,2*Num+5);
	
}




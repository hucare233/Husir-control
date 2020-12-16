/*
 * @Descripttion: ����ת��
 * @version: ��һ��
 * @Author: ���˵�
 * @Date: 2020-12-16 08:40:36
 * @LastEditors: ���˵�
 * @LastEditTime: 2020-12-16 10:07:43
 * @FilePath: \hu_sir-contorl\USER\INC\macro.h
 */
#ifndef _MYDATAPROCESS_H
#define _MYDATAPROCESS_H
/**
  ******************************************************************************
  * @file    MyDataProcess.h
  * @author  Hualuoxi
  * @version V1.3
  * @date    16-4-2018
  * @brief   ��ʵ������ת������������ٶ�
	******************************************************************************
**/

/** 
  * @brief  ��float�������о��ȶ�ʧ�ؽس�2���ֽڣ�����buff��
  */
#define EncodeFloatDataLost(f, buff)           \
  {                                            \
    buff[0] = (u8)(((s16)*f) & 0x00ff);        \
    buff[1] = (u8)((((s16)*f) >> 8) & 0x00ff); \
  }

/** 
  * @brief  ��float�������о��ȶ�ʧ�ؽس�4���ֽڣ�����buff��
	*/
#define DecodeFloatDataLost(f, buff)                              \
  {                                                               \
    *f = (double)((s16)((u16)(buff[1] & 0x00ff) << 8) | buff[0]); \
  }

/** 
  * @brief   ��float�޾��ȶ�ʧ�Ľس�4���ֽڷ���buff��
	*/
//#define EncodeFloatData(f,buff)						{*(float*)buff = *f;}

/** 
  * @brief   ��16λ�з�������ת����2���ֽڷ���buff��
	*/
#define DecodeFloatData(f, buff) \
  {                              \
    *f = *(float *)buff;         \
  }

/** 
  * @brief   ��16λ�з�������ת����2���ֽڷ���buff��
*/
#define EncodeS16Data(f, buff) \
  {                            \
    *(s16 *)buff = *f;         \
  }
/** 
  * @brief   ��buff�е�2���ֽ�ƴ��Ϊs16
*/
#define DecodeS16Data(f, buff) \
  {                            \
    *f = *(s16 *)buff;         \
  }
/** 
  * @brief   ��16λ�޷�������ת����2���ֽڷ���buff��
	*/
#define EncodeU16Data(f, buff) \
  {                            \
    *(u16 *)buff = *f;         \
  }
/** 
  * @brief   ��buff�е�2���ֽ�ƴ��Ϊu16
	*/
#define DecodeU16Data(f, buff) \
  {                            \
    *f = *(u16 *)buff;         \
  }

/** 
  * @brief        ��32λ�з�������ת����4���ֽڷ���buff��
	*/
#define EncodeS32Data(f, buff) \
  {                            \
    *(s32 *)buff = *f;         \
  }

/** 
  * @brief        ��buff�е�4���ֽ�ƴ��Ϊs32 
	*/
#define DecodeS32Data(f, buff) \
  {                            \
    *f = *(s32 *)buff;         \
  }
#define DecodeFloatData(f, buff) \
  {                              \
    *f = *(float *)buff;         \
  }

/** 
  * @brief  ��float�������о��ȶ�ʧ�ؽس�2���ֽڣ�����buff��
  */
#define EncodeFloatDataLost(f, buff)           \
  {                                            \
    buff[0] = (u8)(((s16)*f) & 0x00ff);        \
    buff[1] = (u8)((((s16)*f) >> 8) & 0x00ff); \
  }

/** 
  * @brief  ����buff��ʼ��2���ֽ�λƴ��ת���ɸ�����
	*/
#define DecodeFloatDataLost(f, buff)                              \
  {                                                               \
    *f = (double)((s16)((u16)(buff[1] & 0x00ff) << 8) | buff[0]); \
  }

/** 
  * @brief   ����buff��ʼ��4���ֽ�λƴ��ת����float
	*/
//#define EncodeFloatData(f,buff)						{*(float*)buff = *f;}

/** 
  * @brief   ��16λ�з�������ת����2���ֽڷ���buff��
*/
#define EncodeS16Data(f, buff) \
  {                            \
    *(s16 *)buff = *f;         \
  }
/** 
  * @brief   ��buff�е�2���ֽ�ƴ��Ϊs16
*/
#define DecodeS16Data(f, buff) \
  {                            \
    *f = *(s16 *)buff;         \
  }
/** 
  * @brief   ��16λ�޷�������ת����2���ֽڷ���buff��
	*/
#define EncodeU16Data(f, buff) \
  {                            \
    *(u16 *)buff = *f;         \
  }
/** 
  * @brief   ��buff�е�2���ֽ�ƴ��Ϊu16
	*/
#define DecodeU16Data(f, buff) \
  {                            \
    *f = *(u16 *)buff;         \
  }

/** 
  * @brief        ��32λ�з�������ת����4���ֽڷ���buff��
	*/
#define EncodeS32Data(f, buff) \
  {                            \
    *(s32 *)buff = *f;         \
  }

/** 
  * @brief        ��buff�е�4���ֽ�ƴ��Ϊs32 
	*/
#define DecodeS32Data(f, buff) \
  {                            \
    *f = *(s32 *)buff;         \
  }

//@brief   ��32λ�з�������ת����4���ֽڷ���buff��
#define EncodeS32To4ByteNone(f, buff) \
  {                                   \
    *(s32 *)buff = *f;                \
  }
#define DecodeS32To4ByteNone(f, buff) \
  {                                   \
    *f = *(s32 *)buff;                \
  }

// @brief   ��16λ�з�������ת����2���ֽڷ���buff��
#define EncodeS16To2ByteNone(f, buff) \
  {                                   \
    *(s16 *)buff = *f;                \
  }
#define DecodeS16To2ByteNone(f, buff) \
  {                                   \
    *f = *(s16 *)buff;                \
  }

//@brief   ��32λ�з�������ת����4���ֽڷ���buff��
#define EncodeFloatTo4ByteNone(f, buff) \
  {                                     \
    *(s32 *)buff = *f;                  \
  }
#define DecodeFloatTo4ByteNone(f, buff) \
  {                                     \
    *f = *(s32 *)buff;                  \
  }

#endif

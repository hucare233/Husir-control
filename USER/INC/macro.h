/*
 * @Descripttion: 数据转换
 * @version: 第一版
 * @Author: 叮咚蛋
 * @Date: 2020-12-16 08:40:36
 * @LastEditors: 叮咚蛋
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
  * @brief   宏实现数据转换，提高运行速度
	******************************************************************************
**/

/** 
  * @brief  将float型数据有精度丢失地截成2个字节，放在buff中
  */
#define EncodeFloatDataLost(f, buff)           \
  {                                            \
    buff[0] = (u8)(((s16)*f) & 0x00ff);        \
    buff[1] = (u8)((((s16)*f) >> 8) & 0x00ff); \
  }

/** 
  * @brief  将float型数据有精度丢失地截成4个字节，放在buff中
	*/
#define DecodeFloatDataLost(f, buff)                              \
  {                                                               \
    *f = (double)((s16)((u16)(buff[1] & 0x00ff) << 8) | buff[0]); \
  }

/** 
  * @brief   将float无精度丢失的截成4个字节放在buff中
	*/
//#define EncodeFloatData(f,buff)						{*(float*)buff = *f;}

/** 
  * @brief   将16位有符号数据转换成2个字节放在buff中
	*/
#define DecodeFloatData(f, buff) \
  {                              \
    *f = *(float *)buff;         \
  }

/** 
  * @brief   将16位有符号数据转换成2个字节放在buff中
*/
#define EncodeS16Data(f, buff) \
  {                            \
    *(s16 *)buff = *f;         \
  }
/** 
  * @brief   将buff中的2个字节拼接为s16
*/
#define DecodeS16Data(f, buff) \
  {                            \
    *f = *(s16 *)buff;         \
  }
/** 
  * @brief   将16位无符号数据转换成2个字节放在buff中
	*/
#define EncodeU16Data(f, buff) \
  {                            \
    *(u16 *)buff = *f;         \
  }
/** 
  * @brief   将buff中的2个字节拼接为u16
	*/
#define DecodeU16Data(f, buff) \
  {                            \
    *f = *(u16 *)buff;         \
  }

/** 
  * @brief        将32位有符号数据转换成4个字节放在buff中
	*/
#define EncodeS32Data(f, buff) \
  {                            \
    *(s32 *)buff = *f;         \
  }

/** 
  * @brief        将buff中的4个字节拼接为s32 
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
  * @brief  将float型数据有精度丢失地截成2个字节，放在buff中
  */
#define EncodeFloatDataLost(f, buff)           \
  {                                            \
    buff[0] = (u8)(((s16)*f) & 0x00ff);        \
    buff[1] = (u8)((((s16)*f) >> 8) & 0x00ff); \
  }

/** 
  * @brief  将从buff开始的2个字节位拼接转换成浮点数
	*/
#define DecodeFloatDataLost(f, buff)                              \
  {                                                               \
    *f = (double)((s16)((u16)(buff[1] & 0x00ff) << 8) | buff[0]); \
  }

/** 
  * @brief   将从buff开始的4个字节位拼接转换成float
	*/
//#define EncodeFloatData(f,buff)						{*(float*)buff = *f;}

/** 
  * @brief   将16位有符号数据转换成2个字节放在buff中
*/
#define EncodeS16Data(f, buff) \
  {                            \
    *(s16 *)buff = *f;         \
  }
/** 
  * @brief   将buff中的2个字节拼接为s16
*/
#define DecodeS16Data(f, buff) \
  {                            \
    *f = *(s16 *)buff;         \
  }
/** 
  * @brief   将16位无符号数据转换成2个字节放在buff中
	*/
#define EncodeU16Data(f, buff) \
  {                            \
    *(u16 *)buff = *f;         \
  }
/** 
  * @brief   将buff中的2个字节拼接为u16
	*/
#define DecodeU16Data(f, buff) \
  {                            \
    *f = *(u16 *)buff;         \
  }

/** 
  * @brief        将32位有符号数据转换成4个字节放在buff中
	*/
#define EncodeS32Data(f, buff) \
  {                            \
    *(s32 *)buff = *f;         \
  }

/** 
  * @brief        将buff中的4个字节拼接为s32 
	*/
#define DecodeS32Data(f, buff) \
  {                            \
    *f = *(s32 *)buff;         \
  }

//@brief   将32位有符号数据转换成4个字节放在buff中
#define EncodeS32To4ByteNone(f, buff) \
  {                                   \
    *(s32 *)buff = *f;                \
  }
#define DecodeS32To4ByteNone(f, buff) \
  {                                   \
    *f = *(s32 *)buff;                \
  }

// @brief   将16位有符号数据转换成2个字节放在buff中
#define EncodeS16To2ByteNone(f, buff) \
  {                                   \
    *(s16 *)buff = *f;                \
  }
#define DecodeS16To2ByteNone(f, buff) \
  {                                   \
    *f = *(s16 *)buff;                \
  }

//@brief   将32位有符号数据转换成4个字节放在buff中
#define EncodeFloatTo4ByteNone(f, buff) \
  {                                     \
    *(s32 *)buff = *f;                  \
  }
#define DecodeFloatTo4ByteNone(f, buff) \
  {                                     \
    *f = *(s32 *)buff;                  \
  }

#endif

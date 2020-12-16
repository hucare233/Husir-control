/*** 
 * @CreatedTime   2020-07-25 10:35:58
 * @LastEditors   未定义
 * @LastEditTime  2020-09-20 14:21:53
 * @FilePath      \Project\User\inc\MyMath.h
 */
#ifndef _MYMATH_H
#define _MYMATH_H

#include <math.h>
#include "struct.h"
#include "stdio.h"

/* some const */
#define PI 3.141593f
#define g 9.8f
#define cos_30 0.8660254f
#define cos_60 0.5f
#define cos_45 0.7071067f
#define sin_30 0.5f
#define sin_60 0.8660254f
#define sin_45 0.7071067f

/* some simple math function */
#define ABS(x) ((x) > 0 ? (x) : -(x)) // 括号x的括号不能丢
#define get_sign(x) ((x) > 0 ? 1 : ((x) < 0 ? -1 : 0))

/* 两点间距离 */
#define CountDistance(x_from, y_from, x_to, y_to) \
    (float)(sqrt((x_to - x_from) * (x_to - x_from) + (y_to - y_from) * (y_to - y_from)))
/* 数量积 */
#define InnerProduct(vector1, vector2) \
    (float)(vector1.x * vector2.x + vector1.y * vector2.y)
/* 上下限限制 */
#define Limit(x, _max, _min) ((x) > _max ? _max : ((x) < _min ? _min : (x)))

#endif

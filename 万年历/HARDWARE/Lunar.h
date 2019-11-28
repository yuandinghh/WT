#ifndef _LUNAR_H
#define _LUNAR_H

#include "sys.h"

extern u8 c_moon,century;
extern u8 year_moon,month_moon,day_moon;

void Conversion(u8 c,u8 year,u8 month,u8 day); //公历转农历函数

#endif

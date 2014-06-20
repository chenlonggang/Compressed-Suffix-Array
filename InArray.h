/*============================================
# Filename: InArray.h
# Ver 1.0 2014-06-09
# Copyright (C) 2014 ChenLonggang (chenlonggang.love@163.com)
#
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 or later of the License.
#
# Description: 
  int vector,every ceil needs data_width bits.
=============================================*/
#ifndef _Inarray
#define _Inarray
#include"BaseClass.h"
#include"savekit.h"
#include"loadkit.h"
class InArray
{
public:
	InArray();
	InArray(integer data_num,integer data_width);
	~InArray(void);
	
	integer GetValue(integer index);
	void SetValue(integer index,integer value);
	
	integer GetNum();
	integer GetDataWidth();
	integer GetMemorySize();
    integer GetValue2(integer index);
	
	integer write(savekit & s);
	integer load(loadkit & s);
private:
	u32 * data;
    integer datanum;
	i64 datawidth;
	u32 mask;
};
#endif

/*============================================
# Filename: ABS_Phi.h
# Ver 1.0 2014-06-09
# Copyright (C) 2014 ChenLonggang (chenlonggang.love@163.com)
#
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 or later of the License.
#
# Description: 
 the abstract class of Phi function,it has two subclass
 GAM_Phi: store gap only with gamma coding
 GRL_Phi: first use runlength for the consecutive gap 1,
          and map the runs value to odd/even,and map other
		  value to enev/odd.it's will be helpful for highly
		  repetive data.
=============================================*/
#ifndef _ABS_PHI
#define _ABS_PHI
#include"BaseClass.h"
#include"savekit.h"
#include"loadkit.h"
class ABS_Phi
{
	public:
		virtual ~ABS_Phi(){};
		virtual integer * GetPhiArray()=0;
		virtual void GetSubPhiArray(integer,integer,integer *)=0;
		virtual integer GetValue(integer i)=0;
		virtual integer RightBoundary(integer r,integer L,integer R)=0;
		virtual integer LeftBoundary(integer l,integer L,integer R)=0;
		virtual integer GetMemorySize()=0;
		virtual integer write(savekit & s)=0;
		virtual integer load(loadkit &s)=0;
};
#endif

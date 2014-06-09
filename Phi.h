/*============================================
# Filename: Phi.h
# Ver 1.0 2014-06-09
# Copyright (C) 2014 ChenLonggang (chenlonggang.love@163.com)
#
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 or later of the License.
#
# Description: 
  A proxy class of the kinds of Phi classes.
  so it holds a base-class pointer,may be points a GAM_Phi object
  may be a GRL_Phi object.
=============================================*/
#include"BaseClass.h"
#include"parmaters.h"
#include"savekit.h"
#include"loadkit.h"
#include"ABS_Phi.h"
#include"GAM_Phi.h"
#include"GRL_Phi.h"
class Phi
{
	public:
		Phi(parmaters *csa);
		Phi();//返回一个Phi的空壳
		~Phi();
		i32 *GetPhiArray();
		i32 GetValue(i32 i);
		i32 RightBoundary(i32 r,i32 L,i32 R);
		i32 LeftBoundary(i32 l,i32 L,i32 R);
		i32 Size();
		i32 write(savekit & s);
		i32 load(loadkit &s);
	private:
		ABS_Phi *phi;
		int type;

};

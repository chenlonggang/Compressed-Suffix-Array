/*============================================
# Filename: GRL_Phi.h
# Ver 1.0 2014-06-09
# Copyright (C) 2014 ChenLonggang (chenlonggang.love@163.com)
#
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 or later of the License.
#
# Description: 
  first use run-length coding for consecutive gap 1,and map 
  the runs value to odd/even,and map other gap to even/odd,
  and use the gamma coding to code the new values,it's good for
  highly-repetive data,like xml and html and programe sources data
=============================================*/
#ifndef _GRL_PHI
#define _GRL_PHI
#include"ABS_Phi.h"
#include"InArray.h"
#include"parmaters.h"
#include"BaseClass.h"
class GRL_Phi:public ABS_Phi
{
	public:
		GRL_Phi(parmaters * csa);
		GRL_Phi();
		~GRL_Phi();

		i32 * GetPhiArray();
		i32 GetValue(i32 i);
		i32 RightBoundary(i32 r,i32 L,i32 R);
		i32 LeftBoundary(i32 l,i32 L,i32 R);
		i32 GetMemorySize();
		i32 write(savekit & s);
		i32 load(loadkit & s);
	private:
		i32 mod(i32);
		i32 GetOneBit(i32 i);
		i32 a;
		i32 b;
		i32 alpsize;
		i32 tablewidth;
		u16 *zerostable;
		u16 *decodevaluenum;
		u16 *decodebitsnum;
		u16 *decoderesult;
		i32  * superoffset;
		InArray* samples;
		InArray * offsects;
		u32 * sequence;
		i32 index;
		i32 lenofsequence;
		i32 lenofsuperoffset;
		i32 n;
		i32 * phivalue;
		i32 blogsize(i32 x);
		void SamplingAndCoding(parmaters * csa);//"接头处"用gap+n编码
		void Test();
		void Append(i32 x);
		int Decodegamma(i32& position,i32 & x);
		void InitionalTables();
		i32 ZerosRun(i32 &position );//从position开始，获取0连续出现的次数.
		u64 GetBits(i32 position ,i32 x);//从position开始，读取x位数据
};
#endif

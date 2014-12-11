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

		integer * GetPhiArray();
		void GetSubPhiArray(integer,integer,integer *){};
		integer GetValue(integer i);
		integer RightBoundary(integer r,integer L,integer R);
		integer LeftBoundary(integer l,integer L,integer R);
		integer GetMemorySize();
		integer write(savekit & s);
		integer load(loadkit & s);
	private:
		integer mod(integer);
		integer GetOneBit(integer i);
		integer a;
		integer b;
		integer alpsize;
		integer tablewidth;
		u16 *zerostable;
		u16 *decodevaluenum;
		u16 *decodebitsnum;
		u16 *decoderesult;
		integer  * superoffset;
		InArray* samples;
		InArray * offsects;
		u32 * sequence;
		integer index;
		integer lenofsequence;
		integer lenofsuperoffset;
		integer n;
		integer * phivalue;
		integer blogsize(integer x);
		void SamplingAndCoding(parmaters * csa);
		void Test();
		void Append(integer x);
		integer Decodegamma(integer& position,integer & x);
		void InitionalTables();
		integer ZerosRun(integer &position );
		u64 GetBits(integer position ,integer x);
};
#endif

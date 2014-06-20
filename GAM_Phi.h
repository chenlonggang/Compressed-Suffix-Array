/*============================================
# Filename: GAM_Phi.h
# Ver 1.0 2014-06-09
# Copyright (C) 2014 ChenLonggang (chenlonggang.love@163.com)
#
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 or later of the License.
#
# Description: 
  The Phi array increses chunk by chunk,so we compress the array by 
  saving the gap between Phi[i+1] and Phi[i],and use gamma coding to 
  store the gap.
=============================================*/
#ifndef _GAM_PHI
#define _GAM_PHI
#include"ABS_Phi.h"
#include"InArray.h"
#include"parmaters.h"
#include"BaseClass.h"
class GAM_Phi:public ABS_Phi
{
	public:
		
		GAM_Phi(parmaters * csa);
		GAM_Phi();
		~GAM_Phi();

		integer * GetPhiArray();
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

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
		Phi();
		~Phi();
		
		integer *GetPhiArray();
		void GetPhiPiece(integer index,integer num,integer *phipiece);
		integer GetValue(integer i);
		integer RightBoundary(integer r,integer L,integer R);
		integer LeftBoundary(integer l,integer L,integer R);
		integer Size();
		
		integer write(savekit & s);
		integer load(loadkit &s);
	
	private:
		ABS_Phi *phi;
		integer type;
};

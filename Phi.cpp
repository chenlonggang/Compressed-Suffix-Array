/*============================================
# Filename: Phi.cpp
# Ver 1.0 2014-06-09
# Copyright (C) 2014 ChenLonggang (chenlonggang.love@163.com)
#
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 or later of the License.
#
# Description: 
=============================================*/
#include"Phi.h"

Phi::Phi()
{
	type=0;
	phi=NULL;
}

Phi::Phi(parmaters *csa)
{
	this->type=csa->phitype;
	switch(this->type)
	{   
		case 0: phi=new GAM_Phi(csa);break;
		case 1: phi=new GRL_Phi(csa);break;
		default:phi=new GAM_Phi(csa);break;
	}
}

integer Phi::GetValue(integer i)
{
	return phi->GetValue(i);
}

integer Phi::RightBoundary(integer r,integer L,integer R)
{
	return phi->RightBoundary(r,L,R);
}

integer Phi::LeftBoundary(integer l,integer L,integer R)
{
	return phi->LeftBoundary(l,L,R);
}

integer Phi::Size()
{
	return phi->GetMemorySize();
}

integer Phi::write(savekit &s)
{
	s.writeinteger(type);
	return phi->write(s);
}

integer Phi::load(loadkit &s)
{
	s.loadinteger(type);
	switch(this->type)
	{
		case 0: phi=new GAM_Phi();break;
		case 1: phi=new GRL_Phi();break;
		default:phi=new GAM_Phi();break;
	}
	return phi->load(s);
}

Phi::~Phi()
{
	if(phi!=NULL)
		delete phi;
	phi=NULL;
}

integer * Phi::GetPhiArray()
{
	return phi->GetPhiArray();
}

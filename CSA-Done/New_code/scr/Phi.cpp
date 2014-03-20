#include"Phi.h"
Phi::Phi()
{
	/*
	this->type=type;
	switch(this->type)
	{
		case 0:phi=new GAM_Phi();break;
		case 1:phi=new GRL_Phi();break;
		default:phi=new GAM_Phi();break;
	}
	*/
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
i32 Phi::GetValue(i32 i)
{
	return phi->GetValue(i);
}
i32 Phi::RightBoundary(i32 r,i32 L,i32 R)
{
	return phi->RightBoundary(r,L,R);
}
i32 Phi::LeftBoundary(i32 l,i32 L,i32 R)
{
	return phi->LeftBoundary(l,L,R);
}
i32 Phi::Size()
{
	return phi->GetMemorySize();
}
i32 Phi::write(savekit &s)
{
	s.writei32(type);
	return phi->write(s);
}
i32 Phi::load(loadkit &s)
{
	s.loadi32(type);
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


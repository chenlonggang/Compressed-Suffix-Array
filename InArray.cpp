/*============================================
# Filename: InArray.cpp
# Ver 1.0 2014-06-09
# Copyright (C) 2014 ChenLonggang (chenlonggang.love@163.com)
#
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 or later of the License.
#
# Description: 
=============================================*/
#include "InArray.h"
#include<string.h>
#include<stdlib.h>
#include<iostream>
using namespace std;


InArray::~InArray(void)
{
	delete [] data;
}

InArray::InArray()
{
}

InArray::InArray(integer len, integer size) 
{
	if(len<0||size<=0)
		cout<<"InArray构造参数输入错误"<<endl;
	else
	{
		this->datanum =len;
		this->datawidth =size;
	    i64 totlesize=datanum*datawidth;
		if(totlesize%32==0)
			totlesize=totlesize/32;
		else
			totlesize=(totlesize/32)+1;
		this->data =new u32[totlesize];
		memset(data,0,4*totlesize);
		mask=((1ULL<<datawidth)-1);
	}
}

//数组中下标为index的位置设置值为value
void InArray::SetValue (integer index, integer v)
{

	if(index>datanum-1|| index<0)
	{
		cerr<<"InArray:index out of boundary"<<endl;
		exit(0) ;
	}
	else if(v>((1LL<<datawidth)-1))
	{
		cerr<<"InArray:value is out of boundary"<<endl;
		exit(0) ;
	}
	else
	{
		/*
		u64 value=v;
		u64 anchor=(index*datawidth)>>5;
		u64 temp1=data[anchor];
		u32 temp2=data[anchor+1];
		temp1=(temp1<<32)+temp2;
		integer overloop=((anchor+2)<<5)-(index+1)*datawidth;
		value=(value<<overloop);
		temp1=temp1+value;
		data[anchor+1]=(temp1&(0xffffffff));
		data[anchor]=(temp1>>32)&(0xffffffff);
		*/
		u64 value=v;
		u64 anchor_start=(index*datawidth)>>5;
		u64 anchor_end=((index+1)*datawidth)>>5;
		if(anchor_end-anchor_start<2){//在同一个u32,跨越两个u32
			u64 temp1=data[anchor_start];
			u32 temp2=data[anchor_start+1];
			temp1=(temp1<<32)+temp2;
			i32 overloop=((anchor_start+2)<<5)-(index+1)*datawidth;
			value=(value<<overloop);
			temp1=temp1+value;
			data[anchor_start+1]=(temp1&(0xffffffff));
			data[anchor_start]=(temp1>>32)&(0xffffffff);
		}
		else{
			i32 s1=(anchor_start+1)*32-index*datawidth;
			i32 s2=datawidth-32-s1;
			data[anchor_start]=(data[anchor_start]|(value>>(datawidth-s1)));
			data[anchor_start+1]=(data[anchor_start+1]|((value>>s2)&(0xffffffff)));
			data[anchor_end]=(data[anchor_end]|(((((1ULL<<s2)-1)&value)<<(32-s2))&(0xffffffff)));
		}


	}


}

integer InArray::GetNum ()
{
	return datanum;
}
integer InArray::GetMemorySize() 
{
	return (datanum*datawidth)/8;
}

		
integer InArray::GetDataWidth() 
{
	return datawidth;
}

integer InArray::GetValue(integer index)
{


	if(index>datanum-1||index<0)
	{
		cerr<<"InArray:GetValue: index out of boundary"<<endl;
		exit(0);
	}
	
	/*
	u64 anchor=(index*datawidth)>>5;
	u64 temp1=data[anchor];
	u32 temp2=data[anchor+1];
	temp1=(temp1<<32)+temp2;
	integer overloop=((anchor+2)<<5)-(index+1)*datawidth;
	return (temp1>>overloop)&mask;
	*/
	u64 anchor_start=(index*datawidth)>>5;
	u64 anchor_end=((index+1)*datawidth)>>5;
	if(anchor_end-anchor_start<2){
		u64 temp1=data[anchor_start];
		u32 temp2=data[anchor_start+1];
		temp1=(temp1<<32)+temp2;
		i32 overloop=((anchor_start+2)<<5)-(index+1)*datawidth;
		return (temp1>>overloop)&mask;
	}
	else{
		u64 temp1=data[anchor_start+0];
		u64 temp2=data[anchor_start+1];
		u64 temp3=data[anchor_start+2];
		i32 s1=(anchor_start+1)*32-index*datawidth;
		i32 s2=datawidth-32-s1;
		return ((temp1<<(32+s2))+(temp2<<s2)+(temp3>>(32-s2)))&mask;
	}
}
integer InArray::write(savekit & s)
{
	s.writeinteger(datanum);
	s.writei64(datawidth);
	i64 len=(datanum*datawidth);
	if(len%32==0)
		len=len/32;
	else
		len=len/32+1;

	s.writei64(len);
	s.writeu32array(data,len);
	return 1;
}
integer InArray::load(loadkit & s)
{
	s.loadi64(datanum);
	s.loadi64(datawidth);
	integer len=0;
	s.loadinteger(len);
	data=new u32[len];
	s.loadu32array(data,len);
	mask=((1ULL<<datawidth)-1);
	return 1;
}




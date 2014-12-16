/*============================================
# Filename: GAM_Phi.cpp
# Ver 1.0 2014-06-09
# Copyright (C) 2014 ChenLonggang (chenlonggang.love@163.com)
#
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 or later of the License.
#
# Description: 
=============================================*/
#include<math.h>
#include<ctime>
#include "GAM_Phi.h"
#include<fstream>
#include<iostream>
using namespace std;

GAM_Phi::GAM_Phi()
{
}

GAM_Phi::GAM_Phi(parmaters *csa)
{
	this->tablewidth=16;
	this->zerostable=new u16[1<<tablewidth];
	this->decodevaluenum=new u16[1<<tablewidth];
	this->decodebitsnum=new u16[1<<tablewidth];
	this->decoderesult=new u16[1<<tablewidth];
   
	this->alpsize =csa->alphabetsize ;
	this->n=csa->n;
	this->a=csa->SL;
	this->b=csa->L;
	this->index=0;

//	cout<<"before tables"<<endl;
	this->InitionalTables();
//	cout<<"tables is OK"<<endl;
//	cout<<"before coding"<<endl;
	this->SamplingAndCoding (csa);
//	cout<<"coding is ok"<<endl;
	delete [] phivalue;
}

GAM_Phi::~GAM_Phi(void)
{
	delete [] superoffset;
	delete samples;
	delete offsects;
	delete [] sequence;

	delete [] zerostable;
	delete [] decodevaluenum;
	delete [] decodebitsnum;
	delete [] decoderesult;
}

void  GAM_Phi::Append (integer x)
{
	u64 y=x;
	integer zeronums=blogsize(y)-1;
	index=index+zeronums;
	integer valuewidth=zeronums+1;
	
	integer anchor_start =(index>>5);
	integer anchor_end=((index+valuewidth)>>5);
	if(anchor_end-anchor_start<2){
		i32 overloop=((anchor_start+2)<<5)-index-valuewidth;
		y=(y<<overloop);
		sequence[anchor_start]=(sequence[anchor_start]|(y>>32));
		sequence[anchor_start+1]=(sequence[anchor_start+1]|(y&0xffffffff));
		index=index+valuewidth;
	}
	else{
		i32 s1=(anchor_start+1)*32-index;
		i32 s2=valuewidth-32-s1;
		sequence[anchor_start]=(sequence[anchor_start]|(y>>(valuewidth-s1)));
		sequence[anchor_start+1]=(sequence[anchor_start+1]|((y>>s2)&(0xffffffff)));
		sequence[anchor_start+2]=(sequence[anchor_start+2]|
				(((((1ULL<<s2)-1)&y)<<(32-s2))&(0xffffffff)));
		index=index+valuewidth;
	}

	/*
	integer anchor=(index>>5);
	integer overloop=((anchor+2)<<5)-index-valuewidth;
	y=(y<<overloop);
	sequence[anchor]=(sequence[anchor]|(y>>32));
	sequence[anchor+1]=(sequence[anchor+1]|(y&0xffffffff));
	index=index+valuewidth;
	*/

}
void GAM_Phi::Test ()
{
	integer i=0;
	integer k=0;
	for(i=0;i<n;i++)
	{
		if(phivalue[i]!=GetValue(i))
			k++;
	}
	if(k==0)
		cout<<"Phi is right"<<endl;
	else
		cout<<"Phi is Sorry"<<endl;
	
}

integer GAM_Phi::blogsize(integer x) 
{
	
	integer len=0;
	while(x>0)
	{
		x=x>>1;
		len++;
	}
	return len;
}

integer GAM_Phi::GetValue(integer index)
{

	integer base=samples->GetValue (index/b);
	integer overloop=index%b;
	integer x=0;
	integer d=0;
	integer position=superoffset[index/a]+offsects->GetValue (index/b);
	integer i=0;
	integer p=0;
	integer num=0;

	while(i<overloop){
		p=this->GetBits (position,16);
		num=this->decodevaluenum[p];
		if(num==0){
			this->Decodegamma(position,d);
			x=(x+d)%n;
			i++;
		}
		else{
			if(i+num>overloop)
				break;
			i=i+num;
			position=position+this->decodebitsnum [p];
			x=(x+this->decoderesult [p])%n;
		}
	}
	if(num==16){
		x=(x+overloop-i)%n;
		return (base+x)%n;
	}
	for(;i<overloop;i++){
		this->Decodegamma (position,d);
		x=x+d;
		x=x%n;
	}
	return (base+x)%n;
}

void GAM_Phi::SamplingAndCoding(parmaters *csa)
{
//	cout<<"before phi array"<<endl;
	integer i,j;
	phivalue=new integer[n];
	integer *temp=new integer[csa->alphabetsize +1];
	for(integer i=0;i<csa->alphabetsize +1;i++)
		temp[i]=csa->start [i];
	integer index=temp[csa->code[csa->lastchar]];
	temp [csa->code [csa->lastchar ]]++;
	integer h=0;
	unsigned char c=0;
	integer pos=0;
	for(integer i=0;i<n;i++){
		pos=csa->SA [i];
		if(pos==0){
			h=i;
			continue;
		}
		c=csa->T[pos-1];
		phivalue[temp [csa->code [c]]++]=i;
	}
	phivalue[index]=h;

//	cout<<"phi array is ok"<<endl;
    delete [] csa->SA;
	delete [] csa->T;
	csa->SA=NULL;
	csa->T=NULL;
	delete [] temp;

	integer pre=0;
	integer totallen=0;
	integer maxlen=0;
	integer len=0;
	integer x=n/a;
	 
//	cout<<"before space"<<endl;
	integer gap=0;
	for( i=0;i<x+1;i++){
		for( j=i*a;j<(i+1)*a&&j<n;j++){
			if(j%b==0){
				pre=phivalue[j];
				continue;
			}
			gap=phivalue[j]-pre;
			if(gap<0)
				gap=n+gap;
			len=len+blogsize(gap)*2-1;
			pre=phivalue[j];
		}
		if(len>maxlen)
			maxlen=len;

		totallen=totallen+len;
		len=0;
	}
//	cout<<"space is computed"<<endl;
//	cout<<"before coding phi array"<<endl;
	this->lenofsequence =totallen/32+1;
	this->sequence =new u32[lenofsequence];
	for(i=0;i<lenofsequence;i++)
		sequence[i]=0;
	this->offsects =new InArray(n/b+1,blogsize(maxlen));
	samples=new InArray(n/b+1,blogsize(n));  

	this->lenofsuperoffset=n/a+1;
	superoffset=new integer[lenofsuperoffset];

	i=0;
	j=0;
	
	gap=0;
	integer index1=0;
	integer index2=0;
	integer index3=0;
	integer len1=0;
	integer len2=0;
	u64 value=0;

	for(integer i=0;i<n;i++){
		if(i%a==0){
			len2=len1;
			superoffset[index3]=len2;
			index3++;
		}
		if(i%b==0){
			value=phivalue[i];

			this->samples ->SetValue (index1,value);
			index1++;
			this->offsects ->SetValue (index2,len1-len2); 
			index2++;
			pre=phivalue[i];
			continue;
		}
		gap=phivalue[i]-pre;
		if(gap<0)
			gap=n+gap;

		pre=phivalue[i];
		len1=len1+blogsize(gap)*2-1;
		Append(gap);
	}
//	cout<<"coding phi array is ok"<<endl;
}

integer GAM_Phi::GetMemorySize()
{
	return samples->GetMemorySize ()+this->offsects ->GetMemorySize ()
		+this->lenofsequence *4+this->lenofsuperoffset *4;
}

integer GAM_Phi::GetOneBit(integer i) 
{
	integer anchor=i/32;
	integer position=i%32;
	return (sequence [anchor]&1<<(31-position))>>(31-position);
}

void GAM_Phi::InitionalTables() 
{
	integer D=this->tablewidth;
	u16 * R=this->zerostable ;
	for(integer i=0;i<D;i++)
		for(integer j=(1<<i);j<(2<<i);j++)
			R[j]=D-1-i;
	R[0]=D;
	u16 * Rn=this->decodevaluenum;
	u16 * Rb=this->decodebitsnum;
	u16 *Rx=this->decoderesult;
	u32 tablesize=(1<<tablewidth);
	u32 B[3]={0xffffffff,0xffffffff,0xffffffff};
	u32 *temp=this->sequence;
	this->sequence=B;
	integer b=0;
	integer num=0;
	integer x=0;
	integer d=0;
	integer preb=0;

	for(u32 i=0;i<tablesize;i++){
		B[0]=(i<<16);
		b=0;
		num=0;
		x=0;
		d=0;
		while(1){
			this->Decodegamma(b, d);
			if(b>tablewidth)
				break;
			x=x+d;
			num++;
			preb=b;
		}
		Rn[i]=num;
		Rb[i]=preb;
		Rx[i]=x;
	}
	this->sequence=temp;
}

integer  GAM_Phi::Decodegamma(integer & position, integer &x)
{
	integer a=this->ZerosRun (position);
	x=this->GetBits (position,a+1);
	position=position+a+1;
	return 2*a+1;
}

integer GAM_Phi::ZerosRun(integer &position) 
{
	integer y=0;
	integer D=this->tablewidth ;
	integer x=this->GetBits(position,D);
	integer w=y=this->zerostable [x];
	while(y==D){
		position=position+D;
		x=this->GetBits (position,D);
		y=this->zerostable [x];
		w=w+y;
	}
	position=position+y;
	return w;
}

u64 GAM_Phi::GetBits (integer position,integer num)
{
	/*
	u32 anchor=position>>5;
	u64 temp1=sequence[anchor];
	u32 temp2=sequence[anchor+1];
	temp1=(temp1<<32)+temp2;
	integer overloop=((anchor+2)<<5)-position-num;
	return (temp1>>overloop)&((1<<num)-1);
	*/
	integer anchor_start=position>>5;
	integer anchor_end=(position+num)>>5;
	if(anchor_end-anchor_start<2){
		u64 temp1=sequence[anchor_start];
		u32 temp2=sequence[anchor_start+1];
		temp1=(temp1<<32)+temp2;
		integer overloop=((anchor_start+2)<<5)-position-num;
		return (temp1>>overloop)&((1<<num)-1);
	}
	else{
		u64 temp1=sequence[anchor_start+0];
		u64 temp2=sequence[anchor_start+1];
		u64 temp3=sequence[anchor_start+2];
		i32 s1=(anchor_start+1)*32-index;
		i32 s2=num-32-s1;
		return ((temp1<<(32+s2))+(temp2<<s2)+(temp3>>(32-s2)))&((1ULL<<num)-1);
	}
}

//在Phi数组[l,r]范围内，从有往左，找第一个phi值<=pr的索引
integer GAM_Phi::RightBoundary(integer pr,integer l,integer r)
{
	integer ans=0;
	integer SL=this->a;
	integer L=this->b;
	integer lb=(l+L-1)/L;
	integer rb=r/L;
	integer b=lb-1;
	integer m=0;
	integer x=0;
	while(lb<=rb){
		m=(lb+rb)>>1;
		x=this->samples->GetValue(m);
		if(x==pr){
			b=m;
			break;
		}
		if(x<pr){
			b=m;
			lb=m+1;
		}
		else
			rb=m-1;
	}
	if(b<0)
		return 0;
	x=this->samples->GetValue(b);
	ans=l-1;
	if(r>(b+1)*L-1)
		r=(b+1)*L-1;
	m=b*L;
	integer d=0;
	integer position=this->superoffset[m/SL]+this->offsects->GetValue(m/L);
	while(m<l){
		this->Decodegamma(position,d);
		x=(x+d)%n;
		m++;
	}


//work , the lowest one

/*
	while(1)
	{
		if(x>pr)
			break;
		ans=m;
		m++;
		if(m>r)
			break;
		this->Decodegamma(position,d);
		x=x+d;
	}

*/

	//works a little better
	integer p=0;
	integer num=0;
	integer bits=0;
	bool loop=false;
	while(x<=pr && m<r){ 
		loop=true;
		p=this->GetBits(position,16);
		num=this->decodevaluenum[p];
		if(num==0){
			m++;
			bits=this->Decodegamma(position,d);
			x=x+d;
		}
		else{
			m=m+num;
			position=position+this->decodebitsnum[p];
			x=mod(x+this->decoderesult[p]);
		}

	}

	if(loop){
		if(num!=0){
			x=mod(x-this->decoderesult[p]);
			position=position-this->decodebitsnum[p];
			m=m-num;
		}
		else{
			m=m-1;
			x=mod(x-d);
			position=position-bits;
		}
	}
	while(1){
		if(x>pr)
			break;
		ans=m;
		m++;
		if(m>r)
			break;
		Decodegamma(position,d);
		x=x+d;
	}

	return ans;
}

//在Phi数组中的[l,r]范围内，由左到右，找第一个Phi值>=pl的索引。
integer GAM_Phi::LeftBoundary(integer pl,integer l,integer r)
{
	integer ans=0;
	integer SL=this->a;
	integer L=this->b;
	integer lb=(l+L-1)/L;
	integer rb=r/L;
    	integer b=rb+1;
	integer m=0;
	integer x=0;

	while(lb<=rb){
		m=(lb+rb)>>1;
		x=this->samples->GetValue(m);
		if(x==pl){
			b=m;
			break;
		}
		if(x>pl){
			b=m;
			rb=m-1;
		}
		else
			lb=m+1;
	}
	if(b==0)
		return 0;
	x=this->samples->GetValue(b-1);
	if(r>b*L-1)
		r=b*L-1;
	ans=r+1;
	m=(b-1)*L;
	integer position=this->superoffset[m/SL]+this->offsects->GetValue(m/L);
	integer d=0;
	while(m<l){
		this->Decodegamma(position,d);
		x=(x+d)%n;
		m++;
	}

/*
	while(1)
	{
		if(x>=pl)
		{
			ans=m;
			break;
		}
		m++;
		if(m>r)
			break;
		Decodegamma(position,d);
		x=x+d;
	}


*/

	integer p=0;
	integer num=0;
	int bits=0;
	bool loop=false;
	while(x<pl && m<r){
		loop=true;
		p=this->GetBits(position,16);
		num=this->decodevaluenum[p];
		if(num!=0){
			m=m+num;
			position=position+this->decodebitsnum[p];
			x=(x+this->decoderesult[p])%n;

		}
		else{
			m++;
			bits=this->Decodegamma(position,d);
			x=x+d;
		}
	}
	if(loop){
		if(num!=0){
			x=mod(x-this->decoderesult[p]);
			position=position-this->decodebitsnum[p];
			m=m-num;
		}
		else{
			m=m-1;
			x=mod(x-d);
			position=position-bits;
		}
	}

	while(1){
		if(x>=pl){
			ans=m;
			break;
		}
		m++;
		if(m>r)
			break;
		Decodegamma(position,d);
		x=x+d;
	}
	
	return ans;
}

integer GAM_Phi::mod(integer x)
{
	if(x<0)
		return x+n;
	else
		return x%n;
}

integer GAM_Phi::write(savekit & s)
{
	s.writeinteger(n);
	s.writeinteger(a);
	s.writeinteger(b);
	s.writeinteger(alpsize);
	s.writeinteger(tablewidth);

	//superoffset
	s.writeinteger(lenofsuperoffset);
	s.writeintegerarray(superoffset,lenofsuperoffset);

	//offset
	offsects->write(s);

	//samples
	samples->write(s);

	//sequences
	s.writeinteger(lenofsequence);
	s.writeu32array(sequence,lenofsequence);

	return 1;
}

integer GAM_Phi::load(loadkit &s)
{
	s.loadinteger(this->n);
	s.loadinteger(this->a);
	s.loadinteger(this->b);
	s.loadinteger(alpsize);
	s.loadinteger(tablewidth);

	this->zerostable=new u16[1<<tablewidth];
	this->decodevaluenum=new u16[1<<tablewidth];
	this->decodebitsnum=new u16[1<<tablewidth];
	this->decoderesult=new u16[1<<tablewidth];
	this->InitionalTables();

	
	s.loadinteger(lenofsuperoffset);
	superoffset=new integer[lenofsuperoffset];
	s.loadintegerarray(superoffset,lenofsuperoffset);


	offsects=new InArray();
	offsects->load(s);

	samples=new InArray();
	samples->load(s);

	//cout<<"OK"<<endl;

	s.loadinteger(lenofsequence);
	sequence=new u32[lenofsequence];
	s.loadu32array(sequence,lenofsequence);
	return 1;
}

integer * GAM_Phi::GetPhiArray()
{
	index=0;
	integer * phi=new integer[n];
	integer times=n/b+1;
	integer pre=0;
	integer d=0;
	for(integer i=0;i<times;i++){
		pre=phi[i*b]=samples->GetValue(i);
		for(integer j=i*b+1;j<(i+1)*b && j<n;j++){
			this->Decodegamma(index,d);
			pre=phi[j]=(pre+d)%n;
		}
	}
	return phi;
}


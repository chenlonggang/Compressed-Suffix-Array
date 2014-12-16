/*============================================
# Filename: CSA.cpp
# Ver 1.0 2014-06-09
# Copyright (C) 2014 ChenLonggang (chenlonggang.love@163.com)
#
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 or later of the License.
#
# Description: 
=============================================*/
#include<stdlib.h>
#include "CSA.h"
#include<cstdio>
#include<cmath>
#include"divsufsort.h"
#include<iostream>
using namespace std;
#define SearchKind 2

integer CSA::save(const char * indexfile){
	savekit s(indexfile);

	s.writeu64(198809102510);
	s.writeinteger(n);
	s.writeinteger(alphabetsize);

	s.writeinteger(SL);
	s.writeinteger(L);
	s.writeinteger(D);
	s.writeinteger(RD);
	//sal
	SAL->write(s);
 	//ranl
	RankL->write(s);
	//code
	s.writeinteger(256);
	s.writeintegerarray(code,256);
	//start
	s.writeinteger(alphabetsize+1);
	s.writeintegerarray(start,alphabetsize+1);
	//incode
	s.writeinteger(alphabetsize);
	s.writeintegerarray(incode,alphabetsize);

	//phi0
	Phi0->write(s);
	s.close();
	return 0;
}

integer CSA::load(const char * indexfile){
	loadkit s(indexfile);
	u64 magicnum;
	s.loadu64(magicnum);
	if(magicnum!=198809102510)
	{
		cerr<<"Not a CSA file"<<endl;
		exit(0);
	}
	s.loadinteger(this->n);
	s.loadinteger(this->alphabetsize);

	s.loadinteger(SL);
	s.loadinteger(L);
	s.loadinteger(D);
	s.loadinteger(RD);

	SAL=new InArray();
	SAL->load(s);

	RankL=new InArray();
	RankL->load(s);
	//code
	integer len=0;
	s.loadinteger(len);
	this->code=new integer[len];
	s.loadintegerarray(this->code,len);
	//start
	s.loadinteger(len);
	this->start=new integer[len];
	s.loadintegerarray(this->start,len);
	//incode
	s.loadinteger(len);
	this->incode=new integer[len];
	s.loadintegerarray(this->incode,len);

	Phi0=new Phi();
	Phi0->load(s);
	s.close();
	return 0;
}

CSA::CSA(const char * sourcefile,integer L,integer D,integer phitype){
		this->SL=L*18;
		this->L =L;
		this->D =D;
		this->RD=D*16;
		this->code =new integer[256];
		for(integer i=0;i<256;i++)
			code[i]=0;

		this->alphabetsize =0;
		uchar *T=NULL;
		T=GetFile(sourcefile);
		statics(T);
		integer *SA=new integer[n];
		
		divsufsort(T,SA,n);
		
		parmaters p={alphabetsize,n,SL,L,start,lastchar,SA,T,code,phitype};
		CreateSupportStructer(&p);

		if(SA!=NULL)
			delete [] SA;
		if(T!=NULL)
			delete [] T;
//		cout<<"CSA  is done"<<endl;

/*
		integer start=0;
		integer num=n-0;
		integer * phiarray=Phi0->GetPhiArray();
		integer * phipiece=new integer[num];
		Phi0->GetPhiPiece(start,num,phipiece);
		for(integer i=0;i<num;i++)
			if(phiarray[i+start]!=phipiece[i]){
				cout<<"fuck "<<phiarray[i+start]<<" "<<phipiece[i]<<" "<<i<<" "<<i+start<<endl;
				break;
			}
*/
}

integer CSA::getN(){
	return n;
}

integer CSA::getAlphabetsize(){
	return this->alphabetsize;
}

CSA::~CSA(void){
	delete SAL;
	delete RankL;
	delete [] start;
	delete Phi0;
	delete [] code;
	delete [] incode;
}

bool CSA::existential(const char *Pattern){
	integer L=0;
	integer R=0;
	Search2(Pattern,L,R);

	if(R>=L)
		return true;
	else
		return false;
}

void CSA::counting(const char *Pattern,integer &num){
	integer L=0;
	integer R=0;

	Search(Pattern,L,R);
//	cout<<L<<" "<<R<<endl;
	num=R-L+1;
}

void CSA::statics(uchar * T){
	for(integer i=0;i<n;i++)
		code[T[i]]++;

	for(integer i=0;i<256;i++)
		if(code[i]!=0)
			this->alphabetsize ++;
	this->start =new integer[this->alphabetsize +1];
	this->start [this->alphabetsize ]=n;
	this->start [0]=0;
	integer k=1;
	integer pre=0;
	for(integer i=0;i<256;i++)
		if(code[i]!=0)
		{
			start[k]=pre+code[i];
			pre=start[k];
			k++;
		}
	this->incode =new integer[this->alphabetsize];
	k=0;
	for(integer i=0;i<256;i++)
		if(code[i]!=0)
		{
			code[i]=k;
			incode[k]=i;
			k++;
		}
		else
			code[i]=-1;
	lastchar=T[n-1];

}

uchar* CSA::GetFile(const char * filename){
	FILE *fp=fopen(filename,"r+");
	if(fp==NULL)
	{
		cout<<"Be sure that the file is available"<<endl;
		exit(0);
	}
	fseek(fp , 0, SEEK_END);       //置读写指针为文件末尾
    this->n = ftell(fp);           //返回读写指针偏移量
	uchar * T=new uchar[n];
	fseek(fp , 0, SEEK_SET);

	integer e=0;
	integer num=0;
	while((e=fread(T+num,sizeof(uchar),n-num,fp))!=0)
		num=num+e;
	if(num!=n)
		exit(0);
	fclose(fp);
	return T;
}

integer CSA::sizeInByte(){
	return (SAL->GetMemorySize ()+Phi0->Size()+RankL->GetMemorySize ());
}

integer CSA::sizeInByteForCount(){
	return Phi0->Size();
}

double CSA::compressRatio(){
	return sizeInByte()/(1.0*n);
}

double CSA::compressRatioForCount(){
	return sizeInByteForCount()/(1.0*n);
}

void CSA::CreateSupportStructer(parmaters *csa){
	integer i=0;
	integer j=0;
	integer step1=D;
	integer step2=RD;
	SAL=new InArray(n/step1+1,blog(n));
	RankL=new InArray(n/step2+1,blog(n));
	for(i=0,j=0;i<n;i=i+step1,j++){
		SAL->SetValue (j,csa->SA[i]);
	}

	for(i=0;i<n;i++){
		if(csa->SA[i]%step2==0)
			RankL->SetValue (csa->SA[i]/step2,i);
	}
	Phi0=new Phi(csa);
}

void CSA::Search2(const char *Pattern, integer &L, integer &R){
	integer len=strlen(Pattern);
	if(len==0){
		L=1;
		R=0;
		return;
	}
	unsigned char c=Pattern[len-1];
	integer coding=code[c];
	if(coding <0||coding>alphabetsize-1){
		L=1;
		R=0;
		return ;
	}
	integer Left=start[coding];
	integer Right=start[coding+1]-1;
	integer l0=0;
	integer r0=0;

	for(integer i=len-2;i>=0;i--){
		c=Pattern[i];
		coding=code[c];
		if(coding<0){
			Left=1;
			Right=0;
			break;
		}
		l0=start[coding];
		r0=start[coding+1]-1;
		
		Right=Phi0->RightBoundary(Right,l0,r0);
		Left=Phi0->LeftBoundary(Left,l0,r0);
		if(Left>Right){
			Left=1;
			Right=0;
			break;
		}
	}
	L=Left;
	R=Right;
	return ;
}

void CSA::Search(const char *Pattern, integer &L, integer &R){
	integer templeft;
	integer tempright;
	integer jj;
	integer mleft;
	integer mright;
	integer middle;
	integer Right;
	integer left;
	integer right;
	integer Left;
	integer i;
	integer len=strlen(Pattern);
	unsigned char c=Pattern[len-1];
	integer coding=code[c];
	if(coding>alphabetsize-1){
		L=1;
		R=0;
		return ;
	}

	Left=start[coding];
	Right=start[coding+1]-1;
	for(i=len-2;i>=0;i--){
		c=Pattern[i];
		coding =code[c];
		if(coding>alphabetsize-1){
			L=1;
			R=0;
			return ;
		}
		left=start[coding];
		right=start[coding+1]-1;
		if(coding==code[lastchar])
			left=left+1;
		if(left>right || Phi0->GetValue (left)>Right || Phi0->GetValue (right)<Left){
 			L=1;
			R=0;
			return ;
		}
		else{
			mleft=left;
			mright=right;
			while(mright-mleft>1){
				middle=(mleft+mright)/2;
				jj=Phi0->GetValue (middle);
				if(jj<Left)
					mleft=middle;
				else
					mright=middle;
			}
			jj=Phi0->GetValue (mleft);
			if(jj>=Left && jj<=Right)
				templeft=mleft;
			else
				templeft=mright;
			mleft=left;
			mright=right;
			while(mright-mleft>1){
				middle=(mleft+mright)/2;
				jj=Phi0->GetValue (middle);
				if(jj>Right)
					mright=middle;
				else
					mleft=middle;
			}
			jj=Phi0->GetValue (mright);
			if(jj>=Left && jj<=Right)
				tempright=mright;
			else
				tempright=mleft;
			Left=templeft;
			Right=tempright;
		    if(Left>Right){
				L=1;
				R=0;
				return;
			}
		}
	}
	if(Left>Right)
		L=1,R=0;
	else
		L=Left,R=Right;
}

integer CSA::lookup(integer i){
	integer D=this->D;
	integer step=0;
	while(i%D!=0){
		i=Phi0->GetValue(i);
		step++;
	}
	i=i/D;
	return (n+SAL->GetValue (i)-step)%n;
}
/*
void CSA::SelfTesting()
{
	cout<<"CSA is Selftesting..."<<endl;
	integer k=0;
	integer value=0;
	for(integer i=0;i<n;i++)
	{
		value=lookup(i);
		if(SA[i]!=value)
			k++;
	}
	if(k!=0)

		cout<<"CSA is wrong     "<<k<<endl;
	else
		cout<<"CSA is right"<<endl;
}
*/

//得到位置i的排名
integer CSA::Inverse(integer i){
	integer RD=this->RD ;
	integer anchor=i/RD;
	integer p=anchor*RD;
	integer sa=this->RankL ->GetValue (anchor);
	while(p<i){
		sa=Phi0->GetValue(sa);
		p++;
	}
	return sa;
}

uchar * CSA::extracting(integer i, integer len){
	if(i+len>n){
		cout<<"overshoot the length of the Doc."<<endl;
		return NULL;
	}
	uchar *p=new uchar[len+1];
	p[len]='\0';
	integer k=0;
	i=this->Inverse (i);
	for(integer j=0;j<len;j++){
		k=this->Phi_list (i);
		p[j]=this->Character (k);
		i=Phi0->GetValue(i);
	}
	return p;
}

integer CSA::Phi_list(integer i){
	integer l=0;
	integer r=this->alphabetsize ;
	integer m=0;
	while(l<r){
		m=(l+r)/2;
		if(start[m]<=i)
			l=m+1;
		else
			r=m;
	}
	return r-1;
}

integer CSA::Character(integer i){
	return incode[i];
}

integer CSA::blog(integer x){
	integer ans=0;
	while(x>0){
		ans++;
		x=x>>1;
	}
	return ans;
}

integer * CSA::locating(const char *Pattern, integer &num){
	integer L=0;
	integer R=0;
	this->Search2(Pattern,L,R);
	num=R-L+1;
	if(L>R)
		return NULL ;
	integer *pos=new integer[num];
	Enumerative1(L,R,pos);
	return pos;
}

void CSA::Enumerative2(integer L,integer  R, integer *&pos){
	integer D=this->D;
	InArray *SAL=this->SAL ;
	integer * distance=new integer[R-L+1];
	integer *pred=new integer[R-L+1];
	integer f=0;
	integer step=0;
	integer q=0;
	integer s=0;
	integer i=0;
	for(integer i=0;i<R-L+1;i++){
		pos[i]=0;
		pred[i]=-1;
	}
	for(integer j=L;j<=R;j++){
		f=0;
		i=j;
		step=0;
		while(i%D!=0){
			i=this->Phi0->GetValue(i);
			step++;
			if(L<=i&&i<=R){
				distance[j-L]=step;
				pred[i-L]=j;
				f=1;
				break;
			}
		}
		if(f==0){
			i=i/D;
			pos[j-L]=SAL->GetValue(i)-step;
		}
	}
	integer fu=0;
	for(integer j=L;j<=R;j++){
		if(pos[j-L]!=0){
			q=j;
			while(pred[q-L]!=-1){
				fu++;
				s=pos[q-L];
				i=pred[q-L];
				step=distance[i-L];
				pos[i-L]=s-step;
				pred[q-L]=-1;
				q=i;
			}
		}
	}
	delete [] pred;
	delete [] distance;
}

void CSA::Enumerative1(integer L,integer R,integer *&pos){
	integer i;
	for(i=L;i<=R;i++){
		pos[i-L]=lookup (i);
	}
}

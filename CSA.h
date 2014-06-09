/*============================================
# Filename: CSA.h
# Ver 1.0 2014-06-09
# Copyright (C) 2014 ChenLonggang (chenlonggang.love@163.com)
#
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 or later of the License.
#
# Description: 
  compressed suffix array,a self-index structure.
  CSA(filename) will build a csa-index for a document nameed filename.
                it's useful for text data,but not for multi-media data.
=============================================*/
#pragma once
#include"savekit.h"
#include"loadkit.h"
#include"InArray.h"
#include<string.h>
#include"Phi.h"
#include"BaseClass.h"
#include"parmaters.h"
#include<fstream>
#include<iostream>
using namespace std;
class CSA
{
public:
	//参数n表示大小,SA表示后缀数组，sourcefile表示原文件，
	//用来记录运行时间，参数L表示Psi的采样步长，D表示SA的采样步长
	CSA(){};
	CSA(const char * sourcefile,i32 L=128,i32 D=32,i32 phitype=0);
	bool Existential(const char * Pattern);
	void Counting(const char * Pattern,int &num);
	void Locating(const char * Pattern,i32 &num,i32 *&pos);
	
	i32 Save(const char * indexfile);
	i32 Load(const char * indexfile);
	
	double Size();
	void Decompress(i32 i,i32 len,unsigned char *p);
	~CSA(void);

	i32 Getalphabetsize();
	i32 GetN();
private:
	CSA(const CSA &);
	CSA& operator=(const CSA & right);
	
	void CreateSupportStructer(parmaters * csa);
	
	void Enumerative1(i32 l,i32 r,i32 *&pos);
	void Enumerative2(i32 l,i32 r,i32 *&pos);
	
	i32 Inverse(i32 i);
	i32 Phi_list(i32 i);//返回排名为i的后缀属于哪个list，这样借助于incode数组就可以恢复出排名为i的后缀的首字母了。
	i32 Character(i32 i);
	
	i32 lookup(i32 i);
	void Search(const char * Pattern,i32 & L, i32 &R);
	void Search2(const char * Pattern,i32 &L,i32 &R);
	
	uchar * Getfile(const char * filename);
	void statics(uchar *T);
	i32 blog(i32 x);
	
	i32 SL,L,D,RD;
	InArray  * SAL;
	InArray * RankL;
	i32 n; 
	i32 * code;
	i32 * incode;//记录编码为i的字符的ASSIC码
	i32 alphabetsize;
	i32 * start;
	uchar lastchar;
	Phi * Phi0;
};

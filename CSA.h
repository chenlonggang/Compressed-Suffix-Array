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
	
	CSA(){};
	CSA(const char * sourcefile,integer L=128,integer D=32,integer phitype=0);
	~CSA();

	bool existential(const char * Pattern);
	void counting(const char * Pattern,integer &num);
	integer * locating(const char * Pattern,integer &num);
	uchar * extracting(integer start,integer len);
	
	integer save(const char * indexfile);
	integer load(const char * indexfile);

	integer getAlphabetsize();
	integer getN();
	integer sizeInByte();
	integer sizeInByteForCount();
	double compressRatio();
	double compressRatioForCount();
private:
	CSA(const CSA &);
	CSA& operator=(const CSA & right);
	
	void CreateSupportStructer(parmaters * csa);
	
	void Enumerative1(integer l,integer r,integer *&pos);
	void Enumerative2(integer l,integer r,integer *&pos);
	
	integer Inverse(integer i);
	integer Phi_list(integer i);
	integer Character(integer i);
	
	integer lookup(integer i);
	void Search(const char * Pattern,integer & L, integer &R);
	void Search2(const char * Pattern,integer &L,integer &R);
	
	uchar * GetFile(const char * filename);
	void statics(uchar *T);
	integer blog(integer x);
	
	integer SL,L,D,RD;
	InArray  * SAL;
	InArray * RankL;
	integer n; 
	integer * code;
	integer * incode;
	integer alphabetsize;
	integer * start;
	uchar lastchar;
	Phi * Phi0;
};

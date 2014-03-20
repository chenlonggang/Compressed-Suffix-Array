#pragma once
#include"savekit.h"
#include"loadkit.h"
#include"InArray.h"
#include<string.h>
#include"Phi.h"
#include"BaseClass.h"
#include"parmaters.h"
#include<stdlib.h>
#include<fstream>
#include<iostream>
using namespace std;
class CSA
{
public:
	
	CSA(const char * sourcefile,i32 L=128,i32 D=32,i32 phitype=0);
	~CSA(void);
	bool Existential(const char * Pattern);
	void Counting(const char * Pattern,i32 &num);
	void Locating(const char * Pattern,i32 &num,i32 *&pos);
	void Decompress(i32 i,i32 len,unsigned char *p);


	i32 Save(const char * indexfile);
	double Size();
	i32 GetGammaSize();
	i32 Getalphabetsize();
	i32 GetN();
private:
	/*在构造中调用，与Save对应，将保存的CSA加载到内从*/
	CSA(const CSA &);
	CSA& operator=(const CSA & right);
	void Load(loadkit &s);
	void statics();
	void CreateSupportStructer();
	i32 lookup(i32 i);
	void Enumerative1(i32 l,i32 r,i32 *&pos);
	void Enumerative2(i32 l,i32 r,i32 *&pos);
	i32 SL,L,D,RD;
	i32 blog(i32 x);
	i32 Inverse(i32 i);
	i32 Phi_list(i32 i);//返回排名为i的后缀属于哪个list，这样借助于incode数组就可以恢复出排名为i的后缀的首字母了。
	
	i32 Character(i32 i);
	void SelfTesting();
	void Getfile(const char * filename);
	void Search(const char * Pattern,i32 & L, i32 &R);
	void Search2(const char * Pattern,i32 &L,i32 &R);
	InArray  * SAL;
	InArray * RankL;
	i32 n; 
	unsigned char * T;
	i32 * SA;
	i32 * code;
	i32 * incode;//记录编码为i的字符的ASSIC码
	i32 alphabetsize;
	i32 * start;
//	i32 level1;
	//i32 level2;
	unsigned char lastchar;

	//Dictionary * D0; 
	//Dictionary * Dm;
	//BitSequence * D0;
	//BitSequence *Dm;
	//Phi * Phi0;
	Phi * Phi0;
	i32 phitype;
	//InArray *Phim;  
	//Phi * Phim;
	//关于Phim不使用Phi结构，而用InArray直接存储的说明
	//中间层的连接表的表头过长，比如l“=3时，表头长度为8个字符，所有组合数为：字符表大小的8次方
	//假设所有组合都出现了，则在表头要加的位数过多，没有实用价值。

};

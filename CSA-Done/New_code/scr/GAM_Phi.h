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

		i32 GetValue(i32 i);
		i32 RightBoundary(i32 r,i32 L,i32 R);
		i32 LeftBoundary(i32 l,i32 L,i32 R);
		i32 GetMemorySize();
		i32 write(savekit & s);
		i32 load(loadkit & s);
	private:
		i32 mod(i32);
		i32 GetOneBit(i32 i);
		i32 a;
		i32 b;
		i32 alpsize;
		i32 tablewidth;
		u16 *zerostable;
		u16 *decodevaluenum;
		u16 *decodebitsnum;
		u16 *decoderesult;
		i32  * superoffset;
		InArray* samples;
		InArray * offsects;
		u32 * sequence;
		i32 index;
		i32 lenofsequence;
		i32 lenofsuperoffset;
		i32 n;
		i32 * phivalue;
		i32 blogsize(i32 x);
		void SamplingAndCoding(parmaters * csa);//"接头处"用gap+n编码
		void Test();
		void Append(i32 x);
		i32 Decodegamma(i32& position,i32 & x);
		void InitionalTables();
		i32 ZerosRun(i32 &position );//从position开始，获取0连续出现的次数.
		u64 GetBits(i32 position ,i32 x);//从position开始，读取x位数据
};
#endif

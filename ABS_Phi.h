#ifndef _ABS_PHI
#define _ABS_PHI
#include"BaseClass.h"
#include"savekit.h"
#include"loadkit.h"
class ABS_Phi
{
	public:
		virtual ~ABS_Phi(){};
		virtual i32 * GetPhiArray()=0;
		virtual i32 GetValue(i32 i)=0;
		virtual i32 RightBoundary(i32 r,i32 L,i32 R)=0;
		virtual i32 LeftBoundary(i32 l,i32 L,i32 R)=0;
		virtual i32 GetMemorySize()=0;
		virtual i32 write(savekit & s)=0;
		virtual i32 load(loadkit &s)=0;

		
};
#endif

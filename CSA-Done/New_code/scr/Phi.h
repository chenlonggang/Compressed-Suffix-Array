#include"BaseClass.h"
#include"parmaters.h"
#include"savekit.h"
#include"loadkit.h"
#include"ABS_Phi.h"
#include"GAM_Phi.h"
#include"GRL_Phi.h"
class Phi
{
	public:
		Phi(parmaters *csa);
		Phi();//返回一个Phi的空壳
		~Phi();
		i32 GetValue(i32 i);
		i32 RightBoundary(i32 r,i32 L,i32 R);
		i32 LeftBoundary(i32 l,i32 L,i32 R);
		i32 Size();
		i32 write(savekit & s);
		i32 load(loadkit &s);
	private:
		ABS_Phi *phi;
		i32 type;

};

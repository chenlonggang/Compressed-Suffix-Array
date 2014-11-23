#include<sys/time.h>
#include"CSA.h"
int main(){
	struct timeval start;
	struct timeval end;
	gettimeofday(&start,NULL);
	CSA csa("CSA.cpp");
	gettimeofday(&end,NULL);
	cout<<"build time "<<(end.tv_sec-start.tv_sec)<<" "<<endl;
	return 0;
}

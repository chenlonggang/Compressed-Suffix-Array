#include<stdlib.h>
#include<sys/time.h>
#include<string.h>
#include"CSA.h"
#include<ctime>
#include<fstream>
#include<iostream>
using namespace std;
#define times 10000
int main(int argc, char ** argv){
	ifstream sources("path.txt");
	char  sourcefile[100];
	long filenum=atoi(argv[1]);
	long currentfile=0;
	struct timeval start;
	struct timeval end;
	for(currentfile=0;currentfile < filenum;currentfile++){
		sources.getline(sourcefile,200);
		cout<<"file:	"<<sourcefile<<endl;
		
		unsigned char * p[times];
		gettimeofday(&start,NULL);
		CSA csa(sourcefile);
		gettimeofday(&end,NULL);
		cout<<"	build-time: "<<(end.tv_sec-start.tv_sec)*1000000+end.tv_usec - start.tv_usec<<endl;

		long n= csa.getN();
		for(long i=0;i<times;i++){
			long x=rand()%(n-50);
			p[i]=csa.extracting(x,20);
		}

		//for counting
		long num=0;
		long sumnum=0;
		gettimeofday(&start,NULL);
		for(long i=0;i<times;i++){
			csa.counting((const char *)p[i],num);
	//		sumnum=sumnum+nun;
		}
	//	cout<<sumnum<<endl;
		gettimeofday(&end,NULL);
		double count_time = (end.tv_sec-start.tv_sec)*1000000.0+end.tv_usec - start.tv_usec;
		cout<<"	count-time: "<<(count_time)/times<<endl;
		
		cout<<"	come-ratio: "<<csa.sizeInByteForCount()/(n*1.0)<<endl;
		for(long i=0;i<times;i++)
			delete [] p[i];
	}
	return 0;
}



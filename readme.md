#Compressed-Suffix-Array(CSA)

##What is it?
	 CSA is a Succinct Data Structure(SDS), SDS can represent an object as whole object
	 in logic,in space close to information-theoretic lower bound of the object while 
	 supporting operations of the original object efficiently.
	 CSA both is implicit expression about SA(suffix array),has the ability of fast pattern maching,
	 and nedds little space.you can build a CSA-index for a document,and the you 
	 have mainly there operations in your hands:
	 counting: count how many a pattern occurs in the document.
	 locating: locate all the position where the pattern occurs.
	 decompress: decompress a piece of the document.
## How to use it?
###just for fun
	 step 1:download it or clone it
	 step 2:make
	 step 3:run my_csa
###build your own program
	 step 1:download or clone it
	 step 2:make
	 step 3:include CSA.h
	 step 4: g++ your_program.cpp -o xx -csa.a

###example
	```cpp
	#include"CSA.h"
	#include<iostream>
	using namespace std;
	int main()
	{
		CSA csa("filename");
		int num;
		csa.Counting("the",num);
		cout<<"pattern the occs "<<num<<" times"<<endl;
		int *pos;
		csa.Locating("love",num.pos);
		cout<<"pattern love occs "<<num<<" times"<<endl;
		cout<<"all the positions are:";
		for(int i=0;i<num;i++)
			cout<<pos[i]<<endl;
		delete [] pos;//it's your duty to delete pos.
		pos=NULL;

		char * sequence;
		int start=0;
		int len =20;
		csa.Extracting(start,len,sequence);
		cout<<"T[start...start+len-1] is "<<sequence<<endl;
		delete [] sequence;//it's your duty to delete sequence.
    	sequence =NULL;

		csa.Save("index.fm");//serialize the fm object to file index.fm
		CSA csa2;
		csa2->Load("index.fm");//restore the fm object from file index.fm

		return 0;
	}
	```

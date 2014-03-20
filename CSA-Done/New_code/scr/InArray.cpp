#include "InArray.h"
#include<iostream>
using namespace std;


InArray::~InArray(void)
{
	delete [] data;
}
void InArray::Initional (i32 num)
{
	for(i32 i=0;i<num;i++)
		data[i]=0;
}
InArray::InArray()
{
}
//构造函数,表示有len个“整数”需要存储，每个整数的位宽为size。
InArray::InArray(i32 len, i32 size) 
{
	if(len<0||size<=0)
		cout<<"InArray构造参数输入错误"<<endl;
	else
	{
	//	cout<<"InArray"<<endl;
		this->datanum =len;
		this->datawidth =size;
	    i64 totlesize=datanum*datawidth;
	//	 cout<<totlesize<<endl;
		if(totlesize%32==0)
			totlesize=totlesize/32;
		else
			totlesize=(totlesize/32)+1;
	//	cout<<"InArray"<<endl;
	//	cout<<len<<"  "<<size<<endl;
	//	cout<<totlesize<<endl;
		this->data =new u32[totlesize];
		//cout<<"InArray"<<endl;
		//for(i32 i=0;i<totlesize;i++)
		//	data[i]=0;
		
		Initional(totlesize);
		mask=((1<<datawidth)-1);
	}
}

//数组中下标为index的位置设置值为value
void InArray::SetValue (i32 index, i32 v)
{

	if(index>datanum-1|| index<0)
	{
		cout<<"下标越界"<<endl;
		return ;
	}
	else if(v>((1<<datawidth)-1))
	{
		cout<<"值越界"<<endl;
		return ;
	}
	else
	{
		 //i64  newindex=index;
		// i64 newdatawidth=datawidth;
		i32 anchor=(index*datawidth)/32;
		i32 position=(index*datawidth)%32;
		u32 value=v;
		if(position+datawidth-1<32)
		{
			u32 temp1=(1<<(position))-1;
			u32 temp2=(1<<(32-(position+datawidth)))-1;
			temp2=temp2<<(position+datawidth);
			temp2=temp2 | temp1;
			data[anchor]=data[anchor] & temp2; 

			value=value<<position;
			temp2=~temp2;
			temp2=value & temp2;
			data[anchor]=temp2 | data[anchor];
		}
		else
		{
			u32 temp1=(1<<position)-1;
			data[anchor]=data[anchor] & temp1;
			temp1=~temp1;
			u32 x=(1<<(32-position))-1;
			x=x & value;
			x=x<<position;
			temp1=temp1 & x;
			data[anchor]=data[anchor] | temp1;

			value=value>>(32-position);
			u32 temp2=(1<<(32-(datawidth-(32-position))))-1;
			temp2=temp2<<(datawidth-(32-position));
			data[anchor+1]=data[anchor+1] & temp2;
			temp2=~temp2;
			temp2=temp2 & value;
			data[anchor+1] =data[anchor+1] | temp2;
		}
	}


}

i32 InArray::GetNum ()
{
	return datanum;
}
i32 InArray::GetMemorySize() 
{
	return (datanum*datawidth)/8;
}

		
i32 InArray::GetDataWidth() 
{
	return datawidth;
}

i32 InArray::GetValue(i32 index)
{

/*	
	if(index>datanum-1||index<0)
	{
		cout<<"下标越界"<<endl;
		return -1;
	}
	
	else
	{
		//i64 newindex=index;
		//i64 newdatawidth=datawidth;
		i32 anchor=(index*datawidth)>>5;
		i32 position=(index*datawidth)&31;
		if(position+datawidth-1<32)
		{
		
		//	u32 temp=(1<<(datawidth))-1;
		//  temp=temp<<(position);
		//	temp=temp & data[anchor];
		//  value=temp>>(position);
		
			//u32 tep=0;
			return (data[anchor]>>position)&mask;
		}
		else
		{
			
			//u32 temp1=(1<<(32-position))-1;
			//temp1=temp1<<(position);
			//temp1=temp1 & data[anchor];
			//temp1=temp1>>(position);
			
		//	u32 temp1=(data[anchor]&(~((1<<position)-1)))>>position;
		//	u32 temp2=(1<<(datawidth-(32-position)))-1;
		//	temp2=temp2 & data[anchor+1];
		//	temp2=temp2<<(32-position);
		//	value=temp1| temp2;
		
		u64 temp=data[anchor+1];
		u64 v=data[anchor];
		return (((temp<<32)|v)>>position)&mask;
			
		}
	}
	
*/	
	u32 anchor=(index*datawidth)>>5;
	u32 temp2=data[anchor];
	u64 temp1=data[anchor+1];
	u32 overloop=index*datawidth-(anchor<<5);
	temp1=(temp1<<32)+temp2;
	return (temp1>>overloop)&mask;

}
i32 InArray::write(savekit & s)
{
	s.writei32(datanum);
	s.writei64(datawidth);
	i64 len=(datanum*datawidth);
	if(len%32==0)
		len=len/32;
	else
		len=len/32+1;

	s.writei32(len);
	s.writeu32array(data,len);
	return 1;
}
i32 InArray::load(loadkit & s)
{
	s.loadi32(datanum);
	s.loadi64(datawidth);
	i32 len=0;
	s.loadi32(len);
	data=new u32[len];
	s.loadu32array(data,len);
	mask=((1<<datawidth)-1);
	return 1;
}




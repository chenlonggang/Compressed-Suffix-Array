#include<math.h>
#include<ctime>
#include "GAM_Phi.h"
#include<fstream>
#include<iostream>
using namespace std;
GAM_Phi::GAM_Phi()
{
}
GAM_Phi::GAM_Phi(parmaters *csa)
{
	this->tablewidth=16;
	this->zerostable=new u16[1<<tablewidth];
	this->decodevaluenum=new u16[1<<tablewidth];
	this->decodebitsnum=new u16[1<<tablewidth];
	this->decoderesult=new u16[1<<tablewidth];
   
	this->alpsize =csa->alphabetsize ;
	this->n=csa->n;
	this->a=csa->SL;
	this->b=csa->L;
	this->index=0;

	this->InitionalTables();
	this->SamplingAndCoding (csa);
//	Test();

	delete [] phivalue;
}



GAM_Phi::~GAM_Phi(void)
{
	delete [] superoffset;
	delete samples;
	delete offsects;
	delete [] sequence;

	delete [] zerostable;
	delete [] decodevaluenum;
	delete [] decodebitsnum;
	delete [] decoderesult;
}
void  GAM_Phi::Append (i32 x)
{
	u64 y=x;
	i32 zeronums=blogsize(y)-1;
	index=index+zeronums;
	i32 valuewidth=zeronums+1;
	i32 anchor=(index>>5);
	i32 overloop=((anchor+2)<<5)-index-valuewidth;
	y=(y<<overloop);
	sequence[anchor]=(sequence[anchor]|(y>>32));
	sequence[anchor+1]=(sequence[anchor+1]|(y&0xffffffff));
	index=index+valuewidth;

}
void GAM_Phi::Test ()
{
	i32 i=0;
	i32 k=0;
	for(i=0;i<n;i++)
	{
		if(phivalue[i]!=GetValue(i))
			k++;
	}
	if(k==0)
		cout<<"Phi is right"<<endl;
	else
		cout<<"Phi is Sorry"<<endl;
	
}
i32 GAM_Phi::blogsize(i32 x) 
{
	
	i32 len=0;
	while(x>0)
	{
		x=x>>1;
		len++;
	}
	return len;
}


i32 GAM_Phi::GetValue(i32 index)
{

	i32 base=samples->GetValue (index/b);
	i32 overloop=index%b;
	i32 x=0;
	i32 d=0;
	i32 position=superoffset[index/a]+offsects->GetValue (index/b);
	i32 i=0;
	i32 p=0;
	i32 num=0;

	while(i<overloop)
	{
		p=this->GetBits (position,16);
		num=this->decodevaluenum[p];
		if(num==0)
		{
			this->Decodegamma(position,d);
			x=(x+d)%n;
			i++;
		}
		else
		{
			if(i+num>overloop)
				break;
			i=i+num;
			position=position+this->decodebitsnum [p];
			x=(x+this->decoderesult [p])%n;
		}
	}
	if(num==16)
	{
		x=(x+overloop-i)%n;
		return (base+x)%n;
	}
	for(;i<overloop;i++)
	{
		this->Decodegamma (position,d);
		x=x+d;
		x=x%n;
	}
	return (base+x)%n;
}
void GAM_Phi::SamplingAndCoding(parmaters *csa)
{
	i32 i,j;

	
	phivalue=new i32[n];
	i32 *temp=new i32[csa->alphabetsize +1];
	for(i32 i=0;i<csa->alphabetsize +1;i++)
		temp[i]=csa->start [i];
	i32 index=temp[csa->code[csa->lastchar]];
	temp [csa->code [csa->lastchar ]]++;
	i32 h=0;
	unsigned char c=0;
	i32 pos=0;
	for(i32 i=0;i<n;i++)
	{
		 pos=csa->SA [i];
		if(pos==0)
		{
			h=i;
			continue;
		}
		c=csa->T[pos-1];
		phivalue[temp [csa->code [c]]++]=i;
	}
	phivalue[index]=h;
    delete [] csa->SA;
	delete [] csa->T;
	csa->SA=NULL;
	csa->T=NULL;

	delete [] temp;

	i32 pre=0;
	i32 totallen=0;
	i32 maxlen=0;
	i32 len=0;
	i32 x=n/a;
	 
	i32 gap=0;
	for( i=0;i<x+1;i++)
	{
		for( j=i*a;j<(i+1)*a&&j<n;j++)
		{
			if(j%b==0)
			{
				pre=phivalue[j];
				continue;
			}
			gap=phivalue[j]-pre;
			if(gap<0)
				gap=n+gap;
			len=len+blogsize(gap)*2-1;
			pre=phivalue[j];
		}
		if(len>maxlen)
			maxlen=len;

		totallen=totallen+len;
		len=0;
	}

	this->lenofsequence =totallen/32+1;
	this->sequence =new u32[lenofsequence];
	for(i=0;i<lenofsequence;i++)
		sequence[i]=0;
	this->offsects =new InArray(n/b+1,blogsize(maxlen));
	samples=new InArray(n/b+1,blogsize(n));  

	this->lenofsuperoffset=n/a+1;
	superoffset=new i32[lenofsuperoffset];

	i=0;
	j=0;
	
	gap=0;
	i32 index1=0;
	i32 index2=0;
	i32 index3=0;
	i32 len1=0;
	i32 len2=0;
	u64 value=0;

	for(i32 i=0;i<n;i++)
	{
		if(i%a==0)
		{
			len2=len1;
			superoffset[index3]=len2;
			index3++;
		}
		if(i%b==0)
		{
			value=phivalue[i];

			this->samples ->SetValue (index1,value);
			index1++;
			this->offsects ->SetValue (index2,len1-len2); 
			index2++;
			pre=phivalue[i];
			continue;
		}
		gap=phivalue[i]-pre;
		if(gap<0)
			gap=n+gap;

		pre=phivalue[i];
		len1=len1+blogsize(gap)*2-1;
		Append(gap);
	}
}
i32 GAM_Phi::GetMemorySize()
{
	return samples->GetMemorySize ()+this->offsects ->GetMemorySize ()+this->lenofsequence *4+this->lenofsuperoffset *4;
}
i32 GAM_Phi::GetOneBit(i32 i) 
{
	i32 anchor=i/32;
	i32 position=i%32;
	return (sequence [anchor]&1<<(31-position))>>(31-position);
}
void GAM_Phi::InitionalTables() 
{
	i32 D=this->tablewidth;
	u16 * R=this->zerostable ;
	for(i32 i=0;i<D;i++)
		for(i32 j=(1<<i);j<(2<<i);j++)
			R[j]=D-1-i;
	R[0]=D;
	u16 * Rn=this->decodevaluenum;
	u16 * Rb=this->decodebitsnum;
	u16 *Rx=this->decoderesult;
	u32 tablesize=(1<<tablewidth);
	u32 B[3]={0xffffffff,0xffffffff,0xffffffff};
	u32 *temp=this->sequence;
	this->sequence=B;
	i32 b=0;
	i32 num=0;
	i32 x=0;
	i32 d=0;
	i32 preb=0;

	for(u32 i=0;i<tablesize;i++)
	{
		B[0]=(i<<16);
		b=0;
		num=0;
		x=0;
		d=0;
		while(1)
		{
			this->Decodegamma(b, d);
			if(b>tablewidth)
				break;
			x=x+d;
			num++;
			preb=b;
		}
		Rn[i]=num;
		Rb[i]=preb;
		Rx[i]=x;
	}
	this->sequence=temp;
}

int  GAM_Phi::Decodegamma(i32 & position, i32 &x)
{
/*
	i32 zeronums=0;
	i32 bit=0;
	while((sequence[position/16]&(1<<(15-position%16)))==0)
	{
		zeronums++;
		position++;
	}
	for(i32 i=0;i<zeronums+1;i++)
	{
		//x=x+((sequence[position/16]&(1<<(15-position%16)))<<(15-position%1    6))*(1<<i);
		bit=(sequence[position/16]&(1<<(15-position%16)))>>(15-position%16);
	 	x=x*2+bit; 
		position++;
	}
*/
	i32 a=this->ZerosRun (position);//ÔÚÊý0µÄ¹ý³ÌÖÐ£¬positionµÄÖµ»á×Ô¶¯žüÐÂ
	x=this->GetBits (position,a+1);//žÃ¹ý³ÌÖÐ£¬²»žüÐÂposition
	position=position+a+1;
	return 2*a+1;
}


//·µ»ØGAM_Phi±àÂëÐòÁÐÖÐ£¬ŽÓpositionÎ»ÖÃ¿ªÊŒ³öÏÖµÄÁ¬ÐøµÄ0µÄžöÊý£¬
//ÔÚŽÎ¹ý³ÌÖÐ£¬positionžüÐÂ¡£
i32 GAM_Phi::ZerosRun(i32 &position) 
{
	
	
	i32 y=0;
	i32 D=this->tablewidth ;//DÄ¬ÈÏÊÇ16.
	i32 x=this->GetBits(position,D);
	i32 w=y=this->zerostable [x];
	while(y==D)
	{
		position=position+D;
		x=this->GetBits (position,D);
		y=this->zerostable [x];
		w=w+y;
	}
	position=position+y;
	return w;
	
/*
	i16 D=this->tablewidth;
	u16 h=GetBits(position,D);
	u16 l=GetBits(position+D,D);
	i32 num=zerostable[h];
	if(num==D)
	{
		num=D+zerostable[l];
		position=position+num;
	
	}
	else
		position=position+num;
	return num;
*/

}
u64 GAM_Phi::GetBits (i32 position,i32 num)
{
/*
	
    //ÓÃÊ±103	
	i32 p=position;
	i32 s=0;
	u64 x=0;
	u64 y=0;
	while(num>0)
	{
		i32 len=min(num,16-p%16);
	    s=(16-(p+len)%16)%16;
		x=(sequence[p/16]>>s)&((1<<len)-1);
		p=p+len;
		num=num-len;
		x=x<<num;
		y=y+x;
	}
	return y;
*/	
	/*
	ÓÃÊ±82
	i32 anchor=position>>4;
	i32 bytes=((position+num-1)>>4)-anchor+1;
	u64 value=0;
	for(i32 i=0;i<bytes;i++)
	{
		value=(value<<16)+sequence[anchor+i];
	}
	i32 overloop=(16-((position+num)&15))&15;
	value=value>>overloop;
	return value&((1<<num)-1);
	*/
	//ÓÃÊ±60
	/*
	u32 anchor=position>>4;
	u64 temp1=sequence[anchor];
	u64 temp2=sequence[anchor+1];
	u64 temp3=sequence[anchor+2];
	temp1=(temp1<<32)+(temp2<<16)+temp3;
	i32 overloop=((anchor+3)<<4)-position-num;
	return (temp1>>overloop)&((1<<num)-1);
*/
	
	u32 anchor=position>>5;
	u64 temp1=sequence[anchor];
	u32 temp2=sequence[anchor+1];
	temp1=(temp1<<32)+temp2;
	i32 overloop=((anchor+2)<<5)-position-num;
	return (temp1>>overloop)&((1<<num)-1);
   

}
//ÔÚÇøŒä[L,R]ÄÚ£¬×îºóÒ»žöphiÖµÐ¡ÓÚprµÄË÷Òý
i32 GAM_Phi::RightBoundary(i32 pr,i32 l,i32 r)
{
	i32 ans=0;
	i32 SL=this->a;
	i32 L=this->b;
	i32 lb=(l+L-1)/L;
	i32 rb=r/L;
	i32 b=lb-1;
	i32 m=0;
	i32 x=0;
	while(lb<=rb)
	{
		m=(lb+rb)>>1;
		x=this->samples->GetValue(m);
		if(x==pr)
		{
			b=m;
			break;
		}
		if(x<pr)
		{
			b=m;
			lb=m+1;
		}
		else
			rb=m-1;
	
	}
	
	x=this->samples->GetValue(b);
	ans=l-1;
	if(r>(b+1)*L-1)
		r=(b+1)*L-1;
	m=b*L;
	i32 d=0;
	i32 position=this->superoffset[m/SL]+this->offsects->GetValue(m/L);
	while(m<l)
	{
		this->Decodegamma(position,d);
		x=(x+d)%n;
		m++;
	}
	//cout<<m<<" "<<l<<"  "<<x<<"  "<<position<<endl;


// dosn't work
/*
	u32 p=0;
	i32 num=0;
	while(x<=pr && m<=r)
	{
		p=this->GetBits(position,16);
		num=this->decodevaluenum[p];
		if(num==0)
		{
			//ans=m;
			m++;
			this->Decodegamma(position,d);
			x=(x+d)%n;
		}
		else
		{
			//ans=m;
			m=m+num;
			position=position+this->decodebitsnum[p];
			x=(x+this->decoderesult[p])%n;
		}
	}
	if(x>pr)
	{
	    if(num==0)
			ans=m-1;
		else
		{
			x=x-this->decoderesult[p];
			m=m-num;
			position=position-this->decodebitsnum[p];
			while(1)
			{
				if(x>pr)
					break;
				ans=m;
				m++;
				if(m>r)
					break;
				this->Decodegamma(position,d);
				x=(x+d)%n;
			}
		}
	}
*/




//work , the lowest one

/*
	while(1)
	{
		if(x>pr)
			break;
		ans=m;
		m++;
		if(m>r)
			break;
		this->Decodegamma(position,d);
		x=x+d;
	}

*/


	
//work
/*
	i32 p=0;
	i32 num=0;
	while(1)
	{
		if(x>pr)
			break;
		ans=m;
		p=GetBits(position,16);
		num=decodevaluenum[p];
		if(num==0)
			m++;
		else
			m=m+num;
		if(m>r)
			break;
		if(num==0)
		{
			Decodegamma(position,d);
			x=x+d;
		}
		else
		{
			position=position+decodebitsnum[p];
			x=x+decoderesult[p];
		}
	}

	if(num!=0)
	{
		if(m>r)
			m=m-num;
		else
		{
			m=m-num;
			position=position-decodebitsnum[p];
			x=x-decoderesult[p];
		}
		while(1)
		{
			if(x>pr)
				break;
			ans=m;
			m++;
			if(m>r)
				break;
			Decodegamma(position,d);
			x=(x+d)%n;
		}
	}
*/


	
//works a little better
	i32 p=0;
	i32 num=0;
	i32 bits=0;
	while(x<=pr && m<r)
	{ 
		p=this->GetBits(position,16);
		num=this->decodevaluenum[p];
		if(num==0)
		{
			m++;
			bits=this->Decodegamma(position,d);
			x=x+d;
		}
		else
		{
			m=m+num;
			position=position+this->decodebitsnum[p];
			x=mod(x+this->decoderesult[p]);
		}
		         //cout<<num<<"  "<<pl-x<<"  "<<endl;
	}
	if(num==16&&m<=r)
		return m+pr-x;
	if(num!=0)
	{
		x=mod(x-this->decoderesult[p]);
		position=position-this->decodebitsnum[p];
		m=m-num;
	}
	else
	{
		m=m-1;
		x=mod(x-d);
		position=position-bits;
	}

	//cout<<m<<" "<<l<<"  "<<x<<"  "<<position<<endl;


	while(1)
	{
		if(x>pr)
			break;
		ans=m;
		m++;
		if(m>r)
			break;
		Decodegamma(position,d);
		x=x+d;
	}


	//cout<<"old right: "<<ans<<endl;
	// cout<<m<<" "<<l<<"  "<<x<<"  "<<position<<" "<<ans<<" "<<pr<<endl;
	return ans;
}
//ÔÚÇøŒä[L,R]ÄÚ£¬µÚÒ»žöphiÖµŽóÓÚplµÄË÷ÒýÖµ¡£
i32 GAM_Phi::LeftBoundary(i32 pl,i32 l,i32 r)
{
	i32 ans=0;
	i32 SL=this->a;
	i32 L=this->b;
	i32 lb=(l+L-1)/L;
	i32 rb=r/L;
    i32 b=rb+1;
	i32 m=0;
	i32 x=0;

	
	while(lb<=rb)
	{
		m=(lb+rb)>>1;
		x=this->samples->GetValue(m);
		if(x==pl)
		{
			b=m;
			break;
		}
		if(x>pl)
		{
			b=m;
			rb=m-1;
		}
		else
			lb=m+1;
	}
	x=this->samples->GetValue(b-1);
	if(r>b*L-1)
		r=b*L-1;
	ans=r+1;
	m=(b-1)*L;
	i32 position=this->superoffset[m/SL]+this->offsects->GetValue(m/L);
	i32 d=0;
	/*
		i32 num=0;
		i32 overloop=l-m;
		u32 p=0;
		while(i<overloop)
		{
			p=this->GetBits (position,16);
			num=this->decodevaluenum[p];
			if(num==0)        
			{
				this->Decodegamma(position,d);
				x=(x+d)%n;
				i++;
				m++;
			}
			else
			{
				if(i+num>overloop)
					break;
				i=i+num;
				m=m+num;
				position=position+this->decodebitsnum [p];
				x=x+this->decoderesult [p];
			}
		}
		for(;i<overloop;i++)
		{
			this->Decodegamma (position,d);
			x=(x+d)%n;
			m++;
		}
	*/




	while(m<l)
	{
		this->Decodegamma(position,d);
		x=(x+d)%n;
		m++;
	}

/*
	while(1)
	{
		if(x>=pl)
		{
			ans=m;
			break;
		}
		m++;
		if(m>r)
			break;
		Decodegamma(position,d);
		x=x+d;
	}


*/

//work!
/*
	i32 p=0;
	i32 num=0;
	while(1)
	{
		if(x>=pl)
		{
			ans=m;
			break;
		}
		p=GetBits(position,16);
		num=decodevaluenum[p];
		if(num==0)
			m++;
		else
			m=m+num;
		if(m>r)
			break;
		if(num==0)
		{
			Decodegamma(position,d);
			x=x+d;
		}
		else
		{
			position=position+decodebitsnum[p];
			x=x+decoderesult[p];
		}
	}
	if(num!=0)
	{
		
		if(m>r)
			m=m-num;
		else
		{
			m=m-num;
			position=position-decodebitsnum[p];
			x=x-decoderesult[p];
		}
		while(1)
		{   
			if(x>=pl)
			{   
				ans=m;
				break;
			}
			m++;
			if(m>r)
				break;
			Decodegamma(position,d);
			x=(x+d)%n;
		}
	}
*/

	
	i32 p=0;
	i32 num=0;
	int bits=0;
	while(x<pl && m<r)
	{
		p=this->GetBits(position,16);
		num=this->decodevaluenum[p];
		if(num!=0)
		{
			//m++;
			//this->Decodegamma(position,d);
			//x=x+d;
			m=m+num;
			position=position+this->decodebitsnum[p];
			x=(x+this->decoderesult[p])%n;

		}
		else
		{
			//m=m+num;
			//position=position+this->decodebitsnum[p];
			//x=(x+this->decoderesult[p])%n;
			m++;
			bits=this->Decodegamma(position,d);
			x=x+d;

		}
		//cout<<num<<"  "<<pl-x<<"  "<<endl;
	}
	
	if(num==16&&m<=r)
		return m+pl-x;
	if(num!=0)
	{
		x=mod(x-this->decoderesult[p]);
		position=position-this->decodebitsnum[p];
		m=m-num;
	}
	else
	{
		m=m-1;
		x=mod(x-d);
		position=position-bits;
	}

//	if(num!=0)
//	{
//		x=mod(x-this->decoderesult[p]);
//		position=position-this->decodebitsnum[p];
//		m=m-num;
//	}
	while(1)
	{
		if(x>=pl)
		{
			ans=m;
			break;
		}
		m++;
		if(m>r)
			break;
		Decodegamma(position,d);
		x=x+d;
	}
		
	


	//cout<<"old left :  "<<ans<<endl;
	return ans;
}
i32 GAM_Phi::mod(i32 x)
{
	if(x<0)
		return x+n;
	else
		return x%n;
}
i32 GAM_Phi::write(savekit & s)
{
	s.writei32(n);
	s.writei32(a);
	s.writei32(b);
	s.writei32(alpsize);
	s.writei32(tablewidth);

	//superoffset
	s.writei32(lenofsuperoffset);
	s.writei32array(superoffset,lenofsuperoffset);

	//offset
	offsects->write(s);

	//samples
	samples->write(s);

	//sequences
	s.writei32(lenofsequence);
	s.writeu32array(sequence,lenofsequence);

	return 1;
}
i32 GAM_Phi::load(loadkit &s)
{
	s.loadi32(this->n);
	s.loadi32(this->a);
	s.loadi32(this->b);
	s.loadi32(alpsize);
	s.loadi32(tablewidth);

	this->zerostable=new u16[1<<tablewidth];
	this->decodevaluenum=new u16[1<<tablewidth];
	this->decodebitsnum=new u16[1<<tablewidth];
	this->decoderesult=new u16[1<<tablewidth];
	this->InitionalTables();

	
	s.loadi32(lenofsuperoffset);
	superoffset=new i32[lenofsuperoffset];
	s.loadi32array(superoffset,lenofsuperoffset);


	offsects=new InArray();
	offsects->load(s);

	samples=new InArray();
	samples->load(s);

	//cout<<"OK"<<endl;

	s.loadi32(lenofsequence);
	sequence=new u32[lenofsequence];
	s.loadu32array(sequence,lenofsequence);
	return 1;
}









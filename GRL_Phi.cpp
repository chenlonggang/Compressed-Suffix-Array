#include<math.h>
#include<ctime>
#include "GRL_Phi.h"
#include<fstream>
#include<iostream>
using namespace std;
GRL_Phi::GRL_Phi()
{
}
GRL_Phi::GRL_Phi(parmaters * csa)
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
	//Test();
	delete [] phivalue;

}



GRL_Phi::~GRL_Phi(void)
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
void  GRL_Phi::Append (i32 x)
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
void GRL_Phi::Test ()
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
	{
		cout<<"Phi is Sorry"<<endl;
		cout<<k<<endl;
	}
	
}
i32 GRL_Phi::blogsize(i32 x) 
{
	
	i32 len=0;
	while(x>0)
	{
		x=x>>1;
		len++;
	}
	return len;
}


i32 GRL_Phi::GetValue(i32 index)
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
		p=this->GetBits(position,16);
		num=this->decodevaluenum[p];
		if(num==0)
		{
			this->Decodegamma(position,d);
			if(d<=this->b*2)
			{
				if(d%2==0)
				{
					if(i+d/2>=overloop)
					{
						x=x+overloop-i;
						return (base+x)%n;
					}
					else
					{
						x=x+d/2;
						i=i+d/2;
					}
				}
				else
				{
					x=x+(d+3)/2;
					i++;
				}
			}
			else
			{
				x=x+d+1-this->b;
				i++;
			}
		}
		else
		{
			if(i+num>overloop)
				break;
			i=i+num;
			position=position+this->decodebitsnum[p];
			x=(x+this->decoderesult[p])%n;
		}
	}
	for(;i<overloop;i++)
	{
		Decodegamma(position,d);
		if(d<=this->b*2)
		{
			if(d%2==0)
			{
				if(i+d/2>=overloop)
				{
					x=x+overloop-i;
					return (base+x)%n;
				}
				else
				{
					x=x+d/2;
					i=i+d/2-1;
				}
			}
			else
			{
				x=x+(d+3)/2;
			}
		}
		else
		{
			x=x+d+1-this->b;
		}
	}
	return (base+x)%n;
	


}
void GRL_Phi::SamplingAndCoding(parmaters *csa)
{

	samples=new InArray(n/b+1,blogsize(n));  //±£Žæ¶þŒ¶Ä¿ÂŒµÄ²ÉÑùÖµ¡£

	this->lenofsuperoffset=n/a+1;
	superoffset=new i32[lenofsuperoffset];  //±£ŽæÒ»Œ¶Ä¿ÂŒµÄÆ«ÒÆÖµ¡£
	
	i32 i,j;

	i32 maxrun=b;

	phivalue=new i32[n];
	i32 *temp=new i32[csa->alphabetsize +1];
	for(i32 i=0;i<csa->alphabetsize +1;i++)
		temp[i]=csa->start [i];
	i32 index=temp[csa->code[csa->lastchar]];
	temp [csa->code [csa->lastchar ]]++;
	i32 h=0;
	uchar c=0;
	for(i32 i=0;i<n;i++)
	{
		i32 pos=csa->SA [i];
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
	i32 runlen=0;
	for( i=0;i<x+1;i++)
	{
		for( j=i*a;j<(i+1)*a&&j<n;j++)
		{
			if(j%b==0)
			{
				 if(runlen>0)
				 {
					 len=len+blogsize(2*runlen)*2-1;
					 runlen=0;
				 }
				 pre=phivalue[j];
				 continue;
			}
			gap=phivalue[j]-pre;
			if(gap<0)
				gap=n+gap;


			if(gap>1)
			{
				if(runlen>0)
				{
					len=len+blogsize(2*runlen)*2-1;
					runlen=0;
				}
				if(gap>=maxrun+2)
					len=len+blogsize(gap+maxrun-1)*2-1;
				else
					len=len+blogsize(gap*2-3)*2-1;
			}
			else
			{
				runlen++;
				if(runlen==maxrun)
				{
					len=len+blogsize(2*runlen)*2-1;
					cout<<"fuck"<<endl;
					runlen=0;
				}
			}
			pre=phivalue[j];
		}
		if(runlen>0)
		{
			len=len+blogsize(2*runlen)*2-1;
			runlen=0;
		}
		if(len>maxlen)
		{
			maxlen=len;
		}
		totallen=totallen+len;
		len=0;
	}

	this->lenofsequence =totallen/32+1;
	this->sequence =new u32[lenofsequence];
	for(i=0;i<lenofsequence;i++)
		sequence[i]=0;
	this->offsects =new InArray(n/b+1,blogsize(maxlen));
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
			if(runlen>0)
			{
				len1=len1+blogsize(2*runlen)*2-1;
				Append(2*runlen);
				runlen=0;
			}
			len2=len1;
			superoffset[index3]=len2;
			index3++;
		}
		if(i%b==0)
		{
			value=phivalue[i];
			this->samples ->SetValue (index1,value);
			index1++;
			if(runlen>0)
			{
				len1=len1+blogsize(2*runlen)*2-1;
				Append(2*runlen);
				runlen=0;
			}
			this->offsects ->SetValue (index2,len1-len2); 
			index2++;
			pre=phivalue[i];
			 
			continue;
		 }
		 gap=phivalue[i]-pre;
		 if(gap<0)
			 gap=n+gap;
		 if(gap>1)
		 {
			 if(runlen>0)
			 {
				 len1=len1+blogsize(2*runlen)*2-1;
				 Append(2*runlen);
				 runlen=0;
			 }
			 if(gap>=maxrun+2)
			 {
				 len1=len1+blogsize(gap+maxrun-1)*2-1;
				Append(gap+maxrun-1);
			 }
			 else
			 {
				 len1=len1+blogsize(gap*2-3)*2-1;
				 Append(gap*2-3);
			 }
		 }
		 else
		 {
			 runlen++;
			 if(runlen==maxrun)
			 {
				 len1=len1+blogsize(2*runlen)*2-1;
				 Append(2*runlen);
				 runlen=0;
			 }
		 }

		 pre=phivalue[i];
		
	 }
	 if(runlen>0)
	 {
		 len1=len1+blogsize(2*runlen)*2-1;
		 Append(2*runlen);
		 runlen=0;
	 }
}


i32 GRL_Phi::GetMemorySize()
{
	return samples->GetMemorySize ()+this->offsects ->GetMemorySize ()+this->lenofsequence *4+this->lenofsuperoffset *4;
}

i32 GRL_Phi::GetOneBit(i32 i) 
{
	i32 anchor=i/32;
	i32 position=i%32;
	return (sequence [anchor]&1<<(31-position))>>(31-position);
}

void GRL_Phi::InitionalTables() 
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
			if(d<=this->b*2)
			{
				if(d%2==0)
				{
					x=x+d/2;
					num=num+d/2;

				}
				else
				{
					x=x+(d+3)/2;
					num++;
				}
			}
			else
			{
				x=x+d+1-this->b;
				num++;
			}
			preb=b;
		}
		Rn[i]=num;
		Rb[i]=preb;
		Rx[i]=x;
	}
	this->sequence=temp;
}

i32  GRL_Phi::Decodegamma(i32 & position, i32 &x)
{
	i32 a=this->ZerosRun (position);
	x=this->GetBits (position,a+1);
	position=position+a+1;
	return 2*a+1;
}

i32 GRL_Phi::ZerosRun(i32 &position) 
{
	
	
	i32 y=0;
	i32 D=this->tablewidth ;
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
}

u64 GRL_Phi::GetBits (i32 position,i32 num)
{
	
	u32 anchor=position>>5;
	u64 temp1=sequence[anchor];
	u32 temp2=sequence[anchor+1];
	temp1=(temp1<<32)+temp2;
	i32 overloop=((anchor+2)<<5)-position-num;
	return (temp1>>overloop)&((1<<num)-1);
   

}

i32 GRL_Phi::RightBoundary(i32 pr,i32 l,i32 r)
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
		if(d<=this->b*2)
		{
			if(d%2==0)
			{
				if(m+d/2>=l)
				{
					x=mod(x+d/2-(m+d/2-l));
					position=position-(m+d/2-l);
					m=l;
					break;
				}
				else
				{
					x=x+d/2;
					m=m+d/2;
				}
			}
			else
			{
				x=(x+(d+3)/2)%n;
				m++;
			}
		}
		else
		{
			x=x+(d+1-this->b)%n;
			m++;
		}
	}

	i32 p=0;
	i32 num=0;
	i32 bits=0;
	bool loop =false;
	while(x<=pr && m<r)
	{ 
		loop =true;
		p=this->GetBits(position,16);
		num=this->decodevaluenum[p];
		if(num==0)
		{
			bits=this->Decodegamma(position,d);
			if(d<this->b*2)
			{
				if(d%2==0)
				{
					m=m+d/2;
					x=x+d/2;
				}
				else
				{
					m=m+1;
					x=x+(d+3)/2;
				}
			}
			else
			{
				m++;
				x=x+d+1-this->b;
			}
		}
		else
		{
			m=m+num;
			position=position+this->decodebitsnum[p];
			x=mod(x+this->decoderesult[p]);
		}
	}



	if(loop)
	{
		if(num!=0)
		{
			x=mod(x-this->decoderesult[p]);
			position=position-this->decodebitsnum[p];
			m=m-num;
		}
		else
		{
			if(d<this->b*2)
			{
				if(d%2==0)
				{
					m=m-d/2;
					x=mod(x-d/2);
				}
				else
				{
					m=m-1;
					x=mod(x-(d+3)/2);
				}
			}
			else
			{
				m=m-1;
				x=mod(x-(d+1-this->b));
			}

			position=position-bits;
		}
	}
	/*
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
	*/

//	cout<<m<<" "<<l<<"  "<<x<<"  "<<position<<endl;


	num=1;
	while(1)
	{
		if(x>pr)
		{
			if(num==1)
				break;
			else
			{
				ans=m+d/2-(x-pr+1);
				break;
			}
		}
		ans=m;
		m=m+num;
		if(m>r)
			break;
		Decodegamma(position,d);
		if(d<this->b*2)
		{
			if(d%2==0)
			{
				num=d/2;
				x=mod(x+d/2);
			}
			else
			{
				num=1;
				x=x+(d+3)/2;
			}
		}
		else
		{
			num=1;
			x=mod(x+d+1-this->b);
		}
	}
	// cout<<m<<" "<<l<<"  "<<x<<"  "<<position<<" "<<ans<<" "<<pr<<endl;
	return ans;
}
i32 GRL_Phi::LeftBoundary(i32 pl,i32 l,i32 r)
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
	if(b==0)
		return 0;
	x=this->samples->GetValue(b-1);
	if(r>b*L-1)
		r=b*L-1;
	ans=r+1;
	m=(b-1)*L;
	i32 position=this->superoffset[m/SL]+this->offsects->GetValue(m/L);
	i32 d=0;


	while(m<l)
	{
		this->Decodegamma(position,d);
		if(d<=this->b*2)
		{
			if(d%2==0)
			{
				if(m+d/2>=l)
				{
					x=(x+d/2-(m+d/2-l))%n;
					position=position-(m+d/2-l);
					m=l;
					break;
				}
				else
				{
					x=x+d/2;
					m=m+d/2;
				}
			}
			else
			{
				x=(x+(d+3)/2)%n;
				m++;
			}
		}
		else
		{
			x=x+(d+1-this->b)%n;
			m++;
		}

	}
	i32 p=0;
	i32 num=0;
	i32 bits=0;
	bool loop=false;
	while(x<pl && m<r)
	{
		loop =true;
		p=this->GetBits(position,16);
		num=this->decodevaluenum[p];
		if(num!=0)
		{
			m=m+num;
			position=position+this->decodebitsnum[p];
			x=(x+this->decoderesult[p])%n;

		}
		else
		{
			bits=this->Decodegamma(position,d);
			if(d<this->b*2)
			{
				if(d%2==0)
				{
					m=m+d/2;
					x=x+d/2;
				}
				else
				{
					m=m+1;
					x=x+(d+3)/2;
				}
			}
			else
			{
				m++;
				x=x+d-this->b+1;
			}

		}
	}

	if(loop)
	{
		if(num!=0)
		{
			x=mod(x-this->decoderesult[p]);
			position=position-this->decodebitsnum[p];
			m=m-num;
		}
		else
		{
			if(d<this->b*2)
			{
				if(d%2==0)
				{
					m=m-d/2;
					x=mod(x-d/2);
				}
				else
				{
					m=m-1;
					x=mod(x-(d+3)/2);
				}
			}
			else
			{
				m=m-1;
				mod(x-(d+1-this->b));
			}
			position=position-bits;
		}
	}
	
	num=1;
	while(1)
	{
		if(x>=pl)
		{
			if(num==1)
			{
				ans=m;
				break;
			}
			else
			{
				ans=m+num-(x-pl);
				break;
			}
		}
		m=m+num;
		if(m>r)
			break;
		Decodegamma(position,d);
		
		if(d<this->b*2)
		{
			if(d%2==0)
			{
				num=d/2;
				x=mod(x+d/2);
			}
			else
			{
				num=1;
				x=x+(d+3)/2;
			}
		}
		else
		{
			num=1;
			x=mod(x+d+1-this->b);
		}
	}
		
	return ans;
}
i32 GRL_Phi::mod(i32 x)
{
	if(x<0)
		return x+n;
	else
		return x%n;
}
i32 GRL_Phi::write(savekit & s)
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
i32 GRL_Phi::load(loadkit &s)
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

i32 * GRL_Phi::GetPhiArray()
{
	i32 * phi=new i32[n];
	i32 pre=0;
	i32 d=0;
	index=0;
	i32 times=n/b+1;
	for(i32 i=0;i<times;i++)
	{
		pre=phi[i*b]=samples->GetValue(i);
		for(i32 j=0;j<(i+1)*b && j<n;)
		{
			this->Decodegamma(index,d);
			if(d<=b*2)
			{
				if(d%2==0)
				{
					for(i32 k=0;k<d/2;k++)
						phi[i*b+j+k]=(pre+1+k)%n;
					j=j+d/2;
					pre=(pre+d/2)%n;
				}
				else
				{
					phi[i*b+j]=pre=(pre+(d+3)/2)%n;
					j++;
				}
			}
			else
			{
				phi[i*b+j]=pre=(pre+d+1-b)%n;
				j++;
			}
		}
	}
	return phi;
}











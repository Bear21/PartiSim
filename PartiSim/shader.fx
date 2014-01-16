// Copyright (c) 2013 All Right Reserved, http://8bitbear.com/
//
// THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY 
// KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// <author>Stephen Wheeler</author>
// <email>bear@8bitbear.com</email>
// <date>2013-01-15</date>
//Texture2D txDiffuse : register( t0 );

//RWByteAddressBuffer txBuffer : register( t0);
ByteAddressBuffer	txBuffer : register( t0);
ByteAddressBuffer	txData : register( t1);
ByteAddressBuffer	txRef : register( t2);
//Texture3D<float2> txData : register( t1);
//Texture2D txBuffer : register( t0 );

struct PS_INPUT_TEX
{
    float4 Pos : SV_POSITION;
	float2 Tex	: TEXCOORD0;
};

cbuffer SimDetails : register( b1 )
{
	float4 Box;
	float tscale;
	int2	dimensions;
}
cbuffer PSCB : register( b0 )
{
	int height;
}

/*	DirectX::XMVectorSet(-1, 1, 0, 0),
	DirectX::XMVectorSet(1, 1, 1, 0),
	DirectX::XMVectorSet(-1, -1, 0, 1),
	DirectX::XMVectorSet(1, -1, 1, 1),*/

PS_INPUT_TEX VS( uint vID : SV_VertexID )
{
    PS_INPUT_TEX output;

	float x = vID % 2;		//vID==0&&2 = x = 0; vID==1&&3 = x = 1
	float y = vID/2;		//vID==0&&1 = y = 0; vID==2&&3 = y = 1
	
	output.Pos = float4(x*2-1, -(y*2-1), 0.5, 1);
	output.Tex = float2(x, y);
    return output;
}

//float4 PS(PS_INPUT_TEX input) : SV_Target
//{
//	int px = trunc(input.Pos.x);
//	int py = trunc(input.Pos.y);
//
//	uint q1 = txBuffer.Load((py+(px*height))*4);
//	if(q1==0)
//		return (float4)1;
//	float density = 1.f-(float)q1/64.f;
//	return float4(density,density,density,density );
//	//return float4(q1, 1-q1, q1, q1);
//	
//		/*int q2 = gids[limitx-1][limity][0];
//		int q3 = gids[limitx][limity-1][0];
//		int q4 = gids[limitx-1][limity-1][0];
//
//		for(unsigned int i=1; i<q1; i++)
//		{
//					
//		}*/
//		//float density = 1.f/q;
//		//float density = 1.f-(float)q1/64.f;
//
//		/*if(q1>64)
//		{
//			return float4(-(density+1)/2, 0.f,0.f,1.f );
//		}
//		else
//		{
//			float d
//			return float4(density,density,density,density );
//		}*/
//		/*if(q1<=64)
//		{*/
//			int q2 = txBuffer.Load((py+((px-1)*height))*4);
//			if(q2>64)
//				q2=64;
//			int q3 = txBuffer.Load(((py-1)+(px*height))*4);
//			if(q3>64)
//				q3=64;
//			int q4 = txBuffer.Load(((py-1)+((px-1)*height))*4);
//			if(q4>64)
//				q4=64;
//			int i;
//			//float density=0.f;
//			//float2 color = (float2)0;
//			//return float4(txData.Sample(samLinear, float3((float)px/1280.f, py/800.f, 0.0f)), 1.f, 1.f);
//			//return float4((float2)txData.Load(int4(px, py, 0, 0)), 1.f, 1.f);
//			int sRef = txRef.Load(((py+(px*height))*4*64));
//			float2 sample = asfloat(txData.Load2(sRef*16));
//			if(sample.x<0)
//				sample.x=-sample.x;
//			if(sample.y<0)
//				sample.y=-sample.y;
//			return float4(sample/64.f, 1.f, 1.f);
//			//density = q1+q2+q3+q4;
//			//for(i=0; i<q1; i++)
//			//{
//			//	//float2 sample = txData.Load(int4(px, py, i, 0));
//			//	float2 sample = asfloat(txRef.Load2(((py+(px*height))*4*64) + i*4));
//			//	//float2 sample = txData.Load(int4(px, py, i, 0));
//			//	sample = sample % 1;
//			//	density+= -(sample.x-1)*-(sample.y-1);
//			//}
//			//for(i=0; i<q2; i++)
//			//{
//			//	float2 sample = txData.Load(int4(px-1, py, i, 0));
//			//	sample = sample % 1;
//			//	density+= sample.x*(1-sample.y);
//			//}
//			//for(i=0; i<q3; i++)
//			//{
//			//	float2 sample = txData.Load(int4(px, py-1, i, 0));
//			//	sample = sample % 1;
//			//	density+= (1-sample.x)*sample.y;
//			//}
//			//for(i=0; i<q4; i++)
//			//{
//			//	float2 sample = txData.Load(int4(px-1, py-1, i, 0));
//			//	sample = sample % 1;
//			//	density+= sample.x*sample.y;
//			//	//color.y=1;
//			//	//color+=float2(0, 0.5);
//			//}
//			//density = 1-density/40.f;
//			//if(density==1)
//			//	return float4(1.f,1.f,1.f,1.f);
//			//else if(density<-1)
//			//	if(density<-6)
//			//	{
//			//		return float4(-(density+2)/4, density, 2+density/4, density );
//			//	}
//			//	else
//			//	{
//			//		return float4(-(density+2)/4, density, -(density+1)/2, density );
//			//	}
//			//return float4(density, density, density, density );
//		/*}
//		else
//		{
//		}*/
//
//}

//float4 PS(PS_INPUT_TEX input) : SV_Target
//{
//	int px = trunc(input.Pos.x);
//	int py = trunc(input.Pos.y);
//
//	uint q1 = txBuffer.Load((py+(px*height))*4);
//	if(q1==0)
//		return (float4)1;
//	float density = 1.f-(float)q1/32.f;
//	if(density==1)
//		return float4(1.f,1.f,1.f,1.f);
//	else if(density<0)
//	{
//		if(density<-9)
//		{
//			return float4(-(density+2)/4, -(density+5)/4, 4+density/4, density );
//		}
//		else
//		{
//			return float4(-(density+2)/6, density, -(density+1)/2, density );
//		}
//	}
//	return float4(density,density,density,density );
//}

float4 PS(PS_INPUT_TEX input) : SV_Target
{
	int px = trunc(input.Pos.x);
	int py = trunc(input.Pos.y);

	uint q1 = txBuffer.Load((px+(py*dimensions.x))*4);
	if(q1==0)
		return (float4)1;
	float density = 1.f-(float)q1/3200.f;
	//density/=100.f;
	if(density==1)
		return float4(1.f,1.f,1.f,1.f);
	else if(density<0)
	{
		if(density<-9)
		{
			return float4(-(density+2)/4, -(density+5)/4, 4+density/4, density );
		}
		else
		{
			return float4(-(density+2)/6, density, -(density+1)/2, density );
		}
	}
	return float4(density,density,density,density );
}

//float4 PS(PS_INPUT_TEX input) : SV_Target
//{
//	int px = trunc(input.Pos.x);
//	int py = trunc(input.Pos.y);
//
//	uint q1 = txBuffer.Load((px+(py*Box.z))*4);
//	if(q1!=0)
//		return float4(0.f,0.f,0.f,0.f);
//
//	return float4(1.f,1.f,1.f,1.f);
//}

//float4 PS(PS_INPUT_TEX input) : SV_Target
//{
//	float px = trunc(input.Pos.x);
//	float py = trunc(input.Pos.y);
//	float4 result = float4(asfloat(txBuffer.Load4((px+(py*width))*16)));
//	return result;
//}
// Copyright (c) 2014 All Right Reserved, http://8bitbear.com/
//
// THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY 
// KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// <author>Stephen Wheeler</author>
// <email>bear@8bitbear.com</email>
// <date>2014-01-15</date>
//Texture2D txDiffuse : register( t0 );

SamplerState samLinear : register( s0 );
Texture2D<float4> tx2Data : register( t0);
ByteAddressBuffer	txBuffer : register( t0);
ByteAddressBuffer	txData : register( t1);
ByteAddressBuffer	txRef : register( t2);


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

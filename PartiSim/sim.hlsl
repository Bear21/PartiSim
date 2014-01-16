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
#include "shader.fx"

struct SimControl
{
	int inputLow;
	int inputHigh;
	float mousePosX;
	float mousePosY;
};

cbuffer SimInput : register( b2 )
{
	float timeP;
	unsigned int numControl;
	int reserved1, reserved2;
	SimControl controlInput[16];
};

SamplerState samLinear : register( s0 );

Texture2D<float4> tx2Data : register( t0);
RWByteAddressBuffer	tx2Buffer : register( u1 );

float4 sim(PS_INPUT_TEX input) : SV_Target
{
	float4 ref = tx2Data.Sample(samLinear, input.Tex);//locs[idx[0]][idx[1]];
	float2 loc = ref.zw;
	float2 vel = ref.xy;
	float4 output;

	if(loc.x<0.f)
	{
		loc.x=-loc.x;
		vel.x=-vel.x;
	}
	else if(loc.x>Box.z)
	{
		loc.x = Box.z+(Box.z-loc.x);
		vel.x=-vel.x;
	}
	if(loc.y<0.f)
	{
		loc.y=-loc.y;
		vel.y=-vel.y;
	}
	else if(loc.y>Box.w)
	{
		loc.y = Box.w+(Box.w-loc.y);
		vel.y=-vel.y;
	}
	for(uint i=0; i<numControl; i++)
	{
		if((controlInput[i].inputLow & 3) < 3)//will only run if 2 or 1, not 2&1
		{
			float2 relvec = float2(controlInput[i].mousePosX-loc.x, controlInput[i].mousePosY-loc.y);

			float dist = sqrt(relvec.x*relvec.x+relvec.y*relvec.y);//get distance for normalisation
			relvec = (relvec/dist);
			if(controlInput[i].inputLow==1)
			{
				float mult = 1.f-dist/100.f;
				if(mult>0.f)
					vel+=relvec*mult*(timeP*200.f);
			}
			else if (controlInput[i].inputLow==2)
			{
				float mult = 1.f-dist/250.f;
				if(mult>0.f)
					vel-=relvec*(mult*mult)*(timeP*100.f);
			}
		}
		if((controlInput[i].inputLow & 4) == 4)
		{
			//reserved for middle mouse
		}
	}
	vel-=vel*0.1*timeP;

	output.xy = vel;
	output.zw = loc+vel*timeP;

	int result;
	
	uint addr = trunc(loc.x*tscale);
	addr += (trunc(loc.y*tscale))*dimensions.x;
	addr*=4;
	tx2Buffer.InterlockedAdd(addr, 100, result);


	//antialiasing.
	/*float2 aa = loc % 1;
	uint addr = trunc(loc.x*tscale);
	addr += (trunc(loc.y*tscale))*dimensions.x;
	addr*=4;
	float share = (1-aa.x)*(1-aa.y);
	tx2Buffer.InterlockedAdd(addr, share*100, result);
	addr += 4;
	share = (aa.x)*(1-aa.y);
	tx2Buffer.InterlockedAdd(addr, share*100, result);
	addr -=4;
	addr+=dimensions.x*4;
	share = (1-aa.x)*(aa.y);
	tx2Buffer.InterlockedAdd(addr, share*100, result);
	addr += 4;
	share = (aa.x)*(aa.y);
	tx2Buffer.InterlockedAdd(addr, share*100, result);*/

	return output;
}


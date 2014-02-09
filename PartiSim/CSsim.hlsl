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


Texture2D<float4> tx2Data : register( t0);
RWTexture2D<float4> tx2DataOut : register( u0);
RWByteAddressBuffer	tx2Buffer : register( u1 );

[numthreads(16, 16, 1)]
void CSSim( uint3 DTid : SV_DispatchThreadID )
{
	float4 ref = tx2Data.Load(DTid);
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
	//decay velocity
	vel-=vel*0.1*timeP;

	output.xy = vel;
	output.zw = loc+vel*timeP;

	int result;
	
	uint addr = trunc(loc.x*tscale);
	addr += (trunc(loc.y*tscale))*dimensions.x;
	addr*=4;
	tx2Buffer.InterlockedAdd(addr, 100, result);

	tx2DataOut[DTid.xy] = output;
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

	//return output;
}
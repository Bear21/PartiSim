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
SamplerState samLinear : register( s0 );
Texture2D<float4> tx2Data : register( t0);

float4 halt(PS_INPUT_TEX input) : SV_Target
{
	return float4(0.f, 0.f, tx2Data.Sample(samLinear, input.Tex).zw);
}
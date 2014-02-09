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
#pragma once
#pragma pack(push, 1) // ensure no padding
__declspec(align(4)) struct SimControl
{
	int inputLow;
	int inputHigh;
	float mousePosX;
	float mousePosY;
};

struct SimInput
{
	float timeP;
	unsigned int numControl;
	int reserved1, reserved2;
	SimControl controlInput[16];
};
#pragma pack(pop)
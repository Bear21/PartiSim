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
#include <Windows.h>
class TimePast
{
private:
	__int64 tickps;
	LARGE_INTEGER start, finish;
public:
	TimePast()
	{
		QueryPerformanceFrequency((LARGE_INTEGER*)&tickps);
		QueryPerformanceCounter((LARGE_INTEGER*)&start);
	}
	void Reset()
	{
		QueryPerformanceCounter((LARGE_INTEGER*)&start);
	}
	float Check()
	{
		QueryPerformanceCounter((LARGE_INTEGER*)&finish);
		float result = (float)(finish.QuadPart-start.QuadPart)/tickps;
		start = finish;
		return result;
	}
	float Peek()
	{
		QueryPerformanceCounter((LARGE_INTEGER*)&finish);
		return (float)(finish.QuadPart-start.QuadPart)/tickps;
	}

		
};
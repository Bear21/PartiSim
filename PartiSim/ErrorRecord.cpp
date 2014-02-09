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
#include "ErrorRecord.h"
#include <stdio.h>

namespace AppErrors
{
	DxAppErrors g_errorList[1024];
	int g_count=0;

	DxAppErrors Record(DxAppErrors error)
	{
		if(error!=0)
			g_errorList[g_count++]=error;
		return error;
	}

	void Dump(char *filename)
	{
		FILE *errorOut;
		fopen_s(&errorOut, filename, "wtS");
		for(int i=0; i<g_count; i++)
		{
			fprintf_s(errorOut, "0x%08X\n", g_errorList[i]);
		}
		fclose(errorOut);
	}
}
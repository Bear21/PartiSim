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
#include "DxApp.h"
#include <tchar.h>

using namespace AppErrors;

void ExtractArgument(wchar_t *start, wchar_t *destination);

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);

	//take it off the stack
	DxApp *app = new DxApp();

	int startvalue=1;
	wchar_t *readFile=0, *writeFile=0;
	wchar_t *hostName=0;
	wchar_t *param =0;
	int timeSample=0;
	int renderMode=0;

	if(param=wcsstr(lpCmdLine, L"+r "))
	{
		readFile = new wchar_t[64];
		ExtractArgument(param+3, readFile);
		startvalue=18;
	}

	if(param=wcsstr(lpCmdLine, L"+w "))
	{
		writeFile = new wchar_t[64];
		ExtractArgument(param+3, writeFile);
		startvalue+=8;
	}

	if(param=wcsstr(lpCmdLine, L"+h "))
	{
		wchar_t *timeInput = new wchar_t[64]; // change this to framedelay
		ExtractArgument(param+3, timeInput);

		startvalue+=4;
		swscanf_s(timeInput, L"%d", &timeSample, 4);
		delete [] timeInput;
	}
	else if(param=wcsstr(lpCmdLine, L"+c "))
	{
		hostName = new wchar_t[64];
		ExtractArgument(param+3, hostName);

		startvalue+=32;
	}

	if(param=wcsstr(lpCmdLine, L"+t "))
	{
		wchar_t *timeInput = new wchar_t[64];
		ExtractArgument(param+3, timeInput);

		swscanf_s(timeInput, L"%d", &timeSample, 4);
		delete [] timeInput;
	}

	if(param=wcsstr(lpCmdLine, L"+compute"))
	{
		renderMode=1;
	}
	
	DxAppSetupDesc daDesc = {hInstance, L"PartiSim", L"PartiSim", WS_OVERLAPPEDWINDOW | WS_VISIBLE, 1280, 800, nCmdShow, renderMode, startvalue, timeSample, readFile, writeFile, hostName};
	if(Record((DxAppErrors)app->Init(&daDesc)) != 0)
	{
		Dump("errorlog.txt");
		return 0;
	}
	if(readFile)
		delete [] readFile;
	if(writeFile)
		delete [] writeFile;
	if(hostName)
		delete [] hostName;
	

	int result = app->Run();
	delete app;
	return result;
}

void ExtractArgument(wchar_t *start, wchar_t *destination)
{
	int spn = wcscspn(start, L" ");
	memcpy_s(destination, 128, start, spn*2);
	destination[spn] = 0;
}
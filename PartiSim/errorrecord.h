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

namespace AppErrors
{
	enum DxAppErrors
	{
		noError	= 0,
		//window related
		failedRegister = -0x11000,
		failedCreateWindow,

	};
	DxAppErrors Record(DxAppErrors error);
	void Dump(char* filename);
}
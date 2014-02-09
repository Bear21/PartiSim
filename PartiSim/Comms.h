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

#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <Mswsock.h>

#include <vector>
#include "Common.h"

#define DEFAULT_TCP_PORT 47815

#define COMMS_CONNECT_VER 502 //should probably increment with build


class Comms
{
private:
	int m_init;
	int m_delay;

	SOCKET m_serverSock;
	int m_tcpPort;

	SOCKET m_clientSock;

	std::vector<SimInput>	m_record;
	unsigned int			m_recordListHelper;
	unsigned int			m_target;
	int						m_host;
public:
	Comms(void);
	~Comms(void);

	int CreateTCP();
	int Accept();
	int Connect(wchar_t *hostname);

	int SendInit(int timeMeth, int delay);
	int RecvInit(int &timeMeth);

	int GetInput(SimInput &io);

private:
	int Initalise();
	void EnableBlocking(int on, int timeOut=0);
	void PushDelay();
};


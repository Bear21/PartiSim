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
#include "Comms.h"
#include <stdio.h>


Comms::Comms(void)
	: m_init(0), m_delay(0), m_recordListHelper(0), m_target(0), m_host(0)
{
}


Comms::~Comms(void)
{
}


int Comms::Initalise()
{
	wchar_t errorMessage[80];
	if(!m_init)
	{
		WSADATA wsaData;
		int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (iResult != NO_ERROR) {
			
			swprintf(errorMessage, L"WSAStartup failed with error: %d", iResult);
			MessageBox(NULL, errorMessage, L"Error", NULL);
			return 1;
		}
		m_init=1;
	}
	return 0;
}

int Comms::CreateTCP()
{
	if(Initalise())
		return -1;

	addrinfo hints;
	addrinfo *result;

	char s_port[6];
	int iResult;
		// Initialize Winsock

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	int port = DEFAULT_TCP_PORT;

	for(int i = 0; i<20; i++)
	{
		sprintf(s_port, "%d", port+i);
		iResult = getaddrinfo(NULL, s_port, &hints, &result);
		if (iResult != 0)
		{
			return 0;
		}

		m_serverSock = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

		iResult = bind(m_serverSock, result->ai_addr, sizeof (SOCKADDR));
		if (iResult != 0)
		{
			iResult = WSAGetLastError();
			if(iResult == 10048)
			{
				closesocket(m_serverSock);
				port++;
			}
			else
			{
				closesocket(m_serverSock);
				port=45101;
				return 0;
			}
		}
		else
			break;
	}


	freeaddrinfo(result);

	iResult = listen(m_serverSock, SOMAXCONN);
	if (iResult == SOCKET_ERROR) {
		closesocket(m_serverSock);
		return 1;
	}
	u_long a=0;// 0 for now, later 1
	m_tcpPort = port;
	ioctlsocket(m_serverSock, FIONBIO, &a);
	return 0;
}

void Comms::EnableBlocking(int on, int timeOut)
{
	on=!on;
	ioctlsocket(m_clientSock, FIONBIO, (u_long*)&on);
	setsockopt(m_clientSock, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeOut, 4);
}

int Comms::Accept()
{
	SOCKET potentialClient;
	sockaddr_in Serveraddr;
	int fromlen = sizeof(sockaddr_in);
	/*m_clientSock*/ potentialClient  = accept(m_serverSock, (sockaddr*)&Serveraddr, &fromlen);
	if(potentialClient==0)
	{
		return -1;
	}
	u_long a=0;
	ioctlsocket(potentialClient, FIONBIO, &a);
	DWORD timeout = 100000;
	int iResult = setsockopt(potentialClient, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, 4);
	int pulse = COMMS_CONNECT_VER;//change with version change
	iResult = send(potentialClient, (char*)&pulse, 4, NULL);
	if(iResult<0)
	{
		//error handle
		return WSAGetLastError();
	}
	iResult = recv(potentialClient, (char*)&pulse, 4, NULL);
	if(iResult<0)
	{
		//error handle
	}
	if(pulse!=~COMMS_CONNECT_VER)
	{
		closesocket(potentialClient);
		return -1;
	}
	else 
		m_clientSock = potentialClient;
	return 0;
}

int Comms::Connect(wchar_t *hostname)
{
	if(Initalise())
		return -1;
	addrinfoW hints;
	addrinfoW *result;

	ZeroMemory(&hints, sizeof(addrinfoW));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;
	wchar_t s_port[6];
	int iResult;
	swprintf(s_port, L"%d", DEFAULT_TCP_PORT);
	//getaddrinfo
	iResult = GetAddrInfoW(hostname, s_port, &hints, &result);
	if (iResult != 0)
	{
		return WSAGetLastError();
	}
	SOCKET TCPSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	DWORD timeout = 2500;
	setsockopt(TCPSock, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, 4);
		
	iResult = connect(TCPSock, result->ai_addr, sizeof(sockaddr));
	if(iResult==SOCKET_ERROR)
	{
		return WSAGetLastError();
	}
	int pulse;
	recv(TCPSock, (char*)&pulse, 4, NULL);
	if(pulse==COMMS_CONNECT_VER)
		pulse=~pulse;
	send(TCPSock, (char*)&pulse, 4, NULL);
	if(pulse==~COMMS_CONNECT_VER)
		m_clientSock = TCPSock;
	else
	{
		//some conflicting version error.
		return -1;
	}
	return 0;
}

int Comms::SendInit(int timeMeth, int delay)
{
	if(m_init==0)
		return 0;
	m_delay = delay;
	int message[] = {timeMeth, delay};
	m_host=1;
	PushDelay();
	return send(m_clientSock, (char*)message, 8, NULL);
}

int Comms::RecvInit(int &timeMeth)
{
	if(m_init==0)
		return 0;
	int message[2];
	int result;
	EnableBlocking(1);
	result = recv(m_clientSock, (char*)message, 8, NULL);
	timeMeth = message[0];
	m_delay = message[1];
	PushDelay();
	return result;
}


int Comms::GetInput(SimInput &io)
{
	char inputBuffer[512];
	SimInput *fromSock = (SimInput*)inputBuffer;
	if(m_recordListHelper<=m_target)
		EnableBlocking(1, 0);
	else
		EnableBlocking(0, 0);
	int result = recv(m_clientSock, inputBuffer, 8, NULL);
	if(m_recordListHelper==m_target && result==-1)
	{
		//timed out...
		return 0;
	}
	else if(result==0)
	{
		//close
		return 0;
	}
	else if(result==8)
	{
		int offset=0;
		
		if(m_record.size() < m_recordListHelper+1)
		{
			fromSock->reserved1 = m_recordListHelper;
			m_record.push_back(*fromSock);
		}
		else
		{
			offset = m_record[m_recordListHelper].numControl;
			m_record[m_recordListHelper].numControl += fromSock->numControl;
			if(m_host==0)
				m_record[m_recordListHelper].timeP = fromSock->timeP;
		}
		if(fromSock->numControl>0)
		{
			result = recv(m_clientSock, inputBuffer+8, 8+16*fromSock->numControl, NULL);
			memcpy(&m_record[m_recordListHelper].controlInput[offset], &fromSock->controlInput[0], 16*fromSock->numControl);
		}
		//m_record[m_recordListHelper].reserved1=m_recordListHelper;
		m_recordListHelper++;
		EnableBlocking(0, 0);
		while((result = recv(m_clientSock, inputBuffer, 8, NULL))==8)
		{
			offset=0;
			
			if(m_record.size() < m_recordListHelper+1)
			{
				fromSock->reserved1 = m_recordListHelper;
				m_record.push_back(*fromSock);
			}
			else
			{
				offset = m_record[m_recordListHelper].numControl;
				m_record[m_recordListHelper].numControl += fromSock->numControl;
				if(m_host==0)
					m_record[m_recordListHelper].timeP = fromSock->timeP;
			}
			if(fromSock->numControl>0)
			{
				result = recv(m_clientSock, inputBuffer+8, 8+16*fromSock->numControl, NULL);
				memcpy(&m_record[m_recordListHelper].controlInput[offset], &fromSock->controlInput[0], 16*fromSock->numControl);
			}
			//m_record[m_recordListHelper].reserved1=m_recordListHelper;
			m_recordListHelper++;
		}
	}
	//send data
	if(io.numControl==0)
	{
		send(m_clientSock, (char*)&io, 8, NULL);
	}
	else
	{
		send(m_clientSock, (char*)&io, 16*(io.numControl+1), NULL); //io.numControl+1 to include the time and numcount in the size
	}
	//send data

	

	int offset=0;
	if(m_record.size() < m_target+m_delay+1)
	{
		io.reserved2=m_target+m_delay;
		m_record.push_back(io);
	}
	else
	{
		offset = m_record[m_target+m_delay].numControl;
		m_record[m_target+m_delay].numControl += io.numControl;
		if(m_host==1)
			m_record[m_target+m_delay].timeP = io.timeP;
	}
	if(m_record[m_target+m_delay].numControl!=0)
		memcpy(&m_record[m_target+m_delay].controlInput[offset], &io.controlInput[0], 16*io.numControl);

	io = m_record[m_target++];

	return 1;
}

void Comms::PushDelay()
{
	SimInput empty;
	ZeroMemory(&empty, sizeof(empty));
	for(m_recordListHelper=0; m_recordListHelper<m_delay; m_recordListHelper++)
	{
		m_record.push_back(empty);
	}
	return;
}
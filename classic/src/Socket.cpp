/****************************
Copyright © 2002-2012 Open Zelda
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
//-----------------------------------------------------------------------------
// File: Socket.cpp
//
// Desc: Handles networking
//       
//-----------------------------------------------------------------------------
#define STRICT

#include "Socket.h"



//-----------------------------------------------------------------------------
// Name: CSocket::CSocket()
// Desc: Initializes CSocket Object
// 
//-----------------------------------------------------------------------------
CSocket::CSocket()
{	
	/*
	SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,"%s\n","CSocket::CSocket() - Initializing CSocket Object");
	port = 5001;
	host = "127.0.0.1";
	server = false;
	selection = 0;
	
	m_Socket.Sock = new SOCKET[1];
	m_Socket.Addr = new sockaddr_in[1];
	m_Socket.Event = new OZ_STATE[1];
	m_Socket.Login = new OZ_LOGIN[1];
	m_Socket.isConnected = new bool[1];
	m_Socket.isConnected[0] = false;
	m_Socket.Max = 1;

	m_Socket.Sock[0] = INVALID_SOCKET;
	ResetEvents(&m_Socket.Event[0]);

	if(WSAStartup(0x0202,&this->wsaData) == SOCKET_ERROR)
	{
		SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,"%s\n","CSocket::CSocket() - Socket Startup failed");
		WSACleanup();
		return;
	}
	*/
}

//-----------------------------------------------------------------------------
// Name: CSocket::~CSocket()
// Desc: Deletes CSocket Object
// 
//-----------------------------------------------------------------------------
CSocket::~CSocket()
{
	/*
	SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,"%s\n","CSocket::~CSocket() - Deleting CSocket Object");
for(int i = 0; i < (m_Socket.Max + 1); i++)
			{
				if(m_Socket.Sock[i] != INVALID_SOCKET)
				{
					CloseSocket(i);
				}
			}
	WSACleanup();
	*/
}

//-----------------------------------------------------------------------------
// Name: CSocket::SetSocket()
// Desc: Changes all Socket info
// 
//-----------------------------------------------------------------------------
void CSocket::SetSocket(int param, char* val)
{
	/*
	switch(param)
	{
	case PORT:
		port = (unsigned int)atoi(val);
		break;
	case ADDRESS:
		host = strdup(val);
		break;
	case SOCKETS:
		// If it is the same value, this is a waste of time
		if(m_Socket.Max == atoi(val))
			break;
		//Cancel out all data first
		if(m_Socket.Max == 1)
		{
			if(m_Socket.Sock[0] != INVALID_SOCKET)
			{
				CloseSocket(0);
			}
		}
		else
		{
			for(int i = 0; i < (m_Socket.Max + 1); i++)
			{
				if(m_Socket.Sock[i] != INVALID_SOCKET)
				{
					CloseSocket(i);
				}
			}
		}

		// Delete old data
		SAFE_DELETE(m_Socket.Sock);
		SAFE_DELETE(m_Socket.Addr);
		SAFE_DELETE(m_Socket.Event);
		SAFE_DELETE(m_Socket.Login);

		// Create new data
		m_Socket.Sock = new SOCKET[atoi(val)];
		m_Socket.Addr = new sockaddr_in[atoi(val)];
		m_Socket.Login = new OZ_LOGIN[atoi(val)];
		m_Socket.Max = atoi(val);

		// Initialize new data
		if(m_Socket.Max == 1)
		{
			m_Socket.Event = new OZ_STATE[atoi(val)];
			m_Socket.Sock[0] = INVALID_SOCKET;
			ResetEvents(&m_Socket.Event[0]);
			ZeroMemory((char *)&m_Socket.ovr, sizeof(m_Socket.ovr));
			//m_Socket.ovr.hEvent = m_Socket.Event[1];
		}
		else
		{
			m_Socket.Event = new OZ_STATE[atoi(val)];
			for(int i = 0; i < (m_Socket.Max + 1); i++)
			{
				m_Socket.Sock[i] = INVALID_SOCKET;
				ResetEvents(&m_Socket.Event[i]);
			}
		}
		break;
	case MODE:
		if(!strcmp(val, "FALSE"))
			server = false;
		else if(!strcmp(val, "TRUE"))
			server = true;
		//else
			// Do nothing, apparently they don't know how to spell
		break;
	case LOGIN:
		m_Socket.Login[selection].User = strdup(val);
		break;
	case PASSWORD:
		m_Socket.Login[selection].Password = strdup(val);
		break;
	}
	*/
}
		
//-----------------------------------------------------------------------------
// Name: CSocket::ConnectSocket()
// Desc: Connects us to the outside world
// 
//-----------------------------------------------------------------------------
int CSocket::ConnectSocket()
{
	/*
	OZINFO info;
	info.socket = m_Socket.Max;
	info.host = host;
	info.port = port;
	int optval, nRet;
	long netEvents;
	int numtries = 0;

	// Connect the socket
	if((m_Socket.Sock[0] = socket(AF_INET, SOCK_STREAM, 0)) == SOCKET_ERROR)
	{
		WriteCheckVal(WSAGetLastError());
		return -1;
	}

	// Acquire the Network info that we need
	m_Socket.Addr[0].sin_family = AF_INET;
	m_Socket.Addr[0].sin_port = htons(this->port);
	memset(m_Socket.Addr[0].sin_zero, '\0', sizeof m_Socket.Addr->sin_zero);
	if(server == true)
		m_Socket.Addr[0].sin_addr.s_addr = INADDR_ANY;
	else
	{
		m_Socket.Addr[0].sin_addr.s_addr = inet_addr(this->host);   // try and parse it if it is an IP address
		if ((unsigned long)m_Socket.Addr[0].sin_addr.s_addr == INADDR_NONE)
		{
			// Host isn't an IP address, try using DNS
			hostent* HE = gethostbyname(this->host);
			if (HE == 0)
			{
				SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,"%s\n","CSocket - Unable to resolve Server!");
				WSACleanup ();
				return 1;
			}
			m_Socket.Addr[0].sin_addr.s_addr = inet_addr(HE->h_addr_list[0]);
		}
	}

	// Check to see if there is more than 1 socket.
	if(m_Socket.Max > 1)
	{
		// Set the exclusive address option
		optval = 1;
		nRet = setsockopt(m_Socket.Sock[0], SOL_SOCKET, 0,
			(char *)&optval, sizeof(optval));

		// Bind it
		if(bind(m_Socket.Sock[0], (sockaddr *)&m_Socket.Addr[0], sizeof(m_Socket.Addr[0])) == SOCKET_ERROR)
		{
			// Check for any errors
			int r = WSAGetLastError();
			if(r != 0)
			{
				switch (r)
				{
				case WSANOTINITIALISED:
					SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,"%s\n","Socket Error - Socket not initialized!");
					break;
				case WSAENETDOWN:
					SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,"%s\n","Socket Error - Network Subsystem failure!");
					break;
				case WSAEADDRINUSE:
					SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,"%s\n","Socket Error - Address in use!");
					break;
				case WSAEINPROGRESS:
					SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,"%s\n","Socket Error - Socket call in progress!");
					break;
				case WSAEINVAL:
					SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,"%s\n","Socket Error - Socket not  bound!");
					break;
				case WSAEACCES:
					SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,"%s\n","Socket Error - Attempt to connect Datagram socket!");
					break;
				case WSAEADDRNOTAVAIL:
					SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,"%s\n","Socket Error - Not a valid Address!");
					break;
				case WSAENOBUFS:
					SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,"%s\n","Socket Error - No buffer space available!");
					break;
				case WSAENOTSOCK:
					SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,"%s\n","Socket Error - Not a socket!");
					break;
				case WSAEFAULT:
					SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,"%s\n","Socket Error - Socket fault!");
					break;
				}
			}
		}
		netEvents = FD_ACCEPT;
		nRet = WSAAsyncSelect(m_Socket.Sock[0], g_hWnd, RECVSOCK_EVENT, netEvents);
	}
	else
	{
		netEvents = (FD_CONNECT|FD_READ|FD_WRITE|FD_CLOSE);
		// Connect it
connectretry:
		if(numtries > 3)
		{
			SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,"%s\n","Socket Error - Unnable to connect!");
			WSACleanup();
			return 0;
		}
		if(connect(m_Socket.Sock[0], (sockaddr *)&m_Socket.Addr[0], sizeof(sockaddr_in)) == SOCKET_ERROR)
		{
			int r = WSAGetLastError();
			if(r != 0)
			{
				switch (r)
				{
				case WSANOTINITIALISED:
					SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,"%s\n","Socket Error - Socket not initialized!");
					break;
				case WSAENETDOWN:
					SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,"%s\n","Socket Error - Network subsystem failure!");
					break;
				case WSAEADDRINUSE:
					SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,"%s\n","Socket Error - Socket's local address in use!");
					break;
				case WSAEINTR:
					SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,"%s\n","Socket Error - Blocking 1.1 Socket call was canceled!");
					break;
				case WSAEINPROGRESS:
					SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,"%s\n","Socket Error - A Blocking call is in progress!");
					break;
				case WSAEALREADY:
					SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,"%s\n","Socket Error - A connect is already in progress!");
					break;
				case WSAEADDRNOTAVAIL:
					SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,"%s\n","Socket Error - Remote address not valid!");
					break;
				case WSAEAFNOSUPPORT:
					SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,"%s\n","Socket Error - Addresses can't be used!");
					break;
				case WSAECONNREFUSED:
					SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,"%s\n","Socket Error - Connection attempt rejected!");
					numtries++;
					goto connectretry;
					break;
				case WSAEFAULT:
					SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,"%s\n","Socket Error - the name or namelen parameter is not valid!");
					break;
				case WSAEINVAL:
					SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,"%s\n","Socket Error - The socket is a listening socket!");
					break;
				case WSAEISCONN:
					SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,"%s\n","Socket Error - Socket is already connected!");
					break;
				case WSAENETUNREACH:
					SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,"%s\n","Socket Error - The network cannot be reached at this time!");
					numtries++;
					goto connectretry;
					break;
				case WSAENOBUFS:
					SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,"%s\n","Socket Error - No buffer space available!");
					break;
				case WSAENOTSOCK:
					SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,"%s\n","Socket Error - The descripter is not a socket!");
					break;
				case WSAETIMEDOUT:
					SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,"%s\n","Socket Error - Attempt to connect timed out!");
					numtries++;
					goto connectretry;
					break;
				case WSAEWOULDBLOCK:
					SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,"%s\n","Socket Error - This is a nonblocking socket!");
					break;
				case WSAEACCES:
					SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,"%s\n","Socket Error - Attempt failed!");
					break;
				default:
					return m_Socket.Sock[0];
				}
				WSACleanup();
				return 0;
			}
		}
		nRet = WSAAsyncSelect(m_Socket.Sock[0], g_hWnd, APPSOCK_EVENT, netEvents);
		if(m_Socket.Sock[0] == INVALID_SOCKET)
		{
			SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,"%s\n","Socket Error!");
			return 0;
}			
		//if(nRet == 0)
			m_Socket.isConnected[0] = true;
	}
	switch(nRet) {
		case WSANOTINITIALISED:
			SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,"%s\n","Socket - Not Initialized");
			break;
		case WSAENETDOWN:
			SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,"%s\n","Socket - Network subsystem failure");
			break;
		case WSAEINVAL:
			SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,"%s\n","Socket - Invalid parameter");
			break;
		case WSAEINPROGRESS:
			SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,"%s\n","Socket - Blocking Call in Progress");
			break;
		case WSAENOTSOCK:
			SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,"%s\n","Socket - Invalid Socket");
			break;
		default:
			SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,"%s\n","Socket - Connected and ready for use.");
			break;
	}
	*/
	return 0;
}

//-----------------------------------------------------------------------------
// Name: CSocket::CloseSocket()
// Desc: Closes a connection
// 
//-----------------------------------------------------------------------------
void CSocket::CloseSocket(int i)
{
	// Remove Socket from banlist only and only if it's not banned
	/*OZ_BANLIST *bList;
	OZ_BANLIST *pList = NULL;
	bList = &m_Socket.BList;
	while(bList->Next != NULL)
	{
		if(bList->Address == m_Socket.Addr)
		{
			if(bList->Count < 3)
				pList->Next = bList->Next;
			SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,"%s\n","Socket - Address found, closing");
		}
		pList = bList;
		bList = (OZ_BANLIST *)pList->Next;
	}*/
	
	// close it and INVALIDATE the socket
	/*
	if(m_Socket.Sock[i] == INVALID_SOCKET)
		return;
	closesocket(m_Socket.Sock[i]);
	SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,"%s\n","Socket - Closed!");
	m_Socket.Sock[i] = INVALID_SOCKET;
	m_Socket.isConnected[i] = false;
	ResetEvents(&m_Socket.Event[i]);
	m_Socket.Connect--;
	WriteCheckVal(m_Socket.Connect);
	*/
}

//-----------------------------------------------------------------------------
// Name: CSocket::WriteSocket()
// Desc: Writes out to the network
// 
//-----------------------------------------------------------------------------
void CSocket::WriteSocket(int i)
{
	// I'm gonna make this as simple as possible and hope it works
	// Just send it and rely on the callback function to handle the rest
/*	int nBytes = m_Socket.Datalen;
	selection = i;
	SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,"%s\n","Socket - Writing...");
	send(m_Socket.Sock[i], m_Socket.Data, m_Socket.Datalen, 0);*/
}

//-----------------------------------------------------------------------------
// Name: CSocket::ReadSocket()
// Desc: Reads incoming traffic
// 
//-----------------------------------------------------------------------------
int CSocket::ReadSocket(int i)
{
	// Hope it works :/
	int nBytes;
	/*
	memset((void *)&m_Socket.Data, '\0', 1024);
	nBytes = recv(m_Socket.Sock[i], m_Socket.Data, 1024, 0);
	m_Socket.Event[i].read = false;
	m_Socket.Data[nBytes] = '\0';
	printf("Socket - Reading... \n%s\n", m_Socket.Data);
	*/
	return nBytes;
}

//-----------------------------------------------------------------------------
// Name: CSocket::CheckSocket()
// Desc: Main Loop that handles Network events
// 
//-----------------------------------------------------------------------------
uint32_t CSocket::CheckSocket()
{
	// HaHa, the meat of the Socket object
	// I will require the Scripter to do all the calls so he/she will have to
	// ensure that they include this function in their script so they know 
	// what's goin on, and handle everyting accordingly.
	// I want to provide full functionality to the user without all the mess
	// to go with it.

	// We need these
	/*uint32_t Events;

	// Gets event info
	Events = WSAWaitForMultipleEvents((m_Socket.Max+1), m_Socket.Event, FALSE, 30, FALSE);
	switch(Events)
            {
            case WSA_WAIT_FAILED:
            case WAIT_IO_COMPLETION:
            case WSA_WAIT_TIMEOUT:
                break;

            default:
				m_Socket.NetworkEvents.lNetworkEvents = 0;
				WSAEnumNetworkEvents(m_Socket.Sock[Events], m_Socket.Event[Events], &m_Socket.NetworkEvents);
				if(FD_ACCEPT & m_Socket.NetworkEvents.lNetworkEvents)
				{
					printf("Socket - Connection requested");
					AcceptSocket();
				}
				return Events;
			}*/
	return false;
}

//-----------------------------------------------------------------------------
// Name: CSocket::AcceptSocket()
// Desc: Accepts an incoming connection, if and only if there is one open
//		  Make sure you ask for enough connections before getting here >:O
//-----------------------------------------------------------------------------
int CSocket::AcceptSocket()
{
	/*
	// Check the accept conditions to make sure this socket is acceptable
	SOCKET temp;
	sockaddr_in addr;
	printf("Socket - Accepting...");
	temp = WSAAccept(m_Socket.Sock[0], (sockaddr *)&addr, NULL, ConditionalAccept, (uint32_t)&m_Socket);
	
	// if temp isn't invalid, then the accept was successful,
	// assign it to a socket
	if(temp != INVALID_SOCKET)
	{
	    int i;
		for(i = 1; i <= (m_Socket.Max-1); i++)
		{
			if(m_Socket.Sock[i] == INVALID_SOCKET)
			{
				m_Socket.Sock[i] = temp;
				m_Socket.Addr[i] = addr;
				printf("Socket - Added to sockets.");
				break;
			}
		}
		WSAAsyncSelect(temp, g_hWnd, APPSOCK_EVENT, (FD_CONNECT|FD_READ|FD_WRITE|FD_CLOSE));
		m_Socket.Event[0].accept = false;
		return i;
	}
	*/
}

//-----------------------------------------------------------------------------
// Name: CSocket::ListenSocket()
// Desc: Wait, you hear that :O  Get the door, its Dominoes
//		  NO! it's an incoming socket
//-----------------------------------------------------------------------------
int CSocket::ListenSocket()
{
	/*
	// Listen for connections
	int nret = listen(m_Socket.Sock[0], SOMAXCONN);
	
	// Check for any errors
	if(nret == SOCKET_ERROR)
	{
		int r = WSAGetLastError();
			if(r != 0)
			{
				switch (r)
				{
				case WSANOTINITIALISED:
					printf("Socket Error - Socket not initialized!");
					break;
				case WSAENETDOWN:
					printf("Socket Error - Network Subsystem failure!");
					break;
				case WSAEADDRINUSE:
					printf("Socket Error - Address in use!");
					break;
				case WSAEINPROGRESS:
					printf("Socket Error - Socket call in progress!");
					break;
				case WSAEINVAL:
					printf("Socket Error - Socket not  bound!");
					break;
				case WSAEISCONN:
					printf("Socket Error - Socket is already connected!");
					break;
				case WSAEMFILE:
					printf("Socket Error - No more Sockets available!");
					break;
				case WSAENOBUFS:
					printf("Socket Error - No buffer space available!");
					break;
				case WSAENOTSOCK:
					printf("Socket Error - Not a socket!");
					break;
				case WSAEOPNOTSUPP:
					printf("Socket Error - Socket does not support listen!");
					break;
				}
			}
	}

	return nret;
	*/
	return 0;
}



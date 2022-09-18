#ifndef SOCKETLIBSOCKEWINDOWS_H
#define SOCKETLIBSOCKEWINDOWS_H

#include "../BasicLib/BasicLib.h"
#include "SocketLibTypes.h"
#include "SocketLibErrors.h"

namespace SocketLib
{
	class Socket;
	const UINT32 MAX_SOCKBUF = 256;
	const UINT32 MAX_SOCK_SENDBUF = 4096;
	const UINT32 MAX_WORKERTHREAD = 4;  
	const UINT32 MAX_CLIENTNUM = 64;

	enum class IocpEventType {
		AcceptEvent,
		SendEvent,
		RecvEvent
	};

	typedef struct _PER_IO_DATA_ {
		WSAOVERLAPPED		overlapped;
		WSABUF					m_wsaBuf;
		Socket*					Owner = nullptr;
		IocpEventType			event;
	} PER_IO_DATA;

	/*---------------------
	*	 IocpObject
	---------------------*/
	typedef struct _Context_ {
		sock					socket = INVALID_SOCKET;
		PER_IO_DATA		recv_data; //XXX TODO multiple wasrecv
		bool					is_connected{ false };
		char					mRecvBuf[2048];
	} Context;

}
#endif
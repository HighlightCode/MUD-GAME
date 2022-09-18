#include "SocketLibSocket.h"

namespace SocketLib
{
	/*---------------------------
	*		Basic Socket
	-----------------------------*/
	Socket::Socket(sock p_socket) : m_sock(p_socket)
	{
		if (p_socket != -1)
		{
			socklen_t s = sizeof(m_localInfo);
			getsockname(p_socket, (sockaddr*)(&m_localInfo), &s);
		}
		m_isblocking = true;
	}

	Socket::~Socket()
	{
		//Close();
	}

	void Socket::Close()
	{
#ifdef _WIN32
		closesocket(m_sock);
#else
		close(m_sock);
#endif
		m_sock = -1;
	}

	void Socket::SetBlocking(bool p_blockmode)
	{
#ifdef _WIN32
		unsigned long mode = !p_blockmode;
		m_sock = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, NULL, WSA_FLAG_OVERLAPPED);
		//err = ioctlsocket(m_sock, FIONBIO, &mode);
#else
		// get the flags
		int flags = fcntl(m_sock, F_GETFL, 0);

		// set or clear the non-blocking flag
		if (p_blockmode == false)
		{
			flags |= O_NONBLOCK;
		}
		else
		{
			flags &= ~O_NONBLOCK;
		}
		err = fcntl(m_sock, F_SETFL, flags);
#endif

		if (m_sock == -1)
		{
			throw Exception(GetError());
		}

		m_isblocking = p_blockmode;
	}

	bool Socket::BindIOCompletionPort(Context* _contextHandle)
	{
		auto hIOCP = CreateIoCompletionPort((HANDLE)_contextHandle->socket
			, mIOCPHandle
			, (ULONG_PTR)(_contextHandle), 0);

		if (NULL == hIOCP || mIOCPHandle != hIOCP)
		{
			throw Exception(GetError());
			return false;
		}

		return true;
	}

	void Socket::BindRecv()
	{
		
		DWORD dwFlag = 0;
		DWORD dwRecvNumBytes = 0;

		mContext.recv_data.m_wsaBuf.len = MAX_SOCKBUF;
		mContext.recv_data.m_wsaBuf.buf = mContext.mRecvBuf;
		mContext.recv_data.event = IocpEventType::RecvEvent;
		//mContext.Owner = this;

		if (m_sock == -1) 
		{
			printf("[WARNING] INVALID SOCKET VALUE");
		}

		int nRet = WSARecv(mContext.socket,
			&(mContext.recv_data.m_wsaBuf),
			1,
			&dwRecvNumBytes,
			&dwFlag,
			(LPWSAOVERLAPPED) & (mContext.recv_data),
			NULL);

		if (nRet == SOCKET_ERROR && (WSAGetLastError() != ERROR_IO_PENDING))
		{
			throw Exception(GetError());
		}
	}

	bool Socket::SendMsg(const int _dataSize, char* pMsg)
	{
		return false;
	}

	bool Socket::SendIO()
	{
		return false;
	}

	/*---------------------------
	*		Data Socket
	-----------------------------*/
	DataSocket::DataSocket(sock p_socket) : Socket(p_socket), m_connected(false)
	{
		if (p_socket != -1)
		{
			socklen_t s = sizeof(m_remoteinfo);
			getpeername(p_socket, (sockaddr*)(&m_remoteinfo), &s);
			m_connected = true;
		}
	}

	void DataSocket::Connect(ipAddress p_addr, port p_port)
	{
		int err;

		// already connected
		if (m_connected == true)
		{
			throw Exception(Error::EAlreadyConnected);
		}

		if (m_sock == -1)
		{
			m_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

			if (m_sock == -1)
			{
				throw Exception(GetError());
			}
		}

		m_remoteinfo.sin_family = AF_INET;
		m_remoteinfo.sin_port = htons(p_port);
		m_remoteinfo.sin_addr.s_addr = p_addr;
		memset(&(m_remoteinfo.sin_zero), 0, 8);

		socklen_t s = sizeof(struct sockaddr);
		err = connect(m_sock, (struct sockaddr*)(&m_remoteinfo), s);
		if (err == -1)
		{
			throw Exception(GetError());
		}

		m_connected = true;

		err = getsockname(m_sock, (struct sockaddr*)(&m_localInfo), &s);
		if (err != 0)
		{
			throw Exception(GetError());
		}
	}

	int DataSocket::Send(const char* p_buffer, int p_size)
	{
		int err = 0;

		/*if (m_connected == false)
		{
			throw Exception(Error::ENotConnected);
		}*/
#ifdef _WIN32
		ZeroMemory(mSendContext.mRecvBuf, sizeof(mSendContext.mRecvBuf));
		mSendContext.recv_data.m_wsaBuf.len = p_size;
		mSendContext.recv_data.m_wsaBuf.buf = mSendContext.mRecvBuf;
		CopyMemory(mSendContext.recv_data.m_wsaBuf.buf, p_buffer, p_size);
		mSendContext.recv_data.event = IocpEventType::SendEvent;

		DWORD dwRecvNumBytes = 0;
		int nRet = WSASend(mContext.socket,
			&(mSendContext.recv_data.m_wsaBuf),
			1,
			&dwRecvNumBytes,
			0,
			(LPWSAOVERLAPPED) &(mSendContext.recv_data), 
			NULL);

		//socket_error이면 client socket이 끊어진걸로 처리한다.
		if (nRet == SOCKET_ERROR && (WSAGetLastError() != ERROR_IO_PENDING))
		{
			printf("[ERROR] WSASend() FAILED : %d\n", WSAGetLastError());
			return false;
		}
#else
		err = send(m_sock, p_buffer, p_size, 0);
#endif
		if (err == -1)
		{
			Error e = GetError();
			if (e != Error::EOperationWouldBlock)
			{
				throw Exception(e);
			}

			err = 0;
		}

		// return the number of bytes successfully sent
		return err;
	}

	// Attempts to recieve data from a socket
	int DataSocket::Receive(char* p_buffer, int p_size)
	{
		int err;

		if (m_connected == false)
		{
			throw Exception(Error::ENotConnected);
		}

		err = recv(m_sock, p_buffer, p_size, 0);
		if (err == 0)
		{
			throw Exception(Error::EConnectionClosed);
		}
		if (err == -1)
		{
			throw Exception(GetError());
		}

		return err;
	}

	void DataSocket::Close()
	{
		if (m_connected == true)
		{
			shutdown(m_sock, 2);
		}

		// close the socket
		Socket::Close();

		m_connected = false;
	}

	/*---------------------------
	*	Listening Socket
	-----------------------------*/
	ListeningSocket::ListeningSocket()
	{
		m_listening = false;
		mIOCPHandle = INVALID_HANDLE_VALUE;
	}

	void ListeningSocket::Listen(port p_port)
	{
		int err;

		if (m_sock == -1)
		{

#ifdef _WIN32
			m_sock = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, NULL, WSA_FLAG_OVERLAPPED);
#else
#endif
			// throw an exception if the socket could not be created
			if (m_sock == -1)
			{
				throw Exception(GetError());
			}
		}

		int reuse = 1;
		err = setsockopt(m_sock, SOL_SOCKET, SO_REUSEADDR, (char*)(&reuse), sizeof(reuse));
		if (err != 0)
		{
			throw Exception(GetError());
		}

		// set up the socket address structure
		m_localInfo.sin_family = AF_INET;
		m_localInfo.sin_port = htons(p_port);
		m_localInfo.sin_addr.s_addr = htonl(INADDR_ANY);
		memset(&(m_localInfo.sin_zero), 0, 8);

		// bind the socket
		err = bind(m_sock, (struct sockaddr*)&m_localInfo, sizeof(struct sockaddr));
		if (err == -1)
		{
			throw Exception(GetError());
		}

		err = listen(m_sock, 8);
		if (err == -1)
		{
			throw Exception(GetError());
		}

		m_listening = true;
		printf("[INFO] LISTENING SERVER INITIALIZED. \n");
	}

	void ListeningSocket::Close()
	{
		Socket::Close();

#ifdef _WIN32
		WSACleanup();
#else
#endif
		// invalidate the variables
		m_listening = false;
	}

}
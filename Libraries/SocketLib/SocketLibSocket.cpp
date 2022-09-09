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
	}

	void Socket::Close()
	{
#ifdef WIN32
		closesocket(m_sock);
#else
		close(m_sock);
#endif
		m_sock = -1;
	}

	void Socket::SetBlocking(bool p_blockmode)
	{
		int err;

#ifdef WIN32
		unsigned long mode = !p_blockmode;
		err = ioctlsocket(m_sock, FIONBIO, &mode);
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

		if (err == -1)
		{
			throw Exception(GetError());
		}

		m_isblocking = p_blockmode;
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
		int err;

		// make sure the socket is connected first.
		if (m_connected == false)
		{
			throw Exception(Error::ENotConnected);
		}

		// attempt to send the data
		err = send(m_sock, p_buffer, p_size, 0);
		if (err == -1)
		{
			Error e = GetError();
			if (e != Error::EOperationWouldBlock)
			{
				throw Exception(e);
			}

			// if the socket is nonblocking, we don't want to send a terminal
			// error, so just set the number of bytes sent to 0, assuming
			// that the client will be able to handle that.
			err = 0;
		}

		// return the number of bytes successfully sent
		return err;
	}

	int DataSocket::Receive(char* p_buffer, int p_size)
	{
		int err;

		// make sure the socket is connected first.
		if (m_connected == false)
		{
			throw Exception(Error::ENotConnected);
		}

		// attempt to recieve the data
		err = recv(m_sock, p_buffer, p_size, 0);
		if (err == 0)
		{
			throw Exception(Error::EConnectionClosed);
		}
		if (err == -1)
		{
			throw Exception(GetError());
		}

		// return the number of bytes successfully recieved
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
	}

	void ListeningSocket::Listen(port p_port)
	{
		int err;

		// first try to obtain a socket descriptor from the OS, if
		// there isn't already one.
		if (m_sock == -1)
		{
			m_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

			// throw an exception if the socket could not be created
			if (m_sock == -1)
			{
				throw Exception(GetError());
			}
		}

		// set the SO_REUSEADDR option on the socket, so that it doesn't
		// hog the port after it closes.
		int reuse = 1;
		err = setsockopt(m_sock, SOL_SOCKET, SO_REUSEADDR,
			(char*)(&reuse), sizeof(reuse));
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

		// now listen on the socket. There is a very high chance that this will
		// be successful if it got to this point, but always check for errors
		// anyway. Set the queue to 8; a reasonable number.
		err = listen(m_sock, 8);
		if (err == -1)
		{
			throw Exception(GetError());
		}

		m_listening = true;
	}

	DataSocket ListeningSocket::Accept()
	{
		sock s;
		struct sockaddr_in socketaddress;

		// try to accept a connection
		socklen_t size = sizeof(struct sockaddr);
		s = accept(m_sock, (struct sockaddr*)&socketaddress, &size);
		if (s == -1)
		{
			throw Exception(GetError());
		}

		// return the newly created socket.
		return DataSocket(s);
	}

	void ListeningSocket::Close()
	{
		Socket::Close();

		// invalidate the variables
		m_listening = false;
	}

#ifdef WIN32
	IocpCore::IocpCore()
	{
		_iocpHandle = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);
	}

	IocpCore::~IocpCore()
	{
		::CloseHandle(_iocpHandle);
	}

	bool IocpCore::Register(IocpObject* iocpObject)
	{
		return ::CreateIoCompletionPort(iocpObject->GetHandle(), _iocpHandle, /*key*/0, 0);
	}

	bool IocpCore::Dispatch(int timeoutMs)
	{
		DWORD numOfBytes = 0;
		ULONG_PTR key = 0;
		IocpEventType* iocpEvent = nullptr;

		if (::GetQueuedCompletionStatus(_iocpHandle, OUT & numOfBytes, OUT & key, OUT reinterpret_cast<LPOVERLAPPED*>(&iocpEvent), timeoutMs))
		{
			IocpObject iocpObject = iocpEvent->owner;
			iocpObject->Dispatch(iocpEvent, numOfBytes);
		}
		else
		{
			int errCode = ::WSAGetLastError();
			switch (errCode)
			{
			case WAIT_TIMEOUT:
				return false;
			default:
				// TODO : ·Î±× Âï±â
				IocpObject piocpObject = iocpEvent->owner;
				iocpObject->Dispatch(iocpEvent, numOfBytes);
				break;
			}
		}

		return true;
	}
#endif
}
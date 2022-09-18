#ifndef SOCKETLIBSOCKET_H
#define SOCKETLIBSOCKET_H

#include <thread>
#include <vector>
#include "../BasicLib/BasicLib.h"
#include "SocketLibTypes.h"
#include "SocketLibErrors.h"

#ifdef _WIN32
	#include "SocketLibWindows.h"
#endif

namespace SocketLib
{
	// forward Declariration
	template<typename protocol, typename defaulthandler>
	class ConnectionManager;

	/*---------------------------
	*		Basic Socket
	-----------------------------*/
	class Socket
	{
	public:
		Socket(sock p_socket = -1);

		virtual ~Socket();

		inline sock GetSock() const { return m_sock; }

		inline port GetLocalPort() const { return ntohs(m_localInfo.sin_port); }

		inline ipAddress GetLocalAddress() const { return m_localInfo.sin_addr.s_addr; }

		void Close();

		void SetBlocking(bool p_blockmode);

	protected:
		sock m_sock;

#ifdef _WIN32
	public:
		HANDLE		mIOCPHandle;

		Context		mContext;

		Context		mSendContext;

		// function for connect iocp handle and socket
		bool BindIOCompletionPort(Context* _contextHandle);

	public:
		// Recv Functions For Iocp Event
		void BindRecv();

		bool SendMsg(const int _dataSize, char* pMsg);

		bool SendIO();

		virtual void Receive() abstract;

#endif

		struct sockaddr_in m_localInfo = {0, };

		bool m_isblocking;
	};

	/*---------------------------
	*		Data Socket
	-----------------------------*/
	class DataSocket : public Socket
	{
	public:
		DataSocket(sock p_socket = -1);

		~DataSocket() override = default;

		inline ipAddress GetRemoteAddress() const	 { return m_remoteinfo.sin_addr.s_addr; }

		inline port GetRemotePort() const { return ntohs(m_remoteinfo.sin_port); }

		inline bool IsConnected() const { return m_connected; }

		void Connect(ipAddress p_addr, port p_port);

		int Send(const char* p_buffer, int p_size);

		int Receive(char* p_buffer, int p_size);

		void Close();

		virtual void Receive() override { };

	protected:

		bool m_connected;

		struct sockaddr_in m_remoteinfo = { 0, }; // struct containing information about remote connection
	};

	/*---------------------------
	*	Listening Socket
	-----------------------------*/
	class ListeningSocket : public Socket
	{
	public:
		ListeningSocket();

		~ListeningSocket() override = default;

		void Listen(port p_port);

		inline bool IsListening() const { return m_listening; }

		void Close();

	protected:
		bool m_listening;

#ifdef _WIN32
	public:
		std::vector<std::thread> mWorkerThreads;

		virtual void Receive() override { };
#endif
	};
}

#endif
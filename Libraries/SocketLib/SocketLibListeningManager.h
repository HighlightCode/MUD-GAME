#ifndef LISTENINGMANAGER_H
#define LISTENINGMANAGER_H

#include "SocketLibTypes.h"
#include "SocketLibSocket.h"
#include "SocketLibErrors.h"
#include "SocketLibConnectionManager.h"
#include <vector>
#include <thread>

namespace SocketLib
{
	// Forward declarations
	template<typename protocol, typename defaulthandler>
	class ConnectionManager;

	template<typename protocol, typename defaulthandler>
	class ListeningManager
	{
	public:
		ListeningManager();

		~ListeningManager();

		void AddPort(port p_port);

		void SetConnectionManager(ConnectionManager<protocol, defaulthandler>* p_manager);

		void Listen();

	protected:

		//SocketSet m_set;

#ifdef _WIN32
		std::thread					mAcceptThread;

		HANDLE						mIOCPHandle;

		bool							mIsAccepterRun = true;

		ListeningSocket			mListeningsock;

		bool							BindIOCompletionPort(Context* _contextHandle);

		bool							CreateAccepterThread();

		void							AcceptThread();

#endif
		ConnectionManager<protocol, defaulthandler>* m_manager;
	};

	template<typename protocol, typename defaulthandler>
	ListeningManager<protocol, defaulthandler>::ListeningManager()
	{
		m_manager = nullptr;

#ifdef _WIN32
		mIOCPHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, MAX_WORKERTHREAD);
		
		if (NULL == mIOCPHandle)
		{
			throw Exception(GetError());
		}

		mIsAccepterRun = true;

		printf("[INFO] SERVER STARTED WITH %d NUMBER CLIENT \n", MAX_CLIENTNUM);
#else

#endif
	}

	template<typename protocol, typename defaulthandler>
	ListeningManager<protocol, defaulthandler>::~ListeningManager()
	{
#ifdef _WIN32
		CloseHandle(mIOCPHandle);
		WSACleanup();
		mIsAccepterRun = false;

		// Destroy Accept Thread
		if (mAcceptThread.joinable())
		{
			mAcceptThread.join();
		}

		printf("[INFO] DESTROY THREAD SUCCESSFULLY \n");
#endif
	}

	template<typename protocol, typename defaulthandler>
	void ListeningManager<protocol, defaulthandler>::AddPort(port p_port)
	{
		mListeningsock.Listen(p_port);
		
		bool bRet = CreateAccepterThread();

		if (false == bRet)
		{
			printf("[INFO] SERVER FAILED \n");
		}

		//m_sockets.push_back(mListeningsock);

		//m_set.AddSocket(lsock);
	}

	template<typename protocol, typename defaulthandler>
	void ListeningManager<protocol, defaulthandler>::
		SetConnectionManager(ConnectionManager<protocol, defaulthandler>* p_manager)
	{
		m_manager = p_manager;
		m_manager->SetIOCPHandle(mIOCPHandle);
	}


	template<typename protocol, typename defaulthandler>
	void ListeningManager<protocol, defaulthandler>::Listen()
	{
#ifdef _WIN32
		//mListeningsock.StartServer();
#endif
	}

	template<typename protocol, typename defaulthandler>
	bool ListeningManager<protocol, defaulthandler>::BindIOCompletionPort(Context* _contextHandle)
	{
		auto hIOCP = ::CreateIoCompletionPort((HANDLE)_contextHandle->socket
			, mIOCPHandle
			, (ULONG_PTR)(_contextHandle), 0);

		if (NULL == hIOCP || mIOCPHandle != hIOCP)
		{
			throw Exception(GetError());
			return false;
		}

		return true;
	}
	template<typename protocol, typename defaulthandler>
	bool ListeningManager<protocol, defaulthandler>::CreateAccepterThread()
	{
		mAcceptThread = std::thread([this]() {AcceptThread(); });

		printf("[INFO] ACCEPT THREAD INITIALIZED \n");

		return true;
	}
	template<typename protocol, typename defaulthandler>
	void ListeningManager<protocol, defaulthandler>::AcceptThread()
	{
		int nAddrLen = sizeof(SOCKADDR_IN);

		while (mIsAccepterRun)
		{
			sock s;
			struct sockaddr_in socketaddress;

			// try to accept a connection
			socklen_t size = sizeof(struct sockaddr);
			s = accept(mListeningsock.GetSock(), (struct sockaddr*)&socketaddress, &size);
			if (INVALID_SOCKET == s)
			{
				continue;
			}

			char clientIP[32] = { 0, };
			inet_ntop(AF_INET, &(socketaddress.sin_addr), clientIP, 32 - 1);
			printf("[INFO] IP(%s), PORT(%d) SOCKET ACCEPTED . \n", clientIP, socketaddress.sin_port);

			DataSocket acceptSocket(s);
			acceptSocket.mIOCPHandle = mIOCPHandle;
			acceptSocket.m_localInfo = socketaddress;
			acceptSocket.mContext.socket = s;
			acceptSocket.mSendContext.socket = s;
			BindIOCompletionPort(&(acceptSocket.mContext));
			//acceptSocket.m_connected = true;
			m_manager->NewConnection(acceptSocket);
		}
	}
}   
#endif
#ifndef CONNECTIONMANAGER_H
#define CONNECTIONMANAGER_H

#include <list>
#include <iostream>

#include "../ThreadLib/ThreadLib.h"
#include "SocketLibTypes.h"
#include "SocketLibErrors.h"
#include "SocketLibSocket.h"

namespace SocketLib
{
	template<typename protocol>
	class Connection;

	/*------------------------------
	*   Connection Manager
	-------------------------------*/
	template<typename protocol, typename defaulthandler>
	class ConnectionManager
	{

		typedef						std::list< Connection<protocol> >				clist;
		typedef typename			std::list< Connection<protocol> >::iterator		clistitr;

#ifdef _WIN32
	public:
		HANDLE						mIOCPHandle;

		int								numConnected = 0;

		bool							mIsWorkerRun = true;

		std::vector<std::thread> mWorkerThreads;

		inline void SetIOCPHandle(HANDLE _iocpHandle) { mIOCPHandle = _iocpHandle; }

		bool CreateWorkerThread();

		void WorkerThread();
#else
#endif

	public:

		ConnectionManager(int p_maxdatarate = 1024,
			int p_sentimeout = 60,
			int p_maxbuffered = 8192);

		~ConnectionManager();

		void NewConnection(DataSocket& p_socket);

		inline int AvailableConnections() const
		{
			return MAX_CLIENTNUM - (int)m_connections.size();
		}

		inline int TotalConnections() const
		{
			return (int)m_connections.size();
		}

		void CloseConnections();

		void Listen();

		void Send();

		inline void Manage()
		{
			Listen();
			Send();
			CloseConnections();
		}

	protected:
		void Close(clistitr p_itr);

	protected:

		clist m_connections;

		int m_maxdatarate;

		int m_sendtimeout;

		int m_maxbuffered;
	};

	template<typename protocol, typename defaulthandler>
	bool ConnectionManager<protocol, defaulthandler>::CreateWorkerThread()
	{
		unsigned int uiThreadId = 0;

		for (DWORD i = 0; i < MAX_WORKERTHREAD; i++)
		{
			mWorkerThreads.emplace_back([this]() {WorkerThread(); });
		}
		printf("[INFO] %d WORKER THREAD INITIALIZED \n", MAX_WORKERTHREAD);

		return true;
	}

	template<typename protocol, typename defaulthandler>
	void ConnectionManager<protocol, defaulthandler>::WorkerThread()
	{
		DWORD							bytes_transferred = 0;
		Context*							ctx_ptr = NULL;
		LPWSAOVERLAPPED			lp_overlapped = NULL;
		PER_IO_DATA*					per_io_ctx = NULL;

		while (mIsWorkerRun)
		{
			bytes_transferred	= 0;
			ctx_ptr				= NULL;
			lp_overlapped		= NULL;
			BOOL bSuccess		= TRUE;
			per_io_ctx			= NULL;

			bSuccess = GetQueuedCompletionStatus(mIOCPHandle,
				&bytes_transferred, (PULONG_PTR)&ctx_ptr,
				(LPOVERLAPPED*)&lp_overlapped, INFINITE);

			if (TRUE == bSuccess && 0 == bytes_transferred && NULL == lp_overlapped)
			{
				mIsWorkerRun = false;
				continue;
			}

			if (NULL == lp_overlapped)
			{
				continue;
			}

			if (FALSE == bSuccess || (0 == bytes_transferred && TRUE == bSuccess))
			{
				//printf("socket(%d) Á¢¼Ó ²÷±è\n", (int)pClientInfo->m_socketClient);
				//CloseSocket(pClientInfo);
				continue;
			}

			per_io_ctx = (PER_IO_DATA*)lp_overlapped;

			// IO EVENT 
			if (IocpEventType::RecvEvent == per_io_ctx->event)
			{
				//printf("%s \n", per_io_ctx->m_wsaBuf.buf);
				Socket* pSocket = per_io_ctx->Owner;
				Connection<protocol>* pConnection = reinterpret_cast<Connection<protocol>*>(pSocket);
				pConnection->Receive(bytes_transferred);
				pSocket->BindRecv();
			}
			else if (IocpEventType::SendEvent == per_io_ctx->event)
			{
				printf("[INFO] SEND COMPLETED ! \n");
				Socket* pSocket = per_io_ctx->Owner;
			}
		}
	}

	template<typename protocol, typename defaulthandler>
	ConnectionManager<protocol, defaulthandler>::
		ConnectionManager(int p_maxdatarate,
			int p_sentimeout,
			int p_maxbuffered)
	{
		m_maxdatarate = p_maxdatarate;
		m_sendtimeout = p_sentimeout;
		m_maxbuffered = p_maxbuffered;

		bool bRet = CreateWorkerThread();
		if (false == bRet) {
			printf("[INFO] CREATE WORKER THREAD FAILED \n");
		}
	}

	template<typename protocol, typename defaulthandler>
	ConnectionManager<protocol, defaulthandler>::~ConnectionManager()
	{
		for (clistitr itr = m_connections.begin(); itr != m_connections.end(); ++itr)
			itr->CloseSocket();

		mIsWorkerRun = false;
		for (auto& th : mWorkerThreads)
		{
			if (th.joinable())
			{
				th.join();
			}
		}
	}

	template<typename protocol, typename defaulthandler>
	void ConnectionManager<protocol, defaulthandler>::
		NewConnection(DataSocket& p_socket)
	{
		Connection<protocol> conn(p_socket);

		if (AvailableConnections() == 0)
		{
			defaulthandler::NoRoom(conn);

			conn.CloseSocket();
		}
		else
		{
			m_connections.emplace_back(conn);

			Connection<protocol>& c = *m_connections.rbegin();

			c.mContext.recv_data.Owner = &c;

			c.mSendContext.recv_data.Owner = &c;

			c.BindRecv();

			// Give the Default Handler to Connection.
			c.AddHandler(new defaulthandler(c));
		}
	}


	template<typename protocol, typename defaulthandler>
	void ConnectionManager<protocol, defaulthandler>::Close(clistitr p_itr)
	{
		p_itr->CloseSocket();

		m_connections.erase(p_itr);
	}


	template<typename protocol, typename defaulthandler>
	void ConnectionManager<protocol, defaulthandler>::Listen()
	{
		int socks = 0;
		if (TotalConnections() > 0)
		{
			//socks = m_set.Poll();
		}

		if (socks > 0)
		{
			clistitr itr = m_connections.begin();
			clistitr c;

			while (itr != m_connections.end())
			{
				c = itr++;

				/*if (m_set.HasActivity(*c))
				{
					try
					{
						c->Receive();

						if (c->GetCurrentDataRate() > m_maxdatarate)
						{
							c->Close();
							c->Handler()->Flooded();
							Close(c);
						}
					}
					catch (...)
					{
						c->Close();
						c->Handler()->Hungup();
						Close(c);
					}
				}*/
			}
		}
	}


	template<typename protocol, typename defaulthandler>
	void ConnectionManager<protocol, defaulthandler>::Send()
	{
		clistitr itr = m_connections.begin();
		clistitr c;

		while (itr != m_connections.end())
		{
			c = itr++;

			/*try
			{
				c->SendBuffer();

				if (c->GetBufferedBytes() > m_maxbuffered ||
					c->GetLastSendTime() > m_sendtimeout)
				{
					c->Close();
					c->Handler()->Hungup();
					Close(c);
				}
			}
			catch (...)
			{
				c->Close();
				c->Handler()->Hungup();
				Close(c);
			}*/
		}
	}


	template<typename protocol, typename defaulthandler>
	void ConnectionManager<protocol, defaulthandler>::CloseConnections()
	{
		clistitr itr = m_connections.begin();
		clistitr c;

		while (itr != m_connections.end())
		{
			c = itr++;

			if (c->Closed())
				Close(c);
		}
	}
}

#endif
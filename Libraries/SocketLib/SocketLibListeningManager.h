#ifndef CONNECTIONMANAGER_H
#define CONNECTIONMANAGER_H

#include <list>
#include <iostream>

#include "ThreadLib/ThreadLib.h"
#include "SocketLibTypes.h"
#include "SocketLibErrors.h"
#include "SocketSet.h"
#include "Connection.h"

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

        typedef                 std::list< Connection<protocol> >           clist;
        typedef typename    std::list< Connection<protocol> >::iterator clistitr;

    public:

        ConnectionManager(int p_maxdatarate = 1024,
            int p_sentimeout = 60,
            int p_maxbuffered = 8192);

        ~ConnectionManager();

        void NewConnection(DataSocket& p_socket);


        inline int AvailableConnections() const
        {
            return MAX - (int)m_connections.size();
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

        SocketSet m_set;
    };

    template<typename protocol, typename defaulthandler>
    ConnectionManager<protocol, defaulthandler>::
        ConnectionManager(int p_maxdatarate,
            int p_sentimeout,
            int p_maxbuffered)
    {
        m_maxdatarate = p_maxdatarate;
        m_sendtimeout = p_sentimeout;
        m_maxbuffered = p_maxbuffered;
    }

    template<typename protocol, typename defaulthandler>
    ConnectionManager<protocol, defaulthandler>::~ConnectionManager()
    {
        clistitr itr;

        for (itr = m_connections.begin(); itr != m_connections.end(); ++itr)
            itr->CloseSocket();
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
            m_connections.push_back(conn);

            Connection<protocol>& c = *m_connections.rbegin();

            c.SetBlocking(false);

            m_set.AddSocket(c);

            c.AddHandler(new defaulthandler(c));

        }
    }


    template<typename protocol, typename defaulthandler>
    void ConnectionManager<protocol, defaulthandler>::Close(clistitr p_itr)
    {
        m_set.RemoveSocket(*p_itr);

        p_itr->CloseSocket();

        m_connections.erase(p_itr);
    }


    template<typename protocol, typename defaulthandler>
    void ConnectionManager<protocol, defaulthandler>::Listen()
    {
        int socks = 0;
        if (TotalConnections() > 0)
        {
            socks = m_set.Poll();
        }

        if (socks > 0)
        {
            clistitr itr = m_connections.begin();
            clistitr c;

            while (itr != m_connections.end())
            {
                c = itr++;

                if (m_set.HasActivity(*c))
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
                }
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

            try
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
            }
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
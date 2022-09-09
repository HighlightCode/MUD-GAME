#ifndef CONNECTION_H
#define CONNECTION_H

#include <stack>
#include "BasicLib/BasicLib.h"
#include "SocketLibTypes.h"
#include "ConnectionManager.h"
#include "SocketLibSocket.h"

namespace SocketLib
{
    const static int BUFFERSIZE = 1024;
    const static int TIMECHUNK = 16;

    /*----------------------------------
    *       Class Connection
    ------------------------------------*/
    template< class protocol >
    class Connection : public DataSocket
    {

    public:
        Connection();
        Connection(DataSocket& p_socket);

    protected:
        void Initialize();

    public:

        BasicLib::sint64 GetLastSendTime() const;

        inline BasicLib::sint64 GetLastReceiveTime() const
        {
            return m_lastReceiveTime;
        }

        inline void Close() { m_closed = true; }

        inline void CloseSocket()
        {
            DataSocket::Close();

            ClearHandlers();
        }

        void BufferData(const char* p_buffer, int p_size);

        void SendBuffer();

        void Receive();

        inline int GetDataRate() const
        {
            return m_lastdatarate;
        }

        inline int GetCurrentDataRate() const
        {
            return m_datarate / TIMECHUNK;
        }

        inline int GetBufferedBytes() const
        {
            return (int)m_sendbuffer.size();
        }

        inline BasicLib::sint64 GetCreationTime() const
        {
            return m_creationtime;
        }

        inline protocol& Protocol() { return m_protocol; }
        inline bool Closed() { return m_closed; }

        inline void SwitchHandler(typename protocol::handler* p_handler)
        {
            if (Handler())
            {
                Handler()->Leave();    
                delete Handler();       
                m_handlerstack.pop();   
            }

            m_handlerstack.push(p_handler);
            p_handler->Enter();    
        }

        inline void AddHandler(typename protocol::handler* p_handler)
        {
            if (Handler())
                Handler()->Leave();
            m_handlerstack.push(p_handler);
            p_handler->Enter();    
        }

        inline void RemoveHandler()
        {
            Handler()->Leave();     
            delete Handler();      
            m_handlerstack.pop();   
            if (Handler())        
                Handler()->Enter();
        }


        inline typename protocol::handler* Handler()
        {
            if (m_handlerstack.size() == 0)
                return 0;
            return m_handlerstack.top();
        }


        void ClearHandlers()
        {
            if (Handler())
                Handler()->Leave();

            while (Handler())
            {
                delete Handler();
                m_handlerstack.pop();
            }
        }



    protected:

        protocol m_protocol;

        std::stack< typename protocol::handler* > m_handlerstack;

        std::string m_sendbuffer;

        int m_datarate;

        int m_lastdatarate;

        BasicLib::sint64 m_lastReceiveTime;

        BasicLib::sint64 m_lastSendTime;

        BasicLib::sint64 m_creationtime;

        bool m_checksendtime;

        char m_buffer[BUFFERSIZE];

        bool m_closed;
    };


    template< class protocol >
    Connection<protocol>::Connection()
    {
        Initialize();
    }

    template< class protocol >
    Connection<protocol>::Connection(DataSocket& p_socket)
        : DataSocket(p_socket)
    {
        Initialize();
    }


    template< class protocol >
    void Connection<protocol>::Initialize()
    {
        m_datarate = 0;
        m_lastdatarate = 0;
        m_lastReceiveTime = 0;
        m_lastSendTime = 0;
        m_checksendtime = false;
        m_creationtime = BasicLib::GetTimeMS();
        m_closed = false;
    }

    template< class protocol >
    BasicLib::sint64 Connection<protocol>::GetLastSendTime() const
    {
        if (m_checksendtime)
        {
            return BasicLib::GetTimeS() - m_lastSendTime;
        }

        return 0;
    }

    template< class protocol >
    void Connection<protocol>::BufferData(const char* p_buffer, int p_size)
    {
        m_sendbuffer.append(p_buffer, p_size);
    }

    template< class protocol >
    void Connection<protocol>::SendBuffer()
    {
        if (m_sendbuffer.size() > 0)
        {
            int sent = Send(m_sendbuffer.data(), (int)m_sendbuffer.size());
            m_sendbuffer.erase(0, sent);

            if (sent > 0)
            {
                m_lastSendTime = BasicLib::GetTimeS();
                m_checksendtime = false;
            }
            else
            {
                if (!m_checksendtime)
                {
                    m_checksendtime = true;
                    m_lastSendTime = BasicLib::GetTimeS();

                }
            }  

        }  
    }

    template< class protocol >
    void Connection<protocol>::Receive()
    {
        int bytes = DataSocket::Receive(m_buffer, BUFFERSIZE);

        BasicLib::sint64 t = BasicLib::GetTimeS();

        if ((m_lastReceiveTime / TIMECHUNK) != (t / TIMECHUNK))
        {
            m_lastdatarate = m_datarate / TIMECHUNK;
            m_datarate = 0;
            m_lastReceiveTime = t;
        }

        m_datarate += bytes;

        m_protocol.Translate(*this, m_buffer, bytes);
    }

}   

#endif
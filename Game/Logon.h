#ifndef SIMPLEMUDLOGON_H
#define SIMPLEMUDLOGON_H

#include "../Libraries/SocketLib/SocketLib.h"
#include "Game.h"
#include <string>

using SocketLib::Telnet;
using SocketLib::Connection;
using std::string;

namespace MUDGame
{
    enum class LogonState
    {
        NEWCONNECTION,
        NEWUSER,
        ENTERNEWPASS,
        ENTERPASS 
    };

    class Logon : public Telnet::handler
    {
        typedef Telnet::handler thandler;
    public:

        Logon(Connection<Telnet>& p_conn)
            : thandler(p_conn)
        {
            m_state = LogonState::NEWCONNECTION;    // Initial State
            m_errors = 0;
        }

        void Handle(string p_data);
        void Enter() ;
        void Leave() {};
        void Hungup()
        {
            std::cout << "[INFO] " << SocketLib::GetIPString(m_connection->GetRemoteAddress()) +
                " - hung up in login state. \n" ;
        };

        void Flooded()
        {
            std::cout << "[INFO] " << SocketLib::GetIPString(m_connection->GetRemoteAddress()) +
                " - flooded in login state." ;
        };

        //  This changes the game state so that the player enters the game.
        void GotoGame(bool p_newbie = false);

        // Tells There is no room
        static void NoRoom(Connection<Telnet>& p_connection)
        {
            static string msg = "Sorry, there is no more room on this server.\r\n";
            try
            {
                p_connection.Send(msg.c_str(), (int)msg.size());
            }
            catch (SocketLib::Exception)
            {
                // do nothing here; probably an exploiter if sending that data
                // causes an exception.
            }
        }

        static bool AcceptibleName(const string& p_name);

    protected:
        LogonState m_state;
        int m_errors;               
        string m_name;              
        string m_pass;            
    };  
}  

#endif
#ifndef MUDGAME_H
#define MUDGAME_H

#include "../Libraries/SocketLib/SocketLib.h"
#include "Player.h"
#include "Room.h"
#include "PlayerDatabase.h"
#include "DatabasePointer.h"
//#include "SimpleMUDLogs.h"
#include <string>

using SocketLib::Telnet;
using SocketLib::Connection;
using std::string;

namespace MUDGame
{
    /*------------------
    *       Game
    --------------------*/
    class Game : public Telnet::handler
    {
        typedef Telnet::handler thandler;
    public:

        /* CONSTRUCTOR */
        Game(Connection<Telnet>& p_conn, player p_player)
            : thandler(p_conn)
        {
            m_player = p_player;
        }

        /* /HANDLER FUNCTIONS */ 
        void            Handle(string p_data);
        void            Enter();
        void            Leave();
        void            Hungup();
        void            Flooded();
        void            GotoTrain();

        /* SEND FUNCTIONS */  
        static void     SendGlobal(const string& p_str);
        static void     SendGame(const string& p_str);
        static void     LogoutMessage(const string& p_reason);
        static void     Announce(const string& p_announcement);
        void             Whisper(string p_str, string p_player);
        
        /* STATUS PRINT FUNTIONS */ 
        static string   WhoList(const string& p_who);
        static string   PrintHelp(PlayerRank p_rank = REGULAR);
        string           PrintStats();
        string           PrintExperience();
        string           PrintInventory();

        /* MAP FUNCTIONS */
        string           PrintMap();
        string           PrintItemStats(const string& name);
        string           PrintSkills(PlayerRank p_rank);
            
        /*  INVENTORY FUNCTIONS */ 
        bool    UseItem(const string& p_item);
        bool    RemoveItem(string p_item);

        
        /* TIMER & RUN LOOP */
        inline static BasicLib::Timer& GetTimer() { return s_timer; }
        inline static bool& Running() { return s_running; }


        /* ROOM FUNCTIONS */
        static string PrintRoom(room p_room);
        static void SendRoom(string p_text, room p_room);
        void Move(int p_direction);
        void GetItem(string p_item);
        void DropItem(string p_item);

        /* ENEMY FUNCTIONS */
        static void EnemyAttack(enemy p_enemy);
        static void PlayerKilled(player p_player);
        void PlayerAttack(const string& p_enemy);
        static void EnemyKilled(enemy p_enemy, player p_player);

    protected:

        player m_player;

        string m_lastcommand;

        static BasicLib::Timer s_timer;
        
        static bool s_running;
    };   

}    

#endif
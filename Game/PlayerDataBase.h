#ifndef PLAYERDATABASE_H
#define PLAYERDATABASE_H


#include <cmath>
#include <string>
#include <map>
#include <set>

#include "EntityDatabase.h"
#include "Player.h"

namespace MUDGame
{

    class PlayerDatabase : public EntityDatabase<Player>
    {
    public:

        static bool Load();
        static bool Save();
        static bool AddPlayer(Player& p_player);

        // helpers
        static inline string PlayerFileName(const string& p_name);
        static void LoadPlayer(string p_name);
        static void SavePlayer(entityid p_player);

        static entityid LastID()
        {
            return m_map.rbegin()->first;
        }

        static iterator findactive(const std::string& p_name)
        {
            return BasicLib::double_find_if(
                begin(), end(), matchentityfull(p_name),
                matchentity(p_name), playeractive());
        }

        static iterator findloggedin(const std::string& p_name)
        {
            return BasicLib::double_find_if(
                begin(), end(), matchentityfull(p_name),
                matchentity(p_name), playerloggedin());
        }

        static void Logout(entityid p_player);

    };  

    inline string PlayerDatabase::PlayerFileName(const string& p_name)
    {
        return string("players/" + p_name + ".plr");
    }
}   

#endif
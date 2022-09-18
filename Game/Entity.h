#ifndef MUDENTITY_H
#define MUDENTITY_H

#include <string>
#include "../Libraries/BasicLib/BasicLib.h"

namespace MUDGame
{
    typedef unsigned int entityid;
    using std::string;

    /*-----------------
    *       Entity
    -------------------*/
    class Entity
    {
    public:

        inline Entity()
        {
            m_name = "UNDEFINED";
            m_id = 0;
        }

        inline string& Name() { return m_name; }

        inline string CompName() const
        {
            return BasicLib::LowerCase(m_name);
        }

        inline bool MatchFull(const string& p_str) const
        {
            return CompName() == BasicLib::LowerCase(p_str);
        }

        inline bool Match(const string& p_str) const
        {
            if (p_str.size() == 0)
                return true;

            string name = CompName();
            string search = BasicLib::LowerCase(p_str);

            size_t pos = name.find(search);

            while (pos != string::npos)
            {
                if (pos == 0 || m_name[pos - 1] == ' ')
                    return true;

                pos = name.find(search, pos + 1);
            }

            return false;
        }

        inline entityid& ID() { return m_id; }

    protected:
        string m_name;
        entityid m_id;
    };

    struct matchentityfull
    {
        matchentityfull(const string& p_str)
            : m_str(p_str) { /* do nothing */
        }

        bool operator() (const Entity& p_entity)
        {
            return p_entity.MatchFull(m_str);
        }

        bool operator() (Entity* p_entity)
        {
            return p_entity != nullptr && p_entity->MatchFull(m_str);
        }

        string m_str;
    };


    struct matchentity
    {
        matchentity(const string& p_str)
            : m_str(p_str) { /* do nothing */
        }

        bool operator() (const Entity& p_entity)
        {
            return p_entity.Match(m_str);
        }

        bool operator() (Entity* p_entity)
        {
            return p_entity != 0 && p_entity->Match(m_str);
        }

        string m_str;
    };

} 

#endif
#ifndef ITEMDATABASE_H
#define ITEMDATABASE_H


#include <string>
#include <map>

#include "EntityDatabase.h"
#include "Item.h"

namespace MUDGame
{
    /*----------------------
    *   ItemDatabase
    -----------------------*/
    class ItemDatabase : public EntityDatabase<Item>
    {
    public:
        // Load Item Data.
        static bool Load();
    };  

}   

#endif
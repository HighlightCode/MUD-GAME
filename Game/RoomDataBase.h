#ifndef ROOMDATABASE_H
#define ROOMDATABASE_H

#include <string>
#include <map>
#include <set>
#include <stdexcept>

#include "EntityDatabase.h"
#include "Room.h"
#include "DatabasePointer.h"

namespace MUDGame
{
    /*---------------------------
    *       RoomDataBase
    ----------------------------*/
    class RoomDatabase : public EntityDatabaseVector<Room>
    {
    public:
        static void LoadTemplates();
        //static void LoadData();
    };
}

#endif
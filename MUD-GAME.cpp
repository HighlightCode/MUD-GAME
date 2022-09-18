
#include <iostream>
#include "Libraries/BasicLib/BasicLib.h"
#include "Libraries/SocketLib/SocketLib.h"
#include "Game/ItemDataBase.h"
#include "Game/RoomDataBase.h"
#include "Game/PlayerDataBase.h"
#include "Game/Logon.h"
#include "Game/Game.h"


using namespace MUDGame;

int main()
{
    ItemDatabase::Load();
    PlayerDatabase::Load(); 
    RoomDatabase::LoadTemplates();
    //RoomDatabase::LoadData();

    std::cout << BasicLib::TimeStamp() << std::endl;
    std::cout << BasicLib::DateStamp() << std::endl;

    BasicLib::Timer ts;
    std::cout << ts.GetString() << std::endl;

    //SocketLib::ListeningManager< lSocket;
    SocketLib::ListeningManager<SocketLib::Telnet, MUDGame::Logon> lm;
    SocketLib::ConnectionManager<SocketLib::Telnet, MUDGame::Logon> cm(128, 60, 2048);

    lm.SetConnectionManager(&cm);
    lm.AddPortW(5000);

    Game::GetTimer().Reset();
    Game::Running() = true;


	while (true)
	{
		std::string inputCmd;
		std::getline(std::cin, inputCmd);

		if (inputCmd == "quit")
		{
			break;
		}
	}

	//lSocket.DestroyThread();
}
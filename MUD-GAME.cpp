
#include <iostream>
#include "Libraries/BasicLib/BasicLib.h"
#include "Libraries/SocketLib/SocketLib.h"

int main()
{
    std::cout << "Hello World!\n";

    std::string name = "abcdefg";

    std::cout << BasicLib::TimeStamp() << std::endl;
    std::cout << BasicLib::DataStamp() << std::endl;

    BasicLib::Timer ts;
    std::cout << ts.GetString() << std::endl;

    SocketLib::ListeningSocket lSocket;

    SocketLib::DataSocket dsock;

    lSocket.Listen(5000);
    dsock = lSocket.Accept();

    char buffer[128] = "Hello there!";
    dsock.Send(buffer, strlen(buffer));
    dsock.Receive(buffer, 128);

}
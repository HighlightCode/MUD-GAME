#ifndef SOCKETLIBTYPES_H
#define SOCKETLIBTYPES_H


// Window
#ifdef _WIN32                 
    #pragma comment(lib,"ws2_32.lib")
    #include <WinSock2.h>
    #include <WS2tcpip.h>
#ifndef socklen_t
    typedef int socklen_t;
#endif

// UNIX/Linux
#else                       
    #include <sys/types.h>      
    #include <sys/socket.h>     
    #include <netinet/in.h>    
    #include <unistd.h>      
    #include <netdb.h>          
    #include <arpa/inet.h>      
    #include <errno.h>          
    #include <fcntl.h>          
#endif

namespace SocketLib
{
#ifdef _WIN32
    typedef SOCKET sock;
#else
    typedef int sock;
#endif

    typedef unsigned short int port;
    typedef unsigned long int ipAddress;
}

#endif
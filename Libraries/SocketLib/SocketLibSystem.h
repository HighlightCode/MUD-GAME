#ifndef SOCKETLIBSYSTEM_H
#define SOCKETLIBSYSTEM_H
#ifdef WIN32
	#define _WINSOCK_DEPRECATED_NO_WARNINGS
#endif
#include <string>
#include "SocketLibTypes.h"

namespace SocketLib
{
	ipAddress GetIpAddress(const std::string p_address);

	std::string GetIPString(ipAddress p_ipAddress);

	std::string GetHostNameString(ipAddress p_ipAddress);

	bool IsIPAddress(const std::string p_address);
}

#endif
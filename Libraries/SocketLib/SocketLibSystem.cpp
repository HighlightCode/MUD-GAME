#include "SocketLibSystem.h"
#include "SocketLibErrors.h"

namespace SocketLib
{
#ifdef _WIN32
	class System
	{
	public:
		System() 
		{
			int rWSAStartup = WSAStartup(MAKEWORD(2, 2), &m_WSAData);
			if (rWSAStartup == -1)
			{
				throw(Exception(GetError()));
			}
		}
		~System()
		{
			WSACleanup();
		}
		
	protected:
		WSADATA m_WSAData;
	};

	System gSystem;
#endif

	ipAddress GetIpAddress(const std::string p_address)
	{
		if (IsIPAddress(p_address))
		{
			ipAddress addr = inet_addr(p_address.c_str());

			if (addr == INADDR_NONE) {
				throw - 1;
			}
			return addr;
		}
		else 
		{
			struct hostent* host = gethostbyname(p_address.c_str());
			if (host == 0)
			{
				throw(Exception(GetError()));
			}
			return *((ipAddress*)host->h_addr);
		}
	}


	std::string GetIPString(ipAddress p_ipAddress)
	{
		char* str = inet_ntoa(*((in_addr*)&p_ipAddress));
		if (str == 0)
		{
			return std::string("Invalid IP Address");
		}
		return std::string(str);
	}

	std::string GetHostNameString(ipAddress p_ipAddress)
	{
		struct hostent* host = gethostbyaddr((char*)&p_ipAddress, 4, AF_INET);
		if (host == 0)
		{
			throw Exception(GetError(false));
		}
		return std::string(host->h_name);
	}

	bool IsIPAddress(const std::string p_address)
	{
		for (size_t i = 0; i < p_address.length(); i++)
		{
			if ((p_address[i] < '0' || p_address[i] > '9') &&
				p_address[i] != '.')
				return false;
		}
		return true;
	}
}

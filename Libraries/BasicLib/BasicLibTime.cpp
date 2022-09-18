#include "BasicLibTime.h"
#include "BasicLibString.h"
#include <chrono>
#include <time.h>
#include <ctime>
#include <sstream>

using namespace std::chrono;

namespace BasicLib
{
	sint64 GetTimeMs()
	{
		return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
	}

	sint64 GetTimeS()
	{
		return GetTimeMs() / 1000;
	}

	sint64 GetTimeM()
	{
		return GetTimeMs() / 60000;
	}

	sint64 GetTimeH()
	{
		return GetTimeMs() / 3600000;
	}

	std::string TimeStamp()
	{
		std::time_t result = std::time(nullptr);
		char buff[20] = { 0, };
		strftime(buff, 20, "%Y:%m:%d", localtime(&result));
		return std::string(buff);
	}

	std::string DateStamp()
	{
		std::time_t result = std::time(nullptr);
		char buff[20] = { 0, };
		strftime(buff, 20, "%H:%M:%S", localtime(&result));
		return std::string(buff);
	}


	Timer::Timer()
	{
		m_starttime = 0;
		m_inittime = 0;
	}

	void Timer::Reset(sint64 p_timepassed)
	{
		m_starttime = p_timepassed;
		m_inittime = GetMS();
	}

	sint64 Timer::GetMS()
	{
		return (GetTimeMs() - m_inittime) + m_starttime;;
	}

	sint64 Timer::GetS()
	{
		return GetMS() / 1000;
	}

	sint64 Timer::GetM()
	{
		return GetMS() / 60000;
	}

	sint64 Timer::GetH()
	{
		return GetMS() / 3600000;
	}

	sint64 Timer::GetD()
	{
		return GetMS() / 86400000;
	}

	sint64 Timer::GetY()
	{
		return GetD() / 365;
	}

	std::string Timer::GetString()
	{
		std::string str;
		sint64 y = GetY();
		sint64 d = GetD() % 365;
		sint64 h = GetH() % 24;
		sint64 m = GetM() % 60;

		if (y > 0)
			str += BasicLib::tostring(y) + " years, ";
		if (d > 0)
			str += BasicLib::tostring(d) + " days, ";
		if (h > 0)
			str += BasicLib::tostring(h) + " hours, ";

		str += BasicLib::tostring(m) + " minutes";

		return str;
	}

}

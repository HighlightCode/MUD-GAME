#ifndef THREADLIBFUNCTIONS_H
#define THREADLIBFUNCTIONS_H

#ifdef _WIN32 
	#include <windows.h>
	#include <map>
#else                       
	#include <pthread.h>
	#include <unistd.h>
#endif

#include "ThreadLibException.h"

namespace ThreadLib
{
	// Thread function pointer
	typedef void (*ThreadFunc)(void*);

#ifdef _WIN32
	typedef DWORD ThreadID;
	extern std::map< DWORD, HANDLE > g_handlemap;
#else
	typedef pthread_t ThreadID;
#endif

	class DummyData
	{
	public:
		ThreadFunc m_func;
		void* m_data;
	};

#ifdef _WIN32
	DWORD WINAPI DummyRun(void* p_data);
#else
	void* DummyRun(void* p_data);
#endif

    inline ThreadID Create(ThreadFunc p_func, void* p_param)
    {
        ThreadID t;
        DummyData* data = new DummyData;
        data->m_func = p_func;
        data->m_data = p_param;

#ifdef _WIN32    
        HANDLE h;
        h = CreateThread(NULL, 0, DummyRun, data, 0, &t);
        if (h != 0)
        {
            g_handlemap[t] = h;
        }
#else           
#endif

        if (t == 0)
        {
            delete data;
            throw Exception(Error::CreationFailure);
        }
        return t;
    }


    inline ThreadID GetID()
    {
#ifdef _WIN32
        return GetCurrentThreadId();
#else
        return pthread_self();
#endif
    }

    inline void WaitForFinish(ThreadID p_thread)
    {
#ifdef _WIN32
        WaitForSingleObject(g_handlemap[p_thread], INFINITE);

        CloseHandle(g_handlemap[p_thread]);

        g_handlemap.erase(p_thread);
#else
        pthread_join(p_thread, NULL);
#endif
    }

    inline void Kill(ThreadID& p_thread)
    {
#ifdef _WIN32
        TerminateThread(g_handlemap[p_thread], 0);

        CloseHandle(g_handlemap[p_thread]);

        g_handlemap.erase(p_thread);
#else
        pthread_cancel(p_thread);
#endif
    }

    inline void YieldThread(int p_milliseconds = 1)
    {
#ifdef _WIN32
        Sleep(p_milliseconds);
#else
        usleep(p_milliseconds * 1000);
#endif
    }

}
#endif
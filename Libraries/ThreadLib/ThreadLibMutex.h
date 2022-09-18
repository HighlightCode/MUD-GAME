#ifndef THREADLIBMUTEX_H
#define THREADLIBMUTEX_H

#ifdef _WIN32                
#include <windows.h>
#else                      
#include <pthread.h>
#endif

#include <string>

namespace ThreadLib
{

    /*---------------------------------------------------------------------------------
    * class Mutex / window: CriticalSection / Linux: pthread_mutex_t
    -----------------------------------------------------------------------------------*/
    class Mutex
    {
    public:
        Mutex()
        {
#ifdef _WIN32
            InitializeCriticalSection(&m_mutex);
#else
            pthread_mutex_init(&m_mutex, 0);
#endif
        }
        ~Mutex()
        {
#ifdef _WIN32
            DeleteCriticalSection(&m_mutex);
#else
            pthread_mutex_destroy(&m_mutex);
#endif
        }
        inline void Lock()
        {
#ifdef _WIN32
            EnterCriticalSection(&m_mutex);
#else
            pthread_mutex_lock(&m_mutex);
#endif
        }
        inline void Unlock()
        {
#ifdef _WIN32
            LeaveCriticalSection(&m_mutex);
#else
            pthread_mutex_unlock(&m_mutex);
#endif
        }

    protected:
#ifdef _WIN32
        CRITICAL_SECTION m_mutex;
#else
        pthread_mutex_t m_mutex;
#endif

    };  

}

#endif
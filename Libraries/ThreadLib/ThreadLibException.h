#ifndef THREADEXCEPTION_H
#define THREADEXCEPTION_H

#include <exception>

#ifdef WIN32                
#include <windows.h>
#else                    
#include <pthread.h>
#endif


namespace ThreadLib
{

    enum class Error
    {
        Unspecified,              
        InitFailure,                
        CreationFailure          
    };


    class Exception : public std::exception
    {
    public:
        Exception(Error p_error = Error::Unspecified)
        {
            m_error = p_error;
        }

        Error GetError() const
        {
            return m_error;
        }

    protected:
        Error m_error;
    };
}

#endif
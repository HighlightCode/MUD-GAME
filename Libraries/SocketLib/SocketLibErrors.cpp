#include "SocketLibErrors.h"


namespace SocketLib
{
    Error GetError(bool p_errno)
    {
#ifdef _WIN32
        return TranslateError(WSAGetLastError(), p_errno);
#else
        if (p_errno == true)
            return TranslateError(errno, p_errno);
        else
            return TranslateError(h_errno, p_errno);
#endif

        return Error::ESeriousError;
    }

#ifdef _WIN32
    Error TranslateError(int p_error, bool p_errno)
    {
        switch (p_error)
        {
        case WSAEINTR:
            return Error::EInterrupted;
        case WSAEACCES:
            return Error::EAccessDenied;
        case WSAEFAULT:
        case WSAEINVAL:
            return Error::EInvalidParameter;
        case WSAEMFILE:
            return Error::ENoSocketsAvailable;
        case WSAEWOULDBLOCK:
            return Error::EOperationWouldBlock;
        case WSAEINPROGRESS:
        case WSAEALREADY:
            return Error::EInProgress;
        case WSAENOTSOCK:
            return Error::EInvalidSocket;
        case WSAEDESTADDRREQ:
            return Error::EAddressRequired;
        case WSAEMSGSIZE:
            return Error::EMessageTooLong;
        case WSAEPROTOTYPE:
            return Error::EProtocolNotSupportedBySocket;
        case WSAENOPROTOOPT:
            return Error::EBadProtocolOption;
        case WSAEPROTONOSUPPORT:
            return Error::EProtocolNotSupported;
        case WSAESOCKTNOSUPPORT:
            return Error::EInvalidSocketType;
        case WSAEOPNOTSUPP:
            return Error::EOperationNotSupported;
        case WSAEPFNOSUPPORT:
            return Error::EProtocolFamilyNotSupported;
        case WSAEAFNOSUPPORT:
            return Error::EAddressFamilyNotSupported;
        case WSAEADDRINUSE:
            return Error::EAddressInUse;
        case WSAEADDRNOTAVAIL:
            return Error::EAddressNotAvailable;
        case WSAENETDOWN:
            return Error::ENetworkDown;
        case WSAENETUNREACH:
            return Error::ENetworkUnreachable;
        case WSAENETRESET:
            return Error::ENetworkReset;
        case WSAECONNABORTED:
            return Error::EConnectionAborted;
        case WSAECONNRESET:
            return Error::EConnectionReset;
        case WSAENOBUFS:
            return Error::ENoMemory;
        case WSAEISCONN:
            return Error::EAlreadyConnected;
        case WSAENOTCONN:
            return Error::ENotConnected;
        case WSAESHUTDOWN:
            return Error::EShutDown;
        case WSAETIMEDOUT:
            return Error::ETimedOut;
        case WSAECONNREFUSED:
            return Error::EConnectionRefused;
        case WSAEHOSTDOWN:
            return Error::EHostDown;
        case WSAEHOSTUNREACH:
            return Error::EHostUnreachable;
        case WSAHOST_NOT_FOUND:
            return Error::EDNSNotFound;
        case WSATRY_AGAIN:
            return Error::EDNSError;
        case WSANO_DATA:
            return Error::ENoDNSData;
        default:
            return Error::ESeriousError;
        }
    }
#else
    Error TranslateError(int p_error, bool p_errno)
    {
        if (p_errno == true)
        {
            switch (p_error)
            {
            case EINTR:
                return EInterrupted;
            case EACCES:
                return EAccessDenied;
            case EFAULT:
            case EINVAL:
                return EInvalidParameter;
            case EMFILE:
                return ENoSocketsAvailable;
            case EWOULDBLOCK:
                return EOperationWouldBlock;
            case EINPROGRESS:
            case EALREADY:
                return EInProgress;
            case ENOTSOCK:
                return EInvalidSocket;
            case EDESTADDRREQ:
                return EAddressRequired;
            case EMSGSIZE:
                return EMessageTooLong;
            case EPROTOTYPE:
                return EProtocolNotSupportedBySocket;
            case ENOPROTOOPT:
                return EBadProtocolOption;
            case EPROTONOSUPPORT:
                return EProtocolNotSupported;
            case ESOCKTNOSUPPORT:
                return EInvalidSocketType;
            case EOPNOTSUPP:
                return EOperationNotSupported;
            case EPFNOSUPPORT:
                return EProtocolFamilyNotSupported;
            case EAFNOSUPPORT:
                return EAddressFamilyNotSupported;
            case EADDRINUSE:
                return EAddressInUse;
            case EADDRNOTAVAIL:
                return EAddressNotAvailable;
            case ENETDOWN:
                return ENetworkDown;
            case ENETUNREACH:
                return ENetworkUnreachable;
            case ENETRESET:
                return ENetworkReset;
            case ECONNABORTED:
                return EConnectionAborted;
            case ECONNRESET:
                return EConnectionReset;
            case ENOBUFS:
                return ENoMemory;
            case EISCONN:
                return EAlreadyConnected;
            case ENOTCONN:
                return ENotConnected;
            case ESHUTDOWN:
                return EShutDown;
            case ETIMEDOUT:
                return ETimedOut;
            case ECONNREFUSED:
                return EConnectionRefused;
            case EHOSTDOWN:
                return EHostDown;
            case EHOSTUNREACH:
                return EHostUnreachable;
            default:
                return ESeriousError;
            }
        }
        else
        {
            switch (p_error)
            {
            case HOST_NOT_FOUND:
                return EDNSNotFound;
            case TRY_AGAIN:
                return EDNSError;
            case NO_DATA:
                return ENoDNSData;
            default:
                return ESeriousError;
            }
        }
    }
#endif

    Exception::Exception(Error p_code)
    {
        m_code = p_code;

        if (p_code == Error::ENotAvailable)
        {
            p_code = p_code;
        }
    }

    Error Exception::ErrorCode()
    {
        return m_code;
    }

    std::string Exception::PrintError()
    {
        switch (m_code)
        {
        case Error::EOperationWouldBlock:
            return "Nonblocking socket operation would have blocked";
        case Error::EInProgress:
            return "This operation is already in progress";
        case Error::EInvalidSocket:
            return "The socket was not valid";
        case Error::EAddressRequired:
            return "A destination address is required";
        case Error::EMessageTooLong:
            return "The message was too long";
        case Error::EProtocolNotSupported:
            return "The protocol is not supported";
        case Error::EProtocolNotSupportedBySocket:
            return "The socket type is not supported";
        case Error::EOperationNotSupported:
            return "The operation is not supported";
        case Error::EProtocolFamilyNotSupported:
            return "The protocol family is not supported";
        case Error::EAddressFamilyNotSupported:
            return "The operation is not supported by the address family";
        case Error::EAddressInUse:
            return "The address is already in use";
        case Error::EAddressNotAvailable:
            return "The address is not available to use";
        case Error::ENetworkDown:
            return "The network is down";
        case Error::ENetworkUnreachable:
            return "The destination network is unreachable";
        case Error::ENetworkReset:
            return "The network connection has been reset";
        case Error::EConnectionAborted:
            return "The network connection has been aborted due to software error";
        case Error::EConnectionReset:
            return "Connection has been closed by the other side";
        case Error::ENoMemory:
            return "There was insufficient system memory to complete the operation";
        case Error::EAlreadyConnected:
            return "The socket is already connected";
        case Error::ENotConnected:
            return "The socket is not connected";
        case Error::EShutDown:
            return "The socket has already been shut down";
        case Error::ETimedOut:
            return "The connection timed out";
        case Error::EConnectionRefused:
            return "The connection was refused";
        case Error::EHostDown:
            return "The host is down";
        case Error::EHostUnreachable:
            return "The host is unreachable";
        case Error::EDNSNotFound:
            return "DNS lookup is not found";
        case Error::EDNSError:
            return "Host not found due to error; try again";
        case Error::ENoDNSData:
            return "Address found, but has no valid data";
        case Error::EInterrupted:
            return "Operation was interrupted";
        case Error::ENoSocketsAvailable:
            return "No more sockets available";
        case Error::EInvalidParameter:
            return "Operation has an invalid parameter";
        case Error::EInvalidSocketType:
            return "Socket type is invalid";
        case Error::EAccessDenied:
            return "Access to this operation was denied";
        case Error::ESocketLimitReached:
            return "The manager has reached its maximum number of sockets";
        default:
            return "undefined or serious error";
        }
    }

}
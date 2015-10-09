/*
=============================================================================
	File:	Network.cpp
	Desc:
=============================================================================
*/
#include <Network_PCH.h>
#pragma hdrstop
#include <Network.h>

namespace Net
{

StaticAssert(sizeof(Socket) == sizeof(SOCKET));

U32 MakeAddress( U8 a, U8 b, U8 c, U8 d )
{
	return (a << 24) | (b << 16) | (c << 8) | d;
}

/**
    Return true if the string only contains characters which are in the defined
    character set.
*/
inline bool
CheckValidCharSet( const char* str, UINT strLength,
				  const char* charSet, UINT charSetLength )
{
	// for each character in the string
    for( UINT i = 0; i < strLength; i++)
    {
		int c = str[ i ];

		// if this character hasn't been found in the character set
		if( !strchr( charSet, c ) )
		{
			// the string contains invalid characters.
			return false;
		}
    }
	// all characters of the string are present in the character set
    return true;
}
inline bool
CheckValidCharSet( const FixedString& str, const ConstCharPtr& charSet )
{
	return CheckValidCharSet( str.ToChars(), str.Length(),
		charSet.ToChars(), charSet.Length() );
}

void
SplitIntoFour( const FixedString& str, const char* delimiter, TStaticList<FixedString,4> &tokens )
{
	tokens.Empty();
	char* ptr = const_cast<char*>(str.ToChars());
	const char* token;
	while (0 != (token = strtok( ptr, delimiter )))
	{
		tokens.Add(FixedString(_InitSlow,token));
		ptr = 0;
	}
}

MX_SWIPED("Nebula 3");
/**
    This method checks if the provided address is an "internet" address,
    not a LAN address (not a class A, B or C network address).
*/
bool
IsInetAddr(const in_addr* addr)
{
    // generate address string from addr
    FixedString addrString;
    addrString.Format("%d.%d.%d.%d", 
        addr->S_un.S_un_b.s_b1, 
        addr->S_un.S_un_b.s_b2, 
        addr->S_un.S_un_b.s_b3, 
        addr->S_un.S_un_b.s_b4); 

    // tokenize string into its members

	TStaticList<FixedString,4>	tokens;
	SplitIntoFour( addrString, ".", tokens );
    Assert(tokens.Num() == 4);

    int b1 = atof( tokens[0].ToChars() );
    int b2 = atof( tokens[1].ToChars() );
    int b3 = atof( tokens[2].ToChars() );
    if ((b1 == 10) && (b2 >= 0) && (b2 <= 254))
    {
        // Class A net
        return false;
    }
    else if ((b1 == 172) && (b2 >= 16) && (b2 <= 31))
    {
        // Class B net
        return false;
    }
    else if ((b1 == 192) && (b2 == 168) && (b3 >= 0) && (b3 <= 254))
    {
        // Class C net
        return false;
    }
    else if (b1 < 224)
    {
        // unknown other local net type
        return false;
    }
    // an internet address
    return true;
}

/**
    This resolves a host name into a IPv4 ip address. The ip address is
    returned in network byte order in the hostAddress argument. The return value
    indicates whether the operation was successful. The following special hostnames 
    can be defined:

    - "any"         resolves to INADDR_ANY (0.0.0.0)
    - "broadcast"   resolves to INADDR_BROADCAST (255.255.255.255)
    - "localhost"   resolves to 127.0.0.1
    - "self"        (NOT IMPLEMENTED ON XBOX360) resolves to the first address of this host
    - "inetself"    (NOT IMPLEMENTED ON XBOX360) resolves to the first address which is not a LAN address

    An empty host name is invalid. A hostname can also be an address string
    of the form xxx.yyy.zzz.www.

    NOTE: resolving host names and host addresses is not supported
    on the Xbox360, this basically means that an Xbox360 devkit can function
    as a server, but not as a client (this is fine for most debugging purposes).
*/
bool
GetHostByName( const FixedString& hostName, in_addr &outAddr )
{
    Assert(!hostName.IsEmpty());

    outAddr.S_un.S_addr = 0;

    if (hostName == "any")
    {
        outAddr.S_un.S_addr = htonl(INADDR_ANY);
        return true;
    }
    else if (hostName == "broadcast")
    {
        outAddr.S_un.S_addr = htonl(INADDR_BROADCAST);
        return true;
    }
    else if ((hostName == "self") || (hostName == "inetself"))
    {
        #if MX_PLATFORM_WIN32
            // get the machine's host name
            char localHostName[512];
            int err = gethostname(localHostName, sizeof(localHostName));
            if (SOCKET_ERROR == err)
            {
                return false;
            }

            // resolve own host name
            struct hostent* he = gethostbyname(localHostName);
            if (0 == he)
            {
                // could not resolve own host name
                return false;
            }

            // initialize with the default address 
            const in_addr* inAddr = (const in_addr *) he->h_addr;
            if (hostName == "inetself")
            {
                // if internet address requested, scan list of ip addresses
                // for a non-Class A,B or C network address
                int i;
                for (i = 0; (0 != he->h_addr_list[i]); i++)
                {
                    if (IsInetAddr((const in_addr *)he->h_addr_list[i]))
                    {
                        inAddr = (in_addr *)he->h_addr_list[i];
                        break;
                    }
                }
            }
            outAddr = *inAddr;
            return true;
        #else // __XBOX360__
            n_error("Win360IpAddress::GetHostByName(): self and inetself not implemented on Xbox360!");
            return false;
        #endif
    }
    else if( CheckValidCharSet(hostName, ".0123456789") )
    {
        // a numeric address...
        outAddr.S_un.S_addr = inet_addr(hostName.ToChars());
        return true;
    }
    else
    {
        #if MX_PLATFORM_WIN32
            // the default case: do a DNS name lookup
            struct hostent* he = gethostbyname(hostName.ToChars());
            if (0 == he)
            {
                // could not resolve host name!
                return false;
            }
            outAddr = *((in_addr*)he->h_addr);
            return true;
        #else // __XBOX360__
            n_error("Win360IpAddress::GetHostByName(): DNS name lookups not supported on Xbox360!");
            return false;
        #endif
    }
}

U32 HostNameToIpAddress( const char* hostName )
{
	in_addr addr;
	GetHostByName( FixedString(_InitSlow,hostName), addr );
	return addr.s_addr;
	StaticAssert( FIELD_SIZE(IPAddress,addr) == FIELD_SIZE(in_addr,s_addr) );
}

bool IsValidSocket( Socket s )
{
	return s != INVALID_SOCKET;
}

bool SetNonBlocking( Socket s )
{
	// Set I/O mode of the socket to non-blocking.
	ULONG	nonBlocking = 1;
	const int nret = ::ioctlsocket( s, FIONBIO, &nonBlocking );
	Assert( SOCKET_ERROR != nret );
	return nret == 0;
}
bool SetBlocking( Socket s, bool blocking )
{
	ULONG	nonBlocking = blocking ? 0 : 1;
	const int nret = ::ioctlsocket( s, FIONBIO, &nonBlocking );
	Assert( SOCKET_ERROR != nret );
	return nret == 0;
}
bool SetSendBufferSize( Socket s, UINT bufferSize )
{
	const int nret = ::setsockopt(
		s,
		SOL_SOCKET,
		SO_SNDBUF,
		(const char*) &bufferSize,
		sizeof(bufferSize)
	);
	Assert( SOCKET_ERROR != nret );
	return nret == 0;
}

void CloseSocket( Socket& s )
{
	if( s != INVALID_SOCKET )
	{
		const int nret = closesocket( s );
		if( nret != 0 ) {
			mxErrf("closesocket() failed with error '%s'.", NET_ErrorString());
		}
		s = INVALID_SOCKET;
	}
}

BOOL SocketCanBeRead( Socket s )
{
	fd_set readSet = { 1, { s } };
	TIMEVAL timeOut = { 0, 0 };

	const int res = ::select( 0, &readSet, 0, 0, &timeOut );

	if( SOCKET_ERROR == res )
	{
		mxErrf("select() failed with error '%s'.", NET_ErrorString());
		return FALSE;
	}
	else if( 0 == res )
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}

/**
    Receive raw data from a socket and write the received data into the
    provided buffer. On a blocking socket this method will block until
    data arrives at the socket. A non-blocking socket would immediately return in
    this case with a WouldBlock result. When valid data has been received
    the method will return with a Success result and the bytesReceived argument
    will contain the number of received bytes. It is not guaranteed that a single
    receive will return all data waiting on the socket.
*/
// returns the number of received bytes;
// returns 0 if connection closed, <0 in case of failure
//
int ReadBytesFromSocket( Socket from, void *buffer, UINT bufferSize )
{
	const int flags = 0;

	const int iResult = ::recv(
		from,
		(char*)buffer,
		bufferSize,
		flags
	);

	if( iResult == 0 )
	{
		// Connection closed
		return 0;
	}

	// Check if an error occurred
	if( iResult < 0 )
	{
		// catch special error conditions
		const int errorCode = WSAGetLastError();

		//  If not a single byte can be written to the socket in non-blocking mode
		//  we'll get an error (this may happen during the speculative write).
		if(
			(errorCode == WSAEWOULDBLOCK)	// Resource temporarily unavailable.
			|| (errorCode == WSAEINTR)		// Interrupted function call.
			|| (errorCode == WSAECONNRESET)	// the connection was forcibly closed by the remote peer
			|| (errorCode == WSAECONNABORTED)// connected aborted
			)
		{
			return 0;
		}
		/*
		if(errorCode == WSAENOTCONN)
		{
			cout << "socket not connected" << endl;
		}
		else if(errorCode == WSAESHUTDOWN )
		{
			cout << "socket has been shut down!" << endl;
		}
		*/
		mxErrf("recv() failed with error '%s'.", NET_ErrorString());
		return -1;
	}

	Assert( iResult >= MIN_PACKET_SIZE );

	NET_DBG_CODE(DBGOUT("got %u bytes\n",iResult));

	// Return the number of bytes received
	return iResult;
}

/**
    Send raw data into the socket. Note that depending on the buffer size
    of the underlying socket implementation and other sockets, the method
    may not be able to send all provided data. In this case, the returned 
    content of bytesSent will be less then numBytes, even though the
    return value will be Success. It is up to the caller to handle the
    extra data which hasn't been sent with the current call.
*/
// returns true if success;
//
BOOL WriteBytesToSocket( Socket to,
						const void* data, UINT numBytesToSend,
						UINT &bytesSent )
{
	bytesSent = 0;

	const int iResult = ::send(
		to,
		(const char*)data,
		numBytesToSend,
		0	// flags
	);

	if( iResult == 0 )
	{
		// Connection closed
		return FALSE;
	}

	// Check if an error occurred
	if( iResult < 0 )
	{
		// catch special error conditions
		const int errorCode = WSAGetLastError();

		//  If not a single byte can be written to the socket in non-blocking mode
		//  we'll get an error (this may happen during the speculative write).

		if( (errorCode == WSAEWOULDBLOCK)	// Resource temporarily unavailable.
			|| (errorCode == WSAEINTR)		// Interrupted function call.
			)
		{
			// operation on non-blocking socket is not complete
			// we'll write it later
			return TRUE;
		}

		if(
			(errorCode == WSAECONNABORTED)// connected aborted
			)
		{
			return FALSE;
		}

		// An error occurred
		mxErrf("send() failed with error '%s'.", NET_ErrorString());
		return FALSE;
	}

	// Bytes sent successfully

	bytesSent = iResult;

	NET_DBG_CODE(DBGOUT("sent %u bytes\n",bytesSent));

	// Bytes sent
	return TRUE;
}

//-------------------------------------------------------------------------------------------------------------//
/**
    This method converts an Windows Socket error code into a
    portable error code used by Win360Socket.
*/
ErrorCode
WSAErrorToErrorCode(int wsaErrorCode)
{
    switch (wsaErrorCode)
    {
        case WSAEINTR:              return ErrorInterrupted; break;            
        case WSAEACCES:             return ErrorPermissionDenied; break;       
        case WSAEFAULT:             return ErrorBadAddress; break;             
        case WSAEINVAL:             return ErrorInvalidArgument; break;        
        case WSAEMFILE:             return ErrorTooManyOpenFiles; break;       
        case WSAEWOULDBLOCK:        return ErrorWouldBlock; break;             
        case WSAEINPROGRESS:        return ErrorInProgress; break;             
        case WSAEALREADY:           return ErrorAlreadyInProgress; break;      
        case WSAENOTSOCK:           return ErrorNotASocket; break;             
        case WSAEDESTADDRREQ:       return ErrorDestAddrRequired; break;       
        case WSAEMSGSIZE:           return ErrorMsgTooLong; break;             
        case WSAEPROTOTYPE:         return ErrorInvalidProtocol; break;        
        case WSAENOPROTOOPT:        return ErrorBadProtocolOption; break;      
        case WSAEPROTONOSUPPORT:    return ErrorProtocolNotSupported; break;   
        case WSAESOCKTNOSUPPORT:    return ErrorSocketTypeNotSupported; break; 
        case WSAEOPNOTSUPP:         return ErrorOperationNotSupported; break;  
        case WSAEPFNOSUPPORT:       return ErrorProtFamilyNotSupported; break; 
        case WSAEAFNOSUPPORT:       return ErrorAddrFamilyNotSupported; break; 
        case WSAEADDRINUSE:         return ErrorAddrInUse; break;              
        case WSAEADDRNOTAVAIL:      return ErrorAddrNotAvailable; break;       
        case WSAENETDOWN:           return ErrorNetDown; break;                
        case WSAENETUNREACH:        return ErrorNetUnreachable; break;         
        case WSAENETRESET:          return ErrorNetReset; break;               
        case WSAECONNABORTED:       return ErrorConnectionAborted; break;      
        case WSAECONNRESET:         return ErrorConnectionReset; break;        
        case WSAENOBUFS:            return ErrorNoBufferSpace; break;          
        case WSAEISCONN:            return ErrorIsConnected; break;            
        case WSAENOTCONN:           return ErrorNotConnected; break;           
        case WSAESHUTDOWN:          return ErrorIsShutdown; break;             
        case WSAETIMEDOUT:          return ErrorIsTimedOut; break;             
        case WSAECONNREFUSED:       return ErrorConnectionRefused; break;      
        case WSAEHOSTDOWN:          return ErrorHostDown; break;               
        case WSAEHOSTUNREACH:       return ErrorHostUnreachable; break;        
        case WSAEPROCLIM:           return ErrorTooManyProcesses; break;       
        case WSASYSNOTREADY:        return ErrorSystemNotReady; break;         
        case WSAVERNOTSUPPORTED:    return ErrorVersionNotSupported; break;    
        case WSANOTINITIALISED:     return ErrorNotInitialized; break;         
        case WSAEDISCON:            return ErrorDisconnecting; break;          
        case WSATYPE_NOT_FOUND:     return ErrorTypeNotFound; break;           
        case WSAHOST_NOT_FOUND:     return ErrorHostNotFound; break;           
        case WSATRY_AGAIN:          return ErrorTryAgain; break;               
        case WSANO_RECOVERY:        return ErrorNoRecovery; break;             
        case WSANO_DATA:            return ErrorNoData; break;                 
        default:
			Unreachable;
            return ErrorUnknown; 
            break;
    }
}

//-------------------------------------------------------------------------------------------------------------//
/**
    Convert an error code to a human readable error message.
*/
const char*
ErrorAsString(ErrorCode err)
{
    switch (err)
    {
        case ErrorNone:                      return "No error.";
        case ErrorUnknown:                   return "Unknown error (not mapped by Win360Socket class).";
        case ErrorInterrupted:               return "Interrupted function call.";
        case ErrorPermissionDenied:          return "Permission denied.";
        case ErrorBadAddress:                return "Bad address.";
        case ErrorInvalidArgument:           return "Invalid argument.";
        case ErrorTooManyOpenFiles:          return "Too many open files (sockets).";
        case ErrorWouldBlock:                return "Operation would block.";
        case ErrorInProgress:                return "Operation now in progress.";
        case ErrorAlreadyInProgress:         return "Operation already in progress.";
        case ErrorNotASocket:                return "Socket operation on non-socket.";
        case ErrorDestAddrRequired:          return "Destination address required";
        case ErrorMsgTooLong:                return "Message too long.";
        case ErrorInvalidProtocol:           return "Protocol wrong type for socket.";
        case ErrorBadProtocolOption:         return "Bad protocol option.";
        case ErrorProtocolNotSupported:      return "Protocol not supported.";
        case ErrorSocketTypeNotSupported:    return "Socket type not supported.";
        case ErrorOperationNotSupported:     return "Operation not supported.";
        case ErrorProtFamilyNotSupported:    return "Protocol family not supported.";
        case ErrorAddrFamilyNotSupported:    return "Address family not supported by protocol family.";
        case ErrorAddrInUse:                 return "Address already in use.";
        case ErrorAddrNotAvailable:          return "Cannot assign requested address.";
        case ErrorNetDown:                   return "Network is down.";
        case ErrorNetUnreachable:            return "Network is unreachable.";
        case ErrorNetReset:                  return "Network dropped connection on reset.";
        case ErrorConnectionAborted:         return "Software caused connection abort.";
        case ErrorConnectionReset:           return "Connection reset by peer.";
        case ErrorNoBufferSpace:             return "No buffer space available.";
        case ErrorIsConnected:               return "Socket is already connected.";
        case ErrorNotConnected:              return "Socket is not connected.";
        case ErrorIsShutdown:                return "Cannot send after socket shutdown.";
        case ErrorIsTimedOut:                return "Connection timed out.";
        case ErrorConnectionRefused:         return "Connection refused.";
        case ErrorHostDown:                  return "Host is down.";
        case ErrorHostUnreachable:           return "No route to host.";
        case ErrorTooManyProcesses:          return "Too many processes.";
        case ErrorSystemNotReady:            return "Network subsystem is unavailable.";
        case ErrorVersionNotSupported:       return "Winsock.dll version out of range.";
        case ErrorNotInitialized:            return "Successful WSAStartup not yet performed.";
        case ErrorDisconnecting:             return "Graceful shutdown in progress.";
        case ErrorTypeNotFound:              return "Class type not found.";
        case ErrorHostNotFound:              return "Host not found.";
        case ErrorTryAgain:                  return "Non-authoritative host not found.";
        case ErrorNoRecovery:                return "This is a nonrecoverable error.";
        case ErrorNoData:                    return "Valid name, no data record of requested type.";
        default:
			Unreachable;
            return "Unknown error";
    }
}
//-------------------------------------------------------------------------------------------------------------//
const char* WSAErrorToChars(int wsaErrorCode)
{
	return ErrorAsString(WSAErrorToErrorCode(wsaErrorCode));
}
//-------------------------------------------------------------------------------------------------------------//
unsigned long IpAddressToNetworkByOrder( const char* ipAddrStr )
{
	// inet_addr converts a string with an IP address in dotted format to
    // a long value which is the IP in network byte order.
	return inet_addr(ipAddrStr);
}
//-------------------------------------------------------------------------------------------------------------//
unsigned long IpAddressToHostByOrder( const char* ipAddrStr )
{
	return ntohl(IpAddressToNetworkByOrder(ipAddrStr));
}
//-------------------------------------------------------------------------------------------------------------//
void IpAddressToChars( unsigned long addr, char buffer[32] )
{
	in_addr	inAddr;
	inAddr.S_un.S_addr = htonl( addr );

	// The inet_ntoa function converts an (Ipv4) Internet network address into an ASCII string in Internet standard dotted-decimal format.
	char* p = inet_ntoa( inAddr );
	mxStrCpyAnsi( buffer, p );
}

MX_SWIPED("idTech3");
/*
====================
NET_ErrorString
====================
*/
const char *NET_ErrorString( void )
{
	int code = WSAGetLastError();

	switch ( code ) {
	case WSAEINTR: return "WSAEINTR";
	case WSAEBADF: return "WSAEBADF";
	case WSAEACCES: return "WSAEACCES";
	case WSAEDISCON: return "WSAEDISCON";
	case WSAEFAULT: return "WSAEFAULT";
	case WSAEINVAL: return "WSAEINVAL";
	case WSAEMFILE: return "WSAEMFILE";
	case WSAEWOULDBLOCK: return "WSAEWOULDBLOCK";
	case WSAEINPROGRESS: return "WSAEINPROGRESS";
	case WSAEALREADY: return "WSAEALREADY";
	case WSAENOTSOCK: return "WSAENOTSOCK";
	case WSAEDESTADDRREQ: return "WSAEDESTADDRREQ";
	case WSAEMSGSIZE: return "WSAEMSGSIZE";
	case WSAEPROTOTYPE: return "WSAEPROTOTYPE";
	case WSAENOPROTOOPT: return "WSAENOPROTOOPT";
	case WSAEPROTONOSUPPORT: return "WSAEPROTONOSUPPORT";
	case WSAESOCKTNOSUPPORT: return "WSAESOCKTNOSUPPORT";
	case WSAEOPNOTSUPP: return "WSAEOPNOTSUPP";
	case WSAEPFNOSUPPORT: return "WSAEPFNOSUPPORT";
	case WSAEAFNOSUPPORT: return "WSAEAFNOSUPPORT";
	case WSAEADDRINUSE: return "WSAEADDRINUSE";
	case WSAEADDRNOTAVAIL: return "WSAEADDRNOTAVAIL";
	case WSAENETDOWN: return "WSAENETDOWN";
	case WSAENETUNREACH: return "WSAENETUNREACH";
	case WSAENETRESET: return "WSAENETRESET";
	case WSAECONNABORTED: return "WSWSAECONNABORTEDAEINTR";
	case WSAECONNRESET: return "WSAECONNRESET";
	case WSAENOBUFS: return "WSAENOBUFS";
	case WSAEISCONN: return "WSAEISCONN";
	case WSAENOTCONN: return "WSAENOTCONN";
	case WSAESHUTDOWN: return "WSAESHUTDOWN";
	case WSAETOOMANYREFS: return "WSAETOOMANYREFS";
	case WSAETIMEDOUT: return "WSAETIMEDOUT";
	case WSAECONNREFUSED: return "WSAECONNREFUSED";
	case WSAELOOP: return "WSAELOOP";
	case WSAENAMETOOLONG: return "WSAENAMETOOLONG";
	case WSAEHOSTDOWN: return "WSAEHOSTDOWN";
	case WSASYSNOTREADY: return "WSASYSNOTREADY";
	case WSAVERNOTSUPPORTED: return "WSAVERNOTSUPPORTED";
	case WSANOTINITIALISED: return "WSANOTINITIALISED";
	case WSAHOST_NOT_FOUND: return "WSAHOST_NOT_FOUND";
	case WSATRY_AGAIN: return "WSATRY_AGAIN";
	case WSANO_RECOVERY: return "WSANO_RECOVERY";
	case WSANO_DATA: return "WSANO_DATA";
	default: return "NO ERROR";
	}
}

/*
=============
Sys_StringToAdr

idnewt
192.246.40.70
12121212.121212121212
=============
*/
#define DO( src,dest )	  \
	copy[0] = s[src];	\
	copy[1] = s[src + 1];	\
	sscanf( copy, "%x", &val );	  \
	( (struct sockaddr_ipx *)sadr )->dest = val

BOOL Sys_StringToSockaddr( const char *s, struct sockaddr *sadr )
{
	struct hostent  *h;
	int val;
	char copy[MAX_STRING_CHARS];

	memset( sadr, 0, sizeof( *sadr ) );

	// check for an IPX address
	if ( ( strlen( s ) == 21 ) && ( s[8] == '.' ) ) {
		( (struct sockaddr_ipx *)sadr )->sa_family = AF_IPX;
		( (struct sockaddr_ipx *)sadr )->sa_socket = 0;
		copy[2] = 0;
		DO( 0, sa_netnum[0] );
		DO( 2, sa_netnum[1] );
		DO( 4, sa_netnum[2] );
		DO( 6, sa_netnum[3] );
		DO( 9, sa_nodenum[0] );
		DO( 11, sa_nodenum[1] );
		DO( 13, sa_nodenum[2] );
		DO( 15, sa_nodenum[3] );
		DO( 17, sa_nodenum[4] );
		DO( 19, sa_nodenum[5] );
	} else {
		( (struct sockaddr_in *)sadr )->sin_family = AF_INET;
		( (struct sockaddr_in *)sadr )->sin_port = 0;

		if ( s[0] >= '0' && s[0] <= '9' ) {
			*(int *)&( (struct sockaddr_in *)sadr )->sin_addr = inet_addr( s );
		} else {
			if ( ( h = gethostbyname( s ) ) == 0 ) {
				return 0;
			}
			*(int *)&( (struct sockaddr_in *)sadr )->sin_addr = *(int *)h->h_addr_list[0];
		}
	}

	return TRUE;
}

#undef DO



void PrintHostInfo()
{
	char chInfo[64];
	int nret = gethostname(chInfo,sizeof(chInfo));
	if( nret ) {
		mxErrf("gethostname() failed: %s",NET_ErrorString());
		return;
	}

	mxPutf("Host name: %s\n",chInfo);

	hostent* sh = gethostbyname( chInfo );
	if( sh )
	{
		mxPutf("Official host name: %s\n",sh->h_name);

		int nAdapter = 0;
		while(	sh->h_addr_list[ nAdapter ] )
		{
			sockaddr_in		adr;
			memcpy(
				&adr.sin_addr,
				sh->h_addr_list[nAdapter],
				sh->h_length
			);
			mxPutf("Host address: %s\n",inet_ntoa(adr.sin_addr));
			nAdapter++;
		}
	}
	else {
		mxErrf("gethostbyname() failed: %s",NET_ErrorString());
	}
}

Time GetTimeMS()
{
	// The timeGetTime function retrieves the system time, in milliseconds.
	// The system time is the time elapsed since Windows was started.
	MX_TODO("Network: this function only works if the server and clients are on the same machine; TODO: system-independent time");
	DWORD ret = ::timeGetTime();
	StaticAssert(sizeof(Time) == sizeof(ret));
	return ret;
}

void TimePeriodToSecMSec( Time startTime, Time endTime,
						 UINT &seconds, UINT& milliseconds )
{
	UINT msec = endTime - startTime;
	seconds = msec / 1000;
	msec -= seconds * 1000;
	milliseconds = msec;
}


BOOL SendBuffered( SOCKET to, PacketQueue & queuedPackets )
{
	PacketBuffer & packet = queuedPackets.First();

	const UINT bytesToSend = Min<UINT>( MAX_SEND_BUFFER_SIZE, packet.BytesToSend() );

	if( !bytesToSend ) {
		NET_DBG_CODE(DBGOUT("sent packet: %u bytes, %u packets left\n",
			queuedPackets.First().ActualMessageLength(),queuedPackets.NumPackets()-1));
		// remove the empty packet from the queue
		queuedPackets.RemoveFirst();
		return TRUE;
	}

	const BYTE* data = packet.Read( bytesToSend );

	UINT bytesSent;

	BOOL bOk = WriteBytesToSocket( to, data, bytesToSend, bytesSent );

	if( !bOk ) {
		return FALSE;
	}

	Assert( bytesSent >= 0 );

	//DBGOUT("sent %u bytes\n",bytesSent);

	return TRUE;
}

void clear_incoming( SOCKET from )
{
	int numBytesRead;
	byte buffer[256];
	do
	{
		numBytesRead = ReadBytesFromSocket( from, buffer, sizeof(buffer) );
	}
	while( numBytesRead > 0 );
}

}//namespace Net

/*
// Convert a u_short from host to TCP/IP network byte order.
u_short htons(u_short hostshort); //Host to network short

// Convert a u_long from host to TCP/IP network byte order.
u_long htonl(u_long hostlong); //Host to network long

// Convert a u_long from TCP/IP network order to host byte order.
u_short ntohs(u_short netshort); //Network to host short

// Convert a u_long from TCP/IP network order to host byte order.
u_long ntohl(u_long netlong); //Network to host long




//Return the IP address of a domain name

DECLARE_STDCALL_P(struct hostent *) gethostbyname(const char*);



//Convert a string address (i.e., "127.0.0.1") to an IP address. Note that  
//this function returns the address into the correct byte order for us so 
//that we do not need to do any conversions (see next section)

unsigned long PASCAL inet_addr(const char*);

*/

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//

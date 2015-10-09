// This is a precompiled header.  Include a bunch of common stuff.
// This is kind of ugly in that it adds a bunch of dependency where it isn't needed.
// But on balance, the compile time is much lower (even incrementally) once the precompiled
// headers contain these headers.

#pragma once

#include <Base/Base.h>
MX_USING_NAMESPACE;

#include <Core/Core.h>

#if !MX_PLATFORM_WIN32
#	error Unsupported platform!
#endif

// Windows headers
#include <winsock2.h>
//#include <mswsock.h>
#include <wsipx.h>
//#include <ws2tcpip.h>
//#include <ipexport.h>
//#include <process.h>
//#include <rpc.h>


//-------------------------------------------------------------------
//	Private stuff
//-------------------------------------------------------------------

#define MX_DEBUG_NETWORK		(0)

#if MX_DEBUG_NETWORK
	#define NET_DBG_CODE(x)		x
#else
	#define NET_DBG_CODE(x)
#endif



namespace Net
{

U32 HostNameToIpAddress( const char* hostName );

const char *NET_ErrorString( void );


class PacketQueue;
class PacketBuffer;

extern int SendBuffered( SOCKET to, PacketQueue & buffer );
//extern BOOL ReadBuffered( SOCKET from, QueuedPacket & packet );
extern void clear_incoming( SOCKET from );

}//namespace Net



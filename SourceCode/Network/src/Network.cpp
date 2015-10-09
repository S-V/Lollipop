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

local_ int g_nNetworkInitialized = 0;

//-------------------------------------------------------------------------------------------------------------//
bool mxNetworkIsReady() { return g_nNetworkInitialized > 0; }
//-------------------------------------------------------------------------------------------------------------//
// This is a one-time init for the Windows Sockets system.
//
bool mxInitializeNetwork()
{
	if( mxNetworkIsReady() ) {
		return true;
	}

	mxPutf("Initializing network engine...\n");

	// Initiate use of the WinSock DLL by this process.

	// The highest version of Windows Sockets specification
	// that the caller can use.
	// The high-order byte specifies the minor version number;
	// the low-order byte specifies the major version number.
	//
	WORD wVersionRequested = MAKEWORD(2,2);	// Use the MAKEWORD(lowbyte, highbyte) macro declared in Windef.h

	// A pointer to the WSADATA data structure that is to receive details of the Windows Sockets implementation.
	//
	WSADATA	wsaData;

	const int result = WSAStartup(
		wVersionRequested,
		&wsaData
	);
	if( result != 0 )
	{
		mxErrf("WSAStartup() failed with error '%s'.", WSAErrorToChars(result));
		/*
		NOTE:
		An application can call the WSAGetLastError function to determine
		the extended error code for other Windows sockets functions
		as is normally done in Windows Sockets even if the WSAStartup function
		fails or the WSAStartup function was not called to properly initialize
		Windows Sockets before calling a Windows Sockets function.
		The WSAGetLastError function is one of the only functions
		in the Winsock 2.2 DLL that can be called in the case of a WSAStartup failure.
		*/
	}
	else
	{
		// Confirm that the WinSock DLL supports 2.2.
		if( LOBYTE(wsaData.wVersion) != 2
			|| HIBYTE(wsaData.wVersion) != 2 )
		{
			mxWarn("Could not find a usable version of Winsock.dll");
		}

		++g_nNetworkInitialized;

		mxPutf(
			"%s, status: %s\n",
			wsaData.szDescription, wsaData.szSystemStatus
		);
	}

	return mxNetworkIsReady();
}
//-------------------------------------------------------------------------------------------------------------//
void mxShutdownNetwork()
{
	if( !mxNetworkIsReady() ) {
		return;
	}

	Assert(g_nNetworkInitialized > 0);

	--g_nNetworkInitialized;
	if( g_nNetworkInitialized > 0 ) {
		return;
	}

	// Shutdown Winsock.

	const int result = WSACleanup();
	if( result != 0 )
	{
		mxWarnf("WSACleanup() failed with error '%s'.", WSAErrorToChars(result));
	}

	mxPutf("Network engine deinitialized.\n");
}

//-------------------------------------------------------------------------------------------------------------//

/*================================
		IPAddress
================================*/

IPAddress::IPAddress()
{
	addr = 0;
	port = 0;
}

IPAddress::IPAddress( const char* hostName, UINT portNumber )
{
	addr = HostNameToIpAddress( hostName );
	port = portNumber;
}

/*================================
		HostInfo
================================*/

HostInfo::HostInfo()
{
	name.ZeroOut();

	// Retrieve the standard host name for the local computer.
	::gethostname( name.ToChars(), name.GetCapacity() );
}

/*================================
		ClientInfo
================================*/

ClientInfo::ClientInfo()
{
	data = nil;
	socket = INVALID_SOCKET;
}
ClientInfo::~ClientInfo()
{
}

}//namespace Net

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//

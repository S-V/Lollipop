/*
=============================================================================
	File:	TCPServer.cpp
	Desc:
=============================================================================
*/
#include <Network_PCH.h>
#pragma hdrstop
#include <Network.h>
#include "TCPUtil.h"
namespace Net
{

/*================================
		Server_TCP
================================*/

Server_TCP::Server_TCP()
{
	Clear();
}

Server_TCP::~Server_TCP()
{
	if( IsOpen() )
	{
		Close();
	}
}

void Server_TCP::Clear()
{
	mServerSocket = INVALID_SOCKET;
	mPort = 0;
	clients.Empty();
	isOpen = false;
}

bool Server_TCP::Open( UINT port )
{
	Assert(!isOpen);

	mxInitializeNetwork();

	Assert(mxNetworkIsReady());

	int	nret;

	// Create the listening socket.

	// The address family to use. Use AF_INET to use the address family of TCP & UDP.
	int addressFamily = AF_INET;	// AF_INET is used to specify the IPv4 address family.

	int socketType = SOCK_STREAM;	// This is a stream-oriented socket

	// The protocol to be used, this value depends on the address family.
	int protocol = IPPROTO_TCP;	// Use TCP rather than UDP

	// Create the listening socket for incoming connections.

	mServerSocket = socket(
		addressFamily,
		socketType,
		protocol
	);
	if( mServerSocket == INVALID_SOCKET )
	{
		mxErrf("socket() failed with error '%s'.", NET_ErrorString());
		return false;
	}

	// Bind socket to a port.

	// This is used to specify how the socket is used and contains the field for the IP and port.
	sockaddr_in	serverAddress;

	serverAddress.sin_family = AF_INET;

	//Accept a connection from any IP using INADDR_ANY
	//You could pass inet_addr("0.0.0.0") instead to accomplish the 
	//same thing. If you want only to watch for a connection from a 
	//specific IP, specify that instead.

	const char* szIP = NULL;	//INADDR_ANY

	if( szIP )
		serverAddress.sin_addr.s_addr = inet_addr( szIP );
	else
		serverAddress.sin_addr.s_addr = htonl( INADDR_ANY );

	serverAddress.sin_port = htons( port );


	nret = bind(
		mServerSocket,
		(struct sockaddr*)&serverAddress,
		sizeof(serverAddress)
	);
	if( SOCKET_ERROR == nret ) {
		// Failed to bind the socket to a port.
		mxErrf("bind() failed with error '%s'.", WSAErrorToChars(nret));
	}

	// Socket bound to port.

	this->mPort = port;


	nret = listen( mServerSocket, SOMAXCONN );
	if( SOCKET_ERROR == nret ) {
		mxErrf("listen() failed with error '%s'.", WSAErrorToChars(nret));
	}


	// Set I/O mode of the listening socket to non-blocking.

	SetNonBlocking( mServerSocket );



	mxPutf("Server initialized\n");


	isOpen = true;

	return true;
}
void Server_TCP::Close()
{
	Assert(isOpen);
	Assert(mxNetworkIsReady());

	for( UINT iClient = 0; iClient < clients.Num(); iClient++ )
	{
		CloseSocket( GetClientInfo( iClient ).socket );
	}
	clients.Clear();

	CloseSocket( mServerSocket );

	Clear();

	mxShutdownNetwork();

	isOpen = false;
}

bool Server_TCP::IsOpen() const
{
	return isOpen;
}

void Server_TCP::Tick()
{
	Assert(IsValidSocket(mServerSocket));

	//DBGOUT("Server_TCP::Tick: %u clients\n",clients.Num());

	int nret;

	// socket descriptor sets
	// fd_set is the set of sockets for which a given status is requested.

	fd_set	readables;	// the set of sockets to be checked for readability
	fd_set	writables;	// the set of sockets to be checked for writability
	fd_set	exceptions;	// the set of sockets to be checked for errors

	// Initializes the sets to the null set.
	FD_ZERO( &readables );
	FD_ZERO( &writables );
	FD_ZERO( &exceptions );

	// Adds the server socket to the input sets.
	FD_SET( mServerSocket, &readables );
	FD_SET( mServerSocket, &writables );
	FD_SET( mServerSocket, &exceptions );


	// Specify time to wait while determining the status of the sockets.
	TIMEVAL timeout;
	timeout.tv_sec = 0; // seconds
	timeout.tv_usec = 0; // microseconds

	for( UINT iClient = 0; iClient < clients.Num(); iClient++ )
	{
		SOCKET s = GetClientInfo( iClient ).socket;
		Assert(s != INVALID_SOCKET);
		// Adds the socket to the sets so that it will be checked..
		FD_SET( s, &readables );	//..for readability and...
		FD_SET( s, &writables );	//..for writability and...
		FD_SET( s, &exceptions );	//..for errors.
	}

	// The select function determines the status of one or more sockets,
	// waiting if necessary, to perform synchronous I/O.

	nret = select(
		0,	// Ignored. The nfds parameter is included only for compatibility with Berkeley sockets.
		&readables,	// An optional pointer to a set of sockets to be checked for readability.
		&writables,	// An optional pointer to a set of sockets to be checked for writability.
		&exceptions,// An optional pointer to a set of sockets to be checked for errors.
		&timeout	// The maximum time for select to wait, provided in the form of a TIMEVAL structure.
	);

	// The select function returns the total number of socket handles that are ready
	// and contained in the fd_set structures,
	// zero if the time limit expired,
	// or SOCKET_ERROR if an error occurred.
	// If the return value is SOCKET_ERROR, WSAGetLastError can be used to retrieve a specific error code.

	//if( nret == 0 || nret == SOCKET_ERROR )
	if( nret <= 0 )
	{
		if( g_bDebugMode ) {
			if( nret == SOCKET_ERROR ) {
				DBGOUT("error: %s\n",NET_ErrorString());
			}
		}
		return;
	}

	Assert( nret > 0 && nret != SOCKET_ERROR );

	// If there are sockets with activity,
	// the function returns the number of sockets that have activity.


	// Check for new client.

	// if serverSocket is contained in the readable set (i.e. can be read from)
	if( FD_ISSET( mServerSocket, &readables ) )
	{
		AcceptNewClient(); // we got one!
	}

	// Poll client connections.

	// run through the existing connections looking for data to read

	for( UINT iClient = 0; iClient < clients.Num(); iClient++ )
	{
		ClientInfo & clInfo = GetClientInfo( iClient );

		SOCKET s = clInfo.socket;
		Assert(s != INVALID_SOCKET);

		// Receive until the peer shuts down the connection
		// socket with errors
		if( FD_ISSET( s, &exceptions ) )
		{
			DEBUG_BREAK;
			DisconnectClient( iClient );
			continue;
		}

		// socket can be read from
		if( FD_ISSET( s, &readables ) )
		{
			// Receive until the peer closes the connection
			if( !TryToRead( iClient ) )
			{
				DisconnectClient( iClient );
				continue;
			}
		}

		// socket can be written to
		if( FD_ISSET( s, &writables ) )
		{
			if( !TryToWrite( iClient ) )
			{
				DisconnectClient( iClient );
				continue;
			}
		}
MX_TODO("Network: the above code doesn't work, need to disconnect GetClientInfo( iClient )automatically");

	}//for each client
}

void Server_TCP::AcceptNewClient()
{
	// Create a new connection.

	// address of the connecting entity
	sockaddr_in addressInfo;
	int addrLength = sizeof(addressInfo);

	SOCKET newSocket = accept(
		mServerSocket,
		cast(sockaddr*) &addressInfo,	// Address of a sockaddr structure
		&addrLength		// Address of a variable containing size of sockaddr struct
	);
	if( newSocket == INVALID_SOCKET ) {
		return;
	}

	SetNonBlocking( newSocket );

	// NOTE: this is important!
	// if the socket's send buffer is too large,
	// we'll have to deal with multiple arriving packets
	// and complicate things unnecessary.

	bool bOk = SetSendBufferSize( newSocket, MAX_SEND_BUFFER_SIZE );
	if( !bOk ) 
	{
		closesocket( newSocket );
		return;
	}

	// IPv4 address of the connecting entity

	FixedString	ipAddress;

	ipAddress.Format(
		"%d.%d.%d.%d",
		addressInfo.sin_addr.S_un.S_un_b.s_b1,
		addressInfo.sin_addr.S_un.S_un_b.s_b2,
		addressInfo.sin_addr.S_un.S_un_b.s_b3,
		addressInfo.sin_addr.S_un.S_un_b.s_b4
	);

	AddClientConnection( newSocket, ipAddress.ToChars() );
}

void Server_TCP::AddClientConnection( Socket newSocket, const char* host )
{
	// If connection is allowed...
	if( accept_client_connection( host ) )
	{
		UINT newClientIndex = clients.Num();

		ClientInfo* newClient = new ClientInfo();
		clients.Add( newClient );

		newClient->socket = newSocket;

		on_client_connected( newClientIndex, newClient->data );
	}
	else
	{
		// Connection not allowed.
		closesocket( newSocket );
	}
}

void Server_TCP::DisconnectClient( UINT iClient )
{
	if( iClient >= clients.Num() ) {
		return;
	}

	ClientInfo & clientInfo = GetClientInfo( iClient );

	Socket	clientSocket = clientInfo.socket;

	clear_incoming( clientSocket );

	clientInfo.readBuffer.Reset();
	clientInfo.writeBuffer.Reset();

	// When the server is done sending data to the client,
	// the shutdown function can be called specifying SD_SEND to shutdown the sending side of the socket.
	// This allows the client to release some of the resources for this socket.
	// The server application can still receive data on the socket.

	shutdown( clientSocket, SD_SEND );

	closesocket( clientSocket );

	clients.RemoveAt( iClient );

	on_client_disconnected( clientInfo.data );
}

BOOL Server_TCP::TryToRead( UINT iClient )
{
	ClientInfo & clientInfo = GetClientInfo( iClient );

	SOCKET	from = clientInfo.socket;

	BYTE	buffer[ MAX_SEND_BUFFER_SIZE ];

	// number of actually read bytes
	int numBytesRead = ReadBytesFromSocket( from, buffer, sizeof(buffer) );
	if( numBytesRead == 0 ) {
		// End of transmission
		return TRUE;
	}
	if( numBytesRead < 0 ) {
		// An error occurred
		return FALSE;
	}

	// numBytesRead of data have been read successfully

	// this is a new packet
	PacketBuffer & readBuffer = clientInfo.readBuffer;
	readBuffer.Append( buffer, numBytesRead );

	return ReadBufferedPackets< Server_TCP >( readBuffer, *this, &iClient );
}

bool Server_TCP::ReceivePacket( const RawPacket& packet, void* param )
{
	UINT clientIndex = *(UINT*)param;

	if( packet.Id() == PT_Disconnected )
	{
		DisconnectClient( clientIndex );
		return false;
	}

	ClientInfo & clientInfo = GetClientInfo( clientIndex );
	void* clientData = clientInfo.data;

	on_received_packet( packet, clientData, clientIndex );

	return true;
}

BOOL Server_TCP::TryToWrite( UINT iClient )
{
	ClientInfo & clientInfo = GetClientInfo( iClient );

	if( clientInfo.writeBuffer.NumPackets() )
	{
		SOCKET clientSocket = clientInfo.socket;

		return SendBuffered( clientSocket, clientInfo.writeBuffer );
	}
	//else
	//{
	//	// send a packet to check client connection

	//	RawPacket	packet;
	//	packet.header.size = sizeof(int);
	//	packet.header.id = PT_Peek;
	//	packet.header.time = Net::GetTimeMS();

	//	int dummy = 0;
	//	packet.data = &dummy;

	//	return SendPacket( iClient, packet );
	//}

	return TRUE;
}

BOOL Server_TCP::accept_client_connection( const char* host )
{
#if MX_DEBUG_NETWORK
	DBGOUT("Incoming connection from IP %s : %u\n",host,mPort);
#endif
	return TRUE;
}

void Server_TCP::on_client_connected( UINT clientNum, void *& userData )
{
#if MX_DEBUG_NETWORK
	cast((UINT&)userData) = clientNum;

	CalendarTime currTime = CalendarTime::GetCurrentLocalTime();

	String timeOfDay;
	GetTimeOfDayString(currTime.hour,currTime.minute,currTime.second,timeOfDay);
	DBGOUT("Accepted connection at %s\n",timeOfDay.ToChars());
#endif
}

void Server_TCP::on_client_disconnected( void* clientData )
{
#if MX_DEBUG_NETWORK
	DBGOUT("Disconnected client: %u\n",(UINT&)clientData);
#endif
}

void Server_TCP::on_received_packet( const RawPacket& p, void* clientData, UINT clientIndex )
{
#if MX_DEBUG_NETWORK
	char	tmp[256];
	UINT	len = Min( p.header.size, NUMBER_OF(tmp)-1 );
	MemCopy(tmp, p.data, len);
	tmp[ len ] = 0;

	UINT seconds, milliseconds;
	Net::TimePeriodToSecMSec( p.header.time, Net::GetTimeMS(), seconds, milliseconds );

	UINT numBytes = p.header.size;
	UINT speed = (numBytes / 1024) / Max<UINT>( seconds, 1 );

	DBGOUT("Received %u bytes in %u sec %u msec (%u KiB/sec) from client %u: %s...\n"
		,numBytes
		,seconds
		,milliseconds
		,speed
		,clientIndex
		,tmp
	);
#endif
}

PacketBuffer& Server_TCP::SendPacket( UINT clientNum )
{
	ClientInfo & clientInfo = GetClientInfo( clientNum );
	return clientInfo.writeBuffer.Add();
}

BOOL Server_TCP::SendPacket( UINT clientNum, const RawPacket& data )
{
	ClientInfo & clientInfo = GetClientInfo( clientNum );

	clientInfo.writeBuffer.Add( data );

	return TRUE;
}

SizeT Server_TCP::Write( const void* pBuffer, SizeT numBytes )
{
	RawPacket	packet;
	packet.header.size = numBytes;
	packet.header.id = PT_Skip;
	packet.header.time = Net::GetTimeMS();
	packet.data = cast(void*) pBuffer;

	for( UINT iClient = 0; iClient < clients.Num(); iClient++ )
	{
		SendPacket( iClient, packet );
	}

	return numBytes;
}

}//namespace Net

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//

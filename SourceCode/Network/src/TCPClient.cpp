/*
=============================================================================
	File:	TCPClient.cpp
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
		Client_TCP
================================*/

Client_TCP::Client_TCP()
{
	Clear();
}

Client_TCP::~Client_TCP()
{
	if( IsOpen() )
	{
		Close();

		mReadBuffer.Clear();
		mWriteBuffer.Clear();
	}
}

bool Client_TCP::Connect( const char *host, UINT port )
{
	Assert(!IsOpen());

	mClientSocket = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
	if( INVALID_SOCKET == mClientSocket )
	{
		mxWarnf("socket() failed with error '%s'.", NET_ErrorString());
		Close();
		return false;
	}

	sockaddr_in	serverAddress;
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons( port );

	ULONG addr = inet_addr( host );
	if( INADDR_NONE != addr )
	{
		serverAddress.sin_addr.s_addr = inet_addr( host );
	}
	else
	{
		hostent* remoteHost = gethostbyname( host );

		char* cp = inet_ntoa(*(struct in_addr*)*remoteHost->h_addr_list);
		serverAddress.sin_addr.s_addr = inet_addr( cp );
	}


	int nret;

	nret = ::connect( mClientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress) );
	if( SOCKET_ERROR == nret ) {
		mxWarnf("connect() failed with error '%s'.", NET_ErrorString());
		CloseSocket( mClientSocket );
		Close();
		return false;
	}

	SetNonBlocking( mClientSocket );

	// NOTE: this is important!
	// if the socket's send buffer is too large,
	// we'll have to deal with multiple arriving packets
	// and complicate things unnecessary.

	bool bOk = SetSendBufferSize( mClientSocket, MAX_SEND_BUFFER_SIZE );
	if( !bOk ) 
	{
		CloseSocket( mClientSocket );
		Close();
		return false;
	}

	return true;
}

void Client_TCP::Close()
{
	if( INVALID_SOCKET != mClientSocket )
	{
		Disconnect();
		CloseSocket( mClientSocket );
	}
	Clear();
}

bool Client_TCP::IsOpen() const
{
	return mClientSocket != INVALID_SOCKET;
}

BOOL Client_TCP::Tick()
{
	Assert(INVALID_SOCKET != mClientSocket);

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

	// Adds the client socket to the input sets.
	FD_SET( mClientSocket, &readables );
	FD_SET( mClientSocket, &writables );
	FD_SET( mClientSocket, &exceptions );


	// Specify time to wait while determining the status of the sockets.
	TIMEVAL timeout;
	timeout.tv_sec = 0; // seconds
	timeout.tv_usec = 0; // microseconds

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
	if( nret < 0 )
	{
		if( g_bDebugMode ) {
			if( nret == SOCKET_ERROR ) {
				mxErrf("select() failed: %s\n",NET_ErrorString());
			}
		}
		Disconnect();
		return FALSE;
	}

	if( FD_ISSET( mClientSocket, &exceptions ) )
	{
		// An error occurred
		Disconnect();
		return FALSE;
	}

	// socket can be read from
	if( FD_ISSET( mClientSocket, &readables ) )
	{
		// Receive until the peer closes the connection
		if( !TryToRead() )
		{
			Disconnect();
			return FALSE;
		}
	}

	// socket can be written to
	if( FD_ISSET( mClientSocket, &writables )
		&& mWriteBuffer.NumPackets() )
	{
		// Send the data until the peer closes the connection
		if( !TryToWrite() )
		{
			Disconnect();
			return FALSE;
		}
	}

	return TRUE;
}

BOOL Client_TCP::TryToWrite()
{
	Assert( mWriteBuffer.NumPackets() );
	return SendBuffered( mClientSocket, mWriteBuffer );
}

// send data to the server
BOOL Client_TCP::SendPacket( const RawPacket& data )
{
	Assert(IsValidSocket(mClientSocket));

	mWriteBuffer.Add( data );

	return TRUE;
}

void Client_TCP::Disconnect()
{
	if( mClientSocket != INVALID_SOCKET )
	{
		// tell the server that we're disconnecting
		RawPacket	msg;
		msg.header.size = 0;
		msg.header.id = PT_Disconnected;
		msg.header.time = Net::GetTimeMS();
		msg.data = nil;

		UINT bytesSent;
		WriteBytesToSocket( mClientSocket, &msg, sizeof msg, bytesSent );


		clear_incoming( mClientSocket );
		CloseSocket( mClientSocket );
		mClientSocket = INVALID_SOCKET;
	}
	mReadBuffer.Reset();
	mWriteBuffer.Reset();
}

BOOL Client_TCP::TryToRead()
{
	BYTE	buffer[ MAX_SEND_BUFFER_SIZE ];

	// number of actually read bytes
	int numBytesRead = ReadBytesFromSocket( mClientSocket, buffer, sizeof(buffer) );
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
	mReadBuffer.Append( buffer, numBytesRead );

	ReadBufferedPackets< Client_TCP >( mReadBuffer, *this, nil );

	return TRUE;
}

bool Client_TCP::ReceivePacket( const RawPacket& packet, void* param )
{
	(void)param;

	on_received_packet( packet );

	return true;
}

void Client_TCP::on_received_packet( const RawPacket& p )
{
#if MX_DEBUG_NETWORK
	char	tmp[256];
	UINT	len = Min( p.header.size, NUMBER_OF(tmp)-1 );
	MemCopy(tmp, p.data, len);
	tmp[ len ] = 0;

//	UINT packetNum = readBuffer.TotalNumPackets();

	UINT seconds, milliseconds;
	Net::TimePeriodToSecMSec( p.header.time, Net::GetTimeMS(), seconds, milliseconds );

	UINT numBytes = p.header.size;
	UINT speed = (numBytes / 1024) / Max<UINT>( seconds, 1 );

	DBGOUT("Received packet: %u bytes in %u sec %u msec (~%u KiB/sec)"
		": %s ...\n"
		,numBytes,seconds,milliseconds,speed
		,tmp
	);
#endif // MX_DEBUG_NETWORK
}

void Client_TCP::on_disconnected()
{
#if MX_DEBUG_NETWORK
	DBGOUT("Disconnected\n");
#endif
}

void Client_TCP::Clear()
{
	mClientSocket = INVALID_SOCKET;
	mReadBuffer.Reset();
	mWriteBuffer.Reset();
}

}//namespace Net

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//

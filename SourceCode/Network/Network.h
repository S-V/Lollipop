#pragma once

#if !MX_PLATFORM_WIN32
#	error Unsupported platform!
#endif

#include <Base/Templates/Containers/Queue/Queue.h>

namespace Net
{

bool mxInitializeNetwork();
void mxShutdownNetwork();
bool mxNetworkIsReady();

struct SetupNetworkUtil 
{
	SetupNetworkUtil()
	{
		mxInitializeNetwork();
	}
	~SetupNetworkUtil()
	{
		mxShutdownNetwork();
	}
};

/*
=======================================================================

	A wrapper around Windows sockets

=======================================================================
*/

/// protocol types
enum Protocol
{
	TCP,            // a reliable TCP connection
	UDP,            // unreliable UDP connection
};

// return values
enum Result
{
	Error = 0,      // an error occured, call GetErrorCode() or GetErrorString() for more info
	Success,        // everything ok
	WouldBlock,     // operation on non-blocking socket is not complete
	Closed,         // connection has been gracefully closed
};

/// error codes
enum ErrorCode
{
	ErrorNone,                   // no error occurred
	ErrorUnknown,                // an "unknown" error occurred     
	ErrorInterrupted,            // WSAEINTR
	ErrorPermissionDenied,       // WSAEACCESS
	ErrorBadAddress,             // WSAEFAULT
	ErrorInvalidArgument,        // WSAEINVAL
	ErrorTooManyOpenFiles,       // WSAEMFILE
	ErrorWouldBlock,             // WSAEWOULDBLOCK
	ErrorInProgress,             // WSAEINPROGRESS
	ErrorAlreadyInProgress,      // WSAEALREADY
	ErrorNotASocket,             // WSAENOTSOCK
	ErrorDestAddrRequired,       // WSAEDESTADDRREQ
	ErrorMsgTooLong,             // WSAEMSGSIZE
	ErrorInvalidProtocol,        // WSAEPROTOTYPE
	ErrorBadProtocolOption,      // WSAENOPROTOOPT
	ErrorProtocolNotSupported,   // WSEAPROTONOSUPPORT
	ErrorSocketTypeNotSupported, // WSAESOCKTNOSUPPORT
	ErrorOperationNotSupported,  // WSAEOPNOTSUPP
	ErrorProtFamilyNotSupported, // WSAEPFNOSUPPORT
	ErrorAddrFamilyNotSupported, // WSAEAFNOSUPPORT
	ErrorAddrInUse,              // WSAEADDRINUSE
	ErrorAddrNotAvailable,       // WSAEADDRNOTAVAIL
	ErrorNetDown,                // WSAENETDOWN
	ErrorNetUnreachable,         // WSAENETUNREACH
	ErrorNetReset,               // WSAENETRESET
	ErrorConnectionAborted,      // WSAECONNABORTED
	ErrorConnectionReset,        // WSAECONNRESET
	ErrorNoBufferSpace,          // WSAENOBUFS
	ErrorIsConnected,            // WSAEISCONN
	ErrorNotConnected,           // WSAENOTCONNECTED
	ErrorIsShutdown,             // WSAESHUTDOWN
	ErrorIsTimedOut,             // WSAETIMEDOUT
	ErrorConnectionRefused,      // WSAECONNREFUSED
	ErrorHostDown,               // WSAEHOSTDOWN
	ErrorHostUnreachable,        // WSAEHOSTUNREACH
	ErrorTooManyProcesses,       // WSAEPROCLIM
	ErrorSystemNotReady,         // WSAESYSNOTREADY
	ErrorVersionNotSupported,    // WSAEVERNOTSUPPORTED
	ErrorNotInitialized,         // WSAENOTINITIALISED
	ErrorDisconnecting,          // WSAEDISCONN
	ErrorTypeNotFound,           // WSAETYPE_NOT_FOUND
	ErrorHostNotFound,           // WSAEHOST_NOT_FOUND
	ErrorTryAgain,               // WSAETRY_AGAIN
	ErrorNoRecovery,             // WSAENO_RECOVERY
	ErrorNoData,                 // WSAENO_DATA
};

const char* ErrorAsString(ErrorCode err);
ErrorCode WSAErrorToErrorCode(int wsaErrorCode);
const char* WSAErrorToChars(int wsaErrorCode);

unsigned long IpAddressToNetworkByOrder( const char* ipAddrStr );
unsigned long IpAddressToHostByOrder( const char* ipAddrStr );

void IpAddressToChars( unsigned long addr, char buffer[32] );


/*
--------------------------------------------------------------
	Socket handle.
--------------------------------------------------------------
*/
typedef ULONG Socket;


bool IsValidSocket( Socket s );

// Set I/O mode of the socket to non-blocking.
bool SetNonBlocking( Socket s );

bool SetBlocking( Socket s, bool blocking );

// Specifies the total per-socket buffer space reserved for sends.
bool SetSendBufferSize( Socket s, UINT bufferSize );



#if MX_PLATFORM_WIN32

	//enum { MAX_SEND_BUFFER_SIZE = 8192 };
	enum { MAX_SEND_BUFFER_SIZE = (1<<16) }; // 65 Kb

#elif MX_PLATFORM == Platform_XBox360

	enum { MAX_SEND_BUFFER_SIZE = 1264 };	// from Xbox360 API documentation

#else

	#error Unsupported platform!

#endif


// returns the number of received bytes;
// returns 0 if connection closed, <0 in case of failure
//
int ReadBytesFromSocket( Socket from,
						void *buffer, UINT bufferSize );


// returns true if success;
//
BOOL WriteBytesToSocket( Socket to,
						const void* data, UINT numBytesToSend,
						UINT &bytesSent );


void CloseSocket( Socket& s );

/**
    This function checks if the socket has received data available.
	This function will never block.
*/
BOOL SocketCanBeRead( Socket s );

/// Peer type
enum PeerType
{
	PEER_SERVER = 1,
	PEER_CLIENT
};
/// Peer's connection state
enum ConnectionState
{
	CS_DISCONNECTED = 0,
	CS_DISCONNECTING,
	CS_CONNECTING,
	CS_CONNECTED
};
enum SocketMode
{
	SM_UNINIT = 0,		// uninitialized socket
	SM_BLOCKING,		// blocking
	SM_NONBLOCKING,		// use select before checking = non blocking
	SM_ASYNCH			// asynchronous, used with windows messages only (NOT UNIX)
};

/*
--------------------------------------------------------------
	Timing.
--------------------------------------------------------------
*/
typedef U32 Time;

Time GetTimeMS();

void TimePeriodToSecMSec( Time startTime, Time endTime,
						 UINT &seconds, UINT& milliseconds );

/*
=======================================================================

	PROTOCOL

=======================================================================
*/

// internal messages to identify packets
enum PacketType : U32
{
	PT_Skip = 0,

	PT_Peek,	// used for testing connection
	PT_Disconnected,	// the peer has disconnected

	// So I can add more without changing user enumerations
	PT_RESERVED_1,
	PT_RESERVED_2,
	PT_RESERVED_3,
	PT_RESERVED_4,
	PT_RESERVED_5,
	PT_RESERVED_6,
	PT_RESERVED_7,
	PT_RESERVED_8,
	PT_RESERVED_9,
	PT_RESERVED_10,

	// User types start here:
	PT_FirstUserId,
};

#pragma pack (push,1)

// packet header
struct PacketHdr
{
	U32		size;	// length of the message in bytes
	U32		id;		// used to identify the contents of this packet
	Time	time;	// when this packet was created (for measuring network speed, etc)
	U32		reserved;	// for padding to a multiple of 16 bytes
};

struct RawPacket
{
	PacketHdr	header;	// length in bytes
	void *		data;	// pointer to the message bytes

public:
	FORCEINLINE U32 Id() const { return header.id; }
	FORCEINLINE U32 MsgSize() const { return header.size; }
};

#pragma pack (pop)


// in host byte order
enum { DEFAULT_PORT = 33333 };

// minimum number of bytes that can be transferred ('quant')
enum { MIN_PACKET_SIZE = sizeof(PacketHdr) };

enum { MAX_PACKET_SIZE = 0x100 };

//enum { MAX_MESSAGE_LENGTH = 0x10000 };

/*
--------------------------------------------------------------
	ByteByffer
--------------------------------------------------------------
*/
class ByteByffer : public mxStreamWriter, public mxStreamReader
{
protected:
	TList< BYTE >	mBytes;
	U32	mReadOffset;

public:

	ByteByffer()
	{
		mReadOffset = 0;
	}

	// clears the buffer and resets read pointer
	void Reset()
	{
		mBytes.Empty();
		mReadOffset = 0;
	}

	void Clear()
	{
		mBytes.Clear();
		mReadOffset = 0;
	}

	// returns the total size of this packet (length of the message is stored in the first 32 bits)
	FORCEINLINE UINT TotalSize() const
	{
		return mBytes.GetDataSize();
	}
	
	FORCEINLINE bool IsEmpty() const
	{
		return !TotalSize();
	}

	void Append( const BYTE* ptr, SizeT numBytes )
	{
		const UINT currSize = TotalSize();
		const UINT newSize = currSize + numBytes;

		mBytes.Reserve( newSize );

		BYTE* dest = mBytes.ToPtr() + currSize;

		MemCopy( dest, ptr, numBytes );

		mBytes.SetNum_Unsafe( newSize );
	}

	OVERRIDES(mxStreamWriter) SizeT Write( const void* pBuffer, SizeT numBytes )
	{
		Append( (const BYTE*) pBuffer, numBytes );
		return numBytes;
	}

	OVERRIDES(mxStreamReader) SizeT Read( void *pBuffer, SizeT numBytes )
	{
		SizeT numBytesCanRead = Max<SizeT>( 0, Min<mxLong>( mBytes.Num() - mReadOffset, numBytes ) );
		if( !numBytesCanRead ) {
			return 0;
		}
		MemCopy( pBuffer, ToPtr() + mReadOffset, numBytesCanRead );
		mReadOffset += numBytesCanRead;
		return numBytesCanRead;
	}

	void AssureSize( U32 numBytes )
	{
		mBytes.Reserve( numBytes );
	}

	FORCEINLINE BYTE* ToPtr()
	{
		return mBytes.ToPtr();
	}
	FORCEINLINE const BYTE* ToPtr() const
	{
		return mBytes.ToPtr();
	}

	void ShiftLeft( U32 bytesLeft )
	{
		Assert( bytesLeft < TotalSize() );

		U32 offset = TotalSize() - bytesLeft;

		MemCopy( ToPtr(), ToPtr() + offset, bytesLeft );

		mBytes.SetNum_Unsafe( bytesLeft );
		mReadOffset = 0;
	}
};

/*
--------------------------------------------------------------
	PacketBuffer
--------------------------------------------------------------
*/
class PacketBuffer : public ByteByffer
{
public:

	MX_DECLARE_CLASS_ALLOCATOR( HeapNetwork, PacketBuffer );

	PacketBuffer()
	{
	}

	void Set( const RawPacket& packet )
	{
		const U32 headerSize = sizeof packet.header;
		const U32 messageSize = packet.header.size;
		const U32 totalSize = headerSize + messageSize;

		//Assert(messageSize > 0);

		// resize the buffer as needed
		mBytes.SetNum( totalSize );

		// store packet size (length of the message)
		PacketHdr & header = *(PacketHdr*)ToPtr();

		header.size = messageSize;
		header.id = packet.header.id;
		header.time = GetTimeMS();

		// store packet data
		MemCopy( ToPtr() + headerSize, packet.data, messageSize );

		mReadOffset = 0;
	}

	FORCEINLINE const PacketHdr& GetHeader() const
	{
		return *((PacketHdr*)ToPtr());
	}
	FORCEINLINE PacketHdr& GetHeader()
	{
		return *((PacketHdr*)ToPtr());
	}

	// returns the reported size of the message in this packet
	FORCEINLINE U32 WantedMessageLength() const
	{
		//Assert(mNum > MIN_PACKET_SIZE);
		return GetHeader().size;
	}

	// returns the actual size of the message in this packet;
	// this can never exceed that reported size of the message
	FORCEINLINE U32 ActualMessageLength() const
	{
		//Assert(mNum > MIN_PACKET_SIZE);
		StaticAssert(FIELD_OFFSET(RawPacket,data) == sizeof(PacketHdr));
		return TotalSize() - sizeof(PacketHdr);
	}

	FORCEINLINE bool IsComplete() const
	{
#if 0
		if( !ToPtr() ) {
			return false;
		}
		if( !TotalSize() ) {
			return false;
		}
#else
		Assert(!IsEmpty());
#endif
		U32 wantedSize = WantedMessageLength();
		U32 actualSize = ActualMessageLength();
		Assert( actualSize <= wantedSize );
		return wantedSize == actualSize;
	}

	FORCEINLINE BYTE* GetMessageBytesPtr()
	{
		return ToPtr() + sizeof(PacketHdr);
	}

	// how many bytes can be read?
	FORCEINLINE UINT BytesToRead() const
	{
		U32 currSize = TotalSize();
		if( !currSize ) {
			return 0;
		}
		if( mReadOffset >= currSize ) {
			return 0;
		}
		return currSize - mReadOffset;
	}

	// how many bytes are left to send
	FORCEINLINE UINT BytesToSend() const
	{
		U32 currSize = TotalSize();
#if 0
		if( !currSize ) {
			return 0;
		}
#else
		Assert(currSize != 0);
#endif
		return currSize - mReadOffset;
	}

	BYTE* CurrPtr()
	{
		return ToPtr() + mReadOffset;
	}

	// advances read pointer
	BYTE* Read( UINT bytes )
	{
		Assert( bytes > 0 );
		Assert( mReadOffset + bytes <= TotalSize() );
		BYTE* retVal = ToPtr() + mReadOffset;
		mReadOffset += bytes;
		return retVal;
	}
	void Skip( UINT bytes )
	{
		Assert( bytes > 0 );
		Assert( mReadOffset + bytes <= TotalSize() );
		mReadOffset += bytes;
	}
	PacketHdr& ReadHeader()
	{
		Assert(mReadOffset == 0);
		Skip( sizeof PacketHdr );
		return GetHeader();
	}

	UINT GetReadOffset() const { return mReadOffset; }
	bool AtStart() const { return mReadOffset == 0; }
	bool AtEnd() const { return mReadOffset >= mBytes.Num(); }

private:
	PREVENT_COPY(PacketBuffer);
};


/*
--------------------------------------------------------------
	PacketQueue
--------------------------------------------------------------
*/
class PacketQueue
{
	Queue< PacketBuffer* >	mPackets;

	UINT	mTotalNumPackets;

public:

	MX_DECLARE_CLASS_ALLOCATOR( HeapNetwork, PacketQueue );

	PacketQueue()
	{
		mTotalNumPackets = 0;
	}
	~PacketQueue()
	{
		for( UINT iPacket = 0; iPacket < mPackets.Num(); iPacket++ )
		{
			delete mPackets[ iPacket ];
		}
	}

	void Add( const RawPacket& packet )
	{
		PacketBuffer* newPacket = new PacketBuffer();
		mPackets.Add( newPacket );
		newPacket->Set( packet );
		++mTotalNumPackets;
	}
	PacketBuffer& Add()
	{
		PacketBuffer* newPacket = new PacketBuffer();
		mPackets.Add( newPacket );
		newPacket->Reset();
		++mTotalNumPackets;
		return *newPacket;
	}

	UINT NumPackets() const { return mPackets.Num(); }

	UINT TotalNumPackets() const { return mTotalNumPackets; }

	PacketBuffer & First()
	{
		return *mPackets.Peek();
	}

	void RemoveFirst()
	{
		First().Reset();
		mPackets.Deque();
	}

	void Reset()
	{
		for( UINT iPacket = 0; iPacket < mPackets.Num(); iPacket++ )
		{
			mPackets[ iPacket ]->Reset();
		}
		mPackets.SetNum_Unsafe( 0 );
	}
	void Clear()
	{
		mPackets.Clear();
	}

private:
	PREVENT_COPY(PacketQueue);
};


/*
--------------------------------------------------------------

Server_TCP

A TCP server opens a socket and listens for connecting TcpClients.
This listen process happens in its own listener-thread
and thus doesn't block the application.
--------------------------------------------------------------
*/

struct ClientInfo : public ReferenceCounted
{
	void *	data;	// user-defined client data
	Socket	socket;	// client connection

	PacketBuffer  	readBuffer;	// only one packet at a time can be received
	PacketQueue 	writeBuffer;

public:

	MX_DECLARE_CLASS_ALLOCATOR( HeapNetwork, ClientInfo );

	ClientInfo();
	~ClientInfo();

private:
	PREVENT_COPY(ClientInfo);
};

class Server_TCP : public mxStreamWriter
{
public:
	Server_TCP();
	~Server_TCP();

	bool Open( UINT port );	// Startup
	void Close();	// Shutdown

	bool IsOpen() const;

	// should be called each frame
	void Tick();

	// Buffered I/O.
	BOOL SendPacket( UINT clientNum, const RawPacket& p );

	void DisconnectClient( UINT iClient );

	UINT NumClients() const { return clients.Num(); }


	// send to all clients
	OVERRIDES(mxStreamWriter) SizeT Write( const void* pBuffer, SizeT numBytes );

protected:

	// called when a client is attempting to connect to this server
	virtual BOOL accept_client_connection( const char* host );
	
	// called when the client has connected to this server
	virtual void on_client_connected( UINT clientNum, void *& userData );

	// called when the client has disconnected from this server
	virtual void on_client_disconnected( void* clientData );

	// the packet is removed from the queue
	virtual void on_received_packet( const RawPacket& p, void* clientData, UINT clientIndex );

public_internal:
	// enqueues a new packet for sending to the specified client
	PacketBuffer& SendPacket( UINT clientNum );

	bool ReceivePacket( const RawPacket& packet, void* param );

private:

	void AcceptNewClient();
	void AddClientConnection( Socket newSocket, const char* host );

	BOOL TryToRead( UINT iClient );
	BOOL TryToWrite( UINT iClient );

	void Clear();

public_internal:
	ClientInfo & GetClientInfo( UINT iClient )
	{
		return *clients[ iClient ];
	}

private:
	Socket	mServerSocket;	// listening socket
	UINT	mPort;

	MX_OPTIMIZE("linked list, ClientInfo inherits from ListItem");
	TList< TRefPtr< ClientInfo > >	clients;

	bool	isOpen;
};

/*
--------------------------------------------------------------
	Client_TCP
--------------------------------------------------------------
*/

class Client_TCP
{
public:
	Client_TCP();
	~Client_TCP();

	bool Connect( const char *host, UINT port );
	void Close();

	bool IsOpen() const;
	bool IsConnected() const { return IsOpen(); }

	// should be called each frame
	BOOL Tick();

	// send data to the server
	BOOL SendPacket( const RawPacket& data );

	void Disconnect();

protected:
	
	// the packet is removed from the queue
	virtual void on_received_packet( const RawPacket& p );

	// called when the client has disconnected from the server
	virtual void on_disconnected();

private:
	BOOL TryToRead();
	BOOL TryToWrite();

	void Clear();

public_internal:
	bool ReceivePacket( const RawPacket& packet, void* param );

	Socket GetSocket() const { return mClientSocket; }

private:
	Socket			mClientSocket;
	PacketBuffer	mReadBuffer;	// incoming data, only one packet at a time can be received
	PacketQueue		mWriteBuffer;
};

/*
--------------------------------------------------------------
	Connection_TCP
--------------------------------------------------------------
*/
class Connection_TCP
{
public:
	Connection_TCP();
	~Connection_TCP();

	template< typename TYPE >
	inline mxStreamWriter & operator << ( const TYPE& data )
	{
		// store data in a buffer
		this->sendStream->Write( &data, sizeof(TYPE) );
		
		// send the buffer in packets (each no more than MAX_SEND_BUFFER_SIZE bytes in size)
		Send( this->sendStream );
		
		return *this;
	}

	template< typename TYPE >
	inline mxStreamWriter & operator >> ( TYPE &data )
	{
		// collect incoming bytes in the buffer
		Recv( this->recvStream );
		
		// copy data from the buffer to the destination
		this->recvStream->Read( &data, sizeof(TYPE) );
		
		return *this;
	}

private:
	// streams for buffering
	mxMemoryStream	sendStream;
	mxMemoryStream	recvStream;
};

void Send( Socket dest, mxDataStream& stream );
void Recv( Socket src, mxDataStream &stream );

/*
-----------------------------------------------------------------------------
	IPAddress

	Represents an IP address, consisting of a IPv4 host address
	and a port number. Performs automatic name lookup on the host name.
	Can extract address information from an URI and    
	automatically converts host names to addresses, and offers the special 
	hostnames "localhost", "any", "broadcast", "self" and "inetself" where:

	- "localhost" will translate to 127.0.0.1
	- "any" will translate to INADDR_ANY, which is 0.0.0.0
	- "broadcast" will translate to INADDR_BROADCAST, which is 255.255.255.255
	- "self" will translate to the first valid TCP/IP address for this host
	(there may be more then one address bound to the host)
	- "inetself" will translate to the first host address which is not 
	a LAN address (which is not a class A, B, or C network) if none such
	exists the address will fall back to "self"
-----------------------------------------------------------------------------
*/
struct IPAddress
{
	U32	addr;	// IPv4 address in binary form (host byte order)
	U32	port;	// port number (host byte order)

public:
	IPAddress();
	IPAddress( const char* hostName, UINT portNumber );
};

/*
-----------------------------------------------------------------------------
	HostInfo
-----------------------------------------------------------------------------
*/
struct HostInfo
{
	FixedString	name;

public:
	HostInfo();
};

}//namespace Net


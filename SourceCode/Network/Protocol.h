/*
=============================================================================
	File:	Protocol.h
	Desc:	Network protocol for interprocess communication.
=============================================================================
*/

#ifndef __MX_NETWORK_PROTOCOL_H__
#define __MX_NETWORK_PROTOCOL_H__

#include <Network/Network.h>


namespace Net
{

// network message identifiers
enum ENetCmd : U32
{
	// BEGIN hardcoded command ids
	Net_MyId = Net::PacketType::PT_FirstUserId,

	NET_Command,	// command passed as a text string
	NET_Response,	// results of executed command

	//NET_FileTranser,

	// END hardcoded command ids

	// START dynamic command ids
	NET_FirstDynamicCmdId
};

class CommandIdDictionary
{
public:
	U32 RegisterCmdName( ConstCharPtr& name );

private:
};

/*
=======================================================================

	PROTOCOL

=======================================================================
*/



#pragma pack (push,1)

// command inputs must be POD-types (no virtual functions, no pointers, etc.)
struct NetCmdId
{
	const FixedString	Name;

protected:
	typedef NetCmdId	THIS_TYPE;

	NetCmdId( const ConstCharPtr& name )
		: Name(name)
	{}

	mxStreamWriter& Pack( mxStreamWriter& stream ) const
	{
		return stream.Pack( this->Name );
	}
	mxStreamReader& UnPack( mxStreamReader& stream )
	{
		FixedString & cmdName = const_cast< FixedString& >( this->Name );
		return stream.Unpack( cmdName );
	}
};

#pragma pack (pop)



// command timeout
struct Timeout
{
	UINT	seconds;
	UINT	milliseconds;

	Timeout()
	{
		seconds = 0;
		milliseconds = 15;
	}
	UINT Milliseconds() const
	{
		return seconds * 1000 + milliseconds;
	}
};



template< class TYPE >
void NetPack( Net::PacketBuffer & buffer, const TYPE& data, U32 packetId )
{
	buffer.Reset();

	Net::PacketHdr	hdr;
	//hdr.size = 0;	// will be determined later
	hdr.id = packetId;
	hdr.time = Net::GetTimeMS();

	// write header
	buffer.Pack( hdr );

	// write data
	buffer << data;

	// update header - write message length (packet size)
	buffer.GetHeader().size = buffer.TotalSize() - (sizeof hdr);
}


template< class TYPE >
void NetUnpack( Net::PacketBuffer & buffer, TYPE &data )
{
	Assert( buffer.AtStart() );

	// skip header
	buffer.Skip( sizeof Net::PacketHdr );

	// retrieve data
	buffer >> data;
}

/*
--------------------------------------------------------------
	NetCommandHelper
--------------------------------------------------------------
*/
class NetCommandHelper
{
	Net::PacketBuffer	writeBuffer;
	Net::PacketBuffer	readBuffer;

	UINT	timeOutMS;

public:

	template< class COMMAND >
	bool ExecuteCommand(
		Net::Socket clientSocket,
		COMMAND & command )
	{
		using namespace Net;

		// send request...

		writeBuffer.Reset();

		NetPack( writeBuffer, command.inputs, NET_Command );

#if MX_DEBUG
		const PacketHdr* hdr = (const PacketHdr*) writeBuffer.ToPtr();

		U32 msgLength = hdr->size;
		U32 packetSize = sizeof(PacketHdr) + msgLength;
		U32 packetId = hdr->id;
		(void)packetId;
#endif

		do
		{
			if( !SendBuffered( clientSocket, writeBuffer ) )
			{
				goto L_Error;
			}

		} while ( writeBuffer.BytesToSend() );


		// and wait for response...

		const UINT startTimeMS = Net::GetTimeMS();

		UINT deltaTime = UINT(-1);


		readBuffer.Reset();

		do
		{
			if( !ReadBuffered( clientSocket, readBuffer ) )
			{
				goto L_Error;
			}

			if( !readBuffer.IsEmpty() && readBuffer.IsComplete() )
			{
				PacketHdr& hdr = readBuffer.ReadHeader();
				Assert(hdr.id = NET_Response);

				readBuffer >> command.results;
				Assert(readBuffer.BytesToRead() == 0);

				//DBGOUT("Command '%s': executed successfully!\n",command.inputs.Name.ToChars());
				return true;
			}

			deltaTime = Net::GetTimeMS() - startTimeMS;

		} while ( deltaTime < timeOutMS );

L_Error:
		mxWarn("ExecuteCommand() failed");
		return false;
	}

	void SetTimeout( const Timeout& timeout )
	{
		timeOutMS = timeout.Milliseconds();
	}

	NetCommandHelper()
	{
		timeOutMS = 15;
	}

private:

};




class DatabaseClient
{
public:
	DatabaseClient() {}
	virtual ~DatabaseClient() {}

	virtual void OnConnectedTo( Net::Socket socket ) {};
};

}//namespace Net





MX_NAMESPACE_BEGIN

/*
--------------------------------------------------------------
	CommandListener
	is capable of taking remote commands.
--------------------------------------------------------------
*/
class CommandListener
{
public:
	virtual ~CommandListener() {}

	virtual void ExecuteCommand( const Net::NetCmdId* cmd, SizeT length,
		Net::Server_TCP* server, UINT client ) = 0;
};

/*
--------------------------------------------------------------
	EngineServer
--------------------------------------------------------------
*/
class EngineServer
	: public Net::Server_TCP
{
public:

	MX_DECLARE_CLASS_ALLOCATOR( HeapNetwork, EngineServer );

	EngineServer();
	virtual ~EngineServer();

	void AttachListenter( CommandListener* listener );
	void DetachListenter( CommandListener* listener );

protected:

	virtual BOOL accept_client_connection( const char* host );
	virtual void on_client_connected( UINT clientNum, void *& userData );
	virtual void on_client_disconnected( void* clientData );
	virtual void on_received_packet( const Net::RawPacket& p, void* clientData, UINT clientIndex );

private:
	void ExecuteCommand( const Net::NetCmdId* cmd, SizeT length, void* clientData );

private:
	TList< CommandListener* >	mListeners;
};

MX_NAMESPACE_END


#endif // !__MX_NETWORK_PROTOCOL_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//

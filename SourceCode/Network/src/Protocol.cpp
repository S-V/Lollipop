/*
=============================================================================
	File:	Protocol.cpp
	Desc:	Network protocol for interprocess communication.
=============================================================================
*/
#include <Network_PCH.h>
#pragma hdrstop
#include <Network.h>

#include <Network/Protocol.h>

namespace Net
{

}//namespace Net


using namespace Net;


MX_NAMESPACE_BEGIN

/*================================
		EngineServer
================================*/

EngineServer::EngineServer()
{

}
//-------------------------------------------------------------------------------------------------------------//
EngineServer::~EngineServer()
{

}
//-------------------------------------------------------------------------------------------------------------//
void EngineServer::AttachListenter( CommandListener* listener )
{
	mListeners.AddUnique(listener);
}
//-------------------------------------------------------------------------------------------------------------//
void EngineServer::DetachListenter( CommandListener* listener )
{
	mListeners.Remove(listener);
}
//-------------------------------------------------------------------------------------------------------------//
BOOL EngineServer::accept_client_connection( const char* host )
{
	return __super::accept_client_connection(host);
}
//-------------------------------------------------------------------------------------------------------------//
void EngineServer::on_client_connected( UINT clientNum, void *& userData )
{
	__super::on_client_connected(clientNum, userData);
}
//-------------------------------------------------------------------------------------------------------------//
void EngineServer::on_client_disconnected( void* clientData )
{
	__super::on_client_disconnected(clientData);
}
//-------------------------------------------------------------------------------------------------------------//
void EngineServer::on_received_packet( const RawPacket& packet, void* clientData, UINT clientIndex )
{
	__super::on_received_packet(packet,clientData,clientIndex);

	switch(packet.Id())
	{
	case NET_Command:
		{
			const NetCmdId* cmd = cast(const NetCmdId*) packet.data;
			ExecuteCommand( cmd,  packet.MsgSize(), clientData );
		}
		break;

	default:
		Fallthrough;
	}
}
//-------------------------------------------------------------------------------------------------------------//
void EngineServer::ExecuteCommand( const NetCmdId* cmd, SizeT length, void* clientData )
{
	UINT clientIndex = (UINT)clientData;

#if MX_DEBUG
	PacketBuffer & readBuffer = GetClientInfo(clientIndex).readBuffer;

	const PacketHdr* hdr = (const PacketHdr*) readBuffer.ToPtr();

	const U32 msgLength = hdr->size;
	const U32 packetSize = sizeof(PacketHdr) + msgLength;
	const U32 packetId = hdr->id;

	(void)msgLength;
	(void)packetSize;
	(void)packetId;
#endif

	for( UINT i = 0; i < mListeners.Num(); i++ )
	{
		mListeners[i]->ExecuteCommand( cmd, length, this, clientIndex );
	}
}

MX_NAMESPACE_END

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//

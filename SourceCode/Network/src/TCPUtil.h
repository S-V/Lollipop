#pragma once

namespace Net
{

template< class RECEIVER >
BOOL ReadBufferedPackets( PacketBuffer& packetBuffer, RECEIVER& receiver, void* param )
{
	Assert( !packetBuffer.IsEmpty() );

	const BYTE* buffer = packetBuffer.ToPtr();
	const U32 bufferSize = packetBuffer.TotalSize();

	// number of bytes left in the input stream
	U32 bytesLeft = bufferSize;

	while( bytesLeft )
	{
		const PacketHdr* hdr = (const PacketHdr*) buffer;

		U32 msgLength = hdr->size;
		U32 packetSize = sizeof(PacketHdr) + msgLength;
		U32 packetId = hdr->id;
		(void)packetId;

		// check if the packet is complete

		if( bytesLeft >= packetSize )
		{
			RawPacket	newPacket;
			newPacket.header = *hdr;
			newPacket.data = (BYTE*)buffer + sizeof(PacketHdr);

			// the whole packet has been received ok
			bool bOk = receiver.ReceivePacket( newPacket, param );
			if( !bOk ) {
				return FALSE;
			}

			bytesLeft -= packetSize;
			buffer += packetSize;

			continue;
		}
		break;
	}

	if( !bytesLeft )
	{
		packetBuffer.Reset();
	}
	else if( bytesLeft < bufferSize )
	{
		packetBuffer.ShiftLeft( bytesLeft );
	}

	return TRUE;
}

inline
BOOL SendBuffered( Socket to, PacketBuffer & readBuffer )
{
	const UINT bytesToSend = Min<UINT>( MAX_SEND_BUFFER_SIZE, readBuffer.BytesToSend() );

	if( !bytesToSend ) {
		return TRUE;
	}

	const BYTE* data = readBuffer.Read( bytesToSend );

	UINT bytesSent;

	BOOL bOk = WriteBytesToSocket( to, data, bytesToSend, bytesSent );

	if( !bOk ) {
		return FALSE;
	}

	Assert( bytesSent >= 0 );

	return TRUE;
}

inline
BOOL ReadBuffered( Socket from, PacketBuffer & packet )
{
	BYTE	buffer[ MAX_SEND_BUFFER_SIZE ];

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

	// store them in the packet
	packet.Append( buffer, numBytesRead );
/*
	const PacketHdr* hdr = (const PacketHdr*) packet.ToPtr();
	U32 msgLength = hdr->size;
	U32 wantedSize = sizeof(PacketHdr) + msgLength;
	U32 actualSize = packet.TotalSize();

	Assert(wantedSize >= actualSize);

	const UINT bytesToRead = wantedSize - actualSize;
*/
	return TRUE;
}

}//namespace Net



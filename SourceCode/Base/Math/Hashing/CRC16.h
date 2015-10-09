// Copyright (C) 2004 Id Software, Inc.
//

#ifndef __CRC16_H__
#define __CRC16_H__
mxSWIPED("idSoftware");

mxNAMESPACE_BEGIN
/*
===============================================================================

	Calculates a checksum for a block of data
	using the CCITT standard CRC-16.

===============================================================================
*/

void CRC16_InitChecksum( unsigned short &crcvalue );
void CRC16_UpdateChecksum( unsigned short &crcvalue, const void *data, int length );
void CRC16_FinishChecksum( unsigned short &crcvalue );
unsigned short CRC16_BlockChecksum( const void *data, int length );

mxNAMESPACE_END

#endif /* !__CRC16_H__ */

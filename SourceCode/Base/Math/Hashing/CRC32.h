
#ifndef __CRC32_H__
#define __CRC32_H__
mxSWIPED("idSoftware");
mxNAMESPACE_BEGIN
/*
===============================================================================

	Calculates a checksum for a block of data
	using the CRC-32.

===============================================================================
*/

void CRC32_InitChecksum( U4 &crcvalue );
void CRC32_UpdateChecksum( U4 &crcvalue, const void *data, SizeT length );
void CRC32_FinishChecksum( U4 &crcvalue );
U4 CRC32_BlockChecksum( const void *data, SizeT length );

mxNAMESPACE_END

#endif /* !__CRC32_H__ */

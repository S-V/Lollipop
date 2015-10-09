
#ifndef __MD4_H__
#define __MD4_H__
mxSWIPED("idSoftware");
mxNAMESPACE_BEGIN

/*
===============================================================================

	Calculates a checksum for a block of data
	using the MD4 message-digest algorithm.

===============================================================================
*/

unsigned long MD4_BlockChecksum( const void *data, int length );


mxNAMESPACE_END

#endif /* !__MD4_H__ */

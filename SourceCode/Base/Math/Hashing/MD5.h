
#ifndef __MD5_H__
#define __MD5_H__
mxSWIPED("idSoftware");
mxNAMESPACE_BEGIN

/*
===============================================================================

	Calculates a checksum for a block of data
	using the MD5 message-digest algorithm.

===============================================================================
*/

unsigned long MD5_BlockChecksum( const void *data, int length );


mxNAMESPACE_END

#endif /* !__MD5_H__ */

/*
=============================================================================
	File:	MemoryStream.h
	Desc:	A data stream which offers read/write access to files.
=============================================================================
*/

#ifndef __MX_MEMORY_STREAM_H__
#define __MX_MEMORY_STREAM_H__
mxSWIPED("Nebula3");
mxNAMESPACE_BEGIN

//
//	mxMemoryStream - A data stream which writes to and reads from system RAM.
//
class mxMemoryStream : public mxDataStream
{
public:
	virtual	bool	Open() override;
	virtual	void	Close() override;

	virtual	bool	IsOpen() const override;
	virtual	bool	CanWrite() const override;
	virtual	bool	CanRead() const override;
	virtual	bool	CanSeek() const override;

	// Read the given number of bytes into the buffer and return the number of bytes actually read.
	//
	virtual	SizeT	Read( void* pBuffer, SizeT numBytes ) override;

	// Write to the stream and return the number of bytes written.
	//
	virtual	SizeT	Write( const void* pBuffer, SizeT numBytes ) override;

	// Set the file pointer to the given offset from the beginning.
	//
	virtual	void	Seek( FileOffset offset, ESeekOrigin origin = ESeekOrigin::Begin ) override;

	// Return the size of data (length of the file if this is a file), in bytes.
	//
	virtual	SizeT	GetSize() const override;

	// Returns true if EOF has been reached.
	virtual	bool	AtEnd() const override;


	// Returns true if the stream provides direct memory access.
	virtual	bool	CanBeMapped() const override;
	
	// Map stream to memory.
	virtual	void *	Map() override;
	virtual	void	Unmap() override;

	//
	//	mxMemoryStream interface :
	//

	// sets new size of the stream in bytes (resizes the internal buffer)
	// can be used to avoid memory reallocations and fragmentation
	void SetSize( SizeT s );

	// get the current position of the read/write cursor
	FilePosition Tell() const;

	bool IsMapped() const;

	// get a direct "raw" pointer to the data
	BYTE* ToPtr() const;


//protected:	friend class IOServer;

	mxMemoryStream(ENoInit);

	// opens the stream
	mxMemoryStream(
		EAccessMode accessMode = EAccessMode::ReadWriteAccess,
		EAccessPattern accessPattern = EAccessPattern::Sequential
	);

	virtual	~mxMemoryStream();

private:
	
	// re-allocate the memory buffer
	void Realloc( SizeT s );

	// return true if there's enough space for n more bytes
	bool HasRoom( SizeT numBytes ) const;

	// make room for at least n more bytes
	void MakeRoom( SizeT numBytes );

private:
	static const SizeT InitialSize = 256;

	SizeT			capacity;
	SizeT			size;
	FilePosition	position;
	BYTE *			buffer;
};

mxNAMESPACE_END

#endif // !__MX_MEMORY_STREAM_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//

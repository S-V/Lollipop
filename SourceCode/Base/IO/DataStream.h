/*
=============================================================================
	File:	DataStream.h
	Desc:	Design influenced by Nebula3' Stream class.
=============================================================================
*/

#ifndef __MX_DATA_STREAM__H__
#define __MX_DATA_STREAM__H__

mxNAMESPACE_BEGIN

//
//	mxDataStream - abstract interface for writing/reading bytes of data.
//
class mxDataStream
	: public ReferenceCountedX
	, public AStreamReader, public AStreamWriter
{
public:

	mxDECLARE_CLASS_ALLOCATOR( HeapStreaming, mxDataStream );

	typedef TRefPtr< mxDataStream >	Ref;

	// opens the stream, returns true if succeeded
	virtual bool	Open();

	// closes the stream
	virtual void	Close();

	virtual bool	IsOpen() const;
	virtual bool	CanWrite() const;
	virtual bool	CanRead() const;
	virtual bool	CanSeek() const;

	EAccessMode		GetAccessMode() const;
	void			SetAccessMode( EAccessMode newAccessMode );

	EAccessPattern	GetAccessPattern() const;
	void			SetAccessPattern( EAccessPattern newAccessPattern );

	// Set the file pointer to the given offset (in bytes) from the beginning.
	//
	virtual void	Seek( FileOffset offset, ESeekOrigin origin = ESeekOrigin::Begin ) = 0;

	// Return the size of data (length of the file if this is a file), in bytes.
	//
	virtual SizeT	GetSize() const = 0;

	// Returns true if the end has been reached.
	virtual bool	AtEnd() const = 0;

	// Flush unsaved data (this function is called automatically when the stream is closed).
	virtual void	Flush();

	// Returns true if the stream provides direct memory access.
	virtual bool	CanBeMapped() const;

	// Map stream to memory.
	virtual void *	Map();

	virtual void	Unmap();

	// Return a time value for reload operations.
	virtual FileTime GetTimeStamp();

	// Checks for testing & debugging.
	virtual bool	IsOk() const;

	virtual void	OnDelete();

	// These functions are provided for convenience.

	inline bool IsWriting()  const {  return (mAccessMode == EAccessMode::WriteAccess);  }
	inline bool IsReading() const {  return (mAccessMode == EAccessMode::ReadAccess);  }

protected:
			mxDataStream();
public:
	virtual	~mxDataStream();

private:	PREVENT_COPY( mxDataStream );

protected:
	EAccessMode		mAccessMode;
	EAccessPattern	mAccessPattern;
};

mxNAMESPACE_END

#endif // !__MX_DATA_STREAM__H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//

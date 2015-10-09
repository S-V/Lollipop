/*
=============================================================================
	File:	mxMemoryStream.cpp
	Desc:
=============================================================================
*/
#include <Base_PCH.h>
#pragma hdrstop
#include <Base.h>

mxNAMESPACE_BEGIN

#define STREAM_HEAP	EMemHeap::HeapTemp

/*================================
		mxMemoryStream
================================*/

mxMemoryStream::mxMemoryStream(ENoInit)
{
    this->capacity = 0;
    this->size = 0;
    this->position = -1;
    this->buffer = nil;
}

mxMemoryStream::mxMemoryStream(
	EAccessMode accessMode,
	EAccessPattern accessPattern
	)
{
	mAccessMode = accessMode;
	mAccessPattern = accessPattern;

	this->capacity = 0;
	this->size = 0;
	this->position = -1;
	this->buffer = nil;

	this->Open();
}

mxMemoryStream::~mxMemoryStream()
{
	 // close the stream if still open
	if( IsOpen() ) {
		Close();
	}
    // release memory buffer if allocated
    if (nil != this->buffer)
    {
        mxFreeX(STREAM_HEAP, this->buffer);
        this->buffer = 0;
    }
}

bool mxMemoryStream::Open()
{
    Assert(!IsOpen());
    
    // nothing to do here, allocation happens in the first Write() call
    // if necessary, all we do is reset the read/write position to the
    // beginning of the stream

    this->position = 0;

	return true;
}

void mxMemoryStream::Close()
{
    Assert(IsOpen());
    if (IsMapped())
    {
        Unmap();
    }
	this->position = -1;
}

bool mxMemoryStream::IsOpen() const
{
	return (this->position != -1);
}

bool mxMemoryStream::CanWrite() const
{
	return true;
}

bool mxMemoryStream::CanRead() const
{
	return true;
}

bool mxMemoryStream::CanSeek() const
{
	return true;
}

SizeT mxMemoryStream::Read( void* pBuffer, SizeT numBytes )
{
    Assert(IsOpen());
	Assert((ReadAccess == mAccessMode) || (ReadWriteAccess == mAccessMode));
    Assert((this->position >= 0) && (this->position <= this->size));

    // check if end-of-stream is near
    SizeT readBytes = (numBytes <= this->size - this->position) ? (numBytes) : (this->size - this->position);
    Assert((this->position + readBytes) <= this->size);
    if (readBytes > 0)
    {
        MemCopy(pBuffer, buffer + this->position, readBytes);
        this->position += readBytes;
    }
    return readBytes;
}

SizeT mxMemoryStream::Write( const void* pSrcBuffer, SizeT numBytes )
{
    Assert(IsOpen());
	Assert((WriteAccess == mAccessMode)
		|| (AppendAccess == mAccessMode) || (ReadWriteAccess == mAccessMode));
    Assert((this->position >= 0) && (this->position <= this->size));

    // if not enough room, allocate more memory
    if (!HasRoom(numBytes))
    {
        MakeRoom(numBytes);
    }

    // write data to stream
    Assert((this->position + numBytes) <= capacity);
    MemCopy(this->buffer + this->position, pSrcBuffer, numBytes);
    this->position += numBytes;
    if (this->position > this->size)
    {
        this->size = this->position;
    }

	return numBytes;
}

void mxMemoryStream::Seek( FileOffset offset, ESeekOrigin origin )
{
    Assert(IsOpen());
    Assert((this->position >= 0) && (this->position <= this->size));
    switch (origin)
	{
	case ESeekOrigin::Begin:
		this->position = offset;
		break;

	case ESeekOrigin::Current:
		this->position += offset;
		break;

	case ESeekOrigin::End:
		this->position = this->size + offset;
		break;
	}

    // make sure read/write position doesn't become invalid
    this->position = Clamp<SizeT>( this->position, 0, this->size );
}

SizeT mxMemoryStream::GetSize() const
{
    return size;
}

bool mxMemoryStream::AtEnd() const
{
    Assert(IsOpen());
    Assert(!IsMapped());
    Assert((this->position >= 0) && (this->position <= this->size));
	return (this->position == (FilePosition)this->size);
}

FilePosition mxMemoryStream::Tell() const
{
    return this->position;
}

bool mxMemoryStream::CanBeMapped() const
{
	return true;
}

/**
    Map the stream for direct memory access. This is much faster then 
    reading/writing, but less flexible. A mapped stream cannot grow, instead
    the allowed memory range is determined by GetSize(). The read/writer must 
    take special care to not read or write past the memory buffer boundaries!
*/
void* mxMemoryStream::Map()
{
    Assert(IsOpen());
    Assert(GetSize() > 0);
    return this->buffer;
}

void mxMemoryStream::Unmap()
{
    Assert(IsOpen());
}

/**
    Returns true if the stream is currently mapped.
*/
bool mxMemoryStream::IsMapped() const
{
    return true;
}

/**
    Get a direct pointer to the raw data. This is a convenience method
    and only works for memory streams.
    NOTE: writing new data to the stream may/will result in an invalid
    pointer, don't keep the returned pointer around between writes!
*/
BYTE* mxMemoryStream::ToPtr() const
{
    Assert(nil != this->buffer);
    return c_cast(BYTE*) this->buffer;
}

void mxMemoryStream::SetSize( SizeT s )
{
    Assert(!IsOpen());
    if (s > this->capacity)
    {
        Realloc(s);
    }
    size = s;
}

bool mxMemoryStream::HasRoom( SizeT numBytes ) const
{
    return ((this->position + numBytes) <= capacity);
}

/**
    This (re-)allocates the memory buffer to a new size. If the new size
    is smaller then the existing size, the buffer contents will be clipped.
*/
void mxMemoryStream::Realloc( SizeT newCapacity )
{
    UBYTE* newBuffer = (UBYTE*) mxAllocX( STREAM_HEAP, newCapacity );
    Assert(nil != newBuffer);
    int newSize = newCapacity < this->size ? newCapacity : this->size;
    if( nil != this->buffer )
    {
        MemCopy(newBuffer, this->buffer, newSize);
        mxFreeX(STREAM_HEAP, this->buffer);
    }
    this->buffer = newBuffer;
    this->size = newSize;
    this->capacity = newCapacity;
    if( this->position > this->size )
    {
        this->position = this->size;
    }
}

/**
    This method makes room for at least N more bytes. The actually allocated
    memory buffer will be greater then that. This operation involves a copy
    of existing data.
*/
void mxMemoryStream::MakeRoom( SizeT numBytes )
{
    Assert(numBytes > 0);
    Assert((this->size + numBytes) > this->capacity);

    // compute new capacity
    SizeT oneDotFiveCurrentSize = this->capacity + (this->capacity >> 1);
    SizeT newCapacity = this->size + numBytes;
    if (oneDotFiveCurrentSize > newCapacity)
    {
        newCapacity = oneDotFiveCurrentSize;
    }
    if (16 > newCapacity)
    {
        newCapacity = 16;
    }
    Assert(newCapacity > this->capacity);

    // (re-)allocate memory buffer
    Realloc(newCapacity);
}

mxNAMESPACE_END

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//

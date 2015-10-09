/*
=============================================================================
	File:	Common.cpp
	Desc:
=============================================================================
*/

#include <Base_PCH.h>
#pragma hdrstop
#include <Base.h>

mxNAMESPACE_BEGIN

const char* MX_STRING_UNKNOWN_ERROR = "unknown error";

ConstCharPtr	LINE_CHARS( "===========================================\n");

UINT mxGetHashCode( const String& str )
{
	return NameHash( str.ToChars() );
}

/*
-----------------------------------------------------------------------------
	Blob
-----------------------------------------------------------------------------
*/

//------------------------------------------------------------------------------
/**
*/
MemoryBlob::MemoryBlob( HMemory heap ) :
ptr(nil),
size(0),
allocSize(0)
, memHeap( heap )
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
bool
MemoryBlob::IsValid() const
{
	return (nil != this->ptr);
}

//------------------------------------------------------------------------------
/**
*/
void
MemoryBlob::Delete()
{
	if (this->IsValid())
	{
		mxFreeX( memHeap, (void*)this->ptr );
		this->ptr = nil;
		this->size = 0;
		this->allocSize = 0;
	}
}

//------------------------------------------------------------------------------
/**
*/
MemoryBlob::~MemoryBlob()
{
	this->Delete();
}

//------------------------------------------------------------------------------
/**
*/
void
MemoryBlob::Allocate(SizeT s)
{
	Assert(!this->IsValid());
	this->ptr = mxAllocX( memHeap, s );
	this->allocSize = s;
	this->size = s;
}

void MemoryBlob::Reallocate( SizeT newSize )
{
	// only re-allocate if not enough space
	if ((0 == this->ptr) || (this->allocSize < newSize))
	{
		const void* oldPtr = this->ptr;
		const SizeT oldSize = this->size;

		this->Allocate(newSize);

		if( oldPtr != nil )
		{
			MemCopy( (void*) this->ptr, oldPtr, oldSize );
			mxFreeX( memHeap, (void*) oldPtr );
		}
	}
	this->size = newSize;
}

//------------------------------------------------------------------------------
/**
*/
void
MemoryBlob::Copy(const void* fromPtr, SizeT fromSize)
{
	Assert((nil != fromPtr) && (fromSize > 0));

	this->Reallocate( fromSize );
	MemCopy( (void*) this->ptr, fromPtr, fromSize);
}

//------------------------------------------------------------------------------
/**
*/
MemoryBlob::MemoryBlob( HMemory heap, const void* fromPtr, SizeT fromSize) :
ptr(0),
size(0),
allocSize(0)
, memHeap( heap )
{    
	this->Copy(fromPtr, fromSize);
}

//------------------------------------------------------------------------------
/**
*/
MemoryBlob::MemoryBlob( HMemory heap, const MemoryBlob& rhs) :
ptr(0),
size(0),
allocSize(0)
, memHeap( heap )
{
	if (rhs.IsValid())
	{
		this->Copy(rhs.ptr, rhs.size);
	}
}

//------------------------------------------------------------------------------
/**
*/
MemoryBlob::MemoryBlob( HMemory heap, SizeT s) :
ptr(0),
size(0),
allocSize(0)
, memHeap( heap )
{
	this->Allocate(s);
}

//------------------------------------------------------------------------------
/**
*/
void
MemoryBlob::operator=(const MemoryBlob& rhs)
{
	if (rhs.IsValid())
	{
		this->Copy(rhs.ptr, rhs.size);
	}
}

//------------------------------------------------------------------------------
/**
*/
bool
MemoryBlob::operator==(const MemoryBlob& rhs) const
{
	return (this->BinaryCompare(rhs) == 0);
}

//------------------------------------------------------------------------------
/**
*/
bool
MemoryBlob::operator!=(const MemoryBlob& rhs) const
{
	return (this->BinaryCompare(rhs) != 0);
}

//------------------------------------------------------------------------------
/**
*/
bool
MemoryBlob::operator>(const MemoryBlob& rhs) const
{
	return (this->BinaryCompare(rhs) > 0);
}

//------------------------------------------------------------------------------
/**
*/
bool
MemoryBlob::operator<(const MemoryBlob& rhs) const
{
	return (this->BinaryCompare(rhs) < 0);
}

//------------------------------------------------------------------------------
/**
*/
bool
MemoryBlob::operator>=(const MemoryBlob& rhs) const
{
	return (this->BinaryCompare(rhs) >= 0);
}

//------------------------------------------------------------------------------
/**
*/
bool
MemoryBlob::operator<=(const MemoryBlob& rhs) const
{
	return (this->BinaryCompare(rhs) <= 0);
}

//------------------------------------------------------------------------------
/**
*/
void
MemoryBlob::Reserve(SizeT s)
{
	if (this->allocSize < s)
	{
		if(0)//this destroys old contents
		{
			this->Delete();
			this->Allocate(s);
		}
		else
		{
			void* pNewMem = mxAllocX( memHeap, s );
			MemCopy(pNewMem,this->ptr,this->size);
			this->Delete();
			this->ptr = pNewMem;
			this->allocSize = s;
			this->size = s;
		}
	}
	this->size = s;
}

//------------------------------------------------------------------------------
/**
*/
void
MemoryBlob::Trim(SizeT trimSize)
{
	Assert(trimSize <= this->size);
	this->size = trimSize;
}

//------------------------------------------------------------------------------
/**
*/
void
MemoryBlob::Set(const void* fromPtr, SizeT fromSize)
{
	this->Copy(fromPtr, fromSize);
}

//------------------------------------------------------------------------------
/**
*/
SizeT
MemoryBlob::GetDataSize() const
{
	Assert(this->IsValid());
	return this->size;
}

//------------------------------------------------------------------------------
/**
*/
IndexT
MemoryBlob::HashCode() const
{
	IndexT hash = 0;
	const char* charPtr = (const char*) this->ptr;
	IndexT i;
	for (i = 0; i < this->size; i++)
	{
		hash += charPtr[i];
		hash += hash << 10;
		hash ^= hash >>  6;
	}
	hash += hash << 3;
	hash ^= hash >> 11;
	hash += hash << 15;
	hash &= ~(1<<31);       // don't return a negative number (in case IndexT is defined signed)
	return hash;
}

/**
    Like strcmp(), but checks the blob contents.
*/
int MemoryBlob::BinaryCompare(const MemoryBlob& rhs) const
{
    Assert(0 != this->ptr);
    Assert(0 != rhs.ptr);
    if (this->size == rhs.size)
    {
        return MemCmp(this->ptr, rhs.ptr, this->size);
    }
    else if (this->size > rhs.size)
    {
        return 1;
    }
    else
    {
        return -1;
    }
}

void MemoryBlob::SetSize( SizeT size )
{
	this->Reallocate( size );
}

AStreamWriter& operator << ( AStreamWriter& file, const MemoryBlob & o )
{
	const U4 len = o.GetDataSize();
	file << len;

	if( len > 0 )
	{
		file.Write( o.ToPtr(), len );
	}

	return file;
}

AStreamReader& operator >> ( AStreamReader& file, MemoryBlob & o )
{
	U4 len;
	file >> len;

	o.SetSize( len );
	if( len > 0 )
	{
		file.Read( o.ToPtr(), len );
	}

	return file;
}

mxArchive& operator && ( mxArchive& archive, MemoryBlob & o )
{
	return Serialize_ArcViaStreams( archive, o );
}

mxNAMESPACE_END

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//

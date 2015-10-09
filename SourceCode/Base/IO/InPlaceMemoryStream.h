/*
=============================================================================
	File:	InPlaceMemoryReader.h
	Desc:	
=============================================================================
*/

#ifndef __MX_InPlaceMemoryStream_H__
#define __MX_InPlaceMemoryStream_H__

mxNAMESPACE_BEGIN

//
//	InPlaceMemoryReader
//
class InPlaceMemoryReader : public AStreamReader
{
public:
	InPlaceMemoryReader( const void* data, SizeT dataSize );
	~InPlaceMemoryReader();

	virtual SizeT GetSize() const override;

	virtual SizeT Read( void *pDest, SizeT numBytes ) override;

	inline const BYTE* GetPtr() const
	{
		return mData + mReadOffset;
	}
	inline BYTE* GetPtr()
	{
		return const_cast<BYTE*>( mData + mReadOffset );
	}
	inline SizeT BytesRead() const
	{
		return mReadOffset;
	}
	inline SizeT Tell() const
	{
		return mReadOffset;
	}
	inline SizeT Size() const
	{
		return mDataSize;
	}

	inline void Seek( SizeT absOffset )
	{
		Assert( absOffset < mDataSize );
		mReadOffset = absOffset;
	}

private:
	const BYTE* mData;
	SizeT mReadOffset;
	const SizeT mDataSize;

private:	PREVENT_COPY( InPlaceMemoryReader );
};

//
//	InPlaceMemoryWriter
//
class InPlaceMemoryWriter : public AStreamWriter
{
public:
	InPlaceMemoryWriter( void *dstBuf, SizeT bufSize );
	~InPlaceMemoryWriter();

	virtual SizeT Write( const void *pSrc, SizeT numBytes ) override;

	inline const BYTE* GetPtr() const
	{
		return mCurrPos;
	}
	inline SizeT BytesWritten() const
	{
		return mCurrPos - mBuffer;
	}

private:
	BYTE	*mBuffer;
	BYTE	*mCurrPos;
	const SizeT mMaxSize;

private:	PREVENT_COPY( InPlaceMemoryWriter );
};

mxNAMESPACE_END

#endif // !__MX_InPlaceMemoryStream_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//

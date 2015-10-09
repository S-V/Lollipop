/*
=============================================================================
	File:	TStaticArray.h
	Desc:	
	ToDo:	Stop reinventing the wheel.
=============================================================================
*/

#ifndef __MX_TStaticArray_H__
#define __MX_TStaticArray_H__

mxNAMESPACE_BEGIN

//
//	TStaticArray - static, fixed-size array (the size is known at compile time).
//
//	It's recommended to use this class instead of bare C-style arrays.
//
template< typename TYPE, const SizeT SIZE >
class TStaticArray
	: public TArrayBase< TYPE, TStaticArray<TYPE,SIZE> >
{
	TYPE	mData[ SIZE ];

public:
	typedef TStaticArray
	<
		TYPE,
		SIZE
	>
	THIS_TYPE;

	typedef
	TYPE
	ITEM_TYPE;

public:
	FORCEINLINE TStaticArray()
	{}

	FORCEINLINE explicit TStaticArray(EInitZero)
	{
		ZERO_OUT(mData);
	}

	FORCEINLINE ~TStaticArray()
	{}

	FORCEINLINE UINT Num() const
	{
		return SIZE;
	}
	FORCEINLINE UINT Capacity() const
	{
		return SIZE;
	}

	FORCEINLINE void Empty()
	{
	}
	FORCEINLINE void Clear()
	{
	}

	FORCEINLINE TYPE * ToPtr()
	{ return mData; }

	FORCEINLINE const TYPE * ToPtr() const
	{ return mData; }


	// Implicit type conversion - intentionally disabled:
	//FORCEINLINE operator TYPE* ()
	//{ return mData; }
	//FORCEINLINE operator const TYPE* () const
	//{ return mData; }

	friend AStreamWriter& operator << ( AStreamWriter& file, const THIS_TYPE &o )
	{
		file.SerializeArray( o.mData, SIZE );
		return file;
	}
	friend AStreamReader& operator >> ( AStreamReader& file, THIS_TYPE &o )
	{
		file.SerializeArray( o.mData, SIZE );
		return file;
	}
	friend mxArchive& operator && ( mxArchive & archive, THIS_TYPE & o )
	{
		TSerializeArray( archive, o.mData, SIZE );
		return archive;
	}

public_internal:

	/// For serialization, we want to initialize the vtables
	/// in classes post data load, and NOT call the default constructor
	/// for the arrays (as the data has already been set).
	FORCEINLINE explicit TStaticArray( _FinishedLoadingFlag )
	{
	}

private:	PREVENT_COPY(THIS_TYPE);
};

//
//	TStaticArray_InitZeroed
//
template< typename TYPE, const SizeT SIZE >
class TStaticArray_InitZeroed : public TStaticArray< TYPE, SIZE >
{
public:
	TStaticArray_InitZeroed()
		: TStaticArray(_InitZero)
	{}
};

mxNAMESPACE_END

#endif // !__MX_TStaticArray_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//

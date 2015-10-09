/*
=============================================================================
	File:	Kernel.h
	Desc:	Core system kernel.
=============================================================================
*/

#ifndef __MX_CORE_ENGINE_KERNEL_H__
#define __MX_CORE_ENGINE_KERNEL_H__

#include <Base/Object/AObject.h>

mxNAMESPACE_BEGIN

class SBaseObject;

namespace Kernel
{
	enum { MAX_OBJECT_NUMBER = (1<<16) };
	enum { MAX_SERIAL_NUMBER = (1<<12) };

}//namespace Kernel


// 0 is the (silently ignored) null GUID value.
enum { mxNULL_OBJECT_GUID = 0 };

// // -1 is the (silently ignored) invalid handle value (INVALID_HANDLE_VALUE)
enum { mxNULL_OBJECT_ID = (U4)~0 };

/*
-----------------------------------------------------------------------------
	ObjectGUID - (static)(globally) unique resource identifier,
	usually assigned by resource compiler
	and known before running the program.

	ObjectGUID's are unique identifiers of resources used for sharing and
	locating the resource data on disc
	as well as serializing references to resources.

	ObjectGUID is guaranteed to be the same when saving/loading,
	it is also same in the editor and in the game.

	We should be able to translate ObjectGUID's into data streams
	in order to load the resource.

	Usually, during development ObjectGUID's will be fat, slow strings
	and in release mode they will be fast, slim integer identifiers.
-----------------------------------------------------------------------------
*/

//
// ObjectGUID - is a persistent object GUID
// that is used to located resource data on disk
// (aka 'resource locator', 'asset reference').
// it's the same when saving/loading.
//
struct ObjectGUID
{
	U4	v;	// id is an integer value

public:
	FORCEINLINE ObjectGUID(ENoInit)
	{}

	FORCEINLINE ObjectGUID()
	{ v = mxNULL_OBJECT_GUID; }

	FORCEINLINE ObjectGUID(EInitInvalid)
	{ v = mxNULL_OBJECT_GUID; }

	FORCEINLINE explicit ObjectGUID( const U4 value )
	{ v = value; }

	FORCEINLINE bool IsNull() const
	{ return v == mxNULL_OBJECT_GUID; }

	FORCEINLINE bool IsValid() const
	{ return v != mxNULL_OBJECT_GUID; }


	mxIMPLEMENT_HANDLE( ObjectGUID, v );

	//FORCEINLINE operator bool () const
	//{ return this->IsValid(); }



	void ToChars( char *buffer, UINT maxChars ) const;

	template< UINT MAX_CHARS >
	void ToChars( char (&buffer) [MAX_CHARS] ) const
	{
		this->ToChars( buffer, MAX_CHARS );
	}
};

typedef const ObjectGUID ObjectGUIDArg;

mxDECLARE_POD_TYPE( ObjectGUID );

template<>
struct THashTrait< ObjectGUID >
{
	static FORCEINLINE UINT GetHashCode( const ObjectGUID& key )
	{
		return key.v;
	}
};

mxIMPLEMENT_FUNCTION_READ_SINGLE(ObjectGUID,ReadObjectGuid);
mxIMPLEMENT_FUNCTION_WRITE_SINGLE(ObjectGUID,WriteObjectGuid);

//---------------------------------------------------------------------------


/*
-----------------------------------------------------------------------------
	ObjectID

	Dynamic resource identifier, a handle representing a reference to an object.
	It's mostly used at run-time for fast and unique identification of resources.


	0 is the (silently ignored) invalid handle value.

	contains index of the object in some global table
	and a tag.

	A tag is a counter that disambiguates several subsequent uses
	of the same resource array slot. It servers as a a 'tombstone'
	to test validity of a handle.

	Need to use dynamic handles instead of ObjectGUIDs
	because access to resources should be done in O(1) time.

	The runtime unique identifier assigned to each resource by the kernel.
	ObjectIDs may not be the same when saving/loading.
-----------------------------------------------------------------------------
*/

class ObjectID
{
	union
	{
		struct
		{
			U2	m_objectIndex;	// internal index for fast access
			U2	m_serialNumber;	// 1.. and always increments, should never wrap around
		};
		U4		m_handleValue;
	};

public:
	FORCEINLINE ObjectID(ENoInit)
	{
	}
	FORCEINLINE ObjectID()
	{
		this->Clear();
	}
	FORCEINLINE ObjectID(EInitInvalid)
	{
		this->Clear();
	}
	FORCEINLINE ObjectID( const ObjectID& other )
	{
		m_handleValue = other.m_handleValue;
	}

	FORCEINLINE void Clear()
	{
		m_handleValue = mxNULL_OBJECT_ID;
	}

	// Tests validity of handle.
	// Returns true if handle is valid, false otherwise.
	//
	FORCEINLINE bool IsValid() const
	{
		return m_handleValue != mxNULL_OBJECT_ID;
	}
	FORCEINLINE bool IsNull() const
	{
		return m_handleValue == mxNULL_OBJECT_ID;
	}

	// NOTE: implicit conversions are intentionally disabled
	//
#if 0
	FORCEINLINE bool operator ! () const
	{
		return this->IsValid();
	}
	FORCEINLINE operator bool() const
	{
		return this->IsValid();
	}
	T& operator*();
	T* operator->();
	const T& operator*() const;
	const T* operator->() const;
#endif

	mxIMPLEMENT_HANDLE( ObjectID, m_handleValue );

public_internal:

	// Only the code that doles out these handles should use these functions.
	// Everyone else should treat them as a transparent type.

	FORCEINLINE explicit ObjectID( UINT handleValue )
	{
		m_handleValue = handleValue;
	}
	FORCEINLINE ObjectID( UINT entryIndex, UINT serialNumber )
	{
		this->Set( entryIndex, serialNumber );
	}

	FORCEINLINE void Set( UINT iEntry, UINT iSerial )
	{
		Assert( iEntry < Kernel::MAX_OBJECT_NUMBER );
		Assert( iSerial < Kernel::MAX_SERIAL_NUMBER );
		m_objectIndex = iEntry;
		m_serialNumber = iSerial;
	}

	FORCEINLINE UINT GetEntryIndex() const
	{
		return m_objectIndex;
	}
	FORCEINLINE UINT GetSerialNumber() const
	{
		return m_serialNumber;
	}
	FORCEINLINE UINT GetHandleValue() const
	{
		return m_handleValue;
	}

	FORCEINLINE void Internal_SetHandleValue( UINT handleValue )
	{
		m_handleValue = handleValue;
	}
	FORCEINLINE void Internal_CopyFrom( const ObjectID& other )
	{
		m_handleValue = other.m_handleValue;
	}

	// Dereferencing.

	FORCEINLINE void* ToVoidPtr() const;

	NO_ASSIGNMENT(ObjectID);
};

typedef const ObjectID ObjectIDArg;

mxDECLARE_POD_TYPE( ObjectID );

/*
-----------------------------------------------------------------------------
	SBaseObject

	base class for entities that are accessible through handles.
-----------------------------------------------------------------------------
*/
// NOTE: this must be an empty base class!
// it must NOT have any virtual member functions!
// it must be movable/copyable, because memory can be moved to improve data locality/coherency
//
struct SBaseObject : public SBaseType
{

};

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

namespace Kernel
{
	void Initialize();
	void Shutdown();

	ObjectID AddObject( void* entity );

	void RemoveObject( ObjectID handle );

	// Query

	// entry lookup by entity handle (return nil if got invalid handle)
	void* HasEntry( ObjectID handle );

	// make sure the handle is valid
	void* LookupObject( ObjectID handle );


}//namespace Kernel

FORCEINLINE void* ObjectID::ToVoidPtr() const
{
	return Kernel::LookupObject( *this );
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

template< typename TYPE >
struct TObjectHandle : public ObjectID
{
	typedef TObjectHandle THIS_TYPE;

	// Use this to dereference the handle.
	FORCEINLINE TYPE* ToPtr() const
	{
		TYPE* o = static_cast< TYPE* >( Kernel::LookupObject( *this ) );
		return o;
	}
};

mxNAMESPACE_END

#endif // !__MX_CORE_ENGINE_KERNEL_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//

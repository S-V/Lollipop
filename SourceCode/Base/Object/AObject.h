/*
=============================================================================
	File:	Object.h
	Desc:	Base class for many objects, root of our class hierarchy.
			It's quite lightweight (only vtbl overhead).
=============================================================================
*/

#ifndef __MX_RTTI_BASE_CLASS_H__
#define __MX_RTTI_BASE_CLASS_H__

#include <Base/Object/TypeRegistry.h>
#include <Base/Object/Reflection.h>


mxNAMESPACE_BEGIN

class mxArchive;
//struct SPostLoadContext;
struct SDbgDumpContext;

/*
-----------------------------------------------------------------------------
	SBaseType

	base class for relocatable (bitwise copyable) classes (including POD types),
	i.e. derived classes must not have virtual functions, pointers to self, etc.
-----------------------------------------------------------------------------
*/
struct SBaseType
{
	// declare aligned allocator
	mxDECLARE_CLASS_ALLOCATOR( HeapGeneric, SBaseType );

	inline SBaseType() {}
	inline ~SBaseType() {}

	static mxClass & StaticClass() { return ms_staticTypeInfo; };

	inline mxClass& rttiGetClass() const { return ms_staticTypeInfo; }

public:	// Reflection / Serialization

	static mxClassMembers& SBaseType::StaticGetReflection()
	{
		return mxClassMembers::dummy;
	}

	// this function is called immediately after deserialization to complete the object's initialization
	inline void PostLoad() {}

private:
	static mxClass		ms_staticTypeInfo;
};

/*
-----------------------------------------------------------------------------
	AObject

	abstract base class for most polymorphic classes in our class hierarchy,
	provides fast RTTI.

	NOTE: if this class is used as a base class when using multiple inheritance,
	this class should go first (to make sure vtable is placed at the beginning).

	NOTE: this class must be abstract!
	NOTE: this class must not define any member fields!
-----------------------------------------------------------------------------
*/
class AObject {
public:
	// declare aligned allocator
	mxDECLARE_CLASS_ALLOCATOR( HeapGeneric, AObject );

	//
	//	Run-Time Type Information (RTTI)
	//

	static mxClass & StaticClass();

	virtual mxClass& rttiGetClass() const;

	// These functions are provided for convenience.

	const char *	rttiGetTypeName() const;
	TypeGUID		rttiGetTypeGuid() const;

	// Returns 'true' if this type inherits from the given type.
	bool	IsA( const mxClass& type ) const;
	bool	IsA( TypeGUIDArg typeCode ) const;

	// Returns 'true' if this type is the same as the given type.
	bool	IsInstanceOf( const mxClass& type ) const;
	bool	IsInstanceOf( TypeGUIDArg typeCode ) const;

	// Returns 'true' if this type inherits from the given type.
	template< class KLASS >
	inline bool IsA() const
	{
		return this->IsA( KLASS::StaticClass() );
	}

	// Returns 'true' if this type is the same as the given type.
	template< class KLASS >
	inline bool Is() const
	{
		return IsInstanceOf( KLASS::StaticClass() );
	}

	bool IsAbstract() const;
	bool IsConcrete() const;

	SizeT GetInstanceSize() const;

public:	// Reflection / Serialization

	static mxClassMembers& AObject::StaticGetReflection()
	{
		return mxClassMembers::dummy;
	}

public:	// Serialization / Streaming

	// serialize persistent data (including pointers to other objects)
	// in binary form
	//
	virtual void Serialize( mxArchive & s ) {}

	// complete loading (e.g. fixup asset references)
	//virtual void PostLoad( const SPostLoadContext& loadArgs ) {}
	virtual void PostLoad() {}

	// perform minor optimizations before saving
	//virtual void PreSave( const SPreSaveContext& saveArgs ) {}

public:	// Cloning (virtual constructor, deep copying)
	//virtual AObject* Clone() { return nil; }

public:	// Testing & Debugging

	// returns the memory address of the object
	const void* GetAddress() const { return this; }

	virtual void DbgAssertValid() {}
	virtual void DbgDumpContents( const SDbgDumpContext& dc ) {}


public:
	virtual	~AObject() = 0;

protected:
	AObject() {}

private:	PREVENT_COPY(AObject);

	static mxClass		ms_staticTypeInfo;
};

#include <Base/Object/AObject.inl>

//
//	DynamicCast< T > - safe (checked) cast, returns a nil pointer on failure
//
template< class TypeTo >
mxINLINE
TypeTo* DynamicCast( AObject* pObject )
{
	AssertPtr( pObject );
	return pObject->IsA( TypeTo::StaticClass() ) ?
		static_cast< TypeTo* >( pObject ) : nil;
}

//
//	ConstCast< T >
//
template< class TypeTo >
mxINLINE
const TypeTo* ConstCast( const AObject* pObject )
{
	AssertPtr( pObject );
	return pObject->IsA( TypeTo::StaticClass() ) ?
		static_cast< const TypeTo* >( pObject ) : nil;
}

//
//	UpCast< T > - unsafe cast, raises an error on failure,
//	assumes that you know what you're doing.
//
template< class TypeTo >
mxINLINE
TypeTo* UpCast( AObject* pObject )
{
	AssertPtr( pObject );
	Assert( pObject->IsA( TypeTo::StaticClass() ) );
	return static_cast< TypeTo* >( pObject );
}

template< class TypeTo >
mxINLINE
TypeTo* SafeCast( AObject* pObject )
{
	if( pObject != nil )
	{
		return pObject->IsA( TypeTo::StaticClass() ) ?
			static_cast< TypeTo* >( pObject ) : nil;
	}
	return nil;
}


namespace ObjectUtil
{
	inline bool Serializable_Class( const mxClass& classInfo )
	{
		return (classInfo.GetMembers().numFields > 0)
			// the root classes don't have any serializable state
			&& (classInfo != SBaseType::StaticClass())
			&& (classInfo != AObject::StaticClass())
			;
	}

	AObject* Create_Object_Instance( const mxClass& classInfo );
	AObject* Create_Object_Instance( TypeGUIDArg classGuid );

	template< class KLASS >
	AObject* Create_Object_Instance()
	{
		AObject* pNewInstance = Create_Object_Instance( KLASS::StaticClass() );
		return UpCast< KLASS >( pNewInstance );
	}

}//namespace ObjectUtil

mxNAMESPACE_END

#endif // !__MX_RTTI_BASE_CLASS_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//

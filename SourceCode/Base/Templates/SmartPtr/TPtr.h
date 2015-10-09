/*
=============================================================================
	File:	TPtr.h
	Desc:	Weak pointer - doesn't take ownership over the object,
			doesn't know about reference counting,
			used to catch programming errors (automatic null pointer checks).
=============================================================================
*/

#ifndef __MX_CONTAINTERS_WEAK_POINTER_H__
#define __MX_CONTAINTERS_WEAK_POINTER_H__

#include <Base/Object/PointerType.h>

mxNAMESPACE_BEGIN

//
//	TPtr< T > - is a weak pointer.
//
template< typename T >
class TPtr {
public:
	FORCEINLINE TPtr()
		: Ptr( nil )
	{}

	FORCEINLINE TPtr( T *pointer )
	{
		//AssertPtr( pointer );
		this->Ptr = pointer;
	}

	FORCEINLINE explicit TPtr( const TPtr<T>& other )
	{
		*this = other;
	}

	FORCEINLINE ~TPtr()
	{
		this->Ptr = nil;
	}

	FORCEINLINE T * operator -> () const
	{
		AssertPtr( this->Ptr );
		return this->Ptr;
	}

	FORCEINLINE T & operator * () const
	{
		AssertPtr( this->Ptr );
		return *this->Ptr;
	}

	FORCEINLINE operator T* ()
	{
		//AssertPtr( this->Ptr );
		return this->Ptr;
	}
	FORCEINLINE operator const T* () const
	{
		AssertPtr( this->Ptr );
		return this->Ptr;
	}

	// commented out: ambiguous conversions
	//template< class U >	// where U : T
	//FORCEINLINE operator U* ()
	//{
	//	return this->Ptr;
	//}

	FORCEINLINE T* ToPtr() const
	{
		AssertPtr( this->Ptr );
		return this->Ptr;
	}


	FORCEINLINE void operator = ( T* newPointer )
	{
	//	AssertPtr( newPointer );
		this->Ptr = newPointer;
	}
	FORCEINLINE void operator = ( const TPtr<T>& other )
	{
	//	Assert( other.IsValid() );
		this->Ptr = other.Ptr;
	}

	FORCEINLINE bool operator ! () const
	{
		return ( this->Ptr == nil );
	}
	FORCEINLINE bool operator == ( T* pObject ) const
	{
		return ( this->Ptr == pObject );
	}
	FORCEINLINE bool operator != ( T* pObject ) const
	{
		return ( this->Ptr != pObject );
	}
	FORCEINLINE bool operator == ( const TPtr<T>& other ) const
	{
		return ( this->Ptr == other.Ptr );
	}
	FORCEINLINE bool operator != ( const TPtr<T>& other ) const
	{
		return ( this->Ptr != other.Ptr );
	}

	FORCEINLINE bool IsNull() const
	{
		return ( this->Ptr == nil );
	}
	FORCEINLINE bool IsValid() const
	{
		return ( this->Ptr != nil );
	}


	FORCEINLINE T& get_ref()
	{
		AssertPtr( this->Ptr );
		return *this->Ptr;
	}

	// Testing & Debugging.

	// An unsafe way to get the pointer.
	FORCEINLINE T*& get_ptr_ref()
	{
		return this->Ptr;
	}
	FORCEINLINE T* get_ptr()
	{
		return this->Ptr;
	}
	FORCEINLINE const T* get_ptr() const
	{
		return this->Ptr;
	}

public:

	//@todo: make a special smart pointer with ConstructInPlace() and Destruct() (and check for double construct/delete)
	void ConstructInPlace()
	{
		Assert( this->Ptr == nil );
		if( this->Ptr == nil )
		{
			mxSTATIC_IN_PLACE_CTOR_X( this->Ptr, T );
		}
	}
	void Destruct()
	{
		if( this->Ptr != nil )
		{
			this->Ptr->~T();
			this->Ptr = nil;
		}
	}
	void Delete()
	{
		if( this->Ptr != nil )
		{
			delete this->Ptr;
			this->Ptr = nil;
		}
	}

public_internal:

	T *		Ptr;
};

template< class KLASS >
struct TypeDeducer< TPtr<KLASS> >
{
	static inline const mxType& GetType()
	{
		static mxPointerType staticTypeInfo(
			"TPtr",
			STypeDescription::For_Type<KLASS*>(),
			T_DeduceTypeInfo<KLASS>()
		);
		return staticTypeInfo;
	}
	static inline ETypeKind GetTypeKind()
	{
		return ETypeKind::Type_Pointer;
	}
};


//---------------------------------------------------------------------------

//
//	TValidPtr< TYPE > - is a weak pointer
//	that always stores a non-null pointer
//	which cannot be reassigned.
//
template< typename TYPE >
class TValidPtr
{
	TYPE *	m_pointer;

public:
	FORCEINLINE TValidPtr( TYPE* pointer )
	{
		AssertPtr( pointer );
		m_pointer = pointer;
	}

	FORCEINLINE ~TValidPtr()
	{
	}

	FORCEINLINE TYPE * operator -> () const
	{
		AssertPtr( m_pointer );
		return m_pointer;
	}

	FORCEINLINE TYPE & operator * () const
	{
		AssertPtr( m_pointer );
		return *m_pointer;
	}

	FORCEINLINE operator TYPE* () const
	{
		AssertPtr( m_pointer );
		return m_pointer;
	}
	FORCEINLINE operator const TYPE* () const
	{
		AssertPtr( m_pointer );
		return m_pointer;
	}

	FORCEINLINE bool operator == ( TYPE* pointer ) const
	{
		return ( m_pointer == pointer );
	}
	FORCEINLINE bool operator != ( TYPE* pointer ) const
	{
		return ( m_pointer != pointer );
	}
	FORCEINLINE bool operator == ( const TValidPtr<TYPE>& other ) const
	{
		return ( m_pointer == other.m_pointer );
	}
	FORCEINLINE bool operator != ( const TValidPtr<TYPE>& other ) const
	{
		return ( m_pointer != other.m_pointer );
	}

public:

	FORCEINLINE void Destruct()
	{
		AssertPtr( m_pointer );
		if( m_pointer != nil )
		{
			m_pointer->~TYPE();
			m_pointer = nil;
		}
	}
	FORCEINLINE void Delete()
	{
		AssertPtr( m_pointer );
		if( m_pointer != nil )
		{
			delete m_pointer;
			m_pointer = nil;
		}
	}

private:	PREVENT_COPY( TValidPtr<TYPE> );
};

//---------------------------------------------------------------------------

//
//	TValidRef< TYPE > - stores a reference
//	that cannot be reassigned.
//
template< typename TYPE >
class TValidRef
{
	TYPE &	m_reference;

public:
	FORCEINLINE TValidRef( TYPE& reference )
		: m_reference( reference )
	{	
	}

	FORCEINLINE ~TValidRef()
	{
	}

	FORCEINLINE TYPE & ToRef() const
	{
		return m_reference;
	}

	FORCEINLINE TYPE & operator & () const
	{
		return m_reference;
	}

	FORCEINLINE bool operator == ( TYPE* pointer ) const
	{
		return ( m_reference == pointer );
	}
	FORCEINLINE bool operator != ( TYPE* pointer ) const
	{
		return ( m_reference != pointer );
	}
	FORCEINLINE bool operator == ( const TValidRef<TYPE>& other ) const
	{
		return ( m_reference == other.m_reference );
	}
	FORCEINLINE bool operator != ( const TValidRef<TYPE>& other ) const
	{
		return ( m_reference != other.m_reference );
	}

private:	PREVENT_COPY( TValidRef<TYPE> );
};


//---------------------------------------------------------------------------

//@fixme: untested
//
//	TIndexPtr< TYPE > - is a handle-based smart pointer.
//
template< typename TYPE >
class TIndexPtr
{
	UINT	m_objectIndex;

	FORCEINLINE TYPE* Get_Pointer_By_Index( const UINT theIndex ) const
	{
		TYPE* thePointer = TYPE::GetPointerByIndex( theIndex );
		return thePointer;
	}
	FORCEINLINE UINT Get_Index_By_Pointer( const TYPE* thePointer ) const
	{
		return TYPE::GetIndexByPointer( thePointer );
	}

public:
	FORCEINLINE bool IsValid() const
	{
		return this->Get_Pointer_By_Index( m_objectIndex ) != nil;
	}
	FORCEINLINE bool IsNull() const
	{
		return !this->IsValid();
	}

	FORCEINLINE TIndexPtr()
	{
		m_objectIndex = INDEX_NONE;
	}
	FORCEINLINE TIndexPtr( TYPE* thePointer )
	{
		*this = thePointer;
	}
	FORCEINLINE TIndexPtr( const TIndexPtr& other )
	{
		*this = other;
	}
	FORCEINLINE ~TIndexPtr()
	{
	}

	FORCEINLINE TYPE * operator -> () const
	{
		Assert( this->IsValid() );
		return this->Get_Pointer_By_Index( m_objectIndex );
	}

	FORCEINLINE TYPE & operator * () const
	{
		Assert( this->IsValid() );
		return *this->Get_Pointer_By_Index( m_objectIndex );
	}

	FORCEINLINE TYPE* ToPtr() const
	{
		return this->Get_Pointer_By_Index( m_objectIndex );
	}
	FORCEINLINE TYPE* SafeToPtr() const
	{
		TYPE* thePointer = this->Get_Pointer_By_Index( m_objectIndex );
		return thePointer;
	}
	FORCEINLINE operator TYPE* () const
	{
		return this->SafeToPtr();
	}
	FORCEINLINE operator const TYPE* () const
	{
		return this->SafeToPtr();
	}

	FORCEINLINE void operator = ( TYPE* thePointer )
	{
		if(!PtrToBool( thePointer )) {
			m_objectIndex = INDEX_NONE;
		} else {
			m_objectIndex = this->Get_Index_By_Pointer( thePointer );
		}
	}
	FORCEINLINE void operator = ( const TIndexPtr<TYPE>& other )
	{
		m_objectIndex = other.m_objectIndex;
	}

	FORCEINLINE bool operator == ( TYPE* thePointer ) const
	{
		AssertPtr( thePointer );
		return m_objectIndex == TYPE::GetIndexByPointer( thePointer );
	}
	FORCEINLINE bool operator != ( TYPE* thePointer ) const
	{
		return !( *this == thePointer );
	}

	FORCEINLINE bool operator == ( const TIndexPtr<TYPE>& other ) const
	{
		return ( m_objectIndex == other.m_objectIndex );
	}
	FORCEINLINE bool operator != ( const TIndexPtr<TYPE>& other ) const
	{
		return ( m_objectIndex != other.m_objectIndex );
	}

public_internal:
	FORCEINLINE UINT& GetHandleRef()
	{
		return m_objectIndex;
	}
	bool UpdateHandle()
	{
		TYPE* thePointer = this->Get_Pointer_By_Index( m_objectIndex );
		if(!PtrToBool( thePointer )) {
			m_objectIndex = INDEX_NONE;
			return false;
		}
		return true;
	}
};


mxNAMESPACE_END

#endif // ! __MX_CONTAINTERS_WEAK_POINTER_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//

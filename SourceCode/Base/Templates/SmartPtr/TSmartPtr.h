/*
=============================================================================
	File:	TSmartPtr.h
	Desc:
=============================================================================
*/

#ifndef __MX_CONTAINTERS_SMART_POINTER_H__
#define __MX_CONTAINTERS_SMART_POINTER_H__

mxNAMESPACE_BEGIN

//
//	CheckingPolicy_NoChecking - doesn't check if the supplied pointer is valid.
//
template< typename T >
class CheckingPolicy_NoChecking {
public:
	static void CheckPtr( T* pointer )
	{
		(void) pointer;
	}
};

//
//	CheckingPolicy_EnforceNotNull - ensures that the given pointer is not NULL.
//
template< typename T >
class CheckingPolicy_EnforceNotNull {
public:
	static void CheckPtr( T* pointer )
	{
		Assert( pointer != NULL );
	}
};

//
//	RefCountingPolicy_NoReferenceCounting
//
template< typename T >
class RefCountingPolicy_NoReferenceCounting {
public:
	static void IncRefCount( T* pointer )
	{
		(void) pointer;
	}
	static void DecRefCount( T* pointer )
	{
		(void) pointer;
	}
};

//
//	RefCountingPolicy_DoReferenceCounting
//
template< typename T >
class RefCountingPolicy_DoReferenceCounting {
public:
	static void IncRefCount( T* pointer )
	{	Assert( pointer );
		pointer->Grab();
	}
	static void DecRefCount( T* pointer )
	{	Assert( pointer );
		pointer->Drop();
	}
};

//
//	TSmartPtr< T >
//
template
<
	typename T,
	template< class > class CheckingPolicy,
	template< class > class RefCountPolicy
>
class TSmartPtr
	: public CheckingPolicy< T >
	, public RefCountPolicy< T >
{
public:
			TSmartPtr();
			TSmartPtr( T* ptr );
			TSmartPtr( const TSmartPtr & other );
			~TSmartPtr();

	// Implicit conversions.

				operator T* () const;
	const T &	operator *  () const;
	const T *	operator -> () const;

	T &			operator *  ();
	T *			operator -> ();

	// Assignment.

	TSmartPtr &		operator = ( const TSmartPtr & rhs );
	TSmartPtr &		operator = ( T* rhs );

	// Comparisons.

	bool	operator == ( const TSmartPtr & rhs );
	bool	operator != ( const TSmartPtr & rhs );

	bool	operator == ( T* rhs );
	bool	operator != ( T* rhs );

	// Getters/Setters.

	void	SetPointer( T * ptr );
	T *		GetPointer() const		{ return m_pointer; }
	T &		GetReference() const	{ return *m_pointer; }

private:
	T *		m_pointer;
};

/*================================
		TSmartPtr
		<
			T,
			CheckingPolicy,
			RefCountPolicy
		>
================================*/

//
//	Helper macros to do less typing.
//
#define TSMART_PTR_TEMPLATE	\
template										\
<												\
	typename T,									\
	template< class > class CheckingPolicy,		\
	template< class > class RefCountPolicy		\
>

#define TSMART_PTR			\
TSmartPtr< T, CheckingPolicy, RefCountPolicy >

//------------------------------------------------------------

TSMART_PTR_TEMPLATE
TSMART_PTR::TSmartPtr()
	: m_pointer( NULL )
{
	CheckingPolicy<T>::CheckPtr( m_pointer );
	RefCountPolicy<T>::IncRefCount( m_pointer );
}

TSMART_PTR_TEMPLATE
TSMART_PTR::TSmartPtr( T * ptr )
{
	CheckingPolicy<T>::CheckPtr( ptr );
	m_pointer = ptr;
	RefCountPolicy<T>::IncRefCount( m_pointer );
}

TSMART_PTR_TEMPLATE
TSMART_PTR::TSmartPtr( const TSMART_PTR & src )
{
	*this = src;
}

TSMART_PTR_TEMPLATE
TSMART_PTR & TSMART_PTR::operator = ( const TSMART_PTR & rhs )
{
	if ( this == rhs ) {
		return *this;
	}

	RefCountPolicy<T>::DecRefCount( m_pointer );
	CheckingPolicy<T>::CheckPtr( rhs.m_pointer );
	m_pointer = rhs.m_pointer;
	RefCountPolicy<T>::IncRefCount( m_pointer );

	return *this;
}

TSMART_PTR_TEMPLATE
TSMART_PTR & TSMART_PTR::operator = ( T * rhs )
{
	if ( m_pointer == rhs ) {
		return *this;
	}
	CheckingPolicy<T>::CheckPtr( rhs );
	RefCountPolicy<T>::DecRefCount( m_pointer );
	m_pointer = rhs;
	RefCountPolicy<T>::IncRefCount( m_pointer );

	return *this;
}

TSMART_PTR_TEMPLATE
TSMART_PTR::~TSmartPtr()
{
	CheckingPolicy<T>::CheckPtr( m_pointer ); // <= CheckPtr the pointer just in case.
	RefCountPolicy<T>::DecRefCount( m_pointer );
}

TSMART_PTR_TEMPLATE
const T* TSMART_PTR::operator -> () const
{
	CheckingPolicy<T>::CheckPtr( m_pointer );
	return m_pointer;
}


TSMART_PTR_TEMPLATE
TSMART_PTR::operator T* () const
{
	CheckingPolicy<T>::CheckPtr( m_pointer );
	return m_pointer;
}

TSMART_PTR_TEMPLATE
const T& TSMART_PTR::operator * () const
{
	CheckingPolicy<T>::CheckPtr( m_pointer );
	return *m_pointer;
}

TSMART_PTR_TEMPLATE
T* TSMART_PTR::operator -> ()
{
	CheckingPolicy<T>::CheckPtr( m_pointer );
	return m_pointer;
}

TSMART_PTR_TEMPLATE
T& TSMART_PTR::operator * ()
{
CheckingPolicy<T>::CheckPtr( m_pointer );
	return *m_pointer;
}

TSMART_PTR_TEMPLATE
bool TSMART_PTR::operator == ( T * rhs )
{
	return m_pointer == rhs;
}

TSMART_PTR_TEMPLATE
bool TSMART_PTR::operator != ( T * rhs )
{
	return m_pointer != rhs;
}

TSMART_PTR_TEMPLATE
bool TSMART_PTR::operator == ( const TSMART_PTR & rhs )
{
	return m_pointer == rhs.m_pointer;
}

TSMART_PTR_TEMPLATE
bool TSMART_PTR::operator != ( const TSMART_PTR & rhs )
{
	return m_pointer != rhs.m_pointer;
}


// Preprocessor cleanup.

#undef TSMART_PTR_TEMPLATE
#undef TSMART_PTR

/*

Usage example:

//
//	NonNullPtr< SomeConcreteType >
//
typedef TSmartPtr
<
	SomeConcreteType,
	CheckingPolicy_EnforceNotNull,
	RefCountingPolicy_NoReferenceCounting
>
NonNullPtr;
*/

mxNAMESPACE_END

#endif // ! __MX_CONTAINTERS_SMART_POINTER_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//

/*
=============================================================================
	File:	TSharedPtr.h
	Desc:	Smart, reference-counted pointers.
=============================================================================
*/

#ifndef __MX_SHARED_POINTER_H__
#define __MX_SHARED_POINTER_H__

#include <Core/Lib/Templates/Containers/HashMap/RBTreeMap.h>

MX_NAMESPACE_BEGIN

/*
=============================================================================
	TSharedPtr is implementation of a smart pointer with reference counting.

	  Try to use this class instead of regular pointers as often as possible
	as it offers safer pointer usage - you do not need to worry about
	releasing memory as it will be automatically freed on destuction.
	It is possible due to the fact TSharedPtr is created on the stack,
	whereas regular pointers are created on the heap.
=============================================================================
*/

//
//	TSharedPtr< T >
//
template< typename T >
class TSharedPtr {
public:
			TSharedPtr( T* ptr );
			TSharedPtr();
			TSharedPtr( const TSharedPtr<T>& src );
			virtual ~TSharedPtr();

	// Implicit conversions.
				operator T* () const;
	const T &	operator *  () const;
	const T *	operator -> () const;

	T &			operator *  ();
	T *			operator -> ();

	// Assignment.

	TSharedPtr< T > &	operator = ( const TSharedPtr<T>& rhs );
	TSharedPtr< T > &	operator = ( T* rhs );

	// Comparisons.

	bool	operator == ( const TSharedPtr<T>& rhs );
	bool	operator != ( const TSharedPtr<T>& rhs );

	bool	operator == ( T* rhs );
	bool	operator != ( T* rhs );

	// Getters/Setters.

	void	SetPointer( T* ptr );
	T *		GetPointer() const		{ return m_pPtr; }
	T &		GetReference() const	{ return *m_pPtr; }

private:
	void FinalizePointer();				// Releases pointer.
	void InitializePointer( T* ptr );	// Initializes pointer.

private:
	T *		m_pPtr;

private:
	// Keeps track of each pointer to instances of this class.
	static RBTreeMap< T*, mxUInt >	ms_RefCounts;
};

//
//	Static members.
//
template< typename T >
RBTreeMap< T*, mxUInt >	TSharedPtr<T>::ms_RefCounts;

/*================================
		TSharedPtr<T>
================================*/

template <typename T>
TSharedPtr<T>::TSharedPtr()
	: m_pPtr( NULL )
{}

template <typename T>
TSharedPtr<T>::TSharedPtr( T* ptr )
{
	InitializePointer( ptr );
}

template <typename T>
TSharedPtr<T>::TSharedPtr( const TSharedPtr<T>& src )
{
	InitializePointer( src.m_pPtr );
}

template <typename T>
TSharedPtr<T>& TSharedPtr<T>::operator = ( const TSharedPtr<T>& rhs )
{
	if ( this == &rhs ) {
		return *this;
	}
	FinalizePointer();
	InitializePointer( rhs.m_pPtr );

	return *this;
}

template <typename T>
TSharedPtr<T>& TSharedPtr<T>::operator = ( T* rhs )
{
	if ( m_pPtr == rhs ) {
		return *this;
	}
	if ( rhs )
	{
		if ( m_pPtr ) {
			FinalizePointer();
		}
		InitializePointer( rhs );
	}
	else {
		FinalizePointer();
	}
	return *this;
}

template <typename T>
TSharedPtr<T>::~TSharedPtr()
{
	FinalizePointer();
}

template <typename T>
void TSharedPtr<T>::SetPointer( T* ptr )
{
	if( m_pPtr == ptr ) {
		return;
	}
	if ( ptr )
	{
		if ( m_pPtr ) {
			FinalizePointer();
		}
		InitializePointer( ptr );
	}
	else {
		FinalizePointer();
	}
}

template <typename T>
void TSharedPtr<T>::InitializePointer( T* ptr )
{
	m_pPtr = ptr;

	if ( ms_RefCounts.find( m_pPtr ) == ms_RefCounts.end() ) {
		ms_RefCounts[ m_pPtr ] = 1;
	}
	else {
		++ms_RefCounts[ m_pPtr ];
	}
}

template <typename T>
void TSharedPtr<T>::FinalizePointer()
{
	if ( ms_RefCounts.find( m_pPtr ) == ms_RefCounts.end() ) {
		return;
	}
	--ms_RefCounts[ m_pPtr ];
	if( ms_RefCounts[ m_pPtr ] == 0 )
	{
		// No more references, so pointer can be safely deleted
		ms_RefCounts.erase( m_pPtr );
		delete m_pPtr;
	}
}

template <typename T>
const T* TSharedPtr<T>::operator -> () const
{
	return m_pPtr;
}

template <typename T>
TSharedPtr<T>::operator T* () const
{
	return m_pPtr;
}

template <typename T>
const T& TSharedPtr<T>::operator * () const
{
	return *m_pPtr;
}

template <typename T>
T* TSharedPtr<T>::operator -> ()
{
	return m_pPtr;
}

template <typename T>
T& TSharedPtr<T>::operator * ()
{
	return *m_pPtr;
}

template <typename T>
bool TSharedPtr<T>::operator == ( T* rhs )
{
	return m_pPtr == rhs;
}

template <typename T>
bool TSharedPtr<T>::operator != ( T* rhs )
{
	return m_pPtr != rhs;
}

template <typename T>
bool TSharedPtr<T>::operator == ( const TSharedPtr<T>& rhs )
{
	return m_pPtr == rhs.m_pPtr;
}

template <typename T>
bool TSharedPtr<T>::operator != ( const TSharedPtr<T>& rhs )
{
	return m_pPtr != rhs.m_pPtr;
}

MX_NAMESPACE_END

#endif // ! __MX_SHARED_POINTER_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//

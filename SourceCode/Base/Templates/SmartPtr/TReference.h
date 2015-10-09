/*
=============================================================================
	File:	TReference.h
	Desc:	Weak pointer - doesn't take ownership over the object,
			doesn't know about reference counting,
			always holds a valid pointer of a templated type.
=============================================================================
*/

#ifndef __MX_CONTAINTERS_REFERENCE_H__
#define __MX_CONTAINTERS_REFERENCE_H__

mxNAMESPACE_BEGIN

//
//	TReference< TYPE > - is a weak pointer
//	that always stores a non-null pointer
//	which cannot be reassigned.
//
template< typename TYPE >
class TReference
{
	TYPE *	m_pointer;

public:
	FORCEINLINE TReference( TYPE& reference )
	{
		AssertPtr( &reference );
		m_pointer = &reference;
	}

	FORCEINLINE ~TReference()
	{
		m_pointer = nil;
	}

	FORCEINLINE operator TYPE& () const
	{
		//AssertPtr( m_pointer );
		return *m_pointer;
	}
	FORCEINLINE operator const TYPE& () const
	{
		//AssertPtr( m_pointer );
		return *m_pointer;
	}

	FORCEINLINE bool operator == ( TYPE* pObject ) const
	{
		return ( m_pointer == pObject );
	}
	FORCEINLINE bool operator != ( TYPE* pObject ) const
	{
		return ( m_pointer != pObject );
	}
	FORCEINLINE bool operator == ( const TReference<TYPE>& other ) const
	{
		return ( m_pointer == other.m_pointer );
	}
	FORCEINLINE bool operator != ( const TReference<TYPE>& other ) const
	{
		return ( m_pointer != other.m_pointer );
	}

public:

	FORCEINLINE void Destruct()
	{
		AssertPtr( m_pointer );
		{
			m_pointer->~TYPE();
			m_pointer = nil;
		}
	}
	FORCEINLINE void Delete()
	{
		AssertPtr( m_pointer );
		{
			delete m_pointer;
			m_pointer = nil;
		}
	}

private:	PREVENT_COPY( TReference<TYPE> );
};

mxNAMESPACE_END

#endif // ! __MX_CONTAINTERS_REFERENCE_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//

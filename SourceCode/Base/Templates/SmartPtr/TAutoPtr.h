/*
=============================================================================
	File:	TAutoPtr.h
	Desc:	Smart pointer for automatic memory deallocation.
			Analog of std::auto_ptr or Boost's 'scoped_ptr'.
=============================================================================
*/

#ifndef __MX_TEMPLATE_AUTO_PTR_H__
#define __MX_TEMPLATE_AUTO_PTR_H__

mxNAMESPACE_BEGIN

//
//	TAutoPtr< T >
//
//	NOTE: This class is not safe for array new's.
//	It will not properly call the destructor for each element
//	and you will silently leak memory.
//	It does work for classes requiring no destructor however.
//
template< typename T >
class TAutoPtr {
public:
	FORCEINLINE explicit TAutoPtr( T* pointer = nil )
		: m_pointer( pointer )
	{}

	FORCEINLINE ~TAutoPtr()
	{
		if( m_pointer ) {
			free_one( m_pointer );
		}
	}

	FORCEINLINE T & operator * () const
	{
		AssertPtr( m_pointer );
		return *m_pointer;
	}

	FORCEINLINE T * operator -> () const
	{
		AssertPtr( m_pointer );
		return m_pointer;
	}

	FORCEINLINE operator T* ()
	{
	//	AssertPtr( Ptr );
		return m_pointer;
	}
	FORCEINLINE operator const T* () const
	{
	//	AssertPtr( Ptr );
		return m_pointer;
	}

	FORCEINLINE T* get_unsafe()
	{
		return m_pointer;
	}

	FORCEINLINE void operator = ( T* pointer )
	{
		if( m_pointer != pointer )
		{
			free_one( m_pointer );
		}
		m_pointer = pointer;
	}

	FORCEINLINE bool operator == ( T* pObject ) const
	{
		return ( m_pointer == pObject );
	}
	FORCEINLINE bool operator != ( T* pObject ) const
	{
		return ( m_pointer != pObject );
	}
	FORCEINLINE bool operator == ( const TAutoPtr<T>& other ) const
	{
		return ( m_pointer == other.Ptr );
	}
	FORCEINLINE bool operator != ( const TAutoPtr<T>& other ) const
	{
		return ( m_pointer != other.Ptr );
	}

private:
	T *		m_pointer;

private:
	// Disallow copies.
	TAutoPtr( const TAutoPtr<T>& other );
	TAutoPtr<T> & operator=( const TAutoPtr<T>& other );
};

mxNAMESPACE_END

#endif // ! __MX_TEMPLATE_AUTO_PTR_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//

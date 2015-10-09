/*
=============================================================================
	File:	StaticPool.h
	Desc:	Static memory pool.
=============================================================================
*/

#ifndef __STATIC_MEMORY_POOL_H__
#define __STATIC_MEMORY_POOL_H__

namespace lol {

//
//	TStaticPool - a static array which can be used as a memory pool.
//
template< typename T, const mxUInt MaxNumItems >
class TStaticPool {
public:
	mxUInt  GetCapacity() const { return MaxNumItems; }

	TStaticPool()
		: m_numItems( 0 )
	{}

	// Assumes that elements are pointers. Deletes elements.
	void DeleteItems()
	{
		// TODO: StaticAssert( T.__is_pointer() );
		for ( mxUInt i = 0; i < m_numItems; ++i ) {
			delete m_items[ i ];
		}
		m_numItems = 0;
	}

	T & operator [] ( mxUInt index )
	{
		BREAK_IF( !(index >= 0 && index < m_numItems) );
		return m_items[ index ];
	}

	const T & operator [] ( mxUInt index ) const
	{
		BREAK_IF( !(index >= 0 && index < m_numItems) );
		return m_items[ index ];
	}

	// Zero out the elements of the array.
	void Zero()
	{
		MemSet( m_items, 0, sizeof( T ) * MaxNumItems );
	}

	T * Ptr()
	{ return m_items; }

	const T * Ptr() const
	{ return m_items; }

	// Make sure that array can hold the given number elements.
	void AssureSize( mxUInt size )
	{ BREAK_IF( size < 0 || size >= MaxNumItems ); }

	// Allocates a new element at the end of the array and returns reference to this new data element.
	T & Alloc()
	{
		AssureSize( m_numItems + 1 );
		return m_items[ m_numItems++ ];
	}

	T & Alloc( mxUInt  count )
	{
		AssureSize( m_numItems + count );
		m_numItems += count;
		return m_items[ m_numItems-1 ];
	}

private:
	T		m_items[ MaxNumItems ];
	mxUInt	m_numItems;
};

}//end of namespace lol

//---------------------------------------------------------
//			Global scope.
//---------------------------------------------------------

/* TODO: how to implement this ?
//
// Overloaded operators for placement new.
// NOTE: must be global.

template< typename T >
FORCEINLINE void * operator new( size_t size, ::lol::TStaticPool< T,? > & rArray )
{
	return & rArray.Alloc();
	//return & ( rArray.All
}

// To prevent compiler warnings.
template< typename T >
FORCEINLINE void operator delete( void* p, ::lol::TStaticPool< T,? > & rArray )
{
	// nothing
}

*/

#endif // !__STATIC_MEMORY_POOL_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//

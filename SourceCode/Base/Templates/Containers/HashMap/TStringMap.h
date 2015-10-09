/*
=============================================================================
	File:	TStringMap.h
	Desc:	A templated hash table with string keys.
	ToDo:	Stop reinventing the wheel.
=============================================================================
*/

#ifndef __MX_STRING_HASH_H__
#define __MX_STRING_HASH_H__

#include <Base/Templates/Containers/HashMap/TMap.h>

mxNAMESPACE_BEGIN


//
//	TStringMap
//

template<
	typename VALUE,
	class HASH_FUNC = THashTrait< String >,
	class EQUALS_FUNC = TEqualsTrait< String >
>
class TStringMap
	: public TMap
	<
		String,
		VALUE,
		HASH_FUNC,
		EQUALS_FUNC
	>
{
public:
	explicit TStringMap( ENoInit )
		: TMap( _NoInit )
	{}

	explicit TStringMap( UINT tableSize = DEFAULT_HASH_TABLE_SIZE, HMemory heap = EMemHeap::DefaultHeap )
		: TMap( tableSize, heap )
	{}
};

#if 0
mxSWIPED("Id Software");
/*
=========================================================

	TStringHash< T >

	A simple hash table which uses strings as keys.

	Modelled on Id Software's class ( idHash ).
=========================================================
*/
template< typename T >
class TStringHash {
public:
					TStringHash( INT newtablesize = 256 );
					TStringHash( const TStringHash< T > &map );
					~TStringHash( void );

					// Returns a default-constructed value if the hash has no item with the given key.
					// NOTE: these are not recommended to use ( performance suffers ).
    T &				operator [] ( const char *key );
	const T			operator [] ( const char *key ) const;

					// Returns the value associated with the key key.
					// If the hash contains no item with key key, the function returns a default-constructed value.
	T				Value( const char *key, T defaultValue );

					// Adds a ( key, value ) pair. Returns false, if such a pair already exists.
	bool			Add( const char *key, T &value );
	void			Insert( const char *key, T &value );
	
	bool			Contains( const char *key ) const;

					// returns total size of allocated memory
	size_t			Allocated( void ) const;

					// returns total size of allocated memory including size of hash table T
	size_t			Size( void ) const;

	void			Set( const char *key, T &value );
	bool			Get( const char *key, T **value = NULL ) const;
	bool			Remove( const char *key );

	void			Clear( void );
	void			DeleteContents( void );

					// the entire contents can be iterated over, but note that the
					// exact index for a given element may change when new elements are added
	INT				Num( void ) const;
	T *				GetIndex( INT index, const String **key = NULL ) const; //HUMANHEAD mdl:  Added ability to retrieve key for saving hashtables.

	INT				GetSpread( void ) const;

private:
	struct hashnode_s {
		String		key;
		T			value;
		hashnode_s *next;

		hashnode_s( const String &k, T v, hashnode_s *n ) : key( k ), value( v ), next( n ) {};
		hashnode_s( const char *k, T v, hashnode_s *n ) : key( k ), value( v ), next( n ) {};
	};

	hashnode_s **	heads;

	INT				tablesize;
	INT				numentries;
	INT				tablesizemask;

	INT				GetHash( const char *key ) const;

	INLINE
	hashnode_s * CreateNode( const char *key, const T &value ) {
		hashnode_s *node, **nextPtr;
		INT hash, s;

		hash = GetHash( key );
		for( nextPtr = &(heads[hash]), node = *nextPtr; node != NULL; nextPtr = &(node->next), node = *nextPtr ) {
			s = strcmp( node->key.c_str(), key );

			if ( s == 0 ) {
				node->value = value;
				return 0;
			}
			if ( s > 0 ) {
				break;
			}
		}

		numentries++;

		*nextPtr = new hashnode_s( key, value, heads[ hash ] );
		(*nextPtr)->next = node;

		return *nextPtr;
	}

	INLINE 
	hashnode_s ** FindNode( const char *key ) const {
		INT hash = GetHash( key ), s;

		for( hashnode_s *node = heads[ hash ]; node; node = node->next ) {
			s = strcmp( node->key.c_str(), key );
			if ( s == 0 ) {
				return &node;
			}
			if ( s > 0 ) {
				break;
			}
		}
		return 0;
	}
};

/*
================
TStringHash< T >::TStringHash
================
*/
template< typename T >
INLINE TStringHash< T >::TStringHash( INT newtablesize )
{
	Assert( IsPowerOfTwo( newtablesize ) );

	tablesize = newtablesize;
	Assert( tablesize > 0 );

	heads = new hashnode_s *[ tablesize ];
	memset( heads, 0, sizeof( *heads ) * tablesize );

	numentries		= 0;

	tablesizemask = tablesize - 1;
}

/*
================
TStringHash< T >::TStringHash
================
*/
template< typename T >
INLINE TStringHash< T >::TStringHash( const TStringHash< T > &map )
{
	INT			i;
	hashnode_s	*node;
	hashnode_s	**prev;

	Assert( map.tablesize > 0 );

	tablesize		= map.tablesize;
	heads			= new hashnode_s *[ tablesize ];
	numentries		= map.numentries;
	tablesizemask	= map.tablesizemask;

	for( i = 0; i < tablesize; i++ ) {
		if ( !map.heads[ i ] ) {
			heads[ i ] = NULL;
			continue;
		}

		prev = &heads[ i ];
		for( node = map.heads[ i ]; node != NULL; node = node->next ) {
			*prev = new hashnode_s( node->key, node->value, NULL );
			prev = &( *prev )->next;
		}
	}
}

/*
================
TStringHash< T >::~TStringHash< T >
================
*/
template< typename T >
INLINE TStringHash< T >::~TStringHash( void ) {
	Clear();
	delete[] heads;
}

/*
================
TStringHash< T >::Allocated
================
*/
template< typename T >
INLINE size_t TStringHash< T >::Allocated( void ) const
{
	return sizeof( heads ) * tablesize + sizeof( *heads ) * numentries;
}

/*
================
TStringHash< T >::Size
================
*/
template< typename T >
INLINE size_t TStringHash< T >::Size( void ) const
{
	return sizeof( TStringHash< T > ) + sizeof( heads ) * tablesize + sizeof( *heads ) * numentries;
}

/*
================
TStringHash< T >::GetHash
================
*/
template< typename T >
INLINE INT TStringHash< T >::GetHash( const char *key ) const
{
	return ( NameHash( key ) & tablesizemask );
}

/*
================
TStringHash< T >::Set
================
*/
template< typename T >
INLINE void TStringHash< T >::Set( const char *key, T &value )
{
	hashnode_s *node, **nextPtr;
	INT hash, s;

	hash = GetHash( key );
	for( nextPtr = &(heads[hash]), node = *nextPtr; node != NULL; nextPtr = &(node->next), node = *nextPtr ) {
		s = strcmp( node->key.c_str(), key );
		if ( s == 0 ) {
			node->value = value;
			return;
		}
		if ( s > 0 ) {
			break;
		}
	}

	numentries++;

	*nextPtr = new hashnode_s( key, value, heads[ hash ] );
	(*nextPtr)->next = node;
}

/*
================
TStringHash< T >::Get
================
*/
template< typename T >
INLINE bool TStringHash< T >::Get( const char *key, T **value ) const
{
	hashnode_s *node;
	INT hash, s;

	hash = GetHash( key );
	for( node = heads[ hash ]; node != NULL; node = node->next ) {
		s = strcmp( node->key.c_str(), key );
		if ( s == 0 ) {
			if ( value ) {
				*value = &node->value;
			}
			return true;
		}
		if ( s > 0 ) {
			break;
		}
	}

	if ( value ) {
		*value = NULL;
	}

	return false;
}

/*
================
TStringHash< T >::GetIndex

the entire contents can be iterated over, but note that the
exact index for a given element may change when new elements are added
================
*/
template< typename T >
INLINE T *TStringHash< T >::GetIndex( INT index, const String **key ) const
{
	hashnode_s	*node;
	INT			count;
	INT			i;

	if ( ( index < 0 ) || ( index > numentries ) ) {
		Assert( 0 );
		return NULL;
	}

	count = 0;
	for( i = 0; i < tablesize; i++ ) {
		for( node = heads[ i ]; node != NULL; node = node->next ) {
			if ( count == index ) {
				//HUMANHEAD mdl
				if ( key ) {
					*key = &node->key;
				}
				//HUMANHEAD END
				return &node->value;
			}
			count++;
		}
	}

	//HUMANHEAD mdl
	if ( key ) {
		*key = NULL;
	}
	//HUMANHEAD END
	return NULL;
}

/*
================
TStringHash< T >::Remove
================
*/
template< typename T >
INLINE bool TStringHash< T >::Remove( const char *key )
{
	hashnode_s	**head;
	hashnode_s	*node;
	hashnode_s	*prev;
	INT			hash;

	hash = GetHash( key );
	head = &heads[ hash ];
	if ( *head ) {
		for( prev = NULL, node = *head; node != NULL; prev = node, node = node->next ) {
			if ( node->key == key ) {
				if ( prev ) {
					prev->next = node->next;
				} else {
					*head = node->next;
				}

				delete node;
				numentries--;
				return true;
			}
		}
	}

	return false;
}

/*
================
TStringHash< T >::Clear
================
*/
template< typename T >
INLINE void TStringHash< T >::Clear( void )
{
	INT			i;
	hashnode_s	*node;
	hashnode_s	*next;

	for( i = 0; i < tablesize; i++ ) {
		next = heads[ i ];
		while( next != NULL ) {
			node = next;
			next = next->next;
			delete node;
		}

		heads[ i ] = NULL;
	}

	numentries = 0;
}

/*
================
TStringHash< T >::DeleteContents
================
*/
template< typename T >
INLINE void TStringHash< T >::DeleteContents( void )
{
	INT			i;
	hashnode_s	*node;
	hashnode_s	*next;

	for( i = 0; i < tablesize; i++ ) {
		next = heads[ i ];
		while( next != NULL ) {
			node = next;
			next = next->next;
			delete node->value;
			delete node;
		}

		heads[ i ] = NULL;
	}

	numentries = 0;
}

/*
================
TStringHash< T >::Num
================
*/
template< typename T >
INLINE INT TStringHash< T >::Num( void ) const {
	return numentries;
}

/*
================
TStringHash< T >::GetSpread
================
*/
template< typename T >
INLINE INT TStringHash< T >::GetSpread( void ) const
{
	INT i, average, error, e;
	hashnode_s	*node;

	// if no items in hash
	if ( !numentries ) {
		return 100;
	}
	average = numentries / tablesize;
	error = 0;
	for ( i = 0; i < tablesize; i++ ) {
		numItems = 0;
		for( node = heads[ i ]; node != NULL; node = node->next ) {
			numItems++;
		}
		e = Abs( numItems - average );
		if ( e > 1 ) {
			error += e - 1;
		}
	}
	return 100 - (error * 100 / numentries);
}

/*
================
T&  TStringHash< T >::operator [] ( const char *key )
================
*/
template< typename T >
INLINE T& TStringHash< T >::operator [] ( const char *key ) 
{
	hashnode_s ** node = FindNode( key );
	if ( !node ) {
		return CreateNode( key, T() )->value;
	}
	return (*node)->value;
}

/*
================
T  TStringHash< T >::operator [] ( const char *key ) const
================
*/
template< typename T >
INLINE const T TStringHash< T >::operator [] ( const char *key ) const
{
	hashnode_s ** node = FindNode( key );
	if ( !node ) {
		return T();
	}
	return (*node)->value;
}

/*
================
   Returns the value associated with the key key.
   If the hash contains no item with the key key, the function returns a default-constructed value.
================
*/
template< typename T >
INLINE T TStringHash< T >::Value( const char *key, T defaultValue )
{
	INT	*p = 0;

	if ( !Get( key, &p ) ) {
		Set( key, defaultValue );
		return defaultValue;
	} else {
		return *p;
	}
}

/*
================
   Adds a ( key, value ) pair. Returns false, if such a pair already exists.
================
*/
template< typename T >
INLINE bool TStringHash< T >::Add( const char *key, T &value ) {
	if ( Get( key ) ) {
		return false;
	}
	Set( key,  value );

	return true;
}

template< typename T >
INLINE void TStringHash< T >::Insert( const char *key, T &value ) {
	Set( key,  value );
}

template< typename T >
INLINE bool TStringHash< T >::Contains( const char *key ) const {
	return Get( key );
}
#endif

mxNAMESPACE_END

#endif // ! __MX_STRING_HASH_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//

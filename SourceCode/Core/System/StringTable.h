/*
=============================================================================
	File:	StringTable.h
	Desc:	global string table,
			shared reference-counted strings for saving memory
=============================================================================
*/

#pragma once

mxNAMESPACE_BEGIN

/*
-----------------------------------------------------------------------------
	mxName
-----------------------------------------------------------------------------
*/

mxSWIPED("static_string by IronPeter");

class mxName
{
public:

	const char *c_str() const throw()
	{
		return &pointer->body[0];
	}
	const char* ToChars() const throw()
	{
		return c_str();
	}

	bool operator == ( const mxName &other ) const throw()
	{
		return pointer == other.pointer;
	}

	bool operator != ( const mxName &other ) const throw()
	{
		return pointer != other.pointer;
	}

	mxName &operator = ( const mxName &other ) throw()
	{
		mxName swp( other );
		strptr *temp = swp.pointer;
		swp.pointer = pointer;
		pointer = temp;
		return *this;
	}

	mxName()  throw()
	{
		pointer = &empty;
		pointer->refcounter++;
	}

	mxName( const mxName &other  ) throw()
	{
		pointer = other.pointer;
		pointer->refcounter++;
	}

	~mxName( )  throw()
	{
		if( --pointer->refcounter == 0 )
		{
			clear();
		}
	}

	mxName( const char *buff ) throw()
	{
		U4 hash, length;

		if( buff == 0 || buff[0] == 0 )
		{
			pointer = &empty;
			pointer->refcounter++;
			return;
		}

		str_hash( buff, hash, length );

		U4 position = hash % HASH_SIZE;
		strptr *entry = table[position];
		while( entry )
		{
			if( entry->length == length && entry->hash == hash && !memcmp( entry->body, buff, length ) )
			{
				pointer = entry;
				++entry->refcounter;
				return;
			}
			entry = entry->next;
		}

		++strnum;

		pointer = alloc_string( length );
		char *buffer = &pointer->body[0];
		memcpy( buffer, buff, length + 1 );
		pointer->hash = hash;
		pointer->next = table[position];
		pointer->refcounter = 1;
		pointer->length = length;
		table[position] = pointer;
	}

	U4 size() const throw()
	{
		return pointer->length;
	}

	U4 hash() const throw()
	{
		return pointer->hash;
	}

	UINT Length() const throw()
	{
		return size();
	}
	bool IsEmpty() const throw()
	{
		return size() == 0;
	}

	UINT NumRefs() const
	{
		return this->pointer->refcounter;
	}

	friend AStreamWriter& operator << ( AStreamWriter& file, const mxName& o );
	friend AStreamReader& operator >> ( AStreamReader& file, mxName& o );

	template< class S >
	friend S& operator & ( S & serializer, mxName & o )
	{
		return serializer.SerializeViaStream( o );
	}

public:

	static SizeT get_str_num() throw()
	{
		return strnum;
	}

	static SizeT get_str_memory() throw()
	{
		return strmemory;
	}

	static void StaticInitialize() throw();
	static void StaticShutdown() throw();


	enum
	{
		HASH_SIZE	= 65536,
		CACHE_SIZE	= 32,
		ALLOC_GRAN	= 8,
		ALLOC_SIZE	= 1024,
		MAX_SIZE = 512,
		MEM_HEAP = EMemHeap::HeapString,
	};

	struct strptr
	{
		strptr*	next;
		U4		refcounter;
		U4		length;
		U4		hash;
		char	body[4];
	};

	struct list_node
	{
		list_node *next;
	};

protected:
	strptr *pointer;

	static strptr    empty;
	static SizeT     strnum;
	static SizeT     strmemory;
	static strptr    *table[HASH_SIZE];
	static list_node *pointers[CACHE_SIZE];
	static list_node *allocs;

	void* Alloc( SizeT bytes )
	{
		return mxAllocX((EMemHeap)MEM_HEAP, bytes);
	}
	void Free( void* ptr )
	{
		mxFreeX((EMemHeap)MEM_HEAP, ptr);
	}

	strptr *alloc_string( U4 length ) throw()
	{
		U4 alloc = sizeof( strptr ) + length - 3; 
		U4 p = ( alloc - 1 ) / ALLOC_GRAN + 1;

		if( p >= CACHE_SIZE )
		{
			return (strptr *)Alloc( alloc );
		}

		alloc = p * ALLOC_GRAN;

		if( pointers[p] == 0 )
		{
			char *data = (char*) Alloc( ALLOC_SIZE );

			strmemory += ALLOC_SIZE;

			list_node *chunk = (list_node *)( data );
			chunk->next = allocs;
			allocs = chunk;

			for( SizeT i = sizeof( list_node ); i + alloc <= ALLOC_SIZE; i += alloc )
			{
				list_node *result = (list_node *)( data + i );
				result->next = pointers[p]; 
				pointers[p] = result;
			}
		}

		list_node *result = pointers[p];
		pointers[p] = result->next;
		return (strptr *)result;
	}

	void release_string( strptr *ptr ) throw()
	{
		U4 alloc = sizeof( strptr ) + ptr->length - 3; 
		U4 p = ( alloc - 1 ) / ALLOC_GRAN + 1;

		if( p >= CACHE_SIZE )
		{   
			Free( ptr );
			return;
		}

		list_node *result = (list_node *)ptr;
		result->next = pointers[p];
		pointers[p] = result;
	}

	void __forceinline str_hash( const char *str, U4 &hash, U4 &length ) throw()
	{       
		U4 res = 0;
		U4 i = 0;

		for(;;)
		{
			U4 v = str[i];
			res = res * 5 + v;
			if( v == 0 )
			{
				hash = res;
				length = i;
				return;
			}
			++i;
		};
	}

	void clear() throw()
	{
		U4 position = pointer->hash % HASH_SIZE;
		strptr *entry = table[position];
		strptr **prev = &table[position];
		while( entry )
		{
			if( entry == pointer )
			{
				*prev = pointer->next;
				release_string( pointer );

				if( --strnum == 0 )
				{
					while( allocs )
					{
						list_node *next = allocs->next;
						Free( allocs );
						allocs = next;
						strmemory -= ALLOC_SIZE;
					}

					for( SizeT i = 0; i < CACHE_SIZE; ++i )
					{
						pointers[i] = 0;
					}
				}

				return;
			}
			prev = &entry->next;
			entry = entry->next;
		}
	}
};

template<>
struct THashTrait< mxName >
{
	static FORCEINLINE UINT GetHashCode( const mxName& key )
	{
		return key.hash();
	}
};

mxNAMESPACE_END

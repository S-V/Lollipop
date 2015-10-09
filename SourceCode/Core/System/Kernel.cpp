/*
=============================================================================
	File:	Kernel.cpp
	Desc:	Core system kernel.
=============================================================================
*/

#include <Core_PCH.h>
#pragma hdrstop
#include <Core.h>

#include <Core/Kernel.h>

mxNAMESPACE_BEGIN

void ObjectGUID::ToChars( char *buffer, UINT maxChars ) const
{
	Assert( maxChars >= 8 );
	mxSTATIC_ASSERT( sizeof v == sizeof UINT );
	mxSPrintfAnsi( buffer, maxChars, "0x%x", v );
}

template< typename OBJECT, UINT MAX_OBJECTS >
class TStaticHandleManager
{
	struct SEntry
	{
		OBJECT *	p;
		U2		serialNumber;
		U2		nextFreeSlot;

	public:
		inline SEntry()
		{
			p = nil;
			serialNumber = 0;
			nextFreeSlot = -1;
		}
	};

	typedef TStaticArray< SEntry, MAX_OBJECTS >	EntriesList;

	UINT		m_firstFreeSlot;
	UINT		m_numEntries;
	EntriesList	m_table;

public:
	TStaticHandleManager()
	{
		this->Clear();
	}

	~TStaticHandleManager()
	{}

	void Clear()
	{
		m_firstFreeSlot = 0;
		m_numEntries = 0;

		for( UINT iEntry = 0; iEntry < MAX_OBJECTS; iEntry++ )
		{
			SEntry & entry = m_table[ iEntry ];
			entry.p = nil;
			entry.serialNumber = 0;
			entry.nextFreeSlot = iEntry+1;
		}

		m_table[ MAX_OBJECTS-1 ].nextFreeSlot = INDEX_NONE;
	}

	inline ObjectID Insert( OBJECT* ptr )
	{
		AssertPtr(ptr);
		Assert(m_numEntries <= MAX_OBJECTS);

		const UINT newIndex = m_firstFreeSlot;
		Assert(newIndex < MAX_OBJECTS);

		SEntry & newEntry = m_table[ newIndex ];
		{
			m_firstFreeSlot = newEntry.nextFreeSlot;

			newEntry.p = ptr;

			Assert(newEntry.serialNumber < Kernel::MAX_SERIAL_NUMBER);
			newEntry.serialNumber++;

			newEntry.nextFreeSlot = INDEX_NONE;
		}

		++m_numEntries;

		return ObjectID( newIndex, newEntry.serialNumber );
	}

	FORCEINLINE OBJECT* Has( ObjectID handle )
	{
		if( handle.IsNull() ) {
			return nil;
		}

		const SEntry& rEntry = m_table[ handle.GetEntryIndex() ];

		return rEntry.serialNumber == handle.GetSerialNumber() ? rEntry.p : nil;
	}

	FORCEINLINE OBJECT* LookUp( ObjectID handle )
	{
		Assert( handle.IsValid() );

		const SEntry& rEntry = m_table[ handle.GetEntryIndex() ];

		return rEntry.serialNumber == handle.GetSerialNumber() ? rEntry.p : nil;
	}

	inline void Remove( ObjectID handle )
	{
		const UINT index = handle.GetEntryIndex();

		Assert(m_table.IsValidIndex(index));
		Assert(m_numEntries > 0);

		SEntry & rEntry = m_table[ index ];
		{
			Assert(rEntry.serialNumber == handle.GetSerialNumber());

			rEntry.p = nil;

			Assert(rEntry.serialNumber < Kernel::MAX_SERIAL_NUMBER);
			rEntry.serialNumber++;

			rEntry.nextFreeSlot = m_firstFreeSlot;
		}

		m_firstFreeSlot = index;

		--m_numEntries;
	}

	FORCEINLINE UINT Num() const
	{
		return m_numEntries;
	}

	inline UINT GetIndexByPointer( const OBJECT* pResourceObject )
	{
		const UINT numEntries = m_table.Capacity();

		for( UINT iEntry = 0; iEntry < numEntries; iEntry++ )
		{
			const SEntry& rEntry = m_table[ iEntry ];

			if( rEntry.p == pResourceObject )
			{
				return iEntry;
			}
		}
		return INDEX_NONE;
	}
};


namespace Kernel
{
	struct SEntry
	{
		void *	ptr;
		UINT	serialNumber;
	};

	struct KernelData
	{
		// array for O(1) access to objects by handles
		TStaticHandleManager< void, (1<<15) >	entries;

	public:
		KernelData()
		{
		}
	};

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	static TPtr< KernelData >	GData;

	static bool g_bInitialized = false;

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	void Initialize()
	{
		Assert(!g_bInitialized);

		GData.ConstructInPlace();
		//GData->entries.Reserve(1024);

		g_bInitialized = true;
	}

	void Shutdown()
	{
		Assert(g_bInitialized);

		GData.Destruct();

		g_bInitialized = false;
	}

	ObjectID AddObject( void* entity )
	{
		//GData->entries.ReserveMore( 1 );

		return GData->entries.Insert( entity );
	}

	void RemoveObject( ObjectID handle )
	{
		return GData->entries.Remove( handle );
	}

	void* HasEntry( ObjectID handle )
	{
		return GData->entries.Has( handle );
	}

	void* LookupObject( ObjectID handle )
	{
		return GData->entries.LookUp( handle );
	}

}//namespace Kernel

mxNAMESPACE_END

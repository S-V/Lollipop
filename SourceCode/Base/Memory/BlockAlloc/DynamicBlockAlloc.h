/*
=============================================================================
	File:	DynamicBlockAlloc.h
	Desc:	Fast dynamic block allocator.
=============================================================================
*/

#ifndef __MX_DYNAMIC_BLOCK_ALLOC_H__
#define __MX_DYNAMIC_BLOCK_ALLOC_H__

#include <Base/Templates/Containers/HashMap/BTree.h>

mxSWIPED("Id Software");

mxNAMESPACE_BEGIN

/*
==============================================================================

	Fast dynamic block allocator.

	No constructor is called for the 'type'.
	Allocated blocks are always 16 byte aligned.

==============================================================================
*/

#if MX_DEBUG_MEMORY
	#define DYNAMIC_BLOCK_ALLOC_CHECK
#endif

template<class type>
class mxDynamicBlock {
public:
	type *							GetMemory( void ) const { return (type *)( ( (byte *) this ) + sizeof( mxDynamicBlock<type> ) ); }
	int								GetSize( void ) const { return Abs( size ); }
	void							SetSize( int s, bool isBaseBlock ) { size = isBaseBlock ? -s : s; }
	bool							IsBaseBlock( void ) const { return ( size < 0 ); }

#ifdef DYNAMIC_BLOCK_ALLOC_CHECK
	int								id[3];
	void *							allocator;
#endif

	int								size;					// size in bytes of the block
	mxDynamicBlock<type> *			prev;					// previous memory block
	mxDynamicBlock<type> *			next;					// next memory block
	mxBTreeNode<mxDynamicBlock<type>,int> *node;			// node in the B-Tree with free blocks
};

template<class type, int baseBlockSize, int minBlockSize>
class mxDynamicBlockAlloc {
public:
									mxDynamicBlockAlloc( void );
									~mxDynamicBlockAlloc( void );

	void							Init( void );
	void							Shutdown( void );

	bool	IsInitialized() const;

	void							SetFixedBlocks( int numBlocks );
	void							SetLockMemory( bool lock );
	void							FreeEmptyBaseBlocks( void );

	type *							Alloc( const int num );
	type *							Resize( type *ptr, const int num );
	void							Free( type *ptr );
	const char *					CheckMemory( const type *ptr ) const;

	int								GetNumBaseBlocks( void ) const { return numBaseBlocks; }
	int								GetBaseBlockMemory( void ) const { return baseBlockMemory; }
	int								GetNumUsedBlocks( void ) const { return numUsedBlocks; }
	int								GetUsedBlockMemory( void ) const { return usedBlockMemory; }
	int								GetNumFreeBlocks( void ) const { return numFreeBlocks; }
	int								GetFreeBlockMemory( void ) const { return freeBlockMemory; }
	int								GetNumEmptyBaseBlocks( void ) const;

	int			GetSizeOfBlock( const type* ptr ) const;

private:
	mxDynamicBlock<type> *			firstBlock;				// first block in list in order of increasing address
	mxDynamicBlock<type> *			lastBlock;				// last block in list in order of increasing address
	mxBTree<mxDynamicBlock<type>,int,4>freeTree;			// B-Tree with free memory blocks
	bool							allowAllocs;			// allow base block allocations
	bool							lockMemory;				// lock memory so it cannot get swapped out

#ifdef DYNAMIC_BLOCK_ALLOC_CHECK
	int								blockId[3];
#endif

	int								numBaseBlocks;			// number of base blocks
	int								baseBlockMemory;		// total memory in base blocks
	int								numUsedBlocks;			// number of used blocks
	int								usedBlockMemory;		// total memory in used blocks
	int								numFreeBlocks;			// number of free blocks
	int								freeBlockMemory;		// total memory in free blocks

	int								numAllocs;
	int								numResizes;
	int								numFrees;

	void							Clear( void );
	mxDynamicBlock<type> *			AllocInternal( const int num );
	mxDynamicBlock<type> *			ResizeInternal( mxDynamicBlock<type> *block, const int num );
	void							FreeInternal( mxDynamicBlock<type> *block );
	void							LinkFreeInternal( mxDynamicBlock<type> *block );
	void							UnlinkFreeInternal( mxDynamicBlock<type> *block );
	void							CheckMemory( void ) const;
};

template<class type, int baseBlockSize, int minBlockSize>
mxDynamicBlockAlloc<type, baseBlockSize, minBlockSize>::mxDynamicBlockAlloc( void ) {
	Clear();
}

template<class type, int baseBlockSize, int minBlockSize>
mxDynamicBlockAlloc<type, baseBlockSize, minBlockSize>::~mxDynamicBlockAlloc( void ) {
	Shutdown();
}

template<class type, int baseBlockSize, int minBlockSize>
void mxDynamicBlockAlloc<type, baseBlockSize, minBlockSize>::Init( void ) {
	freeTree.Init();
}

template<class type, int baseBlockSize, int minBlockSize>
void mxDynamicBlockAlloc<type, baseBlockSize, minBlockSize>::Shutdown( void ) {
	mxDynamicBlock<type> *block;

	for ( block = firstBlock; block != NULL; block = block->next ) {
		if ( block->node == NULL ) {
			FreeInternal( block );
		}
	}

	for ( block = firstBlock; block != NULL; block = firstBlock ) {
		firstBlock = block->next;
		Assert( block->IsBaseBlock() );
		if ( lockMemory ) {
			mxUnlockMemory( block, block->GetSize() + (int)sizeof( mxDynamicBlock<type> ) );
		}
		mxFree16( block );
	}

	freeTree.Shutdown();

	Clear();
}

template<class type, int baseBlockSize, int minBlockSize>
bool mxDynamicBlockAlloc<type, baseBlockSize, minBlockSize>::IsInitialized() const
{
	return (freeTree.GetRoot() != NULL);
}

template<class type, int baseBlockSize, int minBlockSize>
void mxDynamicBlockAlloc<type, baseBlockSize, minBlockSize>::SetFixedBlocks( int numBlocks ) {
	mxDynamicBlock<type> *block;

	for ( int i = numBaseBlocks; i < numBlocks; i++ ) {
		block = ( mxDynamicBlock<type> * ) mxMalloc16( baseBlockSize );
		if ( lockMemory ) {
			mxLockMemory( block, baseBlockSize );
		}
#ifdef DYNAMIC_BLOCK_ALLOC_CHECK
		memcpy( block->id, blockId, sizeof( block->id ) );
		block->allocator = (void*)this;
#endif
		block->SetSize( baseBlockSize - (int)sizeof( mxDynamicBlock<type> ), true );
		block->next = NULL;
		block->prev = lastBlock;
		if ( lastBlock ) {
			lastBlock->next = block;
		} else {
			firstBlock = block;
		}
		lastBlock = block;
		block->node = NULL;

		FreeInternal( block );

		numBaseBlocks++;
		baseBlockMemory += baseBlockSize;
	}

	allowAllocs = false;
}

template<class type, int baseBlockSize, int minBlockSize>
void mxDynamicBlockAlloc<type, baseBlockSize, minBlockSize>::SetLockMemory( bool lock ) {
	lockMemory = lock;
}

template<class type, int baseBlockSize, int minBlockSize>
void mxDynamicBlockAlloc<type, baseBlockSize, minBlockSize>::FreeEmptyBaseBlocks( void ) {
	mxDynamicBlock<type> *block, *next;

	for ( block = firstBlock; block != NULL; block = next ) {
		next = block->next;

		if ( block->IsBaseBlock() && block->node != NULL && ( next == NULL || next->IsBaseBlock() ) ) {
			UnlinkFreeInternal( block );
			if ( block->prev ) {
				block->prev->next = block->next;
			} else {
				firstBlock = block->next;
			}
			if ( block->next ) {
				block->next->prev = block->prev;
			} else {
				lastBlock = block->prev;
			}
			if ( lockMemory ) {
				mxUnlockMemory( block, block->GetSize() + (int)sizeof( mxDynamicBlock<type> ) );
			}
			numBaseBlocks--;
			baseBlockMemory -= block->GetSize() + (int)sizeof( mxDynamicBlock<type> );
			mxFree16( block );
		}
	}

#ifdef DYNAMIC_BLOCK_ALLOC_CHECK
	CheckMemory();
#endif
}

template<class type, int baseBlockSize, int minBlockSize>
int mxDynamicBlockAlloc<type, baseBlockSize, minBlockSize>::GetNumEmptyBaseBlocks( void ) const {
	int numEmptyBaseBlocks;
	mxDynamicBlock<type> *block;

	numEmptyBaseBlocks = 0;
	for ( block = firstBlock; block != NULL; block = block->next ) {
		if ( block->IsBaseBlock() && block->node != NULL && ( block->next == NULL || block->next->IsBaseBlock() ) ) {
			numEmptyBaseBlocks++;
		}
	}
	return numEmptyBaseBlocks;
}

template<class type, int baseBlockSize, int minBlockSize>
type *mxDynamicBlockAlloc<type, baseBlockSize, minBlockSize>::Alloc( const int num ) {
	mxDynamicBlock<type> *block;

	numAllocs++;

	if ( num <= 0 ) {
		return NULL;
	}

	block = AllocInternal( num );
	if ( block == NULL ) {
		return NULL;
	}
	block = ResizeInternal( block, num );
	if ( block == NULL ) {
		return NULL;
	}

#ifdef DYNAMIC_BLOCK_ALLOC_CHECK
	CheckMemory();
#endif

	numUsedBlocks++;
	usedBlockMemory += block->GetSize();

	return block->GetMemory();
}

template<class type, int baseBlockSize, int minBlockSize>
type *mxDynamicBlockAlloc<type, baseBlockSize, minBlockSize>::Resize( type *ptr, const int num ) {

	numResizes++;

	if ( ptr == NULL ) {
		return Alloc( num );
	}

	if ( num <= 0 ) {
		Free( ptr );
		return NULL;
	}

	mxDynamicBlock<type> *block = ( mxDynamicBlock<type> * ) ( ( (byte *) ptr ) - (int)sizeof( mxDynamicBlock<type> ) );

	usedBlockMemory -= block->GetSize();

	block = ResizeInternal( block, num );
	if ( block == NULL ) {
		return NULL;
	}

#ifdef DYNAMIC_BLOCK_ALLOC_CHECK
	CheckMemory();
#endif

	usedBlockMemory += block->GetSize();

	return block->GetMemory();
}

template<class type, int baseBlockSize, int minBlockSize>
int mxDynamicBlockAlloc<type, baseBlockSize, minBlockSize>::GetSizeOfBlock( const type* ptr ) const
{
#ifdef DYNAMIC_BLOCK_ALLOC_CHECK
	Assert( ptr );
#endif
	mxDynamicBlock<type> *block = ( mxDynamicBlock<type> * ) ( ( (byte *) ptr ) - (int)sizeof( mxDynamicBlock<type> ) );
	return block->GetSize();
}

template<class type, int baseBlockSize, int minBlockSize>
void mxDynamicBlockAlloc<type, baseBlockSize, minBlockSize>::Free( type *ptr ) {

	numFrees++;

	if ( ptr == NULL ) {
		return;
	}

	mxDynamicBlock<type> *block = ( mxDynamicBlock<type> * ) ( ( (byte *) ptr ) - (int)sizeof( mxDynamicBlock<type> ) );

	numUsedBlocks--;
	usedBlockMemory -= block->GetSize();

	FreeInternal( block );

#ifdef DYNAMIC_BLOCK_ALLOC_CHECK
	CheckMemory();
#endif
}

template<class type, int baseBlockSize, int minBlockSize>
const char *mxDynamicBlockAlloc<type, baseBlockSize, minBlockSize>::CheckMemory( const type *ptr ) const {
	mxDynamicBlock<type> *block;

	if ( ptr == NULL ) {
		return NULL;
	}

	block = ( mxDynamicBlock<type> * ) ( ( (byte *) ptr ) - (int)sizeof( mxDynamicBlock<type> ) );

	if ( block->node != NULL ) {
		return "memory has been freed";
	}

#ifdef DYNAMIC_BLOCK_ALLOC_CHECK
	if ( block->id[0] != 0x11111111 || block->id[1] != 0x22222222 || block->id[2] != 0x33333333 ) {
		return "memory has invalid id";
	}
	if ( block->allocator != (void*)this ) {
		return "memory was allocated with different allocator";
	}
#endif

	/* base blocks can be larger than baseBlockSize which can cause this code to fail
	mxDynamicBlock<type> *base;
	for ( base = firstBlock; base != NULL; base = base->next ) {
		if ( base->IsBaseBlock() ) {
			if ( ((int)block) >= ((int)base) && ((int)block) < ((int)base) + baseBlockSize ) {
				break;
			}
		}
	}
	if ( base == NULL ) {
		return "no base block found for memory";
	}
	*/

	return NULL;
}

template<class type, int baseBlockSize, int minBlockSize>
void mxDynamicBlockAlloc<type, baseBlockSize, minBlockSize>::Clear( void ) {
	firstBlock = lastBlock = NULL;
	allowAllocs = true;
	lockMemory = false;
	numBaseBlocks = 0;
	baseBlockMemory = 0;
	numUsedBlocks = 0;
	usedBlockMemory = 0;
	numFreeBlocks = 0;
	freeBlockMemory = 0;
	numAllocs = 0;
	numResizes = 0;
	numFrees = 0;

#ifdef DYNAMIC_BLOCK_ALLOC_CHECK
	blockId[0] = 0x11111111;
	blockId[1] = 0x22222222;
	blockId[2] = 0x33333333;
#endif
}

template<class type, int baseBlockSize, int minBlockSize>
mxDynamicBlock<type> *mxDynamicBlockAlloc<type, baseBlockSize, minBlockSize>::AllocInternal( const int num ) {
	mxDynamicBlock<type> *block;
	int alignedBytes = ( num * sizeof( type ) + 15 ) & ~15;

	block = freeTree.FindSmallestLargerEqual( alignedBytes );
	if ( block != NULL ) {
		UnlinkFreeInternal( block );
	} else if ( allowAllocs ) {
		int allocSize = Max( baseBlockSize, alignedBytes + (int)sizeof( mxDynamicBlock<type> ) );
		block = ( mxDynamicBlock<type> * ) mxMalloc16( allocSize );
		if ( lockMemory ) {
			mxLockMemory( block, baseBlockSize );
		}
#ifdef DYNAMIC_BLOCK_ALLOC_CHECK
		memcpy( block->id, blockId, sizeof( block->id ) );
		block->allocator = (void*)this;
#endif
		block->SetSize( allocSize - (int)sizeof( mxDynamicBlock<type> ), true );
		block->next = NULL;
		block->prev = lastBlock;
		if ( lastBlock ) {
			lastBlock->next = block;
		} else {
			firstBlock = block;
		}
		lastBlock = block;
		block->node = NULL;

		numBaseBlocks++;
		baseBlockMemory += allocSize;
	}

	return block;
}

template<class type, int baseBlockSize, int minBlockSize>
mxDynamicBlock<type> *mxDynamicBlockAlloc<type, baseBlockSize, minBlockSize>::ResizeInternal( mxDynamicBlock<type> *block, const int num ) {
	int alignedBytes = ( num * sizeof( type ) + 15 ) & ~15;

#ifdef DYNAMIC_BLOCK_ALLOC_CHECK
	Assert( block->id[0] == 0x11111111 && block->id[1] == 0x22222222 && block->id[2] == 0x33333333 && block->allocator == (void*)this );
#endif

	// if the new size is larger
	if ( alignedBytes > block->GetSize() ) {

		mxDynamicBlock<type> *nextBlock = block->next;

		// try to annexate the next block if it's free
		if ( nextBlock && !nextBlock->IsBaseBlock() && nextBlock->node != NULL &&
				block->GetSize() + (int)sizeof( mxDynamicBlock<type> ) + nextBlock->GetSize() >= alignedBytes ) {

			UnlinkFreeInternal( nextBlock );
			block->SetSize( block->GetSize() + (int)sizeof( mxDynamicBlock<type> ) + nextBlock->GetSize(), block->IsBaseBlock() );
			block->next = nextBlock->next;
			if ( nextBlock->next ) {
				nextBlock->next->prev = block;
			} else {
				lastBlock = block;
			}
		} else {
			// allocate a new block and copy
			mxDynamicBlock<type> *oldBlock = block;
			block = AllocInternal( num );
			if ( block == NULL ) {
				return NULL;
			}
			memcpy( block->GetMemory(), oldBlock->GetMemory(), oldBlock->GetSize() );
			FreeInternal( oldBlock );
		}
	}

	// if the unused space at the end of this block is large enough to hold a block with at least one element
	if ( block->GetSize() - alignedBytes - (int)sizeof( mxDynamicBlock<type> ) < Max( minBlockSize, (int)sizeof( type ) ) ) {
		return block;
	}

	mxDynamicBlock<type> *newBlock;

	newBlock = ( mxDynamicBlock<type> * ) ( ( (byte *) block ) + (int)sizeof( mxDynamicBlock<type> ) + alignedBytes );
#ifdef DYNAMIC_BLOCK_ALLOC_CHECK
	memcpy( newBlock->id, blockId, sizeof( newBlock->id ) );
	newBlock->allocator = (void*)this;
#endif
	newBlock->SetSize( block->GetSize() - alignedBytes - (int)sizeof( mxDynamicBlock<type> ), false );
	newBlock->next = block->next;
	newBlock->prev = block;
	if ( newBlock->next ) {
		newBlock->next->prev = newBlock;
	} else {
		lastBlock = newBlock;
	}
	newBlock->node = NULL;
	block->next = newBlock;
	block->SetSize( alignedBytes, block->IsBaseBlock() );

	FreeInternal( newBlock );

	return block;
}

template<class type, int baseBlockSize, int minBlockSize>
void mxDynamicBlockAlloc<type, baseBlockSize, minBlockSize>::FreeInternal( mxDynamicBlock<type> *block ) {

	Assert( block->node == NULL );

#ifdef DYNAMIC_BLOCK_ALLOC_CHECK
	Assert( block->id[0] == 0x11111111 && block->id[1] == 0x22222222 && block->id[2] == 0x33333333 && block->allocator == (void*)this );
#endif

	// try to merge with a next free block
	mxDynamicBlock<type> *nextBlock = block->next;
	if ( nextBlock && !nextBlock->IsBaseBlock() && nextBlock->node != NULL ) {
		UnlinkFreeInternal( nextBlock );
		block->SetSize( block->GetSize() + (int)sizeof( mxDynamicBlock<type> ) + nextBlock->GetSize(), block->IsBaseBlock() );
		block->next = nextBlock->next;
		if ( nextBlock->next ) {
			nextBlock->next->prev = block;
		} else {
			lastBlock = block;
		}
	}

	// try to merge with a previous free block
	mxDynamicBlock<type> *prevBlock = block->prev;
	if ( prevBlock && !block->IsBaseBlock() && prevBlock->node != NULL ) {
		UnlinkFreeInternal( prevBlock );
		prevBlock->SetSize( prevBlock->GetSize() + (int)sizeof( mxDynamicBlock<type> ) + block->GetSize(), prevBlock->IsBaseBlock() );
		prevBlock->next = block->next;
		if ( block->next ) {
			block->next->prev = prevBlock;
		} else {
			lastBlock = prevBlock;
		}
		LinkFreeInternal( prevBlock );
	} else {
		LinkFreeInternal( block );
	}
}

template<class type, int baseBlockSize, int minBlockSize>
FORCEINLINE void mxDynamicBlockAlloc<type, baseBlockSize, minBlockSize>::LinkFreeInternal( mxDynamicBlock<type> *block ) {
	block->node = freeTree.Add( block, block->GetSize() );
	numFreeBlocks++;
	freeBlockMemory += block->GetSize();
}

template<class type, int baseBlockSize, int minBlockSize>
FORCEINLINE void mxDynamicBlockAlloc<type, baseBlockSize, minBlockSize>::UnlinkFreeInternal( mxDynamicBlock<type> *block ) {
	freeTree.Remove( block->node );
	block->node = NULL;
	numFreeBlocks--;
	freeBlockMemory -= block->GetSize();
}

template<class type, int baseBlockSize, int minBlockSize>
void mxDynamicBlockAlloc<type, baseBlockSize, minBlockSize>::CheckMemory( void ) const {
	mxDynamicBlock<type> *block;

	for ( block = firstBlock; block != NULL; block = block->next ) {
		// make sure the block is properly linked
		if ( block->prev == NULL ) {
			Assert( firstBlock == block );
		} else {
			Assert( block->prev->next == block );
		}
		if ( block->next == NULL ) {
			Assert( lastBlock == block );
		} else {
			Assert( block->next->prev == block );
		}
	}
}

mxNAMESPACE_END

#endif /* ! __MX_DYNAMIC_BLOCK_ALLOC_H__ */

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//

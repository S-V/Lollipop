/*
=============================================================================
	File:	BlockAllocator.h
	Desc:	Block based allocator for fixed size objects.
	Note:	This has been swiped from idLib.
=============================================================================
*/

#ifndef __MX_BLOCK_ALLOCATOR_H__
#define __MX_BLOCK_ALLOCATOR_H__
mxSWIPED("Id Software");
mxNAMESPACE_BEGIN

/*
=============================================================================

	Block based allocator for fixed size objects.

	All objects of the 'type' are properly constructed.
	However, the constructor is not called for re-used objects.

=============================================================================
*/

template< typename type, INT blockSize >
class mxBlockAlloc {
public:
							mxBlockAlloc( void );
							~mxBlockAlloc( void );

	void					Shutdown( void );

	type *					Alloc( void );
	void					Free( type *element );

	INT						GetTotalCount( void ) const { return total; }
	INT						GetAllocCount( void ) const { return active; }
	INT						GetFreeCount( void ) const { return total - active; }

private:
	typedef struct element_s {
		struct element_s *	next;
		type				t;
	} element_t;

	typedef struct block_s {
		element_t			elements[blockSize];
		struct block_s *	next;
	} block_t;

	block_t *				blocks;
	element_t *				free;
	INT						total;
	INT						active;
};

template< typename type, INT blockSize >
mxBlockAlloc<type,blockSize>::mxBlockAlloc( void ) {
	blocks = NULL;
	free = NULL;
	total = active = 0;
}

template< typename type, INT blockSize >
mxBlockAlloc<type,blockSize>::~mxBlockAlloc( void ) {
	Shutdown();
}

template< typename type, INT blockSize >
type *mxBlockAlloc<type,blockSize>::Alloc( void ) {
	if ( !free ) {
		block_t *block = new block_t;
		block->next = blocks;
		blocks = block;
		for ( INT i = 0; i < blockSize; i++ ) {
			block->elements[i].next = free;
			free = &block->elements[i];
		}
		total += blockSize;
	}
	active++;
	element_t *element = free;
	free = free->next;
	element->next = NULL;
	return &element->t;
}

template< typename type, INT blockSize >
void mxBlockAlloc<type,blockSize>::Free( type *t ) {
	element_t *element = (element_t *)( ( (unsigned char *) t ) - ( (INT) &((element_t *)0)->t ) );
	element->next = free;
	free = element;
	active--;
}

template< typename type, INT blockSize >
void mxBlockAlloc<type,blockSize>::Shutdown( void ) {
	while( blocks ) {
		block_t *block = blocks;
		blocks = blocks->next;
		delete block;
	}
	blocks = NULL;
	free = NULL;
	total = active = 0;
}

mxNAMESPACE_END

#endif /* ! __MX_BLOCK_ALLOCATOR_H__ */

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//

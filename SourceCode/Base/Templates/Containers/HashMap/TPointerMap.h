/*
=============================================================================
	File:	TPointerMap.h
	Desc:	A templated hash table which maps pointer-sized integers
			to arbitrary values.
	ToDo:	Stop reinventing the wheel.
=============================================================================
*/

#ifndef __MX_TEMPLATE_POINTER_MAP_H__
#define __MX_TEMPLATE_POINTER_MAP_H__

#include <Base/Templates/Containers/HashMap/TMap.h>

mxNAMESPACE_BEGIN

// hash table which maps pointer-sized integers to arbitrary values
template<
	typename VALUE,
	class HASH_FUNC = mxPointerHasher
>
class TPointerMap
	: public TMap
	<
		const void*,
		VALUE,
		HASH_FUNC
	>
{
public:
	TPointerMap( ENoInit )
		: TMap( _NoInit )
	{}

	explicit TPointerMap( UINT tableSize = DEFAULT_HASH_TABLE_SIZE, HMemory heap = EMemHeap::DefaultHeap )
		: TMap( tableSize, heap )
	{}
};

mxNAMESPACE_END

#endif // ! __MX_TEMPLATE_POINTER_MAP_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//

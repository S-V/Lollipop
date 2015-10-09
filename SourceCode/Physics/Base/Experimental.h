/*
=============================================================================
	File:	Experimental.h
	Desc:	
=============================================================================
*/

#ifndef __PX_Experimental_H__
#define __PX_Experimental_H__

// used to fix up pointers when moving memory contents
//
struct pxPointerRelocator
{
	virtual void RelocatePointer( void *& ptr ) = 0;

	template< typename TYPE >
	inline void Relocate( TYPE *& ptr )
	{
		this->Relocate( *(void**)&ptr );
	}

protected:
	pxPointerRelocator() {}
	virtual ~pxPointerRelocator() {}
};

#endif // !__PX_Experimental_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//

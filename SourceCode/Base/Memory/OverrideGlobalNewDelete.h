// OverrideGlobalNewDelete.h
// include this file to override global 'new' and 'delete'

#pragma once


#if MX_OVERRIDE_GLOBAL_NEWDELETE
	#error Invalid option: MX_OVERRIDE_GLOBAL_NEWDELETE must not be defined prior to this point!
#else
	#undef MX_OVERRIDE_GLOBAL_NEWDELETE
	#define MX_OVERRIDE_GLOBAL_NEWDELETE	1
#endif

	#if defined(new) || defined(delete)
	#	error Operators 'new' and 'delete' must not be defined prior to this point!
	#endif

	#ifdef new
	#  undef new
	#endif
	#ifdef delete
	#  undef delete
	#endif

	#if MX_DEBUG_MEMORY != MEMORY_OPTION_USE_BUILT_IN_MEMORY_TRACKING
		#error Invalid option: This only works with CRT built-in debug memory layer enabled!
	#endif

	#include <crtdbg.h>

	#define DEBUG_NEW_CT new(_NORMAL_BLOCK, __FILE__, __LINE__)
	#define new DEBUG_NEW_CT



/*
	//
	// Overridden 'new' and 'delete' forwarding memory requests to our manager.
	//

	FORCEINLINE
	void* operator new( size_t numBytes )
	{
		mxUSING_NAMESPACE;
		return F_HeapAlloc( EMemHeap::DefaultHeap, numBytes );
	}

	FORCEINLINE
	void operator delete( void* pointer )
	{
		mxUSING_NAMESPACE;
		return F_HeapFree( EMemHeap::DefaultHeap, pointer );
	}

	// array versions

	FORCEINLINE
	void* operator new[]( size_t numBytes )
	{
		mxUSING_NAMESPACE;
		return F_HeapAlloc( EMemHeap::DefaultHeap, numBytes );
	}

	FORCEINLINE
	void operator delete [] ( void* pointer )
	{
		mxUSING_NAMESPACE;
		return F_HeapFree( EMemHeap::DefaultHeap, pointer );
	}

*/


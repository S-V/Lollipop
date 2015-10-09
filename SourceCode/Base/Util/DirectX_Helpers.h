/*
=============================================================================
	File:	DirectX_Helpers.h
	Desc:	Common code for programming with DirectX.
	ToDo:	move this header into GraphicsLib
=============================================================================
*/
#pragma once

//------------------------------------------------------------------------
//	Useful macros
//------------------------------------------------------------------------

#ifndef SAFE_RELEASE
#define SAFE_RELEASE( p )		{ if( p != nil ) { (p)->Release(); (p) = nil; } }
#endif

#define SAFE_ADDREF( p )		{ if( p != nil ) { (p)->AddRef(); }
#define SAFE_ACQUIRE( dst, p )	{ if( dst != nil ) { SAFE_RELEASE(dst); } if( p != nil ) { (p)->AddRef(); } dst = (p); }

#define RELEASE( p )			(p)->Release()


template< class TYPE, UINT COUNT >
inline
void D3D_SafeReleaseArray( TYPE* (&elements) [COUNT] )
{
	for( UINT iElement = 0; iElement < COUNT; ++iElement )
	{
		if( elements[ iElement ] != nil )
		{
			elements[ iElement ]->Release();
		}
	}
}

//------------------------------------------------------------------------
//	Declarations
//------------------------------------------------------------------------

template< class T >
FORCEINLINE
void SafeRelease( T *&p )
{
	if( p != nil ) {
		p->Release();
		p = nil;
	}
}

FORCEINLINE
bool dxCheckedRelease( IUnknown* pInterface )
{
	const ULONG refCount = pInterface->Release();
	Assert( refCount == 0 );
	return( refCount == 0 );
}


// Verify that a pointer to a COM object is still valid
//
// Usage:
//   VERIFY_COM_INTERFACE(pFoo);
//
template< class Q >
void VERIFY_COM_INTERFACE( Q* p )
{
#if MX_DEBUG
	p->AddRef();
	p->Release();
#endif // MX_DEBUG
}

template< class Q >
void FORCE_RELEASE_COM_INTERFACE( Q*& p )
{
	RX_KLUDGE("used to get around the live device warning - this is possibly a bug in SDKLayer");
	while( p->Release() )
		;
	p = nil;
}



template< class Q >
void DbgPutNumRefs( Q* p, const char* msg = nil )
{
#if MX_DEBUG
	// the new reference count
	const UINT numRefs = p->AddRef();
	DBGOUT( "%sNumRefs = %u\n", (msg != nil) ? msg : "", numRefs );
	p->Release();
#endif // MX_DEBUG
}

//
//	dxPtr< T > - smart pointer for safe and automatic handling of DirectX resources.
//
template< class T >
struct dxPtr 
{
	T *	Ptr;

public:
	FORCEINLINE dxPtr()
		: Ptr( NULL )
	{}

	FORCEINLINE dxPtr( T * pointer )
		: Ptr( pointer )
	{
		if ( Ptr ) {
			AcquirePointer();
		}
	}

	FORCEINLINE dxPtr( const dxPtr<T> & other )
	{
		Ptr = other.Ptr;
		if ( Ptr ) {
			AcquirePointer();
		}
	}

	FORCEINLINE ~dxPtr()
	{
		if ( Ptr ) {
			ReleasePointer();
		}
	}

	FORCEINLINE void Release()
	{
		if ( Ptr ) {
			ReleasePointer();
			Ptr = nil;
		}
	}

	FORCEINLINE void operator = ( T * pointer )
	{
		if ( Ptr ) {
			ReleasePointer();
		}
		
		Ptr = pointer;
		
		if ( Ptr ) {
			AcquirePointer();
		}
	}

	FORCEINLINE void operator = ( const dxPtr<T> & other )
	{
		if ( Ptr ) {
			ReleasePointer();
		}

		Ptr = other.Ptr;

		if ( Ptr ) {
			AcquirePointer();
		}
	}

	FORCEINLINE operator T* () const
	{
		return Ptr;
	}

	FORCEINLINE T * operator -> () const
	{
		AssertPtr( Ptr );
		return Ptr;
	}

	FORCEINLINE bool operator == ( const T * pointer )
	{
		return this->Ptr == pointer;
	}
	FORCEINLINE bool operator == ( const dxPtr<T> & other )
	{
		return this->Ptr == other.Ptr;
	}

	FORCEINLINE bool IsNull() const
	{
		return ( nil == Ptr );
	}
	FORCEINLINE bool IsValid() const
	{
		return ( nil != Ptr );
	}

	FORCEINLINE T** ToArrayPtr()
	{
		return &Ptr;
	}
	//@todo: assignGet() ?

private:
	// assumes that the pointer is not null
	FORCEINLINE void AcquirePointer()
	{
		AssertPtr( Ptr );
		Ptr->AddRef();
	}

	// assumes that the pointer is not null
	FORCEINLINE void ReleasePointer()
	{
		AssertPtr( Ptr );
		Ptr->Release();
	}
};


#define mxDECLARE_COM_PTR( className )\
	typedef dxPtr< className > className ## Ptr


//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//

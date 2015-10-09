/*
=============================================================================
	File:	ReferenceCounted.h
	Desc:	A very simple basic class for reference counted objects (analog of Boost's 'intrusive_ptr').
	Note:	Reference counted objects MUST be accessed through smart pointers
			at all times to make sure reference counting scheme operates correctly.
			Failure to do so will result in strange and unpredictable behaviour and crashes.
=============================================================================
*/

#ifndef __MX_REFERENCE_COUNTED_H__
#define __MX_REFERENCE_COUNTED_H__

/*
=======================================================

	ReferenceCounted

=======================================================
*/

mxNAMESPACE_BEGIN

//
//	ReferenceCounted - calls 'delete' on the object when the reference count reaches zero.
//
class ReferenceCounted {
public:
	void	Grab() const;
	bool	Drop() const;	// ( Returns true if the object has been deleted. )

	INT	GetReferenceCount() const;

	ReferenceCounted& operator = ( const ReferenceCounted& other );

protected:
			ReferenceCounted();
	virtual	~ReferenceCounted();

private:
	volatile mutable INT	referenceCounter;	// Number of references to this object.
};

/*================================
		ReferenceCounted
================================*/

FORCEINLINE ReferenceCounted::ReferenceCounted()
	: referenceCounter( 0 )
{}

FORCEINLINE ReferenceCounted::~ReferenceCounted()
{
	Assert( 0 == referenceCounter );
}

FORCEINLINE void ReferenceCounted::Grab() const {
	++referenceCounter;
}

FORCEINLINE bool ReferenceCounted::Drop() const
{	Assert( referenceCounter > 0 );
	--referenceCounter;
	if ( referenceCounter == 0 )
	{
		// 'delete this' is evil (?)
		delete( this );
		return true;
	}
	return false;
}

FORCEINLINE INT ReferenceCounted::GetReferenceCount() const
{
	return referenceCounter;
}

FORCEINLINE ReferenceCounted& ReferenceCounted::operator = ( const ReferenceCounted& other )
{
	return *this;
}

//
//	ReferenceCountedX - the same as ReferenceCounted, but calls Destroy() when the reference count reaches zero.
//
class ReferenceCountedX {
public:
	// 'delete this' is evil (?)
	virtual void Destroy() { delete( this ); }

	void	Grab() const;
	bool	Drop() const;	// ( Returns true if the object has been deleted. )

	INT	GetReferenceCount() const;

	ReferenceCountedX& operator = ( const ReferenceCountedX& other );

	// so that property system works
	FORCEINLINE U4 GetNumRefs() const
	{
		return static_cast<U4>(this->GetReferenceCount());
	}

protected:
			ReferenceCountedX();
	virtual	~ReferenceCountedX();

private:
	volatile mutable INT	referenceCounter;	// Number of references to this object.
};

/*================================
		ReferenceCountedX
================================*/

FORCEINLINE ReferenceCountedX::ReferenceCountedX()
	: referenceCounter( 0 )
{}

FORCEINLINE ReferenceCountedX::~ReferenceCountedX()
{
	Assert( 0 == referenceCounter );
}

FORCEINLINE void ReferenceCountedX::Grab() const {
	++referenceCounter;
}

FORCEINLINE bool ReferenceCountedX::Drop() const
{	Assert( referenceCounter > 0 );
	--referenceCounter;
	if ( referenceCounter == 0 )
	{
		ReferenceCountedX* nonConstPtr = const_cast<ReferenceCountedX*>(this);
		nonConstPtr->Destroy();
		return true;
	}
	return false;
}

FORCEINLINE INT ReferenceCountedX::GetReferenceCount() const
{
	return referenceCounter;
}

FORCEINLINE ReferenceCountedX& ReferenceCountedX::operator = ( const ReferenceCountedX& other )
{
	return *this;
}

//----------------------------------------------------------------------------------------------------------------------

//
//	Grab( ReferenceCounted* ) - increments the reference count of the given object.
//
FORCEINLINE void GRAB( ReferenceCounted* p )
{
	if (PtrToBool( p ))
	{
		p->Grab();
	}
}

//
//	Drop( ReferenceCounted * ) - decrements the reference count of the given object and sets the pointer to null.
//
FORCEINLINE void DROP( ReferenceCounted *& p )
{
	if (PtrToBool( p ))
	{
		p->Drop();
		p = nil;
	}
}

//----------------------------------------------------------------------------------------------------------------------

//
//	TRefPtr< T > - a pointer to a reference-counted object (aka 'instrusive pointer').
//
template< typename T >	// where T : ReferenceCounted
class TRefPtr
{
	T *		m_pObject;	// The shared reference counted object.

public:
			TRefPtr();
			/*explicit*/ TRefPtr( T* refCounted );
			TRefPtr( T* refCounted, EDontAddRef );
			TRefPtr( const TRefPtr& other );
			~TRefPtr();

	// Implicit conversions.

			operator T*	() const;
	T &		operator *	() const;
	T *		operator ->	() const;

	bool	operator !	() const;

	// Assignment.

	TRefPtr &	operator = ( T* pObject );
	TRefPtr &	operator = ( const TRefPtr& other );

	// Comparisons.

	bool	operator == ( T* pObject ) const;
	bool	operator != ( T* pObject ) const;
	bool	operator == ( const TRefPtr& other ) const;
	bool	operator != ( const TRefPtr& other ) const;

	FORCEINLINE		bool	isNull() const { return (nil == m_pObject); }
	FORCEINLINE		bool	isValid() const { return (nil != m_pObject); }

	// Unsafe...
	FORCEINLINE		T *		get_ptr()	{ return m_pObject; }
	FORCEINLINE		T *&	get_ref()	{ return m_pObject; }

private:
	void _dbgChecks() { mxCOMPILE_TIME_ASSERT(sizeof(*this) == sizeof(T*)); }
};

template< typename T >
FORCEINLINE TRefPtr< T >::TRefPtr()
	: m_pObject( nil )
{
	_dbgChecks();
}

template< typename T >
FORCEINLINE TRefPtr< T >::TRefPtr( T* refCounted )
	: m_pObject( refCounted )
{
	if (PtrToBool( m_pObject )) {
		m_pObject->Grab();
	}
	_dbgChecks();
}

template< typename T >
FORCEINLINE TRefPtr< T >::TRefPtr( T* refCounted, EDontAddRef )
	: m_pObject( refCounted )
{
	_dbgChecks();
}

template< typename T >
FORCEINLINE TRefPtr< T >::TRefPtr( const TRefPtr& other )
	: m_pObject( other.m_pObject )
{
	if (PtrToBool( m_pObject )) {
		m_pObject->Grab();
	}
	_dbgChecks();
}

template< typename T >
FORCEINLINE TRefPtr< T >::~TRefPtr()
{
	if (PtrToBool( m_pObject )) {
		m_pObject->Drop();
	}
	m_pObject = nil;
}

template< typename T >
FORCEINLINE TRefPtr< T >::operator T* () const
{//	Assert( m_pObject );
	return m_pObject;
}

template< typename T >
FORCEINLINE T & TRefPtr< T >::operator * () const
{	Assert( m_pObject );
	return *m_pObject;
}

template< typename T >
FORCEINLINE T * TRefPtr< T >::operator -> () const
{
	return m_pObject;
}

template< typename T >
FORCEINLINE bool TRefPtr< T >::operator ! () const
{
	return m_pObject == nil;
}

template< typename T >
FORCEINLINE TRefPtr< T > & TRefPtr< T >::operator = ( T* pObject )
{
	if ( pObject == nil )
	{
		if (PtrToBool( m_pObject )) {
			m_pObject->Drop();
			m_pObject = nil;
		}
		return *this;
	}
	if ( m_pObject != pObject )
	{
		if (PtrToBool( m_pObject )) {
			m_pObject->Drop();
		}
		m_pObject = pObject;
		m_pObject->Grab();
		return *this;
	}
	return *this;
}

template< typename T >
FORCEINLINE TRefPtr< T > & TRefPtr< T >::operator = ( const TRefPtr& other )
{
	return ( *this = other.m_pObject );
}

template< typename T >
FORCEINLINE bool TRefPtr< T >::operator == ( T* pObject ) const
{
	return m_pObject == pObject;
}

template< typename T >
FORCEINLINE bool TRefPtr< T >::operator != ( T* pObject ) const
{
	return m_pObject != pObject;
}

template< typename T >
FORCEINLINE bool TRefPtr< T >::operator == ( const TRefPtr& other ) const
{
	return m_pObject == other.m_pObject;
}

template< typename T >
FORCEINLINE bool TRefPtr< T >::operator != ( const TRefPtr& other ) const
{
	return m_pObject != other.m_pObject;
}


#if 0
//
//	TRef< T > - is a TRefPtr<T> which can never be null
//	(and so null pointer checks can be skipped).
//
template< typename T >	// where T : ReferenceCounted
class TRef
{
	T &		m_rObject;	// The shared reference counted object.

public:
			TRef( T* refCounted );
			TRef( const TRef& other );
			~TRef();

	// Implicit conversions.

			operator T*	() const;
	T &		operator *	() const;
	T *		operator ->	() const;

	bool	operator !	() const;

	// Assignment.

	TRef &	operator = ( T* pObject );
	TRef &	operator = ( const TRef& other );

	// Comparisons.

	bool	operator == ( T* pObject ) const;
	bool	operator != ( T* pObject ) const;
	bool	operator == ( const TRef& other ) const;
	bool	operator != ( const TRef& other ) const;

	FORCEINLINE		bool	isValid() const { return true; }

	// Unsafe...
	FORCEINLINE		T *		get_ptr()	{ return &m_rObject; }
	FORCEINLINE		T *&	get_ref()	{ return &m_rObject; }

private:
	void _dbgChecks() { COMPILE_TIME_ASSERT(sizeof(*this) == sizeof(T*)); }
};

template< typename T >
FORCEINLINE TRef< T >::TRef( T* refCounted )
{
	AssertPtr( refCounted );
	m_rObject = *refCounted;
	m_rObject.Grab();

	_dbgChecks();
}

template< typename T >
FORCEINLINE TRef< T >::TRef( const TRef& other )
{
	m_rObject = other.m_rObject;
	m_rObject.Grab();

	_dbgChecks();
}

template< typename T >
FORCEINLINE TRef< T >::~TRef()
{
	m_rObject.Drop();
}

template< typename T >
FORCEINLINE TRef< T >::operator T* () const
{
	return &m_rObject;
}

template< typename T >
FORCEINLINE T & TRef< T >::operator * () const
{
	return m_rObject;
}

template< typename T >
FORCEINLINE T * TRef< T >::operator -> () const
{
	return &m_rObject;
}

template< typename T >
FORCEINLINE bool TRef< T >::operator ! () const
{
	return true;
}

template< typename T >
FORCEINLINE TRef< T > & TRef< T >::operator = ( T* pObject )
{
	AssertPtr( pObject );
	if ( &m_rObject != pObject )
	{
		m_rObject.Drop();
		m_rObject = *pObject;
		m_rObject.Grab();
		return *this;
	}
	return *this;
}

template< typename T >
FORCEINLINE TRef< T > & TRef< T >::operator = ( const TRef& other )
{
	return ( *this = other.m_rObject );
}

template< typename T >
FORCEINLINE bool TRef< T >::operator == ( T* pObject ) const
{
	return &m_rObject == pObject;
}

template< typename T >
FORCEINLINE bool TRef< T >::operator != ( T* pObject ) const
{
	return &m_rObject != pObject;
}

template< typename T >
FORCEINLINE bool TRef< T >::operator == ( const TRef& other ) const
{
	return m_rObject == other.m_rObject;
}

template< typename T >
FORCEINLINE bool TRef< T >::operator != ( const TRef& other ) const
{
	return m_rObject != other.m_rObject;
}

#endif


/*
--------------------------------------------------------------
	TRefCounted< T >
--------------------------------------------------------------
*/
template< class TYPE >
class TRefCounted : public ReferenceCounted
{
public:
	typedef TRefPtr< TYPE >	Ref;
};



//----------------------------------------------------------------------------------------------------------------------

template< class TYPE >
class RefPtrDropUtil {
public:
	inline RefPtrDropUtil()
	{}

	inline void operator () ( TRefPtr<TYPE>& ptr ) {
		ptr = nil;
	}
	//inline void operator () ( TRef<TYPE>& ptr ) {
	//	ptr = nil;
	//}
	inline void operator () ( ReferenceCounted* ptr ) {
		ptr->Drop();
	}
	inline void operator () ( ReferenceCountedX* ptr ) {
		ptr->Drop();
	}
};

template< class TYPE >
class RefPtrDropUtilX
{
	UINT numDecrements;

public:
	inline RefPtrDropUtilX( UINT numDecrements = 1 )
		: numDecrements(numDecrements)
	{}

	inline void operator () ( TRefPtr<TYPE>& ptr ) {
		for( UINT i = 0; i < numDecrements; i++ ) {
			ptr = nil;
		}
	}
	//inline void operator () ( TRef<TYPE>& ptr ) {
	//	for( UINT i = 0; i < numDecrements; i++ ) {
	//		ptr = nil;
	//	}
	//}
	inline void operator () ( ReferenceCounted* ptr ) {
		for( UINT i = 0; i < numDecrements; i++ ) {
			ptr->Drop();
		}
	}
	inline void operator () ( ReferenceCountedX* ptr ) {
		for( UINT i = 0; i < numDecrements; i++ ) {
			ptr->Drop();
		}
	}
};


mxNAMESPACE_END

//----------------------------------------------------------------------------------------------------------------------

//#define DECLARE_REF_COUNTED( className )	\
//	DECLARE_CLASS( className, ReferenceCounted );	\
//	typedef TRefPtr< ThisType >	Ref;
//
//
//#define DECLARE_REF_COUNTEDX( className )	\
//	DECLARE_CLASS( className, ReferenceCounted );	\
//	typedef TRefPtr< ThisType >	Ref;


#endif // ! __MX_REFERENCE_COUNTED_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//

/*
=============================================================================
	File:	Singleton.h
	Desc:	A very simple singleton template.
	Note:	Don't (mis)use singletons!
=============================================================================
*/

#ifndef __MX_TEMPLATE_SINGLETON_H__
#define __MX_TEMPLATE_SINGLETON_H__

mxNAMESPACE_BEGIN

//
//	TGlobal< TYPE >
//
// If you want to make object of some class singleton
// you have to derive this class from TGlobal class.
//
template< class TYPE >
class TGlobal {
public:
	TGlobal()
	{
		AssertX(!HasInstance(),"Singleton class has already been instantiated.");
		gInstance = static_cast< TYPE* >( this );
	}
	~TGlobal()
	{
		Assert(HasInstance());
		gInstance = nil;
	}

	static bool HasInstance()
	{
		return (gInstance != nil);
	}

	static TYPE & Get()
	{
		Assert(HasInstance());
		return (*gInstance);
	}
	static TYPE* Ptr()
	{
		Assert(HasInstance());
		return gInstance;
	}

	typedef TGlobal<TYPE> THIS_TYPE;

private:PREVENT_COPY( THIS_TYPE );
	// A static pointer to an object of T type.
	// This is the core member of the singleton. 
	// As it is declared as static no more than
	// one object of this type can exist at the time.
	//
	MX_THREAD_LOCAL static TYPE *	gInstance; // the one and only instance
};

template< typename TYPE >
MX_THREAD_LOCAL TYPE * TGlobal< TYPE >::gInstance = nil;


/*
==========================================================
	This can be used to ensure that a particular function
	only gets called one time.
==========================================================
*/
#if MX_DEBUG

	#define DBG_ENSURE_ONLY_ONE_CALL						\
	{													\
		static bool Has_Already_Been_Called = false;	\
		if ( Has_Already_Been_Called )					\
		{												\
			UnreachableX( "singleton error" );			\
		}												\
		Has_Already_Been_Called = true;					\
	}

#else

	#define DBG_ENSURE_ONLY_ONE_CALL

#endif //MX_DEBUG

/*
==========================================================
	Helper macros to implement singleton objects:
    
    - DECLARE_SINGLETON      put this into class declaration
    - IMPLEMENT_SINGLETON    put this into the implemention file
    - CONSTRUCT_SINGLETON    put this into the constructor
    - DESTRUCT_SINGLETON     put this into the destructor

    Get a pointer to a singleton object using the static Instance() method:

    Core::Server* coreServer = Core::Server::Instance();
==========================================================
*/


#define DECLARE_SINGLETON(type) \
public: \
    MX_THREAD_LOCAL static type * TheSingleton; \
    static type * Instance() { Assert(nil != TheSingleton); return TheSingleton; }; \
    static bool HasInstance() { return nil != TheSingleton; }; \
	static type & Get() { Assert(nil != TheSingleton); return (*TheSingleton); }; \
private:	PREVENT_COPY(type);


#define DECLARE_INTERFACE_SINGLETON(type) \
public: \
    static type * TheSingleton; \
    static type * Instance() { Assert(nil != TheSingleton); return TheSingleton; }; \
    static bool HasInstance() { return nil != TheSingleton; }; \
	static type & Get() { Assert(nil != TheSingleton); return (*TheSingleton); }; \
private:


#define IMPLEMENT_SINGLETON(type) \
    MX_THREAD_LOCAL type * type::TheSingleton = nil;


#define IMPLEMENT_INTERFACE_SINGLETON(type) \
    type * type::TheSingleton = nil;


#define CONSTRUCT_SINGLETON \
    Assert(nil == TheSingleton); TheSingleton = this;


#define CONSTRUCT_INTERFACE_SINGLETON \
    Assert(nil == TheSingleton); TheSingleton = this;


#define DESTRUCT_SINGLETON \
    Assert(TheSingleton); TheSingleton = nil;


#define DESTRUCT_INTERFACE_SINGLETON \
    Assert(TheSingleton); TheSingleton = nil;


//===========================================================================

#if MX_DEBUG

	template< class TYPE >
	struct SingleInstance
	{
	protected:
		SingleInstance()
		{
			Assert( !ms__hasBeenCreated );
			ms__hasBeenCreated = true;
		}
		~SingleInstance()
		{
			Assert( ms__hasBeenCreated );
			ms__hasBeenCreated = false;
		}
	private:
		static bool ms__hasBeenCreated;
	};
	
	template< class TYPE >
	bool SingleInstance<TYPE>::ms__hasBeenCreated = false;

#else // MX_DEBUG

	template< class TYPE >
	struct SingleInstance
	{
	protected:
		SingleInstance()
		{
		}
		~SingleInstance()
		{
		}
	};

#endif // !MX_DEBUG


//===========================================================================

template< class TYPE >
struct InstanceCounter
{
	static UINT TotalNumInstances()
	{
		return msTotalNumInstances;
	}

protected:
	InstanceCounter()
	{
		++msTotalNumInstances;
	}
	~InstanceCounter()
	{
		--msTotalNumInstances;
	}
private:
	static UINT msTotalNumInstances;
};

template< typename TYPE >
UINT InstanceCounter<TYPE>::msTotalNumInstances = 0;



//===========================================================================

template< class KLASS >	// where KLASS : TGlobal<KLASS>, Initializable
struct DependsOn
{
protected:
	DependsOn()
	{
		Assert( KLASS::HasInstance() );
		Assert( KLASS::Get().IsInitialized() );
	}
};

template< class KLASS >	// where KLASS : TGlobal<KLASS>
struct DependsOnGlobal
{
protected:
	DependsOnGlobal()
	{
		Assert( KLASS::HasInstance() );
		//Assert( KLASS::Get().IsInitialized() );
	}
};

mxNAMESPACE_END

#endif // ! __MX_TEMPLATE_SINGLETON_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//

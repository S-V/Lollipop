/*
=============================================================================
	File:	Core.cpp
	Desc:	Base system, init/cleanup.
=============================================================================
*/

#include <Base_PCH.h>
#pragma hdrstop
#include <Base.h>

#include <Base/Memory/Memory_Private.h>

#include "Math/Math.h"

#include <Base/JobSystem/JobSystem.h>
#include <Base/Object/TypeRegistry.h>


mxNAMESPACE_BEGIN

//---------------------------------------------------------

namespace
{

TBlob16< mxBaseSubsystem >	g_mxBaseSubsystem;

class mxGlobalLogger : public mxLogManager, public TGlobal< mxGlobalLogger >
{
	enum { MAX_LOGGERS = 4 };
	TStaticList< mxOutputDevice*, MAX_LOGGERS >	loggers;

public:

	mxGlobalLogger()
	{
	}
	virtual ~mxGlobalLogger()
	{
	}

	void Initialize()
	{
	}
	virtual void Close() override
	{
		loggers.Clear();
	}

	virtual	void Log( ELogLevel level, const char* message, UINT numChars ) override
	{
		//RECURSION_GUARD;
		MX_UINT_LOOP_i( loggers.Num() ) {
			loggers[i]->Log( level, message, numChars );
		}
		::OutputDebugStringA( message );
	}
	virtual	void VARARGS Logf( ELogLevel level, const char* fmt, ... ) override
	{
		//RECURSION_GUARD;
		char buffer[ MAX_STRING_CHARS ];
		UINT length;
		MX_GET_VARARGS_ANSI_X( buffer, fmt, length );
		Log( level, buffer, length );
	}

	virtual void Attach( mxOutputDevice* logger ) override
	{
		//RECURSION_GUARD;
		AssertPtr(logger);
		loggers.AddUnique( logger );
	}
	virtual void Detach( mxOutputDevice* logger ) override
	{
		//RECURSION_GUARD;
		AssertPtr(logger);
		loggers.Remove( logger );
	}
	virtual bool IsRedirectingTo( mxOutputDevice* logger ) override
	{
		//RECURSION_GUARD;
		AssertPtr(logger);
		return (loggers.FindPtrIndex( logger ) != INDEX_NONE);
	}
};


//
// this tells how many times the base system has been requested to initialize
// (think 'reference-counting')
//
local_ NiftyCounter	g_iBaseSystemReferenceCount;

local_ FCallback	g_pExitHandler = nil;
local_ void *		g_pExitHandlerArg = nil;

local_ mxGlobalLogger	g_Logger;

}//anonymous namespace

/*================================
		mxBaseSubsystem
================================*/

mxBaseSubsystem::mxBaseSubsystem()
{
	DBG_ENSURE_ONLY_ONE_CALL;

	// Initialize platform-specific services.
	mxPlatform_Init();

	// Initialize the global math.
	Math::Init();

	F_SetupMemorySubsystem();

	mxGlobalLogger::Get().Initialize();

	TypeRegistry::Initialize();

	//mxUtil_StartLogging( &GetGlobalLogger() );
}
//---------------------------------------------------------------------------
mxBaseSubsystem::~mxBaseSubsystem()
{
	if( g_pExitHandler ) {
		g_pExitHandler( g_pExitHandlerArg );
	}

	// Destroy the type system.
	TypeRegistry::Destroy();

	// Shutdown the global math.
	Math::Shutdown();


	//mxUtil_EndLogging( &GetGlobalLogger() );

#if MX_ENABLE_PROFILING
	mxProfileManager::CleanupMemory();
#endif

	mxGlobalLogger::Get().Close();

	F_ShutdownMemorySubsystem();

	mxPlatform_Shutdown();
}

/*
================================
	GetGlobalLogger
================================
*/
mxLogManager& GetGlobalLogger()
{
	return mxGlobalLogger::Get();
}

/*
================================
	mxBaseSystemIsInitialized
================================
*/
bool mxBaseSystemIsInitialized() {
	return (g_iBaseSystemReferenceCount.NumRefs() > 0);
}

/*
================================
	mxInitializeBase
================================
*/
bool mxInitializeBase()
{
	if( g_iBaseSystemReferenceCount.IncRef() ) {
		new(&g_mxBaseSubsystem)mxBaseSubsystem();
	}

	return true;
}

/*
================================
	mxShutdownBase
================================
*/
bool mxShutdownBase()
{
	//if( g_iBaseSystemReferenceCount.NumRefs() <= 0 ) {
	//	mxWarn("Base system has already been shut down.");
	//	return true;
	//}

	if( g_iBaseSystemReferenceCount.DecRef() ) {
		g_mxBaseSubsystem.Destruct();
		return true;
	}

	return false;
}

/*
================================
	ForceExit_BaseSystem
================================
*/
void mxForceExit( int exitCode )
{
	//mxPut( "Exiting forcefully at your request.\n" );
	mxPut( "Program ended at your request.\n" );

	//@todo: call user callback to clean up resources

	g_mxBaseSubsystem.Destruct();

	// shutdown the C runtime, this cleans up static objects but doesn't shut 
	// down the process
	::_cexit();

	//::exit( exitCode );
	::ExitProcess( exitCode );
}

/*
================================
	mxSetExitHandler
================================
*/
void mxSetExitHandler( FCallback pFunc, void* pArg )
{
	g_pExitHandler = pFunc;
	g_pExitHandlerArg = pArg;
}

/*
================================
	mxSetExitHandler
================================
*/
void mxGetExitHandler( FCallback * pFunc, void ** pArg )
{
	AssertPtr(pFunc);
	AssertPtr(pArg);
	*pFunc = g_pExitHandler;
	*pArg = g_pExitHandlerArg;
}

mxNAMESPACE_END

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//

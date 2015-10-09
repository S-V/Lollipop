/*
=============================================================================
	File:	Engine.cpp
	Desc:	
=============================================================================
*/

#include <Engine_PCH.h>
#pragma hdrstop
#include <Engine.h>

#include <Base/Util/LogUtil.h>

#include <Core/Serialization.h>
#include <Core/Resources.h>

#include <Audio/API.h>

#include <Engine/Worlds.h>

mxNAMESPACE_BEGIN

// Global variables

Engine	gEngine;

/*
-----------------------------------------------------------------------------
	SEngineInitArgs
-----------------------------------------------------------------------------
*/
SEngineInitArgs::SEngineInitArgs()
{
}

/*
-----------------------------------------------------------------------------
	SEngineLoadArgs
-----------------------------------------------------------------------------
*/
SEngineLoadArgs::SEngineLoadArgs()
{
	//pathToPak = nil;
	//pathToIni = nil;
}

/*
-----------------------------------------------------------------------------
	SEngineSaveArgs
-----------------------------------------------------------------------------
*/
SEngineSaveArgs::SEngineSaveArgs()
{
	//pathToPak = nil;
	//pathToIni = nil;
}


/*
-----------------------------------------------------------------------------
	Engine
-----------------------------------------------------------------------------
*/
Engine::Engine()
{
}
//---------------------------------------------------------------------------
Engine::~Engine()
{
}
//---------------------------------------------------------------------------
bool Engine::Initialize( const SEngineInitArgs& initArgs )
{
	this->OneTimeInit();

	CHK_VRET_FALSE_IF_NIL( initArgs.client );

	SetupCoreSubsystem();

	//Physics::Initialize();
	graphics.Initialize();
	//Audio::Initialize();

	gEngine.client = initArgs.client;

	//const bool bOk = gEngine.client->SetupClient();

	return true;
}
//---------------------------------------------------------------------------
void Engine::Shutdown()
{
	this->OneTimeDestroy();

	if( gEngine.client != nil )
	{
		//gEngine.client->Close();
		gEngine.client = nil;
	}

	//Audio::Shutdown();
	graphics.Shutdown();
	//Physics::Shutdown();

	ShutdownCoreSubsystem();
}
//---------------------------------------------------------------------------
void Engine::Tick( FLOAT deltaSeconds )
{
	SResourceUpdateArgs	resourceUpdateArgs;
	resourceUpdateArgs.deltaSeconds = deltaSeconds;
	gCore.resources->Tick( resourceUpdateArgs );

	gEngine.client->Tick( deltaSeconds );
}

//---------------------------------------------------------------------------

void Engine::Reset()
{
	ResetCoreSubsystem();
	gEngine.client->ClearState();
}

//---------------------------------------------------------------------------
bool Engine::LoadState( const SEngineLoadArgs& args )
{
	//Assert( !args.pathToLevel.IsEmpty() );
	//DBGOUT("Loading scene from '%s'...\n", args.pathToLevel.ToChars());

#if MX_DEVELOPER
	const UINT startTime = mxGetTimeInMicroseconds();
#endif // MX_DEVELOPER

	// clear current state
	this->Reset();

	// initialize client app
	//if( args.client != nil )
	//{
	//	//args.client->MoundResourceArchives();
	//}

	const bool bOk = gEngine.client->LoadState( *args.serializer );

#if MX_DEVELOPER
	const UINT endTime = mxGetTimeInMicroseconds();
	const UINT elapsedTime = endTime - startTime;
	DEVOUT("Loaded engine state in %u milliseconds.\n", elapsedTime/1000 );
#endif // MX_DEVELOPER

	return bOk;
}
//---------------------------------------------------------------------------
#if MX_EDITOR

bool Engine::SaveState( const SEngineSaveArgs& args )
{
	const bool bOk = gEngine.client->SaveState( *args.serializer );
	return bOk;
}

#endif // MX_EDITOR
//---------------------------------------------------------------------------


mxNAMESPACE_END

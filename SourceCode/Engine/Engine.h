/*
=============================================================================
	File:	Engine.h
	Desc:	Public engine header file.
=============================================================================
*/

#ifndef __MX_ENGINE_H__
#define __MX_ENGINE_H__

#include <Core/Core.h>

#if MX_EDITOR
#include <Core/Editor.h>
#endif // MX_EDITOR


mxNAMESPACE_BEGIN

class AStreamWriter;
class AStreamReader;
class AObjectReader;
class AObjectWriter;
class World;

/*
-----------------------------------------------------------------------------
	AEngineClient

	engine -> client communication;
	interface to the world/level manager;
	in development/editor/test/release modes different clients will be used.
-----------------------------------------------------------------------------
*/
class AEngineClient
	: SetupCoreUtil, SingleInstance< AEngineClient >
{
public:

	//// this function is called after the engine has been initialized
	//virtual bool SetupClient()
	//{
	//	return true;
	//}
	//virtual void CloseClient()
	//{}

	//virtual void AddNewWorld( World* newWorld ) = 0;
	//virtual bool RemoveWorld( World* theWorld ) = 0;

	// high-level functions to save/load the entire universe

	virtual bool LoadState( AObjectReader & archive )
	{
		mxUNUSED(archive);
		return true;
	}
	virtual bool SaveState( AObjectWriter & archive )
	{
		mxUNUSED(archive);
		return true;
	}
	virtual void ClearState()
	{}

	// advances the client state by the specified amount of time, in seconds
	virtual void Tick( FLOAT deltaSeconds )
	{
		mxUNUSED(deltaSeconds);
	}

	virtual UINT NumWorlds() const
	{
		return 0;
	}
	virtual World* WorldAt( UINT index )
	{
		mxUNUSED(index);
		return nil;
	}

protected:
	AEngineClient() {}
	virtual ~AEngineClient() {}
};


/*
-----------------------------------------------------------------------------
	SEngineInitArgs
	used for initializing the engine
-----------------------------------------------------------------------------
*/
struct SEngineInitArgs
{
	TPtr< AEngineClient >	client;	// pointer to client interface

public:
	SEngineInitArgs();
};

/*
-----------------------------------------------------------------------------
	SEngineLoadArgs
	used for loading worlds into the engine
-----------------------------------------------------------------------------
*/
struct SEngineLoadArgs
{
	TPtr< AObjectReader >		serializer;

	//TStackString< 128 >		pathToLevel;	// (required) absolute file path including file name

public:
	SEngineLoadArgs();
};

/*
-----------------------------------------------------------------------------
	SEngineSaveArgs
	used for saving worlds inside the editor
-----------------------------------------------------------------------------
*/
struct SEngineSaveArgs
{
	TPtr< AObjectWriter >	serializer;

	//TStackString< 128 >		pathToLevel;	// (required) absolute file path including file name

public:
	SEngineSaveArgs();
};


/*
-----------------------------------------------------------------------------
	Engine
-----------------------------------------------------------------------------
*/
class Engine : SingleInstance<Engine>, public Initializable
{
public:	// Global variables

	TPtr< AEngineClient >		client;

public:
	bool Initialize( const SEngineInitArgs& initArgs );
	void Shutdown();

	// cleanup level (e.g. after a project is unloaded in the editor)
	void Reset();

public:	// Serialization

	// loads system image
	bool LoadState( const SEngineLoadArgs& args );

	// saves system image
	bool SaveState( const SEngineSaveArgs& args );

public:
	void Tick( FLOAT deltaSeconds );

public_internal:
	Engine();
	~Engine();
};

extern Engine	gEngine;


mxNAMESPACE_END

#endif // !__MX_ENGINE_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//

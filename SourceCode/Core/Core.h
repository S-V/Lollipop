/*
=============================================================================
	File:	Core.h
	Desc:	Foundation Library public header file.
=============================================================================
*/

#ifndef __MX_CORE_H__
#define __MX_CORE_H__


mxNAMESPACE_BEGIN

void SetupCoreSubsystem();
void ShutdownCoreSubsystem();
bool CoreSubsystemIsOpen();

// must be called once per frame
//void UpdateCoreSubsystem();

// Serializes low-level engine state
void SerializeCoreSubsystem( mxArchive& archive );

// called when reloading projects in the editor
void ResetCoreSubsystem();

struct SetupCoreUtil
{
	SetupCoreUtil()
	{
		SetupCoreSubsystem();
	}
	~SetupCoreUtil()
	{
		ShutdownCoreSubsystem();
	}
};

mxREFACTOR("move to Base lib");

union Signed64_Union
{
	struct
	{
		S4	lo;
		S4	hi;
	};
	S8	v;

	mxDECLARE_REFLECTION;
};
union Unsigned64_Union
{
	struct
	{
		U4	lo;
		U4	hi;
	};
	U8	v;

	mxDECLARE_REFLECTION;
};

/*
--------------------------------------------------------------
	STimeStamp
	used mainly for versioning
--------------------------------------------------------------
*/
union STimeStamp
{
	struct
	{
		UINT	year : 6;	// year starting from 2012
		UINT	month : 4;	// [0-11]
		UINT	day : 5;	// [0-30] (day of month)
		UINT	hour : 5;	// [0-23] (hours since midnight)
		UINT	minute : 6;	// minutes after the hour - [0,59]
		UINT	second : 6;	// seconds after the minute - [0,59]
	};
	U4		value;

public:
	mxDECLARE_REFLECTION;

	STimeStamp() {}
	STimeStamp( UINT year, UINT month, UINT day, UINT hour, UINT minute, UINT second );

	enum { YEAR_SHIFT = 2012 };

	void ToChars( ANSICHAR *buffer, UINT maxChars );

	static STimeStamp Compose( UINT year, UINT month, UINT day, UINT hour, UINT minute, UINT second );

	static STimeStamp GetCurrent();
};

mxDECLARE_POD_TYPE(STimeStamp);
mxDECLARE_BASIC_STRUCT(STimeStamp);

#if MX_DEVELOPER
// list of compile options (#defines)
struct DevBuildOptionsList : public TStaticList< const ANSICHAR*, 16 >
{
	void ToChars( ANSICHAR *buffer, UINT maxChars );
};
#endif // MX_DEVELOPER


// can be used for version checks, etc.
//
#pragma pack (push,1)

union mxVersion
{
	struct
	{;
		U2	minor;
		U2	major;
	};
	U4 v;

	mxDECLARE_REFLECTION;
};

void mxGetCurrentEngineVersion( mxVersion &OutVersion );

struct mxVersion64
{
	U2	minor;
	U2	major;
	U2	patch;	// (bug correction number)
	U2	build;

	mxDECLARE_REFLECTION;
};

// Special flags for engine sessions.
enum ESessionFlags
{
//	SF_EditorMode = BIT(0),	// default = 0, so that package can be loaded in both engine versions
	SF_BigEndian = BIT(1),	// default = 0
};
// this structure should be as lightweight as possible
struct mxSessionInfo
{
	U4 fourCC;	// 4-character identification value (FourCC).
	//BYTE codename[16];
	mxVersion version;	// engine version for release/development mode compatibility
	U4 platform;	// EPlatform
	U4 flags;	// ESessionFlags
	U4 bitsInByte;	// number of bits in a byte
	U4 sizeOfInt;
	U4 sizeOfPtr;	// size of void pointer

public:
	mxDECLARE_REFLECTION;

	mxSessionInfo()
	{
		GetCurrent( this );
	}
	explicit mxSessionInfo( U4 magicNum )
	{
		GetCurrent( this );
		this->fourCC = magicNum;
	}
	static void GetCurrent( mxSessionInfo *pOut );

	static bool AreCompatible( const mxSessionInfo& a, const mxSessionInfo& b );

private:
	mxSTATIC_ASSERT( sizeof(STimeStamp) == 4 );
};
#pragma pack (pop)

mxDECLARE_POD_TYPE(mxVersion);
mxDECLARE_BASIC_STRUCT(mxVersion);

mxDECLARE_POD_TYPE(mxVersion64);
mxDECLARE_BASIC_STRUCT(mxVersion64);

mxDECLARE_POD_TYPE(mxSessionInfo);
mxDECLARE_BASIC_STRUCT(mxSessionInfo);

/*
=============================================================================
	Core global variables
=============================================================================
*/

class AEditorInterface;
class TweakUtil;

class mxObjectSystem;

class IOSystem;
class FileArchive;

class SBaseObject;
class SResourceObject;
class ResourceSystem;
class AFilePackage;

class SResourceLoadArgs;
class SFileMetadata;
class SResourceInfo;
class PAK_FileEntry;
class AContentDatabase;

class World;
class AComponent;
class AEntityAspect;
class WorldManager;

// central config ( simple string key -> value registry )
//
struct INIConfigFile
{
	virtual bool GetString( const char* key, String &outValue ) = 0;
	virtual bool GetUInt( const char* key, UINT &outValue, UINT min = 0, UINT max = MAX_UINT32 ) = 0;
	virtual bool GetFloat( const char* key, FLOAT &outValue, FLOAT min = 0.0f, FLOAT max = MX_INFINITY ) = 0;
	virtual bool GetBool( const char* key, bool &outValue ) = 0;

	virtual ~INIConfigFile() {}

protected:
	INIConfigFile() {}
};

//
// single point of access to all core globals
//
struct mxCore
{
	STimeStamp		currTimeStamp;

#if MX_EDITOR
	TPtr<AEditorInterface>	editor;
	TPtr<TweakUtil>			tweaks;
#endif // MX_EDITOR

	TPtr< IOSystem >		ioSystem;
	TPtr< INIConfigFile >	config;

	TPtr< ResourceSystem >	resources;

public:
	mxCore();
};

extern mxCore	gCore;


mxNAMESPACE_END

#endif // !__MX_CORE_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//

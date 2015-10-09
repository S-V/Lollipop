/*
=============================================================================
	File:	Core.cpp
	Desc:	
=============================================================================
*/

#include <Core_PCH.h>
#pragma hdrstop
#include <Core.h>

#include <Core/Kernel.h>
#include <Core/IO/IOSystem.h>
#include <Core/Resources.h>
#include <Core/System/StringTable.h>
#include <Core/Serialization.h>
#include <Core/Entity/System.h>
#include <Core/Util/Tweakable.h>
#include <Core/Util/ConfigFile.h>


mxNAMESPACE_BEGIN

static NiftyCounter	theCoreSystemCounter;

mxCore	gCore;

static const char*	DEFAULT_ENGINE_CONFIG_FILE_NAME("Engine.ini");

mxCore::mxCore()
{
#if MX_EDITOR
	gCore.editor = AEditorInterface::GetDummyInstance();
#endif
}

//---------------------------------------------------------------------------
void SetupCoreSubsystem()
{
	if( theCoreSystemCounter.IncRef() )
	{
		mxInitializeBase();

		DEVOUT("Initializing Core...\n");

		mxName::StaticInitialize();

		gCore.currTimeStamp = STimeStamp::GetCurrent();

#if MX_DEVELOPER

		{
			ANSICHAR	timeStampStr[ MAX_STRING_CHARS ];

			gCore.currTimeStamp.ToChars( timeStampStr, NUMBER_OF(timeStampStr) );

			DEVOUT("Timestamp: %s\n", timeStampStr );
		}


		{
			DevBuildOptionsList	buildOptions;

			if( MX_DEBUG ) {
				buildOptions.Add("MX_DEBUG");
			}
			if( MX_MULTITHREADED ) {
				buildOptions.Add("MX_MULTITHREADED");
			}
			if( MX_CHECKED ) {
				buildOptions.Add("MX_CHECKED");
			}
			if( MX_DEVELOPER ) {
				buildOptions.Add("MX_DEVELOPER");
			}
			if( MX_ENABLE_PROFILING ) {
				buildOptions.Add("MX_ENABLE_PROFILING");
			}
			if( MX_ENABLE_REFLECTION ) {
				buildOptions.Add("MX_ENABLE_REFLECTION");
			}
			if( MX_EDITOR ) {
				buildOptions.Add("MX_EDITOR");
			}

			if( MX_DEMO_BUILD ) {
				buildOptions.Add("MX_DEMO_BUILD");
			}

			ANSICHAR	buildOptionsStr[ MAX_STRING_CHARS ];
			buildOptions.ToChars( buildOptionsStr, NUMBER_OF(buildOptionsStr) );

			DEVOUT("Core library build settings:\n\t%s\n", buildOptionsStr);
		}
#endif // MX_DEVELOPER
 

		Kernel::Initialize();

#if MX_EDITOR
		AssertX( gCore.editor.IsValid(), "Editor system must be initialized" );
#endif // MX_EDITOR

		// Initialize Object system.

		//STATIC_IN_PLACE_CTOR_X( gCore.objectSystem, mxObjectSystem );



#if MX_EDITOR
		gCore.tweaks.ConstructInPlace();
#endif // MX_EDITOR


		// Initialize Input/Output system.
		gCore.ioSystem.ConstructInPlace();

		// Open config file.
		{
			TPtr<ConfigFile> pConfigFile;
			pConfigFile.ConstructInPlace();
			gCore.config = pConfigFile;

			pConfigFile->Load( DEFAULT_ENGINE_CONFIG_FILE_NAME );
		}


		// Initialize resource system.
		gCore.resources.ConstructInPlace();



		EntitySystem_Setup();
	}
}
//---------------------------------------------------------------------------
void ShutdownCoreSubsystem()
{
	if( theCoreSystemCounter.DecRef() )
	{
		DEVOUT("Shutting Down Core...\n");


		EntitySystem_Close();

		// Shutdown resource system.
		gCore.resources.Destruct();

		// Close config file.
		gCore.config.Destruct();

		// Shutdown Input/Output system.
		gCore.ioSystem.Destruct();


#if MX_EDITOR
		gCore.tweaks.Destruct();
#endif // MX_EDITOR



		Kernel::Shutdown();


		mxName::StaticShutdown();

		mxShutdownBase();
	}
}

bool CoreSubsystemIsOpen()
{
	return theCoreSystemCounter.IsOpen();
}

void SerializeCoreSubsystem( mxArchive& archive )
{
	gCore.ioSystem->Serialize( archive );
	gCore.resources->Serialize( archive );
}

void ResetCoreSubsystem()
{
	gCore.ioSystem->Clear();
	gCore.resources->Clear();
}

/*
--------------------------------------------------------------
	Signed64_Union
--------------------------------------------------------------
*/
mxBEGIN_REFLECTION(Signed64_Union)
	mxMEMBER_FIELD2( lo, Low_Part )
	mxMEMBER_FIELD2( hi, High_Part )
mxEND_REFLECTION

/*
--------------------------------------------------------------
	Unsigned64_Union
--------------------------------------------------------------
*/
mxBEGIN_REFLECTION(Unsigned64_Union)
	mxMEMBER_FIELD2( lo, Low_Part )
	mxMEMBER_FIELD2( hi, High_Part )
mxEND_REFLECTION

/*
--------------------------------------------------------------
	mxVersion
--------------------------------------------------------------
*/
mxREFLECT_STRUCT_VIA_STATIC_METHOD(mxVersion)
mxBEGIN_REFLECTION(mxVersion)
	mxMEMBER_FIELD2( v, AsInteger )
mxEND_REFLECTION


void mxGetCurrentEngineVersion( mxVersion &OutVersion )
{
	OutVersion.minor = mxENGINE_VERSION_MINOR;
	OutVersion.major = mxENGINE_VERSION_MAJOR;
}

/*
-----------------------------------------------------------------------------
	mxVersion64
-----------------------------------------------------------------------------
*/
mxREFLECT_STRUCT_VIA_STATIC_METHOD(mxVersion64)
mxBEGIN_REFLECTION(mxVersion64)
	mxMEMBER_FIELD( minor )
	mxMEMBER_FIELD( major )
	mxMEMBER_FIELD( patch )
	mxMEMBER_FIELD( build )
mxEND_REFLECTION

/*
--------------------------------------------------------------
	STimeStamp
--------------------------------------------------------------
*/
mxREFLECT_STRUCT_VIA_STATIC_METHOD(STimeStamp)
mxBEGIN_REFLECTION(STimeStamp)
	mxMEMBER_FIELD2( value, AsInteger )
mxEND_REFLECTION

STimeStamp::STimeStamp( UINT year_, UINT month_, UINT day_, UINT hour_, UINT minute_, UINT second_ )
{
	Assert( year_ < 2030 );
	Assert( month_ <= 12 );
	Assert( day_ <= 31 );
	Assert( hour_ < 24 );
	Assert( minute_ < 60 );
	Assert( second_ < 60 );

	this->year		= year_ - YEAR_SHIFT;	// [0..2012+64) -> [0..63]
	this->month		= month_ - 1;	// [1..12] -> [0..11]
	this->day		= day_ - 1;	// [1..31] -> [0..30]
	this->hour		= hour_;
	this->minute	= minute_;
	this->second	= second_;
}

void STimeStamp::ToChars( ANSICHAR *buffer, UINT maxChars )
{
	const UINT realYear		= year + YEAR_SHIFT;
	const UINT realMonth	= month + 1;
	const UINT realDay		= day + 1;
	const UINT realHour		= hour;
	const UINT realMinute	= minute;
	const UINT realSecond	= second;

	mxSPrintfAnsi( buffer, maxChars,
		"mxTIMESTAMP_YMD_HMS( %u,%u,%u,  %u,%u,%u )",
		realYear, realMonth, realDay,	realHour, realMinute, realSecond );
}

/*static*/ STimeStamp STimeStamp::Compose( UINT year, UINT month, UINT day, UINT hour, UINT minute, UINT second )
{
	return STimeStamp( year, month, day, hour, minute, second );
}

/*static*/ STimeStamp STimeStamp::GetCurrent()
{
	const CalendarTime currentTime = 
		//CalendarTime::GetCurrentSystemTime()
		CalendarTime::GetCurrentLocalTime()
		;
	return STimeStamp::Compose( currentTime.year, currentTime.month, currentTime.day,
		currentTime.hour, currentTime.minute, currentTime.second );
}


#if MX_DEVELOPER
/*
--------------------------------------------------------------
	BuildOptionsList
--------------------------------------------------------------
*/
void DevBuildOptionsList::ToChars( ANSICHAR *buffer, UINT maxChars )
{
	buffer[0] = nil;

	const UINT num = this->Num();

	for( UINT i=0; i < num; i++ )
	{
		ANSICHAR	temp[64];
		mxSPrintfAnsi(
			temp, NUMBER_OF(temp), "%s%s",
			(*this)[i], (i != num-1) ? ", " : ""
			);

		mxStrCatAnsi( buffer, temp );
	}
}
#endif // MX_DEVELOPER



/*
-----------------------------------------------------------------------------
	mxSessionInfo
-----------------------------------------------------------------------------
*/
mxREFLECT_STRUCT_VIA_STATIC_METHOD(mxSessionInfo)
mxBEGIN_REFLECTION(mxSessionInfo)
	mxMEMBER_FIELD( fourCC )
	mxMEMBER_FIELD( version )
	mxMEMBER_FIELD( platform )
	mxMEMBER_FIELD( flags )
	mxMEMBER_FIELD( bitsInByte )
	mxMEMBER_FIELD( sizeOfInt )
	mxMEMBER_FIELD( sizeOfPtr )
mxEND_REFLECTION

void mxSessionInfo::GetCurrent( mxSessionInfo *pOut )
{
	AssertPtr(pOut);

	ZERO_OUT(*pOut);

	pOut->fourCC = MAKEFOURCC('C','R','A','P');

	mxGetCurrentEngineVersion( pOut->version );

	pOut->platform = mxCurrentPlatform();

//	setbit_cond( pOut->flags, MX_EDITOR, SF_EditorMode );
	setbit_cond( pOut->flags, mxIsBigEndian(), SF_BigEndian );

	pOut->bitsInByte = CHAR_BIT;
	pOut->sizeOfInt = sizeof(int);
	pOut->sizeOfPtr = sizeof(void*);
}

bool mxSessionInfo::AreCompatible( const mxSessionInfo& a, const mxSessionInfo& b )
{
	return a.fourCC == b.fourCC
		&& a.version.v == b.version.v
		&& a.platform == b.platform
		&& a.flags == b.flags
		&& a.sizeOfInt == b.sizeOfInt
		&& a.sizeOfPtr == b.sizeOfPtr
		;
}

mxNAMESPACE_END

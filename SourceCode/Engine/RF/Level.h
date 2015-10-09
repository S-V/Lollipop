/*
=============================================================================
	File:	Level.h
	Desc:	Red Faction level.
=============================================================================
*/
#pragma once

#include <Core/Editor.h>
#include <Core/Object.h>

#include <Renderer/Core/SceneView.h>

#include <Engine/Worlds.h>
#include <Engine/Entities.h>

/*
-----------------------------------------------------------------------------
	Level
-----------------------------------------------------------------------------
*/
class RF_Level : public StaticModel
{
public:
	mxDECLARE_CLASS(RF_Level,StaticModel);

	RF_Level();
	~RF_Level();

	void Load( FileReader& stream );

private:
};

// including '.' (i.e. ".rfl")
extern ConstCharPtr RED_FACTION_LEVEL_EXTENSION;

struct rfl_string : public TStackString<64>
{
	enum { MAX_LENGTH = 4096 };

	FORCEINLINE friend AStreamReader& operator >> ( AStreamReader& inStream, rfl_string &o )
	{
		const U2	length = ReadUInt16( inStream );
		if( length == 0 ) {
			return inStream;
		}

		Assert( length < MAX_LENGTH );

		char		buffer[ 512 ];
		Assert( length < NUMBER_OF(buffer) );
		inStream.Read( buffer, length * sizeof buffer[0] );
		buffer[ length ] = 0;

		o.Set( buffer, length );

		return inStream;
	}
};

FORCEINLINE void Skip_rfl_string( AStreamReader& stream )
{
	rfl_string dummy;
	stream >> dummy;
}


struct rfl_header
{
	U4 	signature; /* RFL_SIGNATURE */
	U4 	version; /* 0xC8 is the last supported version in RF 1.2, standard maps has version 0xB4 */
	U4 	timestamp; /* last map modification */
	U4 	player_start_offset; /* rfl_player_start section */
	U4 	level_info_offset; /* rfl_level_info section */
	U4 	sections_count;
	U4 	unknown; // sections data size - 8
	//rfl_string level_name;
	//rfl_string mod_name;
};
mxDECLARE_POD_TYPE(rfl_header);


enum ELevelSection
{
	RFL_END                  = 0x00000000, // exists at the end of level file
	RFL_STATIC_GEOMETRY      = 0x00000100,
	RFL_GEO_REGIONS          = 0x00000200, // geo and non-geo regions
	RFL_LIGHTS               = 0x00000300,
	RFL_CUTSCENE_CAMERAS     = 0x00000400,
	RFL_AMBIENT_SOUNDS       = 0x00000500,
	RFL_EVENTS               = 0x00000600,
	RFL_MP_RESPAWNS          = 0x00000700,
	RFL_LEVEL_PROPERTIES     = 0x00000900,
	RFL_PARTICLE_EMITTERS    = 0x00000A00,
	RFL_GAS_REGIONS          = 0x00000B00,
	RFL_ROOM_EFFECTS         = 0x00000C00,
	RFL_BOLT_EMITTERS        = 0x00000E00,
	RFL_TARGETS              = 0x00000F00,
	RFL_DECALS               = 0x00001000,
	RFL_PUSH_REGIONS         = 0x00001100,
	//RFL_UNKNOWN_SECT       = 0x00001200, // very big, light maps?
	RFL_MOVERS               = 0x00002000,
	RFL_MOVING_GROUPS        = 0x00003000,
	RFL_CUT_SCENE_PATH_NODES = 0x00005000,
	//RFL_UNKNOWN_SECT       = 0x00006000,
	RFL_TGA_UNKNOWN          = 0x00007000, // unknown purpose
	RFL_VCM_UNKNOWN          = 0x00007001, // unknown purpose
	RFL_MVF_UNKNOWN          = 0x00007002, // unknown purpose
	RFL_V3D_UNKNOWN          = 0x00007003, // unknown purpose
	RFL_VFX_UNKNOWN          = 0x00007004, // unknown purpose
	RFL_EAX_EFFECTS          = 0x00008000,
	//RFL_UNKNOWN_SECT       = 0x00010000,
	RFL_NAV_POINTS           = 0x00020000,
	RFL_ENTITIES             = 0x00030000,
	RFL_ITEMS                = 0x00040000,
	RFL_CLUTTERS             = 0x00050000,
	RFL_TRIGGERS             = 0x00060000, // trigger event, auto, door
	RFL_PLAYER_START         = 0x00070000,
	RFL_LEVEL_INFO           = 0x01000000,
	RFL_BRUSHES              = 0x02000000,
	RFL_GROUPS               = 0x03000000,
};

PCHARS ELevelSection_To_Chars( const ELevelSection e );


struct rfl_section_header
{
	U4 	type;	// ELevelSection
	U4 	size;
};
mxDECLARE_POD_TYPE(rfl_section_header);


//mxIMPLEMENT_FUNCTION_READ_SINGLE( rfl_string, Read_rfl_string );

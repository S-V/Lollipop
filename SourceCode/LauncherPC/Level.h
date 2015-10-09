#pragma once

#pragma pack(push, 1)

// badass
#define RFL_SIGNATURE 0xD4BADA55

typedef struct _rfl_string
{
	U2 len;
	BYTE str[];
} rfl_string;

typedef struct _rfl_header_t
{
	U4 signature; /* RFL_SIGNATURE */
	U4 version; /* 0xC8 is the last supported version in RF 1.2, standard maps has version 0xB4 */
	U4 timestamp; /* last map modification */
	U4 player_start_offset; /* rfl_player_start section */
	U4 level_info_offset; /* rfl_level_info section */
	U4 sections_count;
	U4 unknown; // sections data size - 8
	//rfl_string level_name;
	//rfl_string mod_name;
} rfl_header_t;

typedef struct _rfl_section_header_t
{
	U4 type;
	U4 size;
} rfl_section_header_t;

typedef struct _rfl_pos_t
{
	F4 x, y, z;
} rfl_pos_t;

/* Note: section is often followed by elements count */

enum section_type_t
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

typedef struct _rfl_spawn_properties_t
{
	U4 team_id;
	BYTE red_team;
	BYTE blue_team;
	BYTE bot;
} rfl_spawn_properties_t;

#if 0

/* 
RFL file starts with rfl_header_t. Then goes sections. Each sction starts with rfl_section_header_t. Last section is RFL_END.

RED editor sections order:
RFL_TGA_UNKNOWN
RFL_VCM_UNKNOWN
RFL_MVF_UNKNOWN
RFL_V3D_UNKNOWN
RFL_VFX_UNKNOWN
RFL_LEVEL_PROPERTIES
...

Note: rotation matrixes are written in order: 2nd, 3rd, 1st row
*/

struct rfl_room
{
	U32 id;
	F32 aabb_x1, aabb_y1, aabb_z1; // axis aligned bounding box - point 1
	F32 aabb_x2, aabb_y2, aabb_z2; // axis aligned bounding box - point 2
	BYTE is_skyroom; // 1 or 0
	BYTE is_cold; // 1 or 0
	BYTE is_outside; // 1 or 0
	BYTE is_airlock; // 1 or 0
	BYTE liquid_room; // 1 or 0
	BYTE ambient_light; // 1 or 0
	BYTE is_subroom; // 1 or 0
	BYTE unknown;
	F32 life; // -1 == infinite
	rfl_string_t eax_effect;
	F32 liquid_depth; // exists only if liquid_room == 1
	U32 liquid_color; // exists only if liquid_room == 1
	rfl_string_t liquid_surface_texture; // exists only if liquid_room == 1
	F32 liquid_visibility; // exists only if liquid_room == 1
	U32 liquid_type; // exists only if liquid_room == 1
	U32 liquid_alpha; // exists only if liquid_room == 1
	BYTE liquid_unknown[13]; // exists only if liquid_room == 1
	F32 liquid_waveform; // 0xFFFFFFF for None, exists only if liquid_room == 1
	F32 liquid_surface_texture_scroll_u; // exists only if liquid_room == 1
	F32 liquid_surface_texture_scroll_v; // exists only if liquid_room == 1
	U32 ambient_color; // exists only if ambient_light == 1
};

struct rfl_vertex
{
	U32 vertex;
	F32 tex_x;
	F32 tex_y;
	/* F32 unknown[2]; * lightmap texture coordinates? * */
};

struct rfl_face_t
{
	F32 unknown[4];
	U32 texture;
	U32 unknown2; // if not 0xFFFFFFFF rfl_vertex has unknown field; it may be lightmap id
	U32 unknown3; // face id?
	BYTE unknown4[8];
	U32 unknown5; // not 0 for portals
	BYTE unknown6[8];
	U32 room_index;
	U32 vertices_count;
	rfl_vertex vertices[vertices_count];
};

struct rfl_rooms_sect_t
{
	BYTE unknown[10]; // 6 for version 0xB4
	U32 textures_count;
	rfl_string_t textures[textures_count];
	U32 unknown2;
	U32 rooms_count; // only compiled geometry
	rfl_room rooms[rooms_count];
	U32 unknown_count; // equal to rooms_count, only compiled geometry
	struct
	{
		U32 mesh_index;
		U32 links_count; // contained meshes?
		U32 links[links_count];
	} unknown3[unknown_count];
	U32 unknown_count2;
	BYTE unknown3[unknown_count2 * 32];
	U32 vertices_count;
	rfl_pos_t vertices[vertices_count];
	U32 faces_count;
	rfl_face_t faces[faces_count];
	U32 unknown_count3;
	BYTE unknown4[unknown_count3 * 96];
	/* U32 unknown5; * only in version 0xB4 * */
};

struct rfl_level_info
{
	U32 unknown; // 0x00000001
	rfl_string level_name;
	rfl_string author;
	rfl_string date;
	BYTE unknown; // 00
	BYTE multiplayer_level; // 0 or 1
	BYTE unknown2[220];
};

struct rfl_tga_files_t
{
	U32 tga_files_count;
	rfl_string tga_files[tga_files_count]; // many files, not textures
};

struct rfl_vcm_files_t
{
	U32 vcm_files_count;
	rfl_string vcm_files[vcm_files_count];
	U32 unknown[vcm_files_count]; // 0x00000001
};

struct rfl_mvf_files_t
{
	U32 mvf_files_count;
	rfl_string mvf_files[mvf_files_count];
	U32 unknown[mvf_files_count];
};

struct rfl_v3d_files_t
{
	U32 v3d_files_count;
	rfl_string v3d_files[v3d_files_count];
	U32 unknown[v3d_files_count];
};

struct rfl_vfx_files_t
{
	U32 vfx_files_count;
	rfl_string vfx_files[vfx_files_count];
	U32 unknown[vfx_files_count];
};

struct rfl_level_properies_t
{
	rfl_string geomod_texture;
	U32 hardness;
	U32 ambient_color;
	BYTE unknown;
	U32 fog_color;
	F32 fog_near_plane;
	F32 fog_far_plane;
};

struct rfl_lights_t
{
	U32 lights_count;
	rfl_light_t lights[];
};

struct rfl_light_t
{
	U32 uid;
	rfl_string class_name; // "Light"
	BYTE unknown[48];
	rfl_string script_name;
	BYTE unknown2[57];
};

struct rfl_cutscene_camera_t
{
	U32 uid;
	rfl_string class_name; // "Cutscene Camera"
	BYTE unknown[48];
	rfl_string script_name;
	BYTE unknown2; /* 0x00 */
};

struct rfl_event_t
{
	U32 uid;
	rfl_string class_name; // depends on type
	F32 x, y, z;
	rfl_string script_name;
	BYTE unknown;
	F32 delay;
	BYTE bool1;
	BYTE bool2;
	int32_t int1;
	int32_t int2;
	F32 float1;
	F32 float2;
	rfl_string str;
	rfl_string str2;
	U32 links_count;
	U32 links[links_count];
	/* F32 rot_matrix[3][3]; * Only if class_name is Alarm, Teleport, Teleport_Player or Play_Vclip... */
	U32 color;
};

struct rfl_mp_respawns_t
{
	U32 respawns_count;
	rfl_mp_respawn_t respawns[];
};

struct rfl_mp_respawn_t
{
	U32 uid;
	F32 x, y, z;
	F32 rot_matrix[3][3];
	rfl_string script_name;
	BYTE zero; /* 0x00 */
	U32 team;
	BYTE red_team;
	BYTE blue_team;
	BYTE bot;
};

struct rfl_gas_region
{
	U32 uid;
	rfl_string class_name;  // "Gas Region"
	F32 x, y, z;
	F32 rot_matrix[3][3];
	rfl_string script_name;
	BYTE unknown2[17];
};

struct rlf_climbing_region_t
{
	U32 uid;
	rfl_string class_name;  // "Climbing Region"
	F32 x, y, z;
	F32 rot_matrix[3][3];
	rfl_string script_name;
	BYTE unknown2[17];
};

struct rlf_bolt_emiter_t
{
	U32 uid;
	rfl_string class_name; // "Bolt Emiter"
	F32 x, y, z;
	F32 rot_matrix[3][3];
	rfl_string script_name;
	BYTE unknown2[45];
	rfl_string image;
	BYTE unknown3[5];
};

struct rfl_nav_points_t
{
	U32 nav_points_count;
	rfl_nav_point_t nav_points[nav_points_count];
	U16 unknown;
};

struct rfl_nav_point_t
{
	U32 uid;
	BYTE unknown[33];
	U32 links_count;
	U32 links[links_count];
};

struct rfl_entity
{
	U32 uid;
	rfl_string class_name; // depends on type
	F32 x, y, z;
	F32 rot_matrix[3][3];
	rfl_string script_name;
	BYTE unknown;
	U32 cooperation;
	U32 friendliness;
	U32 team_id;
	rfl_string waypoint_list;
	rfl_string waypoint_method;
	BYTE unknown2;
	BYTE boarded; // 1 or 0
	BYTE ready_to_fire_state; // 1 or 0
	BYTE only_attack_player; // 1 or 0
	BYTE weapon_is_holstered; // 1 or 0
	BYTE deaf; // 1 or 0
	U32 sweep_min_angle;
	U32 sweep_max_angle;
	BYTE ignore_terrain_when_firing; // 1 or 0
	BYTE unknown3;
	BYTE start_crouched; // 1 or 0
	F32 life;
	F32 armor;
	U32 fov;
	rfl_string default_primary_weapon;
	rfl_string default_secondary_weapon;
	rfl_string item_drop;
	rfl_string state_anim;
	rfl_string corpse_pose;
	rfl_string skin;
	rfl_string death_anim;
	BYTE ai_mode;
	BYTE ai_attack_style;
	BYTE unknown4[4];
	U32 turret_uid;
	U32 alert_camera_uid;
	U32 alarm_event_uid;
	BYTE run; // 1 or 0
	BYTE start_hidden; // 1 or 0
	BYTE wear_helmet; // 1 or 0
	BYTE end_game_if_killed; // 1 or 0
	BYTE cower_from_weapon; // 1 or 0
	BYTE question_unarmed_player; // 1 or 0
	BYTE dont_hum; // 1 or 0
	BYTE no_shadow; // 1 or 0
	BYTE always_simulate; // 1 or 0
	BYTE perfect_aim; // 1 or 0
	BYTE permanent_corpse; // 1 or 0
	BYTE never_fly; // 1 or 0
	BYTE never_leave; // 1 or 0
	BYTE no_persona_messages; // 1 or 0
	BYTE fade_corpse_immediately; // 1 or 0
	BYTE never_collide_with_player; // 1 or 0
	BYTE use_custom_attack_range; // 1 or 0
	/* F32 custom_attack_range; * if use_custom_attack_range==1 */
	rfl_string left_hand_holding;
	rfl_string right_hand_holding;
};

struct rfl_item_t
{
	U32 uid;
	rfl_string class_name; // depends on type
	F32 x, y, z;
	F32 rot_matrix[3][3];
	rfl_string script_name;
	BYTE zero; /* 0x00 */
	U32 count;
	U32 respawn_time;
	U32 team_id;
};

struct rfl_clutter_t
{
	U32 uid;
	rfl_string class_name; // depends on type
	F32 x, y, z;
	F32 rot_matrix[3][3];
	rfl_string script_name;
	BYTE unknown2[5];
	rfl_string skin;
	U32 links_count;
	U32 links[links_count];
};

struct rfl_trigger_t
{
	U32 uid;
	rfl_string script_name; // depends on type
	BYTE unknown;
	BYTE is_box; // 1 or 0
	BYTE unknown2[3];
	F32 resets_after;
	U16 resets_count; // 0xFFFF - infinity
	U16 unknown3;
	BYTE is_use_key_required; // 1 or 0
	rfl_string key_name;
	BYTE weapon_activates; // 1 or 0
	BYTE unknown4;
	BYTE is_npc; // 1 or 0
	BYTE is_auto; // 1 or 0
	BYTE in_vehicle; // 1 or 0
	F32 x, y, z; // position

	F32 sphere_radius; //     !is_box
	F32 rot_matrix[3][3]; //  is_box
	F32 box_height; //        is_box
	F32 box_width; //         is_box
	F32 box_depth; //         is_box
	BYTE one_way; // 1 or 0, is_box

	U32 airlock_room; // UID
	U32 attached_to; // UID
	U32 use_clutter; // UID
	BYTE disabled; // 1 or 0
	F32 button_active_time;
	F32 inside_time;
	U32 unknown5; // 0xFFFFFFFF
	U32 links_count;
	U32 links[links_count];
};

struct rfl_brushes_sect_t
{
	U32 brushes_count;
	rfl_brush_t brushes[brushes_count];
};

struct rfl_brush_t
{
	U32 uid;
	F32 x, y, z;
	F32 rot_matrix[3][3];
	BYTE unknown[10]; /* 00 00 ... */
	U32 textures_count;
	rfl_string_t textures[textures_count];
	BYTE unknown2[16]; /* 00 00 ... */
	U32 vertices_count;
	rfl_pos_t vertices[vertices_count];
	U32 faces_count;
	rfl_face_t faces[faces_count];
	U32 unknown3; // 0
	U32 flags;
	U32 life;
	U32 unknown4; // 3? 0?
};

enum rfl_brush_flags
{
	RFLB_PORTAL = 0x1,
	RFLB_AIR = 0x2,
	RFLB_DETAIL = 0x4,
	RFLB_EMIT_STEAM = 0x10,
};

#endif // 0

#pragma pack(pop)




/*
=============================================================================
	File:	Level.cpp
	Desc:	Red Faction level.
=============================================================================
*/

#include <Engine_PCH.h>
#pragma hdrstop
#include <Engine.h>

#include <Renderer/Core/Geometry.h>
#include <Renderer/Scene/Light.h>
#include <Engine/Entities.h>
#include <Engine/Worlds.h>

#include "Level.h"

struct Submesh : public ReferenceCounted
{
	TList<Vec3D>	positions;
	TList<Vec2D>	texCoords;
	TList<Vec3D>	normals;

	TList<U2>		indices;

public:
	Submesh()
		: positions(EMemHeap::HeapTemp)
		, texCoords(EMemHeap::HeapTemp)
		, normals(EMemHeap::HeapTemp)
		, indices(EMemHeap::HeapTemp)
	{
	}
};

struct StaticMeshFace : public ReferenceCounted
{
	TList<Vec3D>	positions;
	TList<Vec2D>	texCoords;
	Vec3D			normal;
	U4				texture;

public:
	StaticMeshFace()
		: positions(EMemHeap::HeapTemp)
		, texCoords(EMemHeap::HeapTemp)
		, texture(INDEX_NONE)
	{
	}
};

struct StaticMeshChunk : public ReferenceCounted
{
	TList<Vec3D>	positions;

	TList<TRefPtr<StaticMeshFace>>	faces;

public:
	StaticMeshChunk()
		: positions(EMemHeap::HeapTemp)
		, faces(EMemHeap::HeapTemp)
		//, normals(EMemHeap::HeapTemp)
		//, indices(EMemHeap::HeapTemp)
	{
	}
};

/*
===============================================================================

					MAP LOADING

===============================================================================
*/

ConstCharPtr RED_FACTION_LEVEL_EXTENSION( ".rfl" );


PCHARS ELevelSection_To_Chars( const ELevelSection e )
{
#define CASE_ENUM_TO_STR( e )	case e : return #e

	switch( e )
	{
		CASE_ENUM_TO_STR( RFL_END                   );
		CASE_ENUM_TO_STR( RFL_STATIC_GEOMETRY       );
		CASE_ENUM_TO_STR( RFL_GEO_REGIONS           );
		CASE_ENUM_TO_STR( RFL_LIGHTS                );
		CASE_ENUM_TO_STR( RFL_CUTSCENE_CAMERAS      );
		CASE_ENUM_TO_STR( RFL_AMBIENT_SOUNDS        );
		CASE_ENUM_TO_STR( RFL_EVENTS                );
		CASE_ENUM_TO_STR( RFL_MP_RESPAWNS           );
		CASE_ENUM_TO_STR( RFL_LEVEL_PROPERTIES      );
		CASE_ENUM_TO_STR( RFL_PARTICLE_EMITTERS     );
		CASE_ENUM_TO_STR( RFL_GAS_REGIONS           );
		CASE_ENUM_TO_STR( RFL_ROOM_EFFECTS          );
		CASE_ENUM_TO_STR( RFL_BOLT_EMITTERS         );
		CASE_ENUM_TO_STR( RFL_TARGETS               );
		CASE_ENUM_TO_STR( RFL_DECALS                );
		CASE_ENUM_TO_STR( RFL_PUSH_REGIONS          );
		//CASE_ENUM_TO_STR( RFL_UNKNOWN_SECT        );
		CASE_ENUM_TO_STR( RFL_MOVERS                );
		CASE_ENUM_TO_STR( RFL_MOVING_GROUPS         );
		CASE_ENUM_TO_STR( RFL_CUT_SCENE_PATH_NODES  );
		//CASE_ENUM_TO_STR( RFL_UNKNOWN_SECT        );
		CASE_ENUM_TO_STR( RFL_TGA_UNKNOWN           );
		CASE_ENUM_TO_STR( RFL_VCM_UNKNOWN           );
		CASE_ENUM_TO_STR( RFL_MVF_UNKNOWN           );
		CASE_ENUM_TO_STR( RFL_V3D_UNKNOWN           );
		CASE_ENUM_TO_STR( RFL_VFX_UNKNOWN           );
		CASE_ENUM_TO_STR( RFL_EAX_EFFECTS           );
		//CASE_ENUM_TO_STR( RFL_UNKNOWN_SECT        );
		CASE_ENUM_TO_STR( RFL_NAV_POINTS            );
		CASE_ENUM_TO_STR( RFL_ENTITIES              );
		CASE_ENUM_TO_STR( RFL_ITEMS                 );
		CASE_ENUM_TO_STR( RFL_CLUTTERS              );
		CASE_ENUM_TO_STR( RFL_TRIGGERS              );
		CASE_ENUM_TO_STR( RFL_PLAYER_START          );
		CASE_ENUM_TO_STR( RFL_LEVEL_INFO            );
		CASE_ENUM_TO_STR( RFL_BRUSHES               );
		CASE_ENUM_TO_STR( RFL_GROUPS                );
	}
	return "RFL_Unknown";
}

/*
-----------------------------------------------------------------------------
	Level
-----------------------------------------------------------------------------
*/
mxDEFINE_CLASS(RF_Level,StaticModel);

RF_Level::RF_Level()
{
}

RF_Level::~RF_Level()
{
}

StaticMeshChunk* LoadStaticGeometry( FileReader& stream, const UINT fileVersion )
{
	DEVOUT("Loading static geometry.\n");

	StaticMeshChunk* pSubmesh = new StaticMeshChunk();

	// unknown
	if( fileVersion == 0xB4 ) {
		Skip_N_bytes<6>( stream );
	} else {
		Skip_N_bytes<10>( stream );
	}

	const UINT numTextures = ReadUInt32( stream );
	for( UINT iTexture = 0; iTexture < numTextures; iTexture++ )
	{
		rfl_string	s;
		stream >> s;
		DEVOUT("Read string: %s\n",s.ToChars());
	}

	Skip_4_bytes( stream );

	//SizeT currOffset = stream.Tell();

	const U4 numRooms = ReadUInt32( stream );
	DEVOUT("Num rooms: %u\n",numRooms);

	enum { MAX_ROOMS = 256 };
	Assert(numRooms <= MAX_ROOMS);

	for( UINT iRoom = 0; iRoom < numRooms; iRoom++ )
	{
		const U4 roomId = ReadUInt32( stream );

		DEVOUT("=== Loading room %u (id = %u)\n",iRoom,roomId);


		const AABB bbox = Read_AABB( stream );
		Assert(bbox.IsValid());

		// is_skyroom, is_cold, is_outside, is_airlock
		Skip_4_bytes( stream );

		const bool bLiquidRoom = Read_BYTE( stream ) ? true : false;
		const bool bAmbientLight = Read_BYTE( stream ) ? true : false;
		const bool bSubRoom = Read_BYTE( stream ) ? true : false;
		(void)bSubRoom;

		Skip_1_byte( stream ); // unknown

		const F4 fLife = Read_FLOAT( stream );
		DEVOUT("room life: %f\n",fLife);

		rfl_string eax_effect;
		stream >> eax_effect;
		DEVOUT("eax_effect: %s\n",eax_effect.ToChars());

		if(bLiquidRoom)
		{
			const F4 liquid_depth = Read_FLOAT( stream );
			(void)liquid_depth;

			const U4 liquid_color = ReadUInt32( stream );
			(void)liquid_color;

			DEVOUT("liquid_depth: %f\n",liquid_depth);

			rfl_string liquid_surface_texture;
			stream >> liquid_surface_texture;
			DEVOUT("liquid_surface_texture: %s\n",liquid_surface_texture.ToChars());

			// liquid_visibility, liquid_type, liquid_alpha, liquid_unknown,
			// liquid_waveform, liquid_surface_texture_scroll_u, liquid_surface_texture_scroll_b
			Skip_N_bytes< 12 + 13 + 12 >( stream );
		}

		if(bAmbientLight) {
			Skip_4_bytes( stream ); // ambient_color
		}

#if 0
		m_Rooms.push_back(new CRoom(nId, fLife, bSubRoom));

		/* Create room mesh */
		ostringstream ssName;
		ssName << "Level/" << nId;
		OgreMeshes[i] = MeshManager::getSingleton().createManual(ssName.str(), ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

		OgreMeshes[i]->_setBounds(AxisAlignedBox(vAabb1.x(), vAabb1.y(), vAabb1.z(), vAabb2.x(), vAabb2.y(), vAabb2.z()));
		OgreMeshes[i]->_setBoundingSphereRadius(sqrtf((vAabb2.x() - vAabb1.x())*(vAabb2.x() - vAabb1.x()) +
			(vAabb2.y() - vAabb1.y())*(vAabb2.y() - vAabb1.y()) +
			(vAabb2.z() - vAabb1.z())*(vAabb2.z() - vAabb1.z())) / 2.0f);
#endif
	}//for all rooms

	const UINT cUnknown = ReadUInt32( stream );
	if( numRooms != cUnknown ) {
		mxErrf("numRooms(%u) != cUnknown(%u)\n", numRooms, cUnknown);
	}

	for( UINT i = 0; i < cUnknown; ++i )
	{
		Skip_4_bytes( stream ); // index
		const UINT cUnknown2 = ReadUInt32( stream ); // links_count
		Skip_N_bytes( stream, cUnknown2 * 4 ); // links
	}

	const UINT cUnknown2 = ReadUInt32( stream );
	Skip_N_bytes( stream, cUnknown2 * 32 ); // unknown3

	const UINT numVertices = ReadUInt32( stream );
	DEVOUT("Num vertices: %u\n",numVertices);

	// Read vertex positions.

	pSubmesh->positions.SetNum( numVertices );

	for( UINT i = 0; i < numVertices; ++i )
	{
		stream >> pSubmesh->positions[i];
	}

	const UINT numFaces = ReadUInt32( stream );
	DEVOUT("Num faces: %u\n",numFaces);

	pSubmesh->faces.SetNum( numFaces );

	for( UINT iFace = 0; iFace < numFaces; ++iFace )
	{
		StaticMeshFace* pStaticMeshFace = new StaticMeshFace();
		pSubmesh->faces[iFace] = pStaticMeshFace;

		//const SizeT nPos = stream.Tell();

		Skip_N_bytes<16>( stream );

		const UINT nTexture = ReadUInt32( stream );

		const UINT nUnknown = ReadUInt32( stream ); // its used later (lightmap?)
		Skip_N_bytes<12>( stream ); // unknown3, unknown4

		const UINT nUnknown2 = ReadUInt32( stream ); // its used later (it's not 0 for portals)
		(void)nUnknown2;
		Skip_N_bytes<8>( stream ); // unknown6

		const UINT nRoom = ReadUInt32( stream );
		Assert(nRoom < numRooms);


		if(nTexture != 0xFFFFFFFF) {
			Assert(nTexture < numTextures);
		}

		pStaticMeshFace->texture = nTexture;


		const UINT numFaceVertices = ReadUInt32( stream );
		DBGOUT("Face %u vertices: %u, texture: %u\n", iFace, numFaceVertices, nTexture);
		enum { MAX_FACE_VERTS = 100 };
		if(numFaceVertices > MAX_FACE_VERTS) {
			DBGOUT("Warning! Face %u has %u vertices (level can be corrupted)\n", iFace, numFaceVertices);
		}

		pStaticMeshFace->positions.SetNum( numFaceVertices );
		pStaticMeshFace->texCoords.SetNum( numFaceVertices );


#ifdef OF_CLIENT
		EdSubMesh *pSubMesh = NULL;
		HardwareVertexBufferSharedPtr pVertexBuffer;
		SVertex *pVertices;
		Ogre::uint16 *pIndices;
		const UINT iIndex = 0;

		if(nUnknown2 == 0)
		{
			// create a submesh with the grass material
			pSubMesh = OgreMeshes[nRoom]->createSubMesh();
			if(nTexture != 0xFFFFFFFF)
				pSubMesh->setMaterialName(LoadTexture(Textures[nTexture].c_str(), m_Rooms[nRoom]->IsDetail()));
			pSubMesh->useSharedVertices = false;
			pSubMesh->vertexData = new VertexData();
			pSubMesh->vertexData->vertexStart = 0;
			pSubMesh->vertexData->vertexCount = numFaceVertices;

			// specify a vertex format declaration for our mesh: 3 floats for position, 3 floats for normal, 2 floats for UV
			VertexDeclaration *pDecl = pSubMesh->vertexData->vertexDeclaration;
			pDecl->addElement(0, 0, VET_FLOAT3, VES_POSITION);
			pDecl->addElement(0, pDecl->getVertexSize(0), VET_FLOAT2, VES_TEXTURE_COORDINATES);

			// create a vertex buffer
			pVertexBuffer =
				HardwareBufferManager::getSingleton().createVertexBuffer(pDecl->getVertexSize(0),
				pSubMesh->vertexData->vertexCount,
				HardwareBuffer::HBU_STATIC_WRITE_ONLY);

			pVertices = (SVertex*)pVertexBuffer->lock(HardwareBuffer::HBL_DISCARD);  // start filling in vertex data

			// create an index buffer
			pSubMesh->indexData->indexCount = 3 + (numFaceVertices - 3) * 3;
			pSubMesh->indexData->indexBuffer =
				HardwareBufferManager::getSingleton().createIndexBuffer(HardwareIndexBuffer::IT_16BIT,
				pSubMesh->indexData->indexCount,
				HardwareBuffer::HBU_STATIC_WRITE_ONLY);

			// start filling in index data
			pIndices = (Ogre::uint16*)pSubMesh->indexData->indexBuffer->lock(HardwareBuffer::HBL_DISCARD);
		}
#endif // OF_CLIENT

		for( UINT iFaceVertex = 0; iFaceVertex < numFaceVertices; ++iFaceVertex )
		{
			const UINT nVertex = ReadUInt32( stream );
			const FLOAT u = Read_FLOAT( stream );
			const FLOAT v = Read_FLOAT( stream );

			pStaticMeshFace->positions[ iFaceVertex ] = pSubmesh->positions[ nVertex ];
			pStaticMeshFace->texCoords[ iFaceVertex ].Set( u, v );

			Assert(nVertex < numVertices);

#ifdef OF_CLIENT
			if(pSubMesh)
			{
				pVertices[iFaceVertex].x = Vertices[nVertex].x();
				pVertices[iFaceVertex].y = Vertices[nVertex].y();
				pVertices[iFaceVertex].z = Vertices[nVertex].z();
				pVertices[iFaceVertex].u = u;
				pVertices[iFaceVertex].v = v;

				if(iFaceVertex > 2)
				{
					pIndices[iIndex] = pIndices[0];
					pIndices[iIndex + 1] = pIndices[iIndex - 1];
					iIndex += 2;
				}
				pIndices[iIndex++] = iFaceVertex;
			}
#endif // OF_CLIENT

			if(nUnknown != 0xFFFFFFFF) {
				Skip_N_bytes<8>( stream ); // lightmap coordinates?
			}
		}//face vertices

#ifdef OF_CLIENT
		if(pSubMesh)
		{
			pVertexBuffer->unlock();  // commit vertex changes
			pSubMesh->vertexData->vertexBufferBinding->setBinding(0, pVertexBuffer);  // bind vertex buffer to our submesh
			pSubMesh->indexData->indexBuffer->unlock();  // commit index changes
		}
#endif // OF_CLIENT
	}//faces

	const UINT cUnknown3 = ReadUInt32( stream );
	Skip_N_bytes( stream, cUnknown3 * 96 ); // unknown4

	if( fileVersion == 0xB4 ) {
		Skip_4_bytes( stream ); // unknown5
	}

	DBGOUT("Loaded geometry: %u textures, %u rooms, %u vertices, %u faces\n", numTextures, numRooms, numVertices, numFaces);

	return pSubmesh;
}

void RF_Level::Load( FileReader& stream )
{
	rfl_header	levelHeader;
	stream >> levelHeader;

	rfl_string level_name;
	stream >> level_name;
	DEVOUT("Level name: %s\n",level_name.ToChars());

	rfl_string mod_name;
	stream >> mod_name;
	DEVOUT("Mod name: %s\n",mod_name.ToChars());

	const UINT numSections = levelHeader.sections_count;
	DEVOUT("Num sections: %u\n",numSections);

	TList<TRefPtr<StaticMeshChunk>>	submeshes;

	for( UINT iSection = 0; iSection < numSections; iSection++ )
	{
		rfl_section_header	sectionHeader;
		stream >> sectionHeader;

		const ELevelSection sectionType = c_cast(ELevelSection) sectionHeader.type;
		DEVOUT("Read section: %s\n",ELevelSection_To_Chars(sectionType));

		switch( sectionType )
		{
		case RFL_END :
			goto L_End;
			break;

		case RFL_STATIC_GEOMETRY :
			submeshes.Add(LoadStaticGeometry( stream, levelHeader.version ));
			break;

		default:
			stream.Skip( sectionHeader.size );
			break;
		}
	}

L_End:
	;

	Assert(submeshes.Num() == 1);
	//this->edConstruct();
	UNDONE;
}

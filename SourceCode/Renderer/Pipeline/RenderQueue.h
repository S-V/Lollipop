/*
=============================================================================
	File:	RenderQueue.h
	Desc:	Render queue/display list organization.
=============================================================================
*/
#pragma once

#include <Renderer/Common.h>
#include <Renderer/Scene/Light.h>
#include <Renderer/Scene/Model.h>
//#include <Renderer/Core/Material.h>
//#include <Renderer/Scene/RenderEntity.h>

#define RX_DEBUG_RENDER_QUEUE	(RX_DEBUG_RENDERER)

// define to 1 for faster & less accurate render queue sorting
//#define RX_USE_FAST_COMPARE		(0)


/*
=======================================================================

	Render queue

=======================================================================
*/

// old values for 64-bit sort keys
//#define MATERIAL_KEY_BITS	8
//#define MATERIAL_NUM_BITS	18
//#define DRAW_ORDER_BITS		3
//#define RENDER_STAGE_BITS	3
//
//#define ENTITY_SET_BITS		16
//#define ENTITY_NUM_BITS		16




// NOTE: could use free 4 bits in each 16-byte aligned pointer for storing bucket/order

// 64-bit sort key
union BatchSortKey64
{
	struct
	{
		rxMaterial*	material;	//«32
		UINT32		sort;	// ERenderStage(8) | EDrawOrder(8) | sort(16)
	};
	U8	v;

public:
	static FORCEINLINE UINT ComposeSort32( ERenderStage stage, EDrawOrder order, UINT key )
	{
		return ((UINT)stage << 24) | ((UINT)order << 16) | (key);
	}
	static FORCEINLINE void DecomposeSort32( UINT sort, ERenderStage *stage, EDrawOrder *order )
	{
		*stage = (ERenderStage)	((sort & 0xFF000000u) >> 24);
		*order = (EDrawOrder)	((sort & 0x00FF0000u) >> 16);
	}
};

/*
-----------------------------------------------------------------------------
	rxSurface

	is a single render queue entry

	NOTE: could store render model index and subset in ushorts
-----------------------------------------------------------------------------
*/
mxALIGN_16(struct) rxSurface
{
	BatchSortKey64	k;
	rxModel *		entity;	// renderable model
	UINT			subset;	// mesh subset (batch)

public:

	FORCEINLINE void Encode(
		rxMaterial* material,
		const ERenderStage stage,
		const EDrawOrder order,
		UINT key,
		rxModel* entity,
		UINT subset
	)
	{
		k.material = material;
		k.sort = BatchSortKey64::ComposeSort32( stage, order, key );

		this->entity = entity;
		this->subset = subset;
	}

	struct Compare
	{
		FORCEINLINE bool operator () ( const rxSurface& a, const rxSurface& b ) const
		{
			return a.k.v < b.k.v;
		}
	};

	static void DbgPrint();

	// make sure that structure layout is correct before porting to other platforms
	void StaticChecks()
	{
		mxSTATIC_ASSERT( MX_LITTLE_ENDIAN == 1 );
		mxSTATIC_ASSERT( SIZE_OF_VOID == sizeof U4 );
		mxSTATIC_ASSERT( sizeof BatchSortKey64 == sizeof UINT64 );
	}
};


// this basically describes the order in which lights are processed during deferred lighting
// @todo: split into shadow-casting/non-shadowing sets?
//
enum ELightStage
{
	// global lights rendered as full screen quads
	//LightStage_Global_FullScreenQuad = 0,

	// local lights rendered as screen-aligned quads
	LightStage_Local_FullScreenQuad,	// lights that contain eye position
	//LightStage_Local_SmallScreenQuad,	// (distant) lights that occupy small portion of the screen

	// local lights rendered as convex meshes (light shapes) (they don't enclose the camera position)
	LightStage_Local_LightVolumeMeshes,

	NumLightStages,
};

struct rxLightEntry
{
	rxLight *	p;
	U4			sort;	// ELightStage(8) | ELightType(8) | sort(16)

public:
	struct Compare
	{
		FORCEINLINE bool operator () ( const rxLightEntry& a, const rxLightEntry& b ) const
		{
			return a.sort < b.sort;
		}
	};

	template< class LIGHT_TYPE >	// where LIGHT_TYPE : rxLight
	FORCEINLINE LIGHT_TYPE* Cast() const
	{
		AssertPtr(this->p);
		return c_cast(LIGHT_TYPE*) this->p;
	}
	FORCEINLINE rxLocalLight* CastToLocalLight( ELightType eLightType ) const
	{
		rxLocalLight* pLocalLight = static_cast<rxLocalLight*>(this->p);
		AssertPtr(pLocalLight);
		Assert( pLocalLight->m_lightType == eLightType );
		return pLocalLight;
	}

	static FORCEINLINE UINT ComposeSort32( ELightStage lightStage, ELightType lightType, UINT sortKey )
	{
		return ((UINT)lightStage << 24) | ((UINT)lightType << 16) | (sortKey);
	}
	static FORCEINLINE void DecomposeSort32( UINT sortKey, ELightStage *lightStage, ELightType *lightType )
	{
		*lightStage = (ELightStage)	((sortKey & 0xFF000000u) >> 24);
		*lightType = (ELightType)	((sortKey & 0x00FF0000u) >> 16);
	}
};


// maximum number of draw calls per view
enum { MAX_BATCHES = 2048*2 };

//MAX_DIRECTIONAL_LIGHTS
enum { MAX_GLOBAL_LIGHTS = 4 };

enum { MAX_LOCAL_LIGHTS = 1024 };


/*
-----------------------------------------------------------------------------
	rxRenderQueue
-----------------------------------------------------------------------------
*/
mxALIGN_BY_CACHE_LINE struct rxRenderQueue
{
	UINT	numBatches[RS_MAX];	// number of batches in each bucket
	TStaticList< rxSurface, MAX_BATCHES >	surfaces;

	// global lights (rendered as full screen quads)
	TStaticList< rxParallelLight*, MAX_GLOBAL_LIGHTS >	globalLights;

	// local dynamic lights
	UINT	numLocalLights[NumLightStages][NumLightTypes];	// number of items in each bucket
	TStaticList< rxLightEntry, MAX_LOCAL_LIGHTS >	localLights;

	TPtr< rxSkyModel >		sky;

public:

	FORCEINLINE rxSurface& AddBatch(
		rxMaterial* material,
		const ERenderStage stage,
		const EDrawOrder order,
		UINT key,
		rxModel* entity,
		UINT subset
	)
	{
		++(this->numBatches[ stage ]);

		rxSurface & newBatch = this->surfaces.AddFast_Unsafe();

		newBatch.Encode(
			material,
			stage,
			order,
			key,
			entity,
			subset
		);

		return newBatch;
	}

	FORCEINLINE void AddLight(
		rxLight* pLight,
		const ELightStage eLightStage,
		const ELightType eLightType,
		UINT sort = 0
	)
	{
		this->numLocalLights[ eLightStage ][ eLightType ]++;

		rxLightEntry & newLight = this->localLights.AddFast_Unsafe();

		newLight.sort = rxLightEntry::ComposeSort32( eLightStage, eLightType, sort );

		newLight.p = pLight;
	}

public_internal:
	rxRenderQueue();
	~rxRenderQueue();

	void Initialize();
	void Shutdown();

	void Empty();

	FORCEINLINE UINT NumBatches() const
	{
		return surfaces.Num();
	}
	FORCEINLINE const rxSurface* GetBatches() const
	{
		return surfaces.ToPtr();
	}
	FORCEINLINE rxSurface* GetBatches()
	{
		return surfaces.ToPtr();
	}

	void Sort();

public:
	void DbgPrint( PCSTR header = nil ) const;
};

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//

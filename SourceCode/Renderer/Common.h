#pragma once

#include <Core/Memory.h>

#include <Renderer/Renderer.h>

/*
=======================================================================

	Back-end / Render queue

	batches are sorted by render stage first,
	then by material index (draw order), coarse view space depth
	and, finally, by entity and subobject indices (optional);

=======================================================================
*/

// enumeration of all possible rendering stages
// NOTE: the order is important!
// @refactor: EScenePass, EMaterialStage
//
enum ERenderStage
{
	//RS_Deferred_ZPrePass = 0,
	RS_Deferred_FillGBuffer = 0,
	//RS_Deferred_Translucency,
	RS_Deferred_Lighting,

	RS_Unlit_Opaque,

	RS_Forward_Opaque,
	RS_Forward_SSS,	// Subsurface scattering
	RS_Forward_Translucency,

	//RS_Separable_SSS,	// Separable Subsurface Scattering Pass (Post-processing).

	// NOTE: only 8 bits are available, see rxSurface::stage
	RS_MAX
};

const char* ERenderStageToChars( ERenderStage e );

//
// EDrawOrder - enumerates render priorities.
//
enum EDrawOrder
{
	DO_FIRST = 0,	// marker, don't use!
	DO_Background = DO_FIRST,	// background color

	//DO_IN_GAME_GUI,		// GUI surfaces

	//DO_PRE,				// for post-processing effects that need to be rendered before normal scene objects

	DO_Opaque,			// fully opaque geometry

	//DO_Normal,			// normal scene objects

	//DO_ENVIRONMENT,		// skyboxes, backgrounds representing distant scenery, environments, etc

	//DO_DECALS,			// scorch marks, bullet hits, explosion marks, etc
	//DO_SEE_THROUGH,		// ladders, grates, grills that may have small blended edges
	DO_Transparent,		// translucent and alpha-blended surfaces

	//DO_PORTAL,			// portal surfaces

	//DO_ALMOST_NEAREST,	// weapons, smoke puffs, etc
	//DO_NEAREST,			// blood blobs
	//DO_POST_PROCESS,

	DO_LAST = DO_Transparent,	// marker, don't use!

	// NOTE: must fit into 8 bits, see rxSurface::drawOrder
	DO_MAX = 0xFF
};

const char* EDrawOrderToChars( EDrawOrder e );




/*
=======================================================================

	Performance stats.

=======================================================================
*/

//
//	renderer front-end perf counters
//
struct rxStats
{
	UINT	numBatches;		// approximate number of model batches rendered in the last frame
	UINT	numEntities;	// total number of objects rendered in the last frame
	//UINT	numModels;		// number of models (static & animated) rendered in the last frame
	UINT	numGlobalLights;		// number of global dynamic light sources rendered in the last frame
	UINT	numLocalLights;		// number of local dynamic light sources rendered in the last frame

	UINT	visiblePortals;		// number of portals directly visible in the last frame (not including portals visible through portals)
	UINT	numRenderedPortals;	// total number of portals rendered in the last frame (including portals rendered in portal subviews)
	UINT	numRecursivePortals;// number of portals in the last frame that were rendered inside portal subviews only (purely 'recursive' portals)
	UINT	numOccludedPortals;	// number of portals in the last frame that were occluded (only valid if gfxFrontEnd uses occlusion queries).

	// these variables are persistent across frames
	UINT	frameCount;		// total number of rendered frames; incremented every frame
	UINT	viewCount;		// total number of rendered views; incremented every view (and whenever a mirror/portal subview is rendered)

	UINT	numMaterialChanges;	// number of material changes in the last frame

//	UINT	cull_aabb_cycles;
//	UINT	cull_sphere_cycles;

	//UINT	render_queue_sort_cycles;
	//UINT	fat_buffer_filling_cycles;	// cycles spent filling the g-buffer in the last frame
	//UINT	render_to_shadow_map_cycles;


public:
	rxStats()
	{
		// Initialize performance counters for the first time.

		ZERO_OUT(*this);
	}

	// This function should be called at the beginning of each frame.
	void Reset()
	{
		numBatches	= 0;
		numEntities	= 0;
		//numModels	= 0;
		numLocalLights	= 0;
		numGlobalLights	= 0;

		visiblePortals = 0;
		numRenderedPortals = 0;
		numRecursivePortals = 0;
		numOccludedPortals = 0;

//		lastFrameRenderTime	= 0;

		numMaterialChanges = 0;

		//render_queue_sort_cycles = 0;
		//fat_buffer_filling_cycles = 0;
		//render_to_shadow_map_cycles = 0;
	}
};

extern rxStats	gfxStats;	// front-end stats


/*
=======================================================================

	Utilities.

=======================================================================
*/


void Dbg_DrawBackEndStats(FLOAT fps,
						  UINT x, UINT y,
						  const FColor& color = FColor::WHITE);

void DrawCameraStats(const rxView& view,
					 UINT x, UINT y,
					 const FColor& color = FColor::WHITE);



/*
=======================================================================

	Editor

=======================================================================
*/

class AHitProxy;

// interface for rendering hit proxies, passed to scene objects;
// default impl does nothing
//
struct AHitTesting
{
	// updates world-view-projection matrix;
	virtual void SetTransform( mat4_carg transform );

	// starts rendering a new hit proxy; return index of the hit proxy
	virtual void BeginHitProxy( AHitProxy* pNewHitProxy );

	// finishes rendering current hit proxy
	virtual void EndHitProxy();

protected:
	virtual ~AHitTesting() {}

public:
	static AHitTesting	NullObject;
};

inline
void AHitTesting::SetTransform( mat4_carg transform )
{
	mxUNUSED(transform);
}

inline
void AHitTesting::BeginHitProxy( AHitProxy* pNewHitProxy )
{
	mxUNUSED(pNewHitProxy);
	//return INDEX_NONE;
}

inline
void AHitTesting::EndHitProxy()
{}


struct ATexturePreview : AEditableRefCounted
{
	mxDECLARE_ABSTRACT_CLASS(ATexturePreview,AEditableRefCounted);

	virtual void GetSize( UINT &OutWidth, UINT &OutHeight ) = 0;
	virtual PCSTR GetName() const = 0;
	virtual ID3D11ShaderResourceView* GetSRV() = 0;
};

struct ATextureInspector : public TRefCountedObjectList< ATexturePreview >
{
	virtual void AddDebugItem( ID3D11Texture2D* pTexture, PCSTR name );
	virtual void RemoveDebugItem( ID3D11Texture2D* pTexture );
	virtual ATexturePreview* GetTextureByName( PCSTR name );

	virtual ~ATextureInspector() {}
};

/*
-----------------------------------------------------------------------------
	EdGraphics
-----------------------------------------------------------------------------
*/
struct EdGraphics
	: SingleInstance<EdGraphics>, NonCopyable
	// depends on Renderer
{
public:	// Initialization/Destruction

	virtual ~EdGraphics() {}

	virtual void InitHitTesting( UINT viewportWidth, UINT viewportHeight );
	virtual void CloseHitTesting();

public:	// Debug visualization

	// Render target inspector

	virtual ATextureInspector* GetTextureInspector();

public:	// Hit testing

	//virtual AHitTesting& GetHitTestingInterface() = 0;
};

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//

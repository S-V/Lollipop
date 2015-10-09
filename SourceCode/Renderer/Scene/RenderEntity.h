/*
=============================================================================
	File:	Render entity.h
	Desc:	Base class for renderable objects.
=============================================================================
*/
#pragma once

//#include <Base/Util/Misc.h>

//#include <Core/Entity/Components.h>
//#include <Core/Object.h>

#include <Renderer/Renderer.h>
//#include <Renderer/Editor/HitTesting.h>


// passed to each graphics primitive when it becomes visible
// (when building the render queue, before sorting)
//
struct rxEntityViewContext
{
	const rxSceneContext *	s;
	rxRenderQueue *			q;
	//UINT				iEntity;
};

// hardcoded types to avoid extraneous virtual function calls
enum ERenderEntityType
{
	RE_Unknown = 0,

	RE_StaticModel,
	RE_SkinnedModel,
	RE_CsgModel,
	RE_Vegetation,
	RE_FogVolume,
	RE_Clound,
	RE_Imposter,

	RE_MAX
};
typedef TEnum< ERenderEntityType, UINT >	rxRenderEntityType;


class rxShadowRenderContext;

enum EEditorDrawFlags
{
	EditorDraw_AABB = BIT(0),	// Draw bounding box.
};

/*
-----------------------------------------------------------------------------
	rxShadowCaster
-----------------------------------------------------------------------------
*/
struct rxShadowCaster
{
	// Shadow rendering.

	virtual void rfRenderShadowDepth( const rxShadowRenderContext& context );
	virtual void rfRenderColoredShadowMap( const rxShadowRenderContext& context );

protected:
	inline rxShadowCaster() {}
	virtual ~rxShadowCaster() {}
};

/*
-----------------------------------------------------------------------------
	rxSpatialObject

	represents an abstract 3D object
	which has position, orientation and finite extents

	it is also the base primitive used for visibility culling
-----------------------------------------------------------------------------
*/
struct rxSpatialObject : rxShadowCaster
{

public:	// Spatial location

	// position
	virtual void SetOrigin( const Vec3D& newPos );

	// orientation
	virtual void SetOrientation( const Quat& newRot );

	// (Non-uniform scale is not supported).
	virtual void SetScale( const FLOAT newScale );

	// returns local-to-world matrix
	virtual Matrix4 GetWorldTransform() const;

	// Retrieves the entity's axis aligned bounding box in the world space.
	virtual void GetWorldAABB( rxAABB & bbox );

public:	// Spatial queries

	// test line against visual geometry
	virtual bool CastRay( const Vec3D& start, const Vec3D& dir, FLOAT &fraction );

public:	// Rendering


public:	// Editor

	//EEditorDrawFlags
	//virtual UINT edGetDrawFlags() const
	//{return 0;}

	//virtual void edDrawEditorStuff( const rxSceneContext& sceneContext, BatchRenderer & renderer, AHitTesting & hitTesting )
	//{}

	//// can only bind mesh buffers and issue draw calls
	//virtual void edDrawHitProxy( const rxSceneContext& sceneContext, AHitTesting& hitTesting )
	//{};

protected:
	rxSpatialObject();
	virtual ~rxSpatialObject();
};

/*
-----------------------------------------------------------------------------
	rxRenderEntity

	abstract renderable object,
	is responsible for graphics (visual) aspect of entities.

	'rf' stands for 'render function'

	this design is not very performant,
	but allows for great flexibility and rapid experimentation (?)

	should be be fine for modern PC architectures
	with brainy, branch-friendly CPUs with big caches
-----------------------------------------------------------------------------
*/
struct rxRenderEntity : public rxSpatialObject
{
	typedef rxSpatialObject Super;

public:	// Rendering

	//AddToRenderList
	// generates drawable surfaces, etc.
	virtual void rfSubmitBatches( const rxEntityViewContext& context ) = 0;

	// prepares for actual rendering (binds mesh buffers, changes LoD, etc.)
	//virtual void rfBind( const rxRenderContext& context )
	//{mxDBG_UNIMPLEMENTED;}

	//subObjectIndex
	// draw call only, material already set by the renderer
	//virtual void rfDrawSubset( const rxRenderContext& context, UINT iSubset )
	//{mxDBG_UNIMPLEMENTED;}

protected:
	inline rxRenderEntity( ERenderEntityType type = ERenderEntityType::RE_Unknown )
		: m_internalType( type )
	{}
	virtual ~rxRenderEntity()
	{}

public:
	const rxRenderEntityType	m_internalType;
};

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//

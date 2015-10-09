#pragma once

#include <Base/Util/Misc.h>

#include <Core/Object.h>

#include <Renderer/Common.h>
//#include <Renderer/Pipeline/RenderQueue.h>

/*
=======================================================================

	Graphics material

=======================================================================
*/


// used when submitting batches to the render queue
struct rxMaterialViewContext
{
	const rxSceneContext *	s;
	rxRenderQueue *			q;
	//rxRenderEntity *	pEntity;	// entity that is being rendered
	//UINT				nSubSet;	// entity subset being rendered
	rxModel *			entity;
	UINT 				subset;
	FLOAT	fDistance;	// view-space distance

public:
	bool DbgCheckValid() const;
};

struct rxMaterialRenderContext : rxRenderContext
{
	rxModel *		model;	// entity that is being rendered
	UINT			batch;	// entity subset being rendered
};

struct rxMaterialUpdateContext
{
	FLOAT	deltaSeconds;
};

enum EMaterialQuality
{
	MaterialQuality_VeryLow = 0,
	MaterialQuality_Low,
	MaterialQuality_Medium,
	MaterialQuality_High,
	MaterialQuality_VeryHigh,

	MaterialQuality_MAX
};

#if 0
enum EMaterialFlags : U32
{
	MF_Opaque = BIT(0),	// completely fills the primitive
};
#endif

// returns FALSE when done
//
typedef BOOL F_Material_SubmitBatchesLoop(
	const rxMaterialViewContext& context
);

// used in back-end (immediate rendering mode)
//
typedef void F_Material_BindShaderProgram(
	const rxMaterialRenderContext& context,
	rxMaterial* pMaterial
);

typedef TStaticArray
<
	F_Material_BindShaderProgram*,
	RS_MAX
>
MaterialStages;


/*
-----------------------------------------------------------------------------
	rxMaterial

	represents a graphics material used for rendering.
-----------------------------------------------------------------------------
*/
class rxMaterial
	: public AObject
	, public SResourceObject
{
	mxDECLARE_ABSTRACT_CLASS( rxMaterial, AObject );

	static inline EAssetType StaticGetResourceType()
	{ return EAssetType::Asset_Graphics_Material; }

	typedef TResPtr< rxMaterial >	Ref;

	// use this function for casting to material types;
	// it's basically a cast from (void*)
	template< class MATERIAL >	// where MATERIAL : rxMaterial
	FORCEINLINE MATERIAL* UpCast()
	{
		return checked_cast< MATERIAL* >( this );
	}

public:	// Front-end

	mxOPTIMIZE("replace with a function pointer");

	//AddToRenderList
	virtual void rfSubmitBatches( const rxMaterialViewContext& context ) = 0;

public:	// Back-end data

	// material renderer callbacks - bind shader program for specific render stage
	// (function pointers instead of 'vtbl' for flexibility and speed)

	MaterialStages	rfBindProgram;

	AEditableRefCounted::Ref	pEditor;

public:
	FORCEINLINE void rfBind( ERenderStage stage, const rxMaterialRenderContext& context )
	{
		this->rfBindProgram[ stage ]( context, this );
	}

public:
	// real-time updates
	virtual void Tick( const rxMaterialUpdateContext& updateContext ) {}

public:
	static UINT TotalCount();

protected:
	rxMaterial();
	virtual ~rxMaterial();
};

/*
-----------------------------------------------------------------------------
	rxMaterialSystem

	graphics material manager
-----------------------------------------------------------------------------
*/
class rxMaterialSystem
	: public AResourceManager
	, SingleInstance<rxMaterialSystem>
{
public:
	rxMaterialSystem();
	~rxMaterialSystem();

	virtual SResourceObject* LoadResource( SResourceLoadArgs & loadArgs ) override;
	virtual SResourceObject* GetDefaultResource() override;
};

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//

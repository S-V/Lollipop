/*
=============================================================================
	File:	Renderer.h
	Desc:	Renderer interface.
=============================================================================
*/

#pragma once

// Forward declarations.
class rxRenderer;
class rxDebugger;
class EdGraphics;
class rxPipeline;
class rxSceneContext;
class rxFrameContext;
class rxScene;
class rxSpatialObject;
class rxRenderEntity;
class rxRenderQueue;
class rxSceneView;
class VertexData;
class GrViewport;
class BatchRenderer;
class rxMeshManager;
class rxTextureManager;
class rxMaterialSystem;
class rxRenderWorld;
class rxShadowCastingSet;
class rxMesh;
class rxRenderQueue;
class rxEntityViewContext;
struct SRenderStageContext;
class rxModel;
class rxSkyModel;
class rxRenderContext;
class rxRenderEntity;
class rxSceneContext;
class rxMaterial;
class rxLight;


// 
//	rxSettings - high-level renderer settings.
//
struct rxSettings
{
	HWND	windowHandle;	// must always be valid

	// server for receiving and executing commands
	TPtr< class EngineServer >	server;

public:
	rxSettings()
	{
		windowHandle = nil;
	}
	bool isOk() const
	{
		return 1
			&& (windowHandle != nil)
			;
	}
};

// used for creating viewports
struct rxViewportConfig
{
	WindowHandle	windowHandle;

	// scene viewports are more expensive
	// (they require depth-stencil, G-buffer, hit testing render target storage, etc.)
	bool	bIsSceneViewport;

	bool	bDepthStencil;

public:
	rxViewportConfig();
	bool isOk() const;
};

// set once per frame. it's used for rendering entire viewports
//
struct rxFrameContext
{
	FColor	clearColor;	// Color to clear back buffer with.

public:
	rxFrameContext();
};

// high-level graphics viewport
struct rxViewport : GrViewport, DbgNamedObject
{
	rxViewport();

	inline UINT GetUniqueId() const
	{
		return uniqueId;
	}

private:
	friend class rxRenderer;

	UINT	uniqueId;
};

enum { RX_MAIN_VIEWPORT_ID = 0 };

/*
--------------------------------------------------------------
	rxRenderer
	high-level renderer interface
--------------------------------------------------------------
*/
struct rxRenderer
	: SingleInstance<rxRenderer>, NonCopyable
{
public:	// Global variables.

	// this is only used in editor mode
	TPtr<EdGraphics>	editor;

	TPtr<rxTextureManager>	textures;
	TPtr<rxMaterialSystem>	materials;
	TPtr<rxMeshManager>		meshes;

public:	// Callbacks

	TEvent0<>	OnInitialize;
	TEvent0<>	OnShutdown;
	TEvent2<UINT,UINT>	BeforeMainViewportResized;	// called before the main vp has been reallocated ( newViewportWidth, newViewportHeight )
	TEvent2<UINT,UINT>	AfterMainViewportResized;	// called after the main vp has been reallocated ( newViewportWidth, newViewportHeight )
	TEvent0<>			BeforeMainViewportDestroyed;// called before the main vp has been deallocated

public:	// Initialization/Destruction

	// Initializes the viewport; the first viewport will be the main one
	// and the render system will be initialized for the first time;
	// there can only be one main viewport; only the main viewport can be fullscreen.
	//
	bool CreateViewport( const rxViewportConfig& config, rxViewport &OutViewport );

	// when the last viewport is destroyed the render system will be shut down too
	void DestroyViewport( rxViewport &viewport );

	void OnViewportResized( rxViewport& viewport, UINT newWidth, UINT newHeight );

	bool IsMainViewport( const rxViewport &viewport ) const;

public:	// Rendering


	// it's safe to issue draw calls between BeginScene()/EndScene()
	//
	void BeginScene( rxViewport* viewport, const rxFrameContext& frameContext );

	// Call this function to render the entire scene;
	//
	void DrawScene( const rxSceneContext& sceneContext );

	void EndScene();


	BatchRenderer& GetDrawHelper();

public:	// game-specific crap can start from here on...

	//e.g.
	//void SetLightFlaresEnabled( bool bEnabled = true );

public_internal:
	rxRenderer();
	~rxRenderer();
};

extern rxRenderer gRenderer;



// passed to each graphics primitive during rendering
// (in immediate mode, after render queue sorting)
//
struct rxRenderContext
{
	const rxViewport *		v;		// viewport information
	const rxSceneContext *	s;		// high-level scene information
	D3DDeviceContext *		pD3D;	// low-level immediate device context
	FLOAT					time;	// current time, in seconds
};


//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//

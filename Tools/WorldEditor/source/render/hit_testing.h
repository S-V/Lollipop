#pragma once

class APlaceable;

// used to provide visual hints when moused over a hit proxy
enum EMouseCursor
{
	Mouse_ArrowCursor,	// The standard arrow cursor.
	Mouse_PointingHandCursor,	// A pointing hand cursor that is typically used for clickable elements such as hyperlinks.
	Mouse_OpenHandCursor,	// A cursor representing an open hand, typically used to indicate that the area under the cursor is the visible part of a canvas that the user can click and drag in order to scroll around.
	Mouse_ClosedHandCursor,	// A cursor representing a closed hand, typically used to indicate that a dragging operation is in progress that involves scrolling.
	Mouse_CrossCursor,	// A crosshair cursor, typically used to help the user accurately select a point on the screen.
	Mouse_SizeAllCursor,	// A cursor used for elements that are used to resize top-level windows in any direction.
};

// Abstract hit proxy
// hit proxies are light-weight, transient objects
// used mainly for doing UI hit tests inside the editor,
// they are allocated on per-frame basis (which means they do not exist across frames)
// their destructors may not be called
//
struct AHitProxy : public AEditable
{
	mxDECLARE_ABSTRACT_CLASS( AHitProxy, AEditable );
	mxDECLARE_CLASS_ALLOCATOR( EMemHeap::HeapEditor, AHitProxy );

private:
	friend class HitTesting;

	// Each hit proxy is assigned a unique serial number, starting from one.
	// This is set by the renderer.
	U4	internalIndex;

public:
	virtual APlaceable* IsPlaceable() {return nil;}
	virtual AEditable* GetSelectionProxy() {return this;}
	virtual EMouseCursor GetMouseCursor() const {return Mouse_ArrowCursor;}
	virtual EMouseCursor GetMouseDragCursor() const {return this->GetMouseCursor();}

	enum HitProxyFlags
	{
		// 1 if the object is not dynamically allocated
		// and shouldn't unregister itself in dtor
		HitProxy_Permanent = BIT(0)
	};

protected:
	AHitProxy( UINT flags = 0 );
	virtual ~AHitProxy();

private:
	const UINT		m__flags;
};

// Each hit proxy is assigned a unique serial number, starting from one.
#define NULL_HIT_PROXY_ID	0
#define NULL_HIT_PROXY		((AHitProxy*)nil)
#define MAX_HIT_PROXY_SIZE	(mxKILOBYTE)


// interface for doing hit tests
//
class HitTesting
	: public AHitTesting
	, SingleInstance<HitTesting>
	, NonCopyable
	//, DependsOn<EdGraphics>
{
public:	// Initialization/Destruction

	HitTesting();
	~HitTesting();

	void Initialize( UINT viewportWidth, UINT viewportHeight );
	void Shutdown();

	// called when an editor project is unloaded
	void Clear();	// deletes hit proxies, etc.

public:	// Rendering

	// be sure to call this function once per frame
	void Reset();

	// NOTE: it works only for the main viewport
	// (because render target size and depth-stencil dimensions must match in D3D11)

	void Begin( const rxViewport& viewport );
	void End();

	// updates world-view-projection matrix;
	// valid only inside Begin() / End()
	virtual void SetTransform( mat4_carg transform ) override;

	// starts rendering a new hit proxy; return index of the hit proxy
	virtual void BeginHitProxy( AHitProxy* pNewHitProxy ) override;
	// finishes rendering current hit proxy
	virtual void EndHitProxy() override;

	void RemoveHitProxy( AHitProxy* pHitProxy );

public:
	// Hit testing

	// NOTE: it works only for the main viewport
	// (because render target size and depth-stencil dimensions must match in D3D11)

	AHitProxy* GetHitProxy( const rxViewport& viewport, const UINT x, const UINT y );

	void ReadPixels( TList<R8G8B8A8> &OutPixels, UINT &OutWidth, UINT &OutHeight );

	// sets the hit proxy shader to the given device context
	//void Set( ID3D11DeviceContext* pD3DContext, mat4_carg transform );

};//HitTesting

HitTesting& GetHitTesting();


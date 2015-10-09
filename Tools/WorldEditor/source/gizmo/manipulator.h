#pragma once

#include "editor_system.h"

#include "render/viewports.h"


enum EGizmoMode
{
	Gizmo_Translate,
	Gizmo_Rotate,
	Gizmo_Scale,	// Only uniform scaling is supported
};

enum EGizmoAxis
{
	GizmoAxis_None	= 0,
	GizmoAxis_X		= BIT(0),
	GizmoAxis_Y		= BIT(1),
	GizmoAxis_Z		= BIT(2),
	GizmoAxis_XY	= GizmoAxis_X|GizmoAxis_Y,
	GizmoAxis_XZ	= GizmoAxis_X|GizmoAxis_Z,
	GizmoAxis_YZ	= GizmoAxis_Y|GizmoAxis_Z,
	GizmoAxis_XYZ	= GizmoAxis_X|GizmoAxis_Y|GizmoAxis_Z,
	GizmoAxis_All = -1	// also used when translating object along all axes via dragging its center
};

/*
-----------------------------------------------------------------------------
	HGizmoAxis
-----------------------------------------------------------------------------
*/
struct HGizmoAxis : AHitProxy
{
	mxDECLARE_ABSTRACT_CLASS( HGizmoAxis, AHitProxy );

	EGizmoAxis	axis;

public:
	HGizmoAxis()
		: Super(HitProxy_Permanent)
	{
		axis = EGizmoAxis::GizmoAxis_None;
	}

	//virtual EMouseCursor GetMouseCursor() const {return Mouse_CrossCursor;}

	virtual EMouseCursor GetMouseCursor() const override
	{return Mouse_OpenHandCursor;}

	virtual EMouseCursor GetMouseDragCursor() const override
	{return Mouse_ClosedHandCursor;}
};

struct SGizmoAxesHitProxies
{
	HGizmoAxis	axisHitProxyX;
	HGizmoAxis	axisHitProxyY;
	HGizmoAxis	axisHitProxyZ;

public:
	SGizmoAxesHitProxies()
	{
		axisHitProxyX.axis = GizmoAxis_X;
		axisHitProxyY.axis = GizmoAxis_Y;
		axisHitProxyZ.axis = GizmoAxis_Z;
	}
};

struct SPlaceableObjectState
{
	Vec3D	translation;
	FLOAT	scaleFactor;	// must be positive (only uniform scaling is supported for now)
	Quat	orientation;

public:
	SPlaceableObjectState()
		: translation( Vec3D::vec3_zero )
		, scaleFactor( 1.0f )
		, orientation( _InitIdentity )
	{}
	SPlaceableObjectState( const APlaceable* theEntity )
	{
		this->Set(theEntity);
	}
	void Set( const APlaceable* theEntity )
	{
		translation = theEntity->GetOrigin();
		scaleFactor = theEntity->GetScale();
		orientation = theEntity->GetOrientation();
		orientation.Normalize();
	}
	void Clear()
	{
		translation.SetZero();
		scaleFactor = 1.0f;
		orientation.SetIdentity();
	}
};


/*
-----------------------------------------------------------------------------
	EdGizmo
-----------------------------------------------------------------------------
*/
class EdGizmo
	: public EdViewportClient
	, public EdDrawable
{
public:
	EdGizmo();
	virtual ~EdGizmo();

	// the passed pointer can be null
	void SetObject( APlaceable* pObject );
	void OnObjectDestroyed( APlaceable* pObject );
	void Deactivate();

public:	//==- EdViewportClient

	virtual void OnKeyPressed( const EdSceneViewport& viewport, const EKeyCode key );
	virtual void OnKeyReleased( const EdSceneViewport& viewport, const EKeyCode key );

	virtual void OnMouseButton( const EdSceneViewport& viewport, const SMouseButtonEvent& args );
	virtual void OnMouseMove( const EdSceneViewport& viewport, const SMouseMoveEvent& args );
	virtual void OnMouseWheel( const EdSceneViewport& viewport, const SMouseWheelEvent& args );

	virtual void OnUpdate( const EdSceneViewport& viewport, FLOAT deltaSeconds ) override;

public:	//==- EdDrawable

	virtual void DrawBackground( const EdDrawContext& context ) override;
	virtual void Draw( const EdDrawContext& context ) override;
	virtual void DrawForeground( const EdDrawContext& context ) override;

private:
	void RotateObject( const Matrix3& rot );

private:
	TPtr< APlaceable >		m_selected;		// currently selected entity
	TPtr< APlaceable >		m_hightlighted;	// provides visual feedback when moused over
	SPlaceableObjectState	m_oldState;		// previous state of selected entity for interpolating transforms

	EGizmoMode	m_currentMode;
	EGizmoAxis	m_currentAxis;

	// used for rendering gizmo axes with special color when the mouse is hovering over them
	EGizmoAxis	m_highlightedAxes;

	SGizmoAxesHitProxies	m_hitProxies;
};


/*
-----------------------------------------------------------------------------
	EdManipulator

	presents interface for manipulating spatial properties
	of placeable objects
-----------------------------------------------------------------------------
*/
class EdManipulator
	: public EdModule
{
public:
	EdManipulator();
	~EdManipulator();

	virtual void OnSelectionChanged( AEditable* theObject ) override;
	virtual void OnObjectDestroyed( AEditable* theObject ) override;

private:
	EdGizmo		m_gizmo;

	//// only one tool at a time can be active
	//TPtr< EdGizmo >		m_currentGizmo;
	//EdGizmo_Translate	m_translationGizmo;
};





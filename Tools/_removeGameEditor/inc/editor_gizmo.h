#pragma once

#include <Core/App/Client.h>
#include <Renderer/Scene/Model.h>
#include <Renderer/Core/SceneView.h>
#include <Renderer/Scene/RenderEntity.h>

#include "editor_common.h"
#include "editor_system.h"
#include "viewports.h"


class EdModeTools;

enum EEditorMode
{
	EdMode_Brush,
	EdMode_Face,	// Surface
	EdMode_Vertex,
	EdMode_Material,
	EdMode_Object,
	EdMode_Group,
};

enum EGizmoMode
{
	Gizmo_Translate,
	Gizmo_Rotate,
	Gizmo_Scale,	// Only uniform scaling is supported
};

enum EGizmoAxis
{
	GizmoAxis_None = 0,
	GizmoAxis_X = BIT(0),
	GizmoAxis_Y = BIT(1),
	GizmoAxis_Z = BIT(2),
	GizmoAxis_XY	= GizmoAxis_X|GizmoAxis_Y,
	GizmoAxis_XZ	= GizmoAxis_X|GizmoAxis_Z,
	GizmoAxis_YZ	= GizmoAxis_Y|GizmoAxis_Z,
	GizmoAxis_XYZ	= GizmoAxis_X|GizmoAxis_Y|GizmoAxis_Z,
	GizmoAxis_All = -1	// also used when translating object along all axes via dragging its center
};

const char* GizmoAxisToChars( const EGizmoAxis eAxis );

struct HGizmoAxis : AHitProxy
{
	mxDECLARE_ABSTRACT_CLASS( HGizmoAxis, AHitProxy );

	const EGizmoAxis	axis;

	HGizmoAxis( EGizmoAxis theAxis )
		: axis( theAxis )
	{
	}

	//virtual EMouseCursor GetMouseCursor() const {return Mouse_CrossCursor;}
	virtual EMouseCursor GetMouseCursor() const {return Mouse_OpenHandCursor;}
	virtual EMouseCursor GetMouseDragCursor() const {return Mouse_ClosedHandCursor;}
};

/*
-----------------------------------------------------------------------------
	EdTool
-----------------------------------------------------------------------------
*/
class EdTool : public EdDrawable, public EdProjectChild
{
public:
	virtual ~EdTool() {}

public:
	// the passed pointer is never null
	virtual bool ActivateOnObjectClicked( const EdSceneViewport& viewport, AHitProxy* pHitProxy )
	{return false;};

	// activate if object is selected (in tree view or inside viewport)
	// the passed pointer is never null
	virtual bool ActivateOnSelectionChanged( rxSpatialObject* theObject )
	{return false;};

	virtual void OnObjectDestroyed( AEditable* theObject )
	{}

	virtual void Deactivate()
	{
		this->OnProjectUnloaded();
	}

	virtual void Tick( const EdSceneViewport& viewport, FLOAT deltaSeconds )
	{
	}

public:
	virtual bool OnKeyPressed( const EdSceneViewport& viewport, const EKeyCode key )
	{return false;};

	virtual bool OnKeyReleased( const EdSceneViewport& viewport, const EKeyCode key )
	{return false;};

	virtual bool OnMouseButton( const EdSceneViewport& viewport, const int mouseX, const int mouseY, const EMouseButton btn, const bool pressed )
	{return false;};

	virtual bool OnMouseMove( const EdSceneViewport& viewport, const int mouseDeltaX, const int mouseDeltaY, const int mouseX, const int mouseY )
	{return false;};

	virtual bool OnMouseDrag( const EdSceneViewport& viewport, const int mouseDeltaX, const int mouseDeltaY, const int mouseX, const int mouseY )
	{return false;};

	virtual bool OnMouseWheel( const EdSceneViewport& viewport, const int scroll )
	{return false;};

public:
	EdTool();
};


struct EditableModelBatch : AEditable
{
	mxDECLARE_CLASS(EditableModelBatch,AEditable);

	TPtr<rxModelBatch>		pModelBatch;
	TPtr<rxSpatialObject>	pEntity;

public:
	EditableModelBatch();
	void Clear();
	void edCreateProperties( EdPropertyCollector & outProperties, bool bClear = true ) override;
	const char* edToChars( UINT column ) const override;
};

struct EntityState
{
	Vec3D	translation;
	FLOAT	scaleFactor;	// must be positive (only uniform scaling is supported for now)
	Quat	orientation;

public:
	EntityState()
		: translation( Vec3D::vec3_zero )
		, scaleFactor( 1.0f )
		, orientation( _InitIdentity )
	{}
	EntityState( const rxSpatialObject* theEntity )
	{
		this->Set(theEntity);
	}
	void Set( const rxSpatialObject* theEntity )
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
	EdTool_Gizmo
-----------------------------------------------------------------------------
*/
class EdTool_Gizmo : public EdTool
{
public:
	typedef EdTool	Super;

	EdTool_Gizmo();
	~EdTool_Gizmo();

	virtual void OnProjectLoaded() override;
	virtual void OnProjectUnloaded() override;

	virtual bool ActivateOnObjectClicked( const EdSceneViewport& viewport, AHitProxy* pHitProxy ) override;
	virtual bool ActivateOnSelectionChanged( rxSpatialObject* theObject ) override;
	virtual void OnObjectDestroyed( AEditable* theObject ) override;
	virtual void Deactivate() override;

	virtual void Tick( const EdSceneViewport& viewport, FLOAT deltaSeconds ) override;

	virtual bool OnKeyPressed( const EdSceneViewport& viewport, const EKeyCode key ) override;
	virtual bool OnKeyReleased( const EdSceneViewport& viewport, const EKeyCode key ) override;

	virtual bool OnMouseButton( const EdSceneViewport& viewport, const int mouseX, const int mouseY, const EMouseButton btn, const bool pressed ) override;
	virtual bool OnMouseMove( const EdSceneViewport& viewport, const int mouseDeltaX, const int mouseDeltaY, const int mouseX, const int mouseY ) override;
	virtual bool OnMouseDrag( const EdSceneViewport& viewport, const int mouseDeltaX, const int mouseDeltaY, const int mouseX, const int mouseY ) override;
	virtual bool OnMouseWheel( const EdSceneViewport& viewport, const int scroll ) override;

	virtual void Draw( const EdDrawContext& context ) override;
	virtual void DrawForeground( const EdDrawContext& context ) override;
	virtual void DrawHitProxy( const EdDrawContext& context ) override;

private:
	bool Check_ClickedOnTranslationCircle( const EdSceneViewport& viewport );

protected:
	TPtr<rxSpatialObject>	m_selectedEntity;
	//EditableModelBatch	m_selectedSubmesh;

	EntityState	m_prevState;

	EGizmoMode	m_currentMode;
	EGizmoAxis	m_currentAxis;

	// for visual feedback when the mouse is hovering over a gizmo axis
	EGizmoAxis	m_highlightAxis;
};

/*
-----------------------------------------------------------------------------
	EdModeTools
-----------------------------------------------------------------------------
*/
class EdModeTools
	: public EdModule
	, public EdViewportClient
	, public EdDrawable
	, public EdWidgetChild
	, public TGlobal< EdModeTools >
{
public:
	EdModeTools();
	~EdModeTools();

	virtual void PreInit() override;
	virtual void SetDefaultValues() override;
	virtual void PostInit() override;
	virtual void Shutdown() override;

	virtual void OnProjectUnloaded() override;

	// serialize app-specific settings
	virtual void SerializeWidgetLayout( QDataStream & stream ) override;

	//virtual EdViewportClient* IsViewportClient() { return this; }

	// EdViewportClient interface

	virtual void OnKeyPressed( const EdSceneViewport& viewport, const EKeyCode key );
	virtual void OnKeyReleased( const EdSceneViewport& viewport, const EKeyCode key );

	virtual void OnMouseButton( const EdSceneViewport& viewport, const int mouseX, const int mouseY, const EMouseButton btn, const bool pressed );
	virtual void OnMouseMove( const EdSceneViewport& viewport, const int mouseDeltaX, const int mouseDeltaY, const int mouseX, const int mouseY );
	virtual void OnMouseWheel( const EdSceneViewport& viewport, const int scroll );

	virtual void OnUpdate( const EdSceneViewport& viewport, FLOAT deltaSeconds ) override;

	virtual void Draw( const EdDrawContext& context ) override;
	virtual void DrawForeground( const EdDrawContext& context ) override;

	void OnSelectionChangedInSceneTreeView( AEditable* pObject );

protected:
	void OnSelectionChanged( AEditable* theObject ) override;
	void OnObjectDestroyed( AEditable* theObject ) override;

private:
	void SetActiveTool( EdTool* newTool );
	bool ActivateToolOnSelectionChanged( EdTool* newTool, rxSpatialObject* pSelectedObject );
	void DeactivateCurrentTool();

private:
	// only one tool at a time can be active
	EdTool *		m_currentTool;	// never null

	// registered tools
	TList<EdTool*>	m_knownTools;

	EdTool	m_defaultTool;

private:
	EdTool_Gizmo	m_gizmoTool;

	// provides visual feedback when moused over
	TPtr<rxSpatialObject>	m_highlightEntity;
};

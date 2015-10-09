#include "stdafx.h"

#include <Base/Math/Line.h>
#include <Core/Util/Tweakable.h>
#include <Renderer/Renderer.h>
#include <Renderer/Scene/Model.h>
#include <Renderer/Scene/RenderWorld.h>
#include <Renderer/Util/BatchRenderer.h>
#include <Engine/Worlds.h>

#include "main_window_frame.h"
#include "app.h"

#include "editor_gizmo.h"
//#include "scene_renderer.h"

inline void DrawEntityAABB( rxSpatialObject* pEntity, BatchRenderer& renderer, const FColor& color )
{
	rxAABB	aabb;
	pEntity->GetAABB( aabb );

	renderer.DrawAABB( aabb, color );
}

// Scaling so gizmo stays same size no matter what perspective/fov/distance
//
static float GetGizmoScale( const Vec3D& eyePos, const Vec3D& objPos )
{
	// Scale the gizmo relative to the distance.
	float distance = (eyePos - objPos).GetLength();
	float scaleFactor = distance;
	return scaleFactor;
}

mxDEFINE_ABSTRACT_CLASS( HGizmoAxis );

const char* GizmoAxisToChars( const EGizmoAxis eAxis )
{
	switch( eAxis )
	{
	case EGizmoAxis::GizmoAxis_X :	return "X";
	case EGizmoAxis::GizmoAxis_Y :	return "Y";
	case EGizmoAxis::GizmoAxis_Z :	return "Z";
		MX_NO_SWITCH_DEFAULT;
	}
	return nil;
}

/*
-----------------------------------------------------------------------------
	EdTool
-----------------------------------------------------------------------------
*/
EdTool::EdTool()
{

}

mxDEFINE_CLASS(EditableModelBatch);

EditableModelBatch::EditableModelBatch()
{
	this->Clear();
}
void EditableModelBatch::Clear()
{
	this->uiBeginRefresh();

	pModelBatch = nil;
	pEntity = nil;

	this->uiEndRefresh();
}
void EditableModelBatch::edCreateProperties( EdPropertyCollector & outProperties, bool bClear )
{
	Super::edCreateProperties( outProperties, bClear );

	if( pModelBatch != nil )
		pModelBatch->edCreateProperties( outProperties, false );

	//if( pEntity != nil )
	//	pEntity->rfCreateProperties( outProperties, false );
}
const char* EditableModelBatch::edToChars( UINT column ) const
{
	return "Submesh";
}

/*
-----------------------------------------------------------------------------
	EdTool_Gizmo
-----------------------------------------------------------------------------
*/

#define AXIS_ARROW_SEGMENTS	6

struct AxisArrowUtil
{
	Vec3D	axisRootVertex;
	Vec3D	arrowBaseVertex;
	Vec3D	arrowHeadVertex;
	Vec3D	arrowVertices[ AXIS_ARROW_SEGMENTS + 1 ];

	AxisArrowUtil()
	{
		// Pre-compute the vertices for drawing axis arrows.

		static FLOAT AXIS_ARROW_RADIUS = 0.09f;
		HOT_FLOAT(AXIS_ARROW_RADIUS);

		static FLOAT ARROW_BASE_HEIGHT = 1.8f;
		HOT_FLOAT(ARROW_BASE_HEIGHT);

		static FLOAT ARROW_TOTAL_LENGTH = 2.0f;
		HOT_FLOAT(ARROW_TOTAL_LENGTH);

		//const FLOAT ARROW_CONE_HEIGHT = arrowHeadVertex.z - arrowBaseVertex.z;

		static FLOAT ARROW_CONE_HEIGHT = 1.6f;
		HOT_FLOAT(ARROW_CONE_HEIGHT);


		axisRootVertex = Vec3D( 0,0,0 );
		arrowBaseVertex = Vec3D( 0,0,ARROW_BASE_HEIGHT );
		arrowHeadVertex = Vec3D( 0,0,ARROW_TOTAL_LENGTH );


		// Generate the axis arrow cone

		// Generate the vertices for the base of the cone

		for( UINT iSegment = 0 ; iSegment <= AXIS_ARROW_SEGMENTS ; iSegment++ )
		{
			const FLOAT theta = iSegment * (MX_TWO_PI / AXIS_ARROW_SEGMENTS);	// in radians

			FLOAT	s, c;
			mxSinCos( theta, s, c );

			arrowVertices[ iSegment ].Set( AXIS_ARROW_RADIUS * s, AXIS_ARROW_RADIUS * c, ARROW_CONE_HEIGHT );
		}
	}
	void Draw( BatchRenderer & batchRenderer, const Matrix4& transform, const FColor& color )
	{
		const Vec3D apex = transform.TransformVector( arrowHeadVertex );

		// draw the 'stem'
		batchRenderer.DrawLine3D( transform.GetTranslation(), apex, FColor::WHITE, color );

		// draw the arrow head
		for( UINT iSegment = 0 ; iSegment < AXIS_ARROW_SEGMENTS ; iSegment++ )
		{
			const Vec3D& p0 = transform.TransformVector( arrowVertices[ iSegment ] );
			const Vec3D& p1 = transform.TransformVector( arrowVertices[ iSegment + 1 ] );

			// Draw the base triangle of the cone.
			// NOTE: no need because we disabled backface culling
			//batchRenderer.DrawSolidTriangle3D( p0,p1,arrowBaseVertex,color );

			// Draw the top triangle of the cone.

			batchRenderer.DrawSolidTriangle3D( p0, p1, apex, color );
		}
	}
};

inline FLOAT GetTranslateGizmoAxisScale( const Vec3D& eyePos, const Vec3D& objPos )
{
	static FLOAT TRANSL_AXIS_SCALE = 0.1f;
	HOT_FLOAT(TRANSL_AXIS_SCALE);
	const FLOAT	axisScale = TRANSL_AXIS_SCALE * GetGizmoScale( eyePos, objPos );
	return axisScale;
}

inline FLOAT GetRotationGizmoAxisScale( const Vec3D& eyePos, const Vec3D& objPos )
{
	static FLOAT ROT_GIZMO_AXIS_SCALE = 0.2f;
	HOT_FLOAT(ROT_GIZMO_AXIS_SCALE);
	const FLOAT	axisScale = ROT_GIZMO_AXIS_SCALE * GetGizmoScale( eyePos, objPos );
	return axisScale;
}

Ray3D GetGizmoAxisInWorldSpace( const rxSpatialObject* pEntity, const EGizmoAxis eAxis )
{
	const Vec3D& origin = pEntity->GetOrigin();

	const Matrix4	R = pEntity->GetOrientation().ToMat4();

	const Vec3D		axisX = R[0].ToVec3();
	const Vec3D		axisY = R[1].ToVec3();
	const Vec3D		axisZ = R[2].ToVec3();

	switch( eAxis )
	{
	case GizmoAxis_X :
		return Ray3D( origin, axisX );

	case GizmoAxis_Y :
		return Ray3D( origin, axisY );

	case GizmoAxis_Z :
		return Ray3D( origin, axisZ );

		MX_NO_SWITCH_DEFAULT;
	}
	return Ray3D();
}

inline Vec3D GetTranslationGizmoPickPointInWorldSpace( const EdSceneViewport& viewport, const Vec2D& xyNDC,
											const rxSpatialObject* pEntity, const EGizmoAxis eAxis )
{
	Ray3D	gizmoAxisWS = GetGizmoAxisInWorldSpace( pEntity, eAxis );
	Ray3D	eyeRay = GetEyeRay( viewport, xyNDC );

	Vec3D	pointOnGizmo, pointOnEyeRay;
	ELineStatus eLineStatus = IntersectLines( gizmoAxisWS, eyeRay, pointOnGizmo, pointOnEyeRay );
	//Assert( eLineStatus != Lines_Parallel );
	if( eLineStatus != Lines_Parallel )
	{
		return pointOnGizmo;
	}

	return gizmoAxisWS.origin
		+ gizmoAxisWS.direction * GetTranslateGizmoAxisScale( eyeRay.origin, pEntity->GetOrigin() );
}

inline FLOAT GetTranslationHandleRadius( const Vec3D& eyePos, const Vec3D& objPos )
{
	FLOAT	eyeDist = (eyePos - objPos).LengthSqr();
	eyeDist = maxf(eyeDist,0.01f);
	eyeDist = mxSqrt(eyeDist);

	static FLOAT PICK_DIST = 0.01f;
	HOT_FLOAT(PICK_DIST);
	return PICK_DIST * eyeDist;
}

static void Draw_TranslateHandleAtCenter( const EdDrawContext& context, const rxSpatialObject* entity, const FColor& axisColor )
{
	const Vec3D	objPos = entity->GetOrigin();

	FLOAT radius = GetTranslationHandleRadius( context.sceneContext.GetOrigin(), objPos );

	const UINT numSides = 16;

	context.renderer.DrawCircle(
		objPos,
		context.sceneContext.GetCameraRightVector(),
		context.sceneContext.GetCameraUpVector(),
		axisColor,
		radius,
		numSides
		);
}

static void Draw_TranslationGizmoAxes( const EdDrawContext& context, const rxSpatialObject* entity, UINT axisMask = GizmoAxis_All, const FColor* forceColor = nil )
{
	BatchRenderer &	batchRenderer = context.renderer;
	AHitTesting &	hitTesting = context.hitTesting;

	const Vec3D	objPos = entity->GetOrigin();

	//FLOAT radius = GetTranslationHandleRadius( context.sceneContext.GetOrigin(), objPos );

	const FLOAT	axisScale = GetTranslateGizmoAxisScale( context.sceneContext.GetOrigin(), objPos );

	const Matrix4	S = Matrix4::CreateScale(axisScale);
	const Matrix4	R = entity->GetOrientation().ToMat4();
	const Matrix4	T = Matrix4::CreateTranslation(objPos);

	const Vec3D		axisX = R[0].ToVec3();
	const Vec3D		axisY = R[1].ToVec3();
	const Vec3D		axisZ = R[2].ToVec3();

	MX_OPTIMIZE(precache AxisArrowUtil);
	AxisArrowUtil	util;

	if( axisMask & GizmoAxis_X )
	{
		BEGIN_HIT_PROXY( hitTesting, HGizmoAxis, EGizmoAxis::GizmoAxis_X );
		util.Draw( batchRenderer, S*MatrixFromAxes( axisY, axisZ, axisX )*T, forceColor ? *forceColor : FColor::RED );
		batchRenderer.Flush();
		END_HIT_PROXY( hitTesting );
	}

	if( axisMask & GizmoAxis_Y )
	{
		BEGIN_HIT_PROXY( hitTesting, HGizmoAxis, EGizmoAxis::GizmoAxis_Y );
		util.Draw( batchRenderer, S*MatrixFromAxes( axisZ, axisX, axisY )*T, forceColor ? *forceColor : FColor::GREEN );
		batchRenderer.Flush();
		END_HIT_PROXY( hitTesting );
	}

	if( axisMask & GizmoAxis_Z )
	{
		BEGIN_HIT_PROXY( hitTesting, HGizmoAxis, EGizmoAxis::GizmoAxis_Z );
		util.Draw( batchRenderer, S*MatrixFromAxes( axisX, axisY, axisZ )*T, forceColor ? *forceColor : FColor::BLUE );
		batchRenderer.Flush();
		END_HIT_PROXY( hitTesting );
	}
}

static void Draw_RotationGizmoAxes( const EdDrawContext& context, const rxSpatialObject* entity, UINT axisMask = GizmoAxis_All, const FColor* forceColor = nil )
{
	BatchRenderer &	renderer = context.renderer;
	AHitTesting &	hitTesting = context.hitTesting;

	const Vec3D	objPos = entity->GetOrigin();

	const FLOAT	axisScale = GetRotationGizmoAxisScale( context.sceneContext.GetOrigin(), objPos );

	const UINT numCircleSegments = 32;

	const Matrix3 rot = entity->GetOrientation().ToMat3();

	const Vec3D axisX = rot * Vec3D( 1.0f, 0.0f, 0.0f );
	const Vec3D axisY = rot * Vec3D( 0.0f, 1.0f, 0.0f );
	const Vec3D axisZ = rot * Vec3D( 0.0f, 0.0f, 1.0f );

	if( axisMask & GizmoAxis_X )
	{
		BEGIN_HIT_PROXY( hitTesting, HGizmoAxis, EGizmoAxis::GizmoAxis_X );
		renderer.DrawCircle(
			objPos,
			axisY,
			axisZ,
			forceColor ? *forceColor : FColor::RED,
			axisScale,
			numCircleSegments
		);
		renderer.Flush();
		END_HIT_PROXY( hitTesting );
	}

	if( axisMask & GizmoAxis_Y )
	{
		BEGIN_HIT_PROXY( hitTesting, HGizmoAxis, EGizmoAxis::GizmoAxis_Y );
		renderer.DrawCircle(
			objPos,
			axisX,
			axisZ,
			forceColor ? *forceColor : FColor::GREEN,
			axisScale,
			numCircleSegments
		);
		renderer.Flush();
		END_HIT_PROXY( hitTesting );
	}

	if( axisMask & GizmoAxis_Z )
	{
		BEGIN_HIT_PROXY( hitTesting, HGizmoAxis, EGizmoAxis::GizmoAxis_Z );
		renderer.DrawCircle(
			objPos,
			axisX,
			axisY,
			forceColor ? *forceColor : FColor::BLUE,
			axisScale,
			numCircleSegments
		);
		renderer.Flush();
		END_HIT_PROXY( hitTesting );
	}
}

static void Draw_UniformScalingHandle( const EdDrawContext& context, const rxSpatialObject* entity, const FColor* forceColor = nil )
{
	const EdSceneViewport& viewport = context.viewport;
	BatchRenderer &	batchRenderer = context.renderer;
	//AHitTesting &	hitTesting = context.hitTesting;


	const Vec3D	objPos = entity->GetOrigin();

	Ray3D	eyeRay = GetEyeRay(viewport);

	Plane3D	gizmoPlane( (eyeRay.origin - objPos).GetNormalized(), objPos );

	FLOAT	f;
	gizmoPlane.RayIntersection( eyeRay.origin, eyeRay.direction, f );

	Vec3D	pickPoint = eyeRay.origin + eyeRay.direction * f;

	const FColor& color = forceColor ? *forceColor : FColor::YELLOW;

	batchRenderer.DrawDashedLine(
		objPos, pickPoint, 1.0f, color, color
	);

}



/*
-----------------------------------------------------------------------------
	EdTool_Gizmo
-----------------------------------------------------------------------------
*/
EdTool_Gizmo::EdTool_Gizmo()
{
	this->OnProjectUnloaded();

	m_currentMode = Gizmo_Translate;
	m_currentAxis = GizmoAxis_None;

	m_highlightAxis = GizmoAxis_None;
}

EdTool_Gizmo::~EdTool_Gizmo()
{

}

void EdTool_Gizmo::OnProjectLoaded()
{
}

void EdTool_Gizmo::OnProjectUnloaded()
{
	m_selectedEntity = nil;
	//m_selectedSubmesh.Clear();

	//save current mode
	//m_currentMode = Gizmo_Translate;

	m_currentAxis = GizmoAxis_None;

	m_highlightAxis = GizmoAxis_None;
}

bool EdTool_Gizmo::ActivateOnObjectClicked( const EdSceneViewport& viewport, AHitProxy* pHitProxy )
{
	if( nil == pHitProxy ) {
		return false;
	}

	// special cases

	//HModelBatch* pHModelBatch = SafeCast<HModelBatch>(pHitProxy);
	//if(pHModelBatch)
	//{
	//	m_selectedSubmesh.pModelBatch = pHModelBatch->pBatch;
	//	m_selectedSubmesh.pEntity = pHModelBatch->pEntity;
	//}

	// general case

	rxSpatialObject* pEntity = pHitProxy->GetParentEntity();

	if( pEntity != nil )
	{
		m_selectedEntity = pEntity;

		bool bOk = this->ActivateOnSelectionChanged( pEntity );

		Check_ClickedOnTranslationCircle(viewport);

		return bOk;
	}

	return false;
}

bool EdTool_Gizmo::ActivateOnSelectionChanged( rxSpatialObject* theObject )
{
	// general case
	//m_selectedEntity = theObject;

Unimplemented_Checked;
	//if( m_selectedEntity != nil )
	//{
	//	//force refresh
	//	EdSystem::Get().Notify_SelectionChanged(nil);
	//	//EdSystem::Get().OnSelectionChanged(&m_selectedSubmesh);
	//	EdSystem::Get().Notify_SelectionChanged(m_selectedEntity->rfGetEditor());

	//	m_prevState.Set(m_selectedEntity);
	//	return true;
	//}

	return false;
}

void EdTool_Gizmo::OnObjectDestroyed( AEditable* theObject )
{
	Unimplemented_Checked;
	//if( m_selectedEntity != nil )
	//{
	//	if( /*m_selectedEntity == theObject ||*/ m_selectedEntity->rfGetEditor() == theObject )
	//	{
	//		m_selectedEntity = nil;
	//		//m_selectedSubmesh.Clear();

	//		m_prevState.Clear();
	//	}
	//}
}

void EdTool_Gizmo::Deactivate()
{
	Super::Deactivate();
}

bool EdTool_Gizmo::Check_ClickedOnTranslationCircle( const EdSceneViewport& viewport )
{
	if( m_selectedEntity != nil )
	{
		m_prevState.Set(m_selectedEntity);

		Ray3D	eyeRay = GetEyeRay(viewport);

		FLOAT	dist = eyeRay.Distance( m_selectedEntity->GetOrigin() );

		FLOAT	pickingDist = GetTranslationHandleRadius( eyeRay.origin, m_selectedEntity->GetOrigin() );

		if( dist < pickingDist )
		{
			m_currentAxis = GizmoAxis_All;
			//dbgout << "Picked\n";

			return true;
		}
	}
	return false;
}

bool EdTool_Gizmo::OnKeyPressed( const EdSceneViewport& viewport, const EKeyCode key )
{
	if( key == EKeyCode::Key_M
		|| key == EKeyCode::Key_T )
	{
		m_currentMode = Gizmo_Translate;
		return true;
	}
	if( key == EKeyCode::Key_R )
	{
		m_currentMode = Gizmo_Rotate;
		return true;
	}
	if( key == EKeyCode::Key_E )
	{
		m_currentMode = Gizmo_Scale;
		return true;
	}
	return false;
}

bool EdTool_Gizmo::OnKeyReleased( const EdSceneViewport& viewport, const EKeyCode key )
{
	if( key == EKeyCode::Key_Escape )
	{
		this->Deactivate();
		return false;
	}
	return false;
}

bool EdTool_Gizmo::OnMouseButton( const EdSceneViewport& viewport, const int mouseX, const int mouseY, const EMouseButton btn, const bool pressed )
{
	if( btn == EMouseButton::LeftMouseButton )
	{
		if( pressed )
		{
			AHitProxy* pHitProxy = viewport.objAtCursor;
			HGizmoAxis* pHGizmoAxis = SafeCast<HGizmoAxis>(pHitProxy);
			if(pHGizmoAxis)
			{
				//DBGOUT("Gizmo: %s\n",GizmoAxisToChars(pHGizmoAxis->axis));
				m_currentAxis = pHGizmoAxis->axis;
				return true;
			}

			if( this->Check_ClickedOnTranslationCircle(viewport) )
			{
				return true;
			}

			if(ActivateOnObjectClicked( viewport, pHitProxy ))
			{
				return true;
			}
		}
		else // if released
		{
			if( m_selectedEntity != nil )
			{
				m_prevState.Set(m_selectedEntity);
			}
		}
	}

	m_currentAxis = GizmoAxis_None;

	return false;
}

bool EdTool_Gizmo::OnMouseMove( const EdSceneViewport& viewport, const int mouseDeltaX, const int mouseDeltaY, const int mouseX, const int mouseY )
{
	if( !m_selectedEntity ) {
		return false;
	}

#if 0
	switch( m_currentMode )
	{
	case EGizmoMode::Gizmo_Translate :
		break;

	case EGizmoMode::Gizmo_Scale :
		break;

	case EGizmoMode::Gizmo_Rotate :
		break;

		MX_NO_SWITCH_DEFAULT;
	}
#endif

	{
		AHitProxy* pHitProxy = viewport.objAtCursor;
		HGizmoAxis* pHGizmoAxis = SafeCast<HGizmoAxis>(pHitProxy);
		if(pHGizmoAxis)
		{
			m_highlightAxis = pHGizmoAxis->axis;
			return true;
		}
	}

	m_highlightAxis = EGizmoAxis::GizmoAxis_None;

	return false;
}

bool EdTool_Gizmo::OnMouseDrag( const EdSceneViewport& viewport, const int mouseDeltaX, const int mouseDeltaY, const int mouseX, const int mouseY )
{
	if( m_selectedEntity == nil )
	{
		return false;
	}

	// gizmo pick point, in screen space
	Vec2D	pickPosNDC;
	PointToNDC( viewport, viewport.dragStartPosition.x(), viewport.dragStartPosition.y(), pickPosNDC );

	Vec2D	currPosNDC;
	PointToNDC( viewport, mouseX, mouseY, currPosNDC );

	// we need to convert screen space delta to world space movement
	Vec2D	deltaNDC = currPosNDC - pickPosNDC;
	//dbgout << "delta=" << deltaNDC << dbgout.NewLine();

	switch( m_currentMode )
	{
	case EGizmoMode::Gizmo_Translate :
		if( m_currentAxis == GizmoAxis_None )
		{
			return false;
		}
		if( m_currentAxis == GizmoAxis_All )
		{
L_TranslateAll:
			const rxView& eye = viewport.GetView();

			Ray3D	pickRay = GetEyeRay( viewport, currPosNDC );

			FLOAT	prevDist = (m_prevState.translation - eye.origin).LengthSqr();
			if( prevDist < VECTOR_EPSILON )
			{
				return false;
			}
			prevDist = mxSqrt(prevDist);


			Vec3D	newEntityPos = eye.origin + pickRay.direction * prevDist;

			m_selectedEntity->SetOrigin( newEntityPos );
		}
		else
		{
			Vec3D	gizmoPickPos = GetTranslationGizmoPickPointInWorldSpace( viewport, pickPosNDC, m_selectedEntity, m_currentAxis );
			//Vec3D	centerOfGizmo = m_selectedEntity->GetOrigin();
			Vec3D	pointOnGizmo = GetTranslationGizmoPickPointInWorldSpace( viewport, currPosNDC, m_selectedEntity, m_currentAxis );
			Vec3D	translationDelta = pointOnGizmo - gizmoPickPos;

			const FLOAT MAX_TRANSLATION_DIST = 100.0f;
			translationDelta.Clamp(Vec3D(-MAX_TRANSLATION_DIST),Vec3D(MAX_TRANSLATION_DIST));

			Vec3D	newEntityPos = m_prevState.translation + translationDelta;

			m_selectedEntity->SetOrigin( newEntityPos );
		}
		break;

		case EGizmoMode::Gizmo_Scale :
			if( m_currentAxis == GizmoAxis_All )
			{
				goto L_TranslateAll;
			}
			else
			{
				//dbgout << "Scaling\n";
				FLOAT mag = deltaNDC.LengthFast();

				FLOAT	newEntityScale = m_prevState.scaleFactor + mag * signf(deltaNDC.x);
				newEntityScale = maxf(newEntityScale,0.01f);

				m_selectedEntity->SetScale( newEntityScale );
			}
			break;

		case EGizmoMode::Gizmo_Rotate :
			//if( m_currentAxis == GizmoAxis_None )
			//{
			//	return false;
			//}
			if( m_currentAxis == GizmoAxis_All )
			{
				goto L_TranslateAll;
			}
			else
			{
				//dbgout << "Rotating\n";
MX_UNDONE;
				static FLOAT	ROT_ARC_RADIUS = 1.0f;
				HOT_FLOAT(ROT_ARC_RADIUS);

				// starting point of rotation arc
				Vec3D	vDownPt = ConvertScreenPointToVector(
					viewport,
					viewport.dragStartPosition.x(), viewport.dragStartPosition.y(),
					ROT_ARC_RADIUS
				);

				// current point of rotation arc
				Vec3D	vCurrPt = ConvertScreenPointToVector(
					viewport,
					mouseX, mouseY,
					ROT_ARC_RADIUS
				);

#if 0
				Quat	qRot = QuatFromBallPoints( vDownPt, vCurrPt );
				qRot.Normalize();
#else
				Vec3D	axis = Cross( vDownPt, vCurrPt );
				axis.NormalizeSafe();

				F4		angle = AngleBetween( vDownPt, vCurrPt );
				Quat	qRot( axis, angle );
				qRot.Normalize();
#endif

				Quat	q = qRot * m_prevState.orientation;
				q.Normalize();

				m_selectedEntity->SetOrientation(q);
			}
			break;

		MX_NO_SWITCH_DEFAULT;
	}
	return false;
}

bool EdTool_Gizmo::OnMouseWheel( const EdSceneViewport& viewport, const int scroll )
{
	return false;
}

void EdTool_Gizmo::Tick( const EdSceneViewport& viewport, FLOAT deltaSeconds )
{
	Super::Tick(viewport,deltaSeconds);
}

void EdTool_Gizmo::Draw( const EdDrawContext& context )
{
	if( m_selectedEntity != nil )
	{
		//DrawEntityAABB( m_selectedEntity, context.renderer, FColor::GREEN );
		m_selectedEntity->edDrawEditorStuff(context.sceneContext,context.renderer,AHitTesting::NullObject);
	}
#if 0
	if( m_selectedSubmesh.pModelBatch != nil
		&& m_selectedSubmesh.pEntity != nil )
	{
		const Sphere localBounds = m_selectedSubmesh.pModelBatch->bounds;

		// transform bounds from local to world space

		const Sphere worldBounds = localBounds.Transform(
			m_selectedSubmesh.pEntity->GetOrigin(),
			m_selectedSubmesh.pEntity->GetOrientation(),
			m_selectedSubmesh.pEntity->GetScale()
		);

		const UINT numSegments = 8;

		context.renderer.DrawCircle(
			worldBounds.mOrigin,
			context.sceneContext.GetCameraRightVector(),
			context.sceneContext.GetCameraUpVector(),
			FColor::DARK_GREY,
			worldBounds.mRadius * 0.7f,
			numSegments
		);
	}
#endif
}

void EdTool_Gizmo::DrawForeground( const EdDrawContext& context )
{
	if( m_selectedEntity != nil )
	{
		//if( m_highlightAxis == GizmoAxis_All )
		{
			const FColor axisColor = FColor::WHITE;
			Draw_TranslateHandleAtCenter( context, m_selectedEntity, axisColor );
		}

		switch( m_currentMode )
		{
		case Gizmo_Translate :
			Draw_TranslationGizmoAxes( context, m_selectedEntity );

			//if( m_highlightAxis != GizmoAxis_None && m_currentAxis != GizmoAxis_All && !IsDragging(context.viewport) )
			{
				const FColor axisColor = FColor::WHITE;
				if( m_currentAxis != GizmoAxis_None )
				{
					Draw_TranslationGizmoAxes( context, m_selectedEntity, m_currentAxis, &axisColor );
				}
				else if( m_highlightAxis != GizmoAxis_None )
				{
					Draw_TranslationGizmoAxes( context, m_selectedEntity, m_highlightAxis, &axisColor );
				}
			}
			break;

		case Gizmo_Rotate :
#if 0
			//if( m_currentAxis != GizmoAxis_All )
			{
				Draw_RotationGizmoAxes( context, m_selectedEntity );
			}
			if( m_highlightAxis != GizmoAxis_None )
			{
				const FColor axisColor = FColor::WHITE;
				Draw_RotationGizmoAxes( context, m_selectedEntity, m_highlightAxis, &axisColor );
			}
#endif
			break;

		case Gizmo_Scale :
			if( IsDragging( context.viewport ))
			{
				if( m_currentAxis != GizmoAxis_All )
				{
					Draw_UniformScalingHandle( context, m_selectedEntity );
				}
			}
			break;

			MX_NO_SWITCH_DEFAULT;
		}
	}
}

void EdTool_Gizmo::DrawHitProxy( const EdDrawContext& context )
{
	MX_UNUSED(context);
}

/*
-----------------------------------------------------------------------------
	EdModeTools
-----------------------------------------------------------------------------
*/


//!--------------------------------------------------------------------
#define DO_FOR_EACH_TOOL( WHAT )\
	for( UINT iTool = 0; iTool < m_knownTools.Num(); iTool++ )\
	{\
		EdTool* pTool = m_knownTools[ iTool ];\
		pTool->WHAT;\
	}

#define DO_FOR_EACH_TOOL_UNTIL( WHAT )\
	for( UINT iTool = 0; iTool < m_knownTools.Num(); iTool++ )\
	{\
		EdTool* pTool = m_knownTools[ iTool ];\
		if( pTool->WHAT )	break;\
	}

#define DO_FOR_EACH_TOOL_EXCEPT( WHAT )\
	for( UINT iTool = 0; iTool < m_knownTools.Num(); iTool++ )\
	{\
		EdTool* pTool = m_knownTools[ iTool ];\
		if( pTool->WHAT )	continue;\
	}

#define ACTIVATE_TOOL_IF( WHAT )\
	for( UINT iTool = 0; iTool < m_knownTools.Num(); iTool++ )\
	{\
		EdTool* pTool = m_knownTools[ iTool ];\
		if( pTool->WHAT ) {\
			this->SetActiveTool( pTool );\
			break;\
		}\
	}

#define ACTIVATE_TOOL_WHEN( WHAT, bRESULT )\
	bRESULT = false;\
	for( UINT iTool = 0; iTool < m_knownTools.Num(); iTool++ )\
	{\
		EdTool* pTool = m_knownTools[ iTool ];\
		if( pTool->WHAT ) {\
			this->SetActiveTool( pTool );\
			bRESULT = true;\
		}\
	}


//!--------------------------------------------------------------------


EdModeTools::EdModeTools()
{
	m_currentTool = & m_defaultTool;
}

EdModeTools::~EdModeTools()
{
}

void EdModeTools::PreInit()
{
}

void EdModeTools::SetDefaultValues()
{
}

void EdModeTools::PostInit()
{
	m_knownTools.Add( &m_gizmoTool );

	MX_CONNECT_THIS( EdSystem::Get().Event_SelectionChanged, EdModeTools, OnSelectionChanged );
}

void EdModeTools::Shutdown()
{
	this->DeactivateCurrentTool();
	MX_DISCONNECT_THIS( EdSystem::Get().Event_SelectionChanged );
}

void EdModeTools::SerializeWidgetLayout( QDataStream & stream )
{
	//DBG_TRACE_CALL;
}

void EdModeTools::SetActiveTool( EdTool* newTool )
{
	AssertPtr(newTool);
	m_currentTool = newTool;
}

void EdModeTools::DeactivateCurrentTool()
{
	m_currentTool->Deactivate();
	m_currentTool = &m_defaultTool;
}

void EdModeTools::OnKeyPressed( const EdSceneViewport& viewport, const EKeyCode key )
{
	if( m_currentTool->OnKeyPressed( viewport, key ) )
	{
		return;
	}

	if( key == Key_Escape )
	{
		this->DeactivateCurrentTool();
		return;
	}

	ACTIVATE_TOOL_IF(OnKeyPressed( viewport, key ));
}

void EdModeTools::OnKeyReleased( const EdSceneViewport& viewport, const EKeyCode key )
{
	if( m_currentTool->OnKeyReleased( viewport, key ) )
	{
		return;
	}
	ACTIVATE_TOOL_IF(OnKeyReleased( viewport, key ));
}

void EdModeTools::OnMouseButton( const EdSceneViewport& viewport, const int mouseX, const int mouseY, const EMouseButton btn, const bool pressed )
{
	if( m_currentTool->OnMouseButton( viewport, mouseX, mouseY, btn, pressed ) )
	{
		return;
	}

	if( btn == EMouseButton::LeftMouseButton )
	{
		if( pressed )
		{
			AHitProxy* pHitProxy = viewport.objAtCursor;

			bool bActivated;
			ACTIVATE_TOOL_WHEN( ActivateOnObjectClicked( viewport, pHitProxy ), bActivated );

			if( !bActivated )
			{
				EdSystem::Get().Notify_SelectionChanged(nil);
				this->DeactivateCurrentTool();
			}
			else
			{
				return;
			}
		}//if( pressed )
	}//LeftMouseButton

	if( btn == EMouseButton::RightMouseButton )
	{
	}//RightMouseButton

	ACTIVATE_TOOL_IF(OnMouseButton( viewport, mouseX, mouseY, btn, pressed ));
}

void EdModeTools::OnMouseMove( const EdSceneViewport& viewport, const int mouseDeltaX, const int mouseDeltaY, const int mouseX, const int mouseY )
{
	const bool bHighlightObjects = true;
	if( bHighlightObjects )
	{
		AHitProxy* pHitProxy = viewport.objAtCursor;
		if( pHitProxy != nil )
		{
			rxSpatialObject* pEntity = pHitProxy->GetParentEntity();
			m_highlightEntity = pEntity;
		}
		else
		{
			m_highlightEntity = nil;
		}
	}

	if( IsDragging( viewport ) )
	{
		if( m_currentTool->OnMouseDrag( viewport, mouseX, mouseY, mouseDeltaX, mouseDeltaY ) )
		{
			m_highlightEntity = nil;
			return;
		}
	}

	if( m_currentTool->OnMouseMove( viewport, mouseX, mouseY, mouseDeltaX, mouseDeltaY ) )
	{
		m_highlightEntity = nil;
		return;
	}

	ACTIVATE_TOOL_IF(OnMouseMove( viewport, mouseX, mouseY, mouseDeltaX, mouseDeltaY ));
}

void EdModeTools::OnMouseWheel( const EdSceneViewport& viewport, const int scroll )
{
	if( m_currentTool->OnMouseWheel( viewport, scroll ) )
	{
		return;
	}
	ACTIVATE_TOOL_IF(OnMouseWheel( viewport, scroll ));
}

void EdModeTools::OnSelectionChanged( AEditable* theObject )
{
	// NOTE: we cannot activate/deactivate the tools,
	// because they can emit 'OnSelectionChanged' signals

	//this->DeactivateCurrentTool();

	// MUST BE EMPTY!
}

void EdModeTools::OnObjectDestroyed( AEditable* theObject )
{
	if( m_currentTool != nil )
	{
		m_currentTool->OnObjectDestroyed( theObject );
	}
Unimplemented_Checked;
	MX_BUG("when removing objects");
	//if( m_highlightEntity && m_highlightEntity->rfGetEditor() == theObject )
	//{
	//	m_highlightEntity = nil;
	//}
}

void EdModeTools::OnProjectUnloaded()
{
	this->DeactivateCurrentTool();
}

void EdModeTools::OnUpdate( const EdSceneViewport& viewport, FLOAT deltaSeconds )
{
	m_currentTool->Tick( viewport, deltaSeconds );
}

void EdModeTools::Draw( const EdDrawContext& context )
{
	if(m_highlightEntity != nil)
	{
		const UINT flags = m_highlightEntity->edGetDrawFlags();
		if( flags & EditorDraw_AABB ) {
			DrawEntityAABB(m_highlightEntity,context.renderer,FColor::WHITE);
		}
	}
}

void EdModeTools::DrawForeground( const EdDrawContext& context )
{
	if(m_highlightEntity != nil)
	{
		Draw_TranslateHandleAtCenter(context,m_highlightEntity,FColor::WHITE);
	}
}

bool EdModeTools::ActivateToolOnSelectionChanged( EdTool* pTool, rxSpatialObject* pSelectedObject )
{
	if( pTool->ActivateOnSelectionChanged( pSelectedObject ) )
	{
		this->SetActiveTool( pTool );
		return true;
	}
	return false;
}

void EdModeTools::OnSelectionChangedInSceneTreeView( AEditable* pObject )
{
	for( UINT iTool = 0; iTool < m_knownTools.Num(); iTool++ )
	{
		EdTool* pTool = m_knownTools[ iTool ];

		AEntity* pEntity = SafeCast<AEntity>( pObject );

		if( pEntity != nil )
		{
			this->ActivateToolOnSelectionChanged( pTool, pEntity->GetVisuals() );
		}

		// special cases
		MX_UNDONE;
		//{
		//	//EditorLocalLightProxy* pLightView = dynamic_cast<EditorLocalLightProxy*>( pObject );
		//	EditorLocalLightProxy* pLightView = SafeCast<EditorLocalLightProxy>( pObject );
		//	if( pLightView != nil )
		//	{
		//		this->ActivateToolOnSelectionChanged( pTool, &pLightView->m_controller );
		//	}
		//}
	}
}

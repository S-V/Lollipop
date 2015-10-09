// @todo: refactor
//
#include "stdafx.h"

#include <Core/Util/Tweakable.h>

#include <Renderer/Util/BatchRenderer.h>

#include "gizmo/manipulator.h"
#include "render/viewports.h"


inline
void F_Draw_AABB( const AABB& bounds, BatchRenderer& renderer, const FColor& color )
{
	rxAABB	aabb;
	rxAABB_From_AABB( aabb, bounds );

	renderer.DrawAABB( aabb, color );
}

static
void F_Draw_AABB_Of_Placeable( const APlaceable* o, BatchRenderer& renderer, const FColor& color )
{
	AABB	aabb;
	o->GetWorldAABB( aabb );

	// expand a bit to prevent line flickering
	const Vec3D size = aabb.Size();
	aabb.ExpandSelf( size * 0.01f );

	F_Draw_AABB( aabb, renderer, color );
}

// Scaling so gizmo stays same size no matter what perspective/fov/distance
//
inline
FLOAT GetGizmoScale( const Vec3D& eyePos, const Vec3D& objPos )
{
	// Scale the gizmo relative to the distance.
	FLOAT distance = (eyePos - objPos).GetLength();
	FLOAT scaleFactor = distance;
	return scaleFactor;
}

/*
-----------------------------------------------------------------------------
	AxisArrowUtil
-----------------------------------------------------------------------------
*/
struct AxisArrowUtil
{
	enum { AXIS_ARROW_SEGMENTS = 6 };

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

inline
FLOAT GetTranslateGizmoAxisScale( const Vec3D& eyePos, const Vec3D& objPos )
{
	static FLOAT TRANSL_AXIS_SCALE = 0.1f;
	HOT_FLOAT(TRANSL_AXIS_SCALE);
	const FLOAT	axisScale = TRANSL_AXIS_SCALE * GetGizmoScale( eyePos, objPos );
	return axisScale;
}

inline
FLOAT GetRotationGizmoAxisScale( const Vec3D& eyePos, const Vec3D& objPos )
{
	static FLOAT ROT_GIZMO_AXIS_SCALE = 0.2f;
	HOT_FLOAT(ROT_GIZMO_AXIS_SCALE);
	const FLOAT	axisScale = ROT_GIZMO_AXIS_SCALE * GetGizmoScale( eyePos, objPos );
	return axisScale;
}

static
Ray3D GetGizmoAxisInWorldSpace( const APlaceable* pEntity, const EGizmoAxis eAxis )
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

		mxNO_SWITCH_DEFAULT;
	}
	return Ray3D();
}

inline
Vec3D F_Get_Translation_Gizmo_Pick_Point_In_World_Space( const EdSceneViewport& viewport
														, const Vec2D& xyNDC,
														const APlaceable* pEntity
														, const EGizmoAxis eAxis
														)
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

inline
FLOAT F_Get_Translate_Handle_Radius( const Vec3D& eyePos, const Vec3D& objPos )
{
	FLOAT	eyeDist = (eyePos - objPos).LengthSqr();
	eyeDist = maxf(eyeDist,0.01f);
	eyeDist = mxSqrt(eyeDist);

	static FLOAT PICK_DIST = 0.01f;
	HOT_FLOAT(PICK_DIST);
	return PICK_DIST * eyeDist;
}


static
bool F_Hit_Translate_Handle_At_Center( APlaceable* o, const EdSceneViewport& viewport )
{
	const Ray3D	eyeRay = GetEyeRay(viewport);
	const FLOAT	dist = eyeRay.Distance( o->GetOrigin() );
	const FLOAT	pickingDist = F_Get_Translate_Handle_Radius( eyeRay.origin, o->GetOrigin() );
	return dist < pickingDist;
}

static
void F_Draw_Translate_Handle_At_Center( const EdDrawContext& context, const APlaceable* entity, const FColor& axisColor )
{
	const Vec3D	objPos = entity->GetOrigin();

	const FLOAT radius = F_Get_Translate_Handle_Radius( context.sceneContext.GetOrigin(), objPos );

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

static
void Draw_TranslationGizmoAxes( SGizmoAxesHitProxies& hitProxies
							   , const EdDrawContext& context
							   , const APlaceable* entity
							   , UINT axisMask = GizmoAxis_All
							   , const FColor* forceColor = nil
							   )
{
	BatchRenderer &	batchRenderer = context.renderer;
	AHitTesting &	hitTesting = context.hitTesting;

	const Vec3D	objPos = entity->GetOrigin();

	//FLOAT radius = F_Get_Translate_Handle_Radius( context.sceneContext.GetOrigin(), objPos );

	const FLOAT	axisScale = GetTranslateGizmoAxisScale( context.sceneContext.GetOrigin(), objPos );

	const Matrix4	S = Matrix4::CreateScale(axisScale);
	const Matrix4	R = entity->GetOrientation().ToMat4();
	const Matrix4	T = Matrix4::CreateTranslation(objPos);

	const Vec3D		axisX = R[0].ToVec3();
	const Vec3D		axisY = R[1].ToVec3();
	const Vec3D		axisZ = R[2].ToVec3();

	mxOPTIMIZE(precache AxisArrowUtil);
	AxisArrowUtil	util;

	if( axisMask & GizmoAxis_X )
	{
		hitTesting.BeginHitProxy( &hitProxies.axisHitProxyX );
		util.Draw( batchRenderer, S*MatrixFromAxes( axisY, axisZ, axisX )*T, forceColor ? *forceColor : FColor::RED );
		batchRenderer.Flush();
		hitTesting.EndHitProxy();
	}

	if( axisMask & GizmoAxis_Y )
	{
		hitTesting.BeginHitProxy( &hitProxies.axisHitProxyY );
		util.Draw( batchRenderer, S*MatrixFromAxes( axisZ, axisX, axisY )*T, forceColor ? *forceColor : FColor::GREEN );
		batchRenderer.Flush();
		hitTesting.EndHitProxy();
	}

	if( axisMask & GizmoAxis_Z )
	{
		hitTesting.BeginHitProxy( &hitProxies.axisHitProxyZ );
		util.Draw( batchRenderer, S*MatrixFromAxes( axisX, axisY, axisZ )*T, forceColor ? *forceColor : FColor::BLUE );
		batchRenderer.Flush();
		hitTesting.EndHitProxy();
	}
}

static
void Draw_RotationGizmoAxes( SGizmoAxesHitProxies& hitProxies
							, const EdDrawContext& context
							, const APlaceable* entity
							, UINT axisMask = GizmoAxis_All
							, const FColor* forceColor = nil
							)
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
		hitTesting.BeginHitProxy( &hitProxies.axisHitProxyX );
		renderer.DrawCircle(
			objPos,
			axisY,
			axisZ,
			forceColor ? *forceColor : FColor::RED,
			axisScale,
			numCircleSegments
		);
		renderer.Flush();
		hitTesting.EndHitProxy();
	}

	if( axisMask & GizmoAxis_Y )
	{
		hitTesting.BeginHitProxy( &hitProxies.axisHitProxyY );
		renderer.DrawCircle(
			objPos,
			axisX,
			axisZ,
			forceColor ? *forceColor : FColor::GREEN,
			axisScale,
			numCircleSegments
		);
		renderer.Flush();
		hitTesting.EndHitProxy();
	}

	if( axisMask & GizmoAxis_Z )
	{
		hitTesting.BeginHitProxy( &hitProxies.axisHitProxyZ );
		renderer.DrawCircle(
			objPos,
			axisX,
			axisY,
			forceColor ? *forceColor : FColor::BLUE,
			axisScale,
			numCircleSegments
		);
		renderer.Flush();
		hitTesting.EndHitProxy();
	}
}

static
void F_Draw_Uniform_Scale_Handle( const EdDrawContext& context, const APlaceable* entity, const FColor* forceColor = nil )
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
	HGizmoAxis
-----------------------------------------------------------------------------
*/
mxDEFINE_ABSTRACT_CLASS( HGizmoAxis );

/*
-----------------------------------------------------------------------------
	EdGizmo
-----------------------------------------------------------------------------
*/
EdGizmo::EdGizmo()
{
	m_currentMode = Gizmo_Translate;
	m_currentAxis = GizmoAxis_None;
	m_highlightedAxes = GizmoAxis_None;

	mxCONNECT_THIS( EdSystem::Get().Event_PlaceableBeingDestroyed, EdGizmo, OnObjectDestroyed );
}

EdGizmo::~EdGizmo()
{
	mxDISCONNECT_THIS( EdSystem::Get().Event_PlaceableBeingDestroyed );
}

void EdGizmo::SetObject( APlaceable* pObject )
{
	m_selected = pObject;

	if( m_selected != nil )
	{
		m_oldState.Set( m_selected );
	}
	else
	{
		m_oldState.Clear();
	}
}

void EdGizmo::OnObjectDestroyed( APlaceable* pObject )
{
	if( m_selected == pObject )
	{
		this->Deactivate();
	}
	if( m_hightlighted == pObject )
	{
		m_hightlighted = nil;
	}
}

void EdGizmo::Deactivate()
{
	m_selected = nil;
	m_oldState.Clear();

	m_currentAxis = GizmoAxis_None;
}

void EdGizmo::RotateObject( const Matrix3& rot )
{
	if( m_selected != nil )
	{
		//DBGOUT("rotating object\n");

		Quat	q = rot.ToQuat() * m_oldState.orientation;
		q.Normalize();

		m_selected->SetOrientation(q);

		m_oldState.orientation = q;
	}
}

void EdGizmo::OnKeyPressed( const EdSceneViewport& viewport, const EKeyCode key )
{
	if( key == EKeyCode::Key_M
		|| key == EKeyCode::Key_T )
	{
		m_currentMode = Gizmo_Translate;
	}
	if( key == EKeyCode::Key_R )
	{
		m_currentMode = Gizmo_Rotate;
	}
	if( key == EKeyCode::Key_E )
	{
		m_currentMode = Gizmo_Scale;
	}

	if( m_selected != nil )
	{
		switch( m_currentMode )
		{
		case EGizmoMode::Gizmo_Translate :
			break;

		case EGizmoMode::Gizmo_Scale :
			break;

		case EGizmoMode::Gizmo_Rotate :
			{
				Matrix3	rotMatix;
				if( key == EKeyCode::Key_Left )
				{
					rotMatix.SetRotationX( -DEG2RAD(1) );
					this->RotateObject( rotMatix );
				}
				if( key == EKeyCode::Key_Right )
				{
					rotMatix.SetRotationX( +DEG2RAD(1) );
					this->RotateObject( rotMatix );
				}
				if( key == EKeyCode::Key_Up )
				{
					rotMatix.SetRotationZ( -DEG2RAD(1) );
					this->RotateObject( rotMatix );
				}
				if( key == EKeyCode::Key_Down )
				{
					rotMatix.SetRotationZ( +DEG2RAD(1) );
					this->RotateObject( rotMatix );
				}
			}
			break;
		}
	}
}

void EdGizmo::OnKeyReleased( const EdSceneViewport& viewport, const EKeyCode key )
{
	if( key == EKeyCode::Key_Escape )
	{
		m_selected = nil;
	}
}

void EdGizmo::OnMouseButton( const EdSceneViewport& viewport, const SMouseButtonEvent& args )
{
	if( args.button == EMouseButton::LeftMouseButton )
	{
		if( args.pressed )
		{
			AHitProxy* pHitProxy = viewport.objAtCursor;

			if( pHitProxy == nil )
			{
				this->Deactivate();
				return;
			}

			AssertPtr(pHitProxy);

			{
				APlaceable* pPlaceable = pHitProxy->IsPlaceable();
				if( pPlaceable != nil )
				{
					this->SetObject( pPlaceable );

					//AEditable* pEditable = pPlaceable->GetEditorObject();
					//if( pEditable != nil )
					//{
					//	gCore.editor->Notify_SelectionChanged( pEditable );
					//}
					gCore.editor->Notify_SelectionChanged( pHitProxy->GetSelectionProxy() );
				}
			}

			if( m_selected != nil )
			{
				HGizmoAxis* pHGizmoAxis = SafeCast<HGizmoAxis>( pHitProxy );
				if( pHGizmoAxis != nil )
				{
					m_currentAxis = pHGizmoAxis->axis;
				}
			}

			if( m_selected != nil )
			{
				if( F_Hit_Translate_Handle_At_Center( m_selected, viewport ) )
				{
					m_currentAxis = GizmoAxis_All;
					m_oldState.Set( m_selected );
				}
			}
		}
		else // if released
		{
			if( m_selected != nil )
			{
				m_oldState.Set( m_selected );
			}
			m_currentAxis = GizmoAxis_None;
		}
	}
}

void EdGizmo::OnMouseMove( const EdSceneViewport& viewport, const SMouseMoveEvent& args )
{
	AHitProxy* pHitProxy = viewport.objAtCursor;

	const bool bHighlightObjects = true;
	if( bHighlightObjects )
	{
		if( pHitProxy != nil )
		{
			APlaceable* pPlaceable = pHitProxy->IsPlaceable();
			m_hightlighted = pPlaceable;

			HGizmoAxis* pHGizmoAxis = SafeCast<HGizmoAxis>( pHitProxy );
			if( pHGizmoAxis != nil )
			{
				m_highlightedAxes = pHGizmoAxis->axis;
			}
			else
			{
				m_highlightedAxes = EGizmoAxis::GizmoAxis_None;
			}
		}
		else
		{
			m_hightlighted = nil;
		}
	}

	if( viewport.IsDraggingMouse() && m_selected != nil )
	{
		// gizmo pick point, in screen space
		Vec2D	pickPosNDC;
		PointToNDC( viewport, viewport.dragStartPosition.x(), viewport.dragStartPosition.y(), pickPosNDC );
		//DBGOUT("pickPosNDC: %f, %f\n",pickPosNDC.x,pickPosNDC.y);

		Vec2D	currPosNDC;
		PointToNDC( viewport, args.mouseX, args.mouseY, currPosNDC );
		//DBGOUT("currPosNDC: %f, %f\n",currPosNDC.x,currPosNDC.y);

		// we need to convert screen space delta to world space movement
		Vec2D	deltaNDC = currPosNDC - pickPosNDC;
		//dbgout << "delta=" << deltaNDC << dbgout.NewLine();

		switch( m_currentMode )
		{
		case EGizmoMode::Gizmo_Translate :
			if( m_currentAxis == GizmoAxis_None )
			{
				return;
			}
			if( m_currentAxis == GizmoAxis_All )
			{
L_TranslateAll:
				const rxView& eye = viewport.GetView();

				Ray3D	pickRay = GetEyeRay( viewport, currPosNDC );

				FLOAT	prevDist = (m_oldState.translation - eye.origin).LengthSqr();
				if( prevDist < VECTOR_EPSILON )
				{
					return;
				}
				prevDist = mxSqrt(prevDist);


				Vec3D	newEntityPos = eye.origin + pickRay.direction * prevDist;

				m_selected->SetOrigin( newEntityPos );
			}
			else
			{
				Vec3D	gizmoPickPos = F_Get_Translation_Gizmo_Pick_Point_In_World_Space( viewport, pickPosNDC, m_selected, m_currentAxis );
				//Vec3D	centerOfGizmo = m_selected->GetOrigin();
				Vec3D	pointOnGizmo = F_Get_Translation_Gizmo_Pick_Point_In_World_Space( viewport, currPosNDC, m_selected, m_currentAxis );
				Vec3D	translationDelta = pointOnGizmo - gizmoPickPos;

				const FLOAT MAX_TRANSLATION_DIST = 100.0f;
				translationDelta.Clamp(Vec3D(-MAX_TRANSLATION_DIST),Vec3D(MAX_TRANSLATION_DIST));

				Vec3D	newEntityPos = m_oldState.translation + translationDelta;

				m_selected->SetOrigin( newEntityPos );
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

				FLOAT	newEntityScale = m_oldState.scaleFactor + mag * signf(deltaNDC.x);
				newEntityScale = maxf(newEntityScale,0.01f);

				m_selected->SetScale( newEntityScale );
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
				mxUNDONE;
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
					args.mouseX, args.mouseY,
					ROT_ARC_RADIUS
					);

#if 0
				Quat	qRot = QuatFromBallPoints( vDownPt, vCurrPt );
				qRot.Normalize();
#else
				Vec3D	axis = Cross( vDownPt, vCurrPt );
				axis.Normalize();

				F4		angle = AngleBetween( vDownPt, vCurrPt );
				Quat	qRot( axis, angle );
				qRot.Normalize();
#endif

				Quat	q = qRot * m_oldState.orientation;
				q.Normalize();

				m_selected->SetOrientation(q);
			}
			break;

			mxNO_SWITCH_DEFAULT;
		}
	}
}

void EdGizmo::OnMouseWheel( const EdSceneViewport& viewport, const SMouseWheelEvent& args )
{
}

void EdGizmo::OnUpdate( const EdSceneViewport& viewport, FLOAT deltaSeconds )
{
	//Super::Tick(viewport,deltaSeconds);
}

void EdGizmo::DrawBackground( const EdDrawContext& context )
{
	mxUNUSED(context);
}

void EdGizmo::Draw( const EdDrawContext& context )
{
	if( m_selected != nil )
	{
		m_selected->Placeable_Draw_Editor_Stuff( context );

		F_Draw_AABB_Of_Placeable( m_selected, context.renderer, FColor::GREEN );
	}
	if( m_hightlighted != nil )
	{
		if( !context.viewport.IsDraggingMouse() )
		{
			F_Draw_AABB_Of_Placeable( m_hightlighted, context.renderer, FColor::WHITE );
		}
	}
}

void EdGizmo::DrawForeground( const EdDrawContext& context )
{
	if( m_selected != nil )
	{
		//if( m_highlightAxis == GizmoAxis_All )
		{
			const FColor axisColor = FColor::WHITE;
			F_Draw_Translate_Handle_At_Center( context, m_selected, axisColor );
		}

		switch( m_currentMode )
		{
		case Gizmo_Translate :
			{
				Draw_TranslationGizmoAxes( m_hitProxies, context, m_selected );

				// highlight selected axes

				const FColor axisColor = FColor::WHITE;

				if( m_currentAxis != GizmoAxis_None )
				{
					Draw_TranslationGizmoAxes( m_hitProxies, context, m_selected, m_currentAxis, &axisColor );
				}
				else if( m_highlightedAxes != GizmoAxis_None )
				{
					Draw_TranslationGizmoAxes( m_hitProxies, context, m_selected, m_highlightedAxes, &axisColor );
				}
			}
			break;

		case Gizmo_Rotate :
#if 0
			//if( m_currentAxis != GizmoAxis_All )
			{
				Draw_RotationGizmoAxes( context, m_selected );
			}
			if( m_highlightAxis != GizmoAxis_None )
			{
				const FColor axisColor = FColor::WHITE;
				Draw_RotationGizmoAxes( context, m_selected, m_highlightAxis, &axisColor );
			}
#endif
			mxUNDONE;
			break;

		case Gizmo_Scale :
			if( context.viewport.IsDraggingMouse() )
			{
				if( m_currentAxis != GizmoAxis_All )
				{
					F_Draw_Uniform_Scale_Handle( context, m_selected );
				}
			}
			break;

			mxNO_SWITCH_DEFAULT;
		}
	}
}

/*
-----------------------------------------------------------------------------
	EdManipulator
-----------------------------------------------------------------------------
*/
EdManipulator::EdManipulator()
{

}

EdManipulator::~EdManipulator()
{

}

void EdManipulator::OnSelectionChanged( AEditable* theObject )
{
	AObjectEditor* pObjectEditor = SafeCast<AObjectEditor>( theObject );
	if( pObjectEditor != nil )
	{
		APlaceable* pPlaceable = pObjectEditor->IsPlaceable();
		m_gizmo.SetObject( pPlaceable );
	}
}

void EdManipulator::OnObjectDestroyed( AEditable* theObject )
{
	AObjectEditor* pObjectEditor = SafeCast<AObjectEditor>( theObject );
	if( pObjectEditor != nil )
	{
		APlaceable* pPlaceable = pObjectEditor->IsPlaceable();
		m_gizmo.OnObjectDestroyed( pPlaceable );
	}
}

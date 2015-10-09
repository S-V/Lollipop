#include "Renderer_PCH.h"
#pragma hdrstop
#include "Renderer.h"

#include <Core/Editor/EditableProperties.h>
#include "RenderEntity.h"

/*
-----------------------------------------------------------------------------
	rxShadowCaster
-----------------------------------------------------------------------------
*/
void rxShadowCaster::rfRenderShadowDepth( const rxShadowRenderContext& context )
{
	mxUNUSED(context);
	mxDBG_UNREACHABLE;
}

void rxShadowCaster::rfRenderColoredShadowMap( const rxShadowRenderContext& context )
{
	mxUNUSED(context);
	mxDBG_UNREACHABLE;
}

/*
-----------------------------------------------------------------------------
	rxSpatialObject
-----------------------------------------------------------------------------
*/
rxSpatialObject::rxSpatialObject()
{
}

rxSpatialObject::~rxSpatialObject()
{

}

void rxSpatialObject::SetOrigin( const Vec3D& newPos )
{
	mxUNUSED(newPos);
	mxDBG_UNREACHABLE;
}

void rxSpatialObject::SetOrientation( const Quat& newRot )
{
	mxUNUSED(newRot);
	mxDBG_UNREACHABLE;
}

void rxSpatialObject::SetScale( const FLOAT newScale )
{
	mxUNUSED(newScale);
	mxDBG_UNREACHABLE;
}

Matrix4 rxSpatialObject::GetWorldTransform() const
{
	return Matrix4::mat4_identity;
}

void rxSpatialObject::GetWorldAABB( rxAABB & bbox )
{
	rxAABB_Infinity(bbox);
}

bool rxSpatialObject::CastRay( const Vec3D& start, const Vec3D& dir, FLOAT &fraction )
{
	rxAABB	bbox;
	this->GetWorldAABB(bbox);
	float4 vStart = vec_load1(start);
	float4 vDir = vec_load0(dir);
	return XNA::IntersectRayAxisAlignedBox(vStart,vDir,&bbox,&fraction);
}

/*
-----------------------------------------------------------------------------
	rxRenderEntity
-----------------------------------------------------------------------------
*/

NO_EMPTY_FILE

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//

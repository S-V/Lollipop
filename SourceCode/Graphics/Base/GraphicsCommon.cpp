/*
=============================================================================
	File:	GraphicsCommon.cpp
	Desc:	
=============================================================================
*/
#include "Graphics_PCH.h"
#pragma hdrstop
#include "Graphics.h"

// for key codes
#include <Core/App/Client.h>
#include <Core/Editor/EditableProperties.h>

mxBEGIN_BASIC_STRUCT(rxAABB)
	mxMEMBER_FIELD(Center)
	mxMEMBER_FIELD(Extents)
mxEND_REFLECTION

mxBEGIN_BASIC_STRUCT(rxOOBB)
	mxMEMBER_FIELD(Center)
	mxMEMBER_FIELD(Extents)
	mxMEMBER_FIELD(Orientation)
mxEND_REFLECTION

mxBEGIN_BASIC_STRUCT(Quat)
	mxMEMBER_FIELD(quad)
mxEND_REFLECTION

mxBEGIN_BASIC_STRUCT(rxTransform)
	mxMEMBER_FIELD(orientation)
	mxMEMBER_FIELD(translation)
	mxMEMBER_FIELD(scaleFactor)
mxEND_REFLECTION

//---------------------------------------------------------------------------
const char* rxUtil_GetVertexElementSemanticStr( EVertexElementUsage semantics )
{
	switch( semantics )
	{
	case EVertexElementUsage::VEU_Position :		return ("VEU_Position");
	case EVertexElementUsage::VEU_Normal :			return ("VEU_Normal");
	
	case EVertexElementUsage::VEU_TexCoords :		return ("VEU_TexCoords");

	case EVertexElementUsage::VEU_Color :			return ("VEU_Color");
	
	case EVertexElementUsage::VEU_Tangent :			return ("VEU_Tangent");
	case EVertexElementUsage::VEU_Binormal :		return ("VEU_Binormal");
	
	case EVertexElementUsage::VEU_BlendWeights :	return ("VEU_BlendWeights");
	case EVertexElementUsage::VEU_BlendIndices :	return ("VEU_BlendIndices");

	case EVertexElementUsage::VEU_Unknown :			return ("VEU_Unknown");
	}
	Unreachable;
	return nil;
}
//---------------------------------------------------------------------------
EVertexElementUsage rxUtil_GetVertexElementSemanticEnum( const char* str )
{
	StackString		tmp(_InitSlow,str);
	tmp.ToUpper();

	str = tmp.ToChars();

	mxUNSAFE("strchr");
	if(strstr(str,"POSITION")) {
		return VEU_Position;
	}
	if(strstr(str,"NORMAL")) {
		return VEU_Normal;
	}

	if(strstr(str,"TEXCOORD")) {
		return VEU_TexCoords;
	}

	if(strstr(str,"COLOR")) {
		return VEU_Color;
	}

	if(strstr(str,"TANGENT")) {
		return VEU_Tangent;
	}
	if(strstr(str,"BINORMAL")) {
		return VEU_Binormal;
	}

	if(strstr(str,"BLENDWEIGHTS")) {
		return VEU_BlendWeights;
	}
	if(strstr(str,"BLENDINDICES")) {
		return VEU_BlendIndices;
	}

	Unimplemented;
	return VEU_Unknown;
}
//---------------------------------------------------------------------------
const char* rxUtil_GetVertexElementNameBySemantic( EVertexElementUsage semantic )
{
	switch( semantic )
	{
	case EVertexElementUsage::VEU_Position :		return ("position");
	case EVertexElementUsage::VEU_Normal :			return ("normal");

	case EVertexElementUsage::VEU_TexCoords :		return ("texCoords");

	case EVertexElementUsage::VEU_Color :			return ("color");

	case EVertexElementUsage::VEU_Tangent :			return ("tangent");
	case EVertexElementUsage::VEU_Binormal :		return ("binormal");

	case EVertexElementUsage::VEU_BlendWeights :	return ("blendWeights");
	case EVertexElementUsage::VEU_BlendIndices :	return ("blendIndices");

	case EVertexElementUsage::VEU_Unknown :			return ("unknown");
	}
	Unreachable;
	return nil;
}
//---------------------------------------------------------------------------
void rxUtil_GenerateVertexElementName( EVertexElementUsage semantic, const char* SemanticName, UINT SemanticIndex, StackString &OutName )
{
	OutName.Empty();

	const char* baseName = rxUtil_GetVertexElementNameBySemantic(semantic);

	if( SemanticIndex == 0 )
	{
		OutName.SetString(baseName);
	}
	else
	{
		OutName.Format("%s%u",baseName,SemanticIndex);
	}
}





/*================================
		IndexedMesh
================================*/

IndexedMesh::IndexedMesh()
{
	positions = nil;
	texCoords = nil;
	tangents = nil;
	binormals = nil;
	normals = nil;
	colors = nil;
	numVertices = 0;

	indices = nil;
	indexStride = sizeof U4;
	numIndices = 0;

	bounds.Clear();

	//subsets = nil;
	//numSubsets = 0;

	//primType = EPrimitiveType::PT_TriangleList;

	//bOwnsData = false;
}
//---------------------------------------------------------------------------
IndexedMesh::~IndexedMesh()
{
	//if( bOwnsData )
	//{
	//	SAFE_DELETE_ARRAY( positions );
	//	SAFE_DELETE_ARRAY( texCoords );
	//	SAFE_DELETE_ARRAY( tangents );
	//	SAFE_DELETE_ARRAY( binormals );
	//	SAFE_DELETE_ARRAY( normals );
	//	SAFE_DELETE_ARRAY( colors );

	//	if( indexStride == sizeof U4 )
	//	{
	//		delete[] (U4*)indices;
	//	}
	//	else
	//	{	Assert( indexStride == sizeof U2 );
	//		delete[] (U2*)indices;
	//	}

	//	SAFE_DELETE_ARRAY( subsets );
	//}
}
//---------------------------------------------------------------------------
bool IndexedMesh::isOk() const
{
	return true
		&& positions != nil
		&& numVertices > 0

		&& indices != nil
		&& ((indexStride == 2) || (indexStride == 4))
		&& numIndices > 0

		//&& subsets != nil
		;
}
//---------------------------------------------------------------------------
// static
const char* IndexedMesh::GetStreamNameBySemantic( EVertexElementUsage usage )
{
	switch( usage )
	{
	case EVertexElementUsage::VEU_Position :		return ("positions");
	case EVertexElementUsage::VEU_Normal :			return ("normals");
	case EVertexElementUsage::VEU_TexCoords :		return ("texCoords");
	case EVertexElementUsage::VEU_Color :			return ("colors");
	case EVertexElementUsage::VEU_Tangent :			return ("tangents");
	case EVertexElementUsage::VEU_Binormal :		return ("binormals");
	//case EVertexElementUsage::VEU_BlendWeights :	return ("blendweights");
	//case EVertexElementUsage::VEU_BlendIndices :	return ("blendindices");
	//case EVertexElementUsage::VEU_Unknown :			return ("unknown");
	}
	Unreachable;
	return nil;
}


const char* GetImageFileFormatFileExt( EImageFileFormat e )
{
	switch( e )
	{
	case EImageFileFormat::IFF_BMP :
		return (".bmp");

	case EImageFileFormat::IFF_JPG :
		return (".jpg");

	case EImageFileFormat::IFF_PNG :
		return (".png");

	case EImageFileFormat::IFF_DDS :
		return (".dds");

	case EImageFileFormat::IFF_TIFF :
		return (".tiff");

	case EImageFileFormat::IFF_GIF :
		return (".gif");

	case EImageFileFormat::IFF_WMP :
		return (".wmp");

	default:
		/* fallthrough */;
	}
	return ("unknown");
}

/*
================================
		GetDriverName
================================
*/
const char* rxUtil_GetDriverName( EDriverType driver )
{
	switch( driver )
	{
		case EDriverType::GAPI_None :
			return ("Null");

		case EDriverType::GAPI_Direct3D_9 :
			return ("Direct3D 9.0");

		case EDriverType::GAPI_Direct3D_9S :
			return ("Direct3D 9 on shaders");

		case EDriverType::GAPI_Direct3D_10 :
			return ("Direct3D 10.0");

		case EDriverType::GAPI_Direct3D_10_1 :
			return ("Direct3D 10.1");

		case EDriverType::GAPI_Direct3D_11 :
			return ("Direct3D 11.0");

		case EDriverType::GAPI_OpenGL :
			return ("OpenGL");

		case EDriverType::GAPI_OpenGLS :
			return ("OpenGL on shaders");

		case EDriverType::GAPI_AutomaticSelection :
			return ("Auto");

		default:
			/* fallthrough */;
	}
	return ("Unknown");
}

//===================================================================

/*
	This code is based on information obtained from the following site:
	http://www.pcidatabase.com/
*/

EDeviceVendor rxUtil_GetGPUVendorEnum( UINT vendorId )
{
	switch ( vendorId ) {
		case 0x3D3D	:
			return EDeviceVendor::Vendor_3DLABS;

		case 0x1002 :
			return EDeviceVendor::Vendor_ATI;

		case 0x8086 :
			return EDeviceVendor::Vendor_Intel;

		case 0x102B :
			return EDeviceVendor::Vendor_Matrox;

		case 0x10DE	:
			return EDeviceVendor::Vendor_NVidia;

		case 0x5333	:
			return EDeviceVendor::Vendor_S3;

		case 0x1039	:
			return EDeviceVendor::Vendor_SIS;

		default:
			// empty
			;
	}
	return EDeviceVendor::Vendor_Unknown;
}

/*
	This code is based on information obtained from the following site:
	http://www.pcidatabase.com/
*/

const WChar * rxUtil_GetGPUVendorName( EDeviceVendor vendor )
{
	switch ( vendor ) {
		case EDeviceVendor::Vendor_3DLABS :
			return L"3Dlabs, Inc. Ltd";

		case EDeviceVendor::Vendor_ATI :
			return L"ATI Technologies Inc. / Advanced Micro Devices, Inc.";

		case EDeviceVendor::Vendor_Intel :
			return L"Intel Corporation";

		case EDeviceVendor::Vendor_Matrox :
			return L"Matrox Electronic Systems Ltd.";

		case EDeviceVendor::Vendor_NVidia :
			return L"NVIDIA Corporation";

		case EDeviceVendor::Vendor_S3 :
			return L"S3 Graphics Co., Ltd";

		case EDeviceVendor::Vendor_SIS :
			return L"SIS";

		case EDeviceVendor::Vendor_Unknown :
		default:
			return L"Unknown vendor";
	}
	//Unreachable;
}


//-------------------------------------------------------------------------------------------------------------//

const char* GetShaderTypeString( EShaderType type )
{
	switch( type )
	{
	case EShaderType::ST_Unknown_Shader:	return "Unknown shader";

	case EShaderType::ST_Vertex_Shader :	return "Vertex shader";
	case EShaderType::ST_Hull_Shader :		return "Hull shader";
	case EShaderType::ST_Tessellation_Shader :return "Tessellation shader";
	case EShaderType::ST_Domain_Shader :	return "Domain shader";
	case EShaderType::ST_Geometry_Shader :	return "Geometry shader";
	case EShaderType::ST_Pixel_Shader :		return "Pixel shader";

	case EShaderType::ST_Compute_Shader :	return "Compute shader";
	}
	Unreachable;
	return nil;
}

/*================================
			rxView
================================*/

mxDEFINE_CLASS(rxView);
mxBEGIN_REFLECTION(rxView)
	mxMEMBER_FIELD2( right,	Right_Dir )
	mxMEMBER_FIELD2( up,		Up_Dir )
	mxMEMBER_FIELD2( look,	Look_Dir )
	mxMEMBER_FIELD2( origin,	Origin_Pos )
	mxMEMBER_FIELD2( nearZ, Near_Clip )
	mxMEMBER_FIELD2( farZ, Far_Clip )
	mxMEMBER_FIELD2( fovY, Vertical_FoV )
	mxMEMBER_FIELD2( aspectRatio, Aspect_Ratio )
mxEND_REFLECTION;

void rxView::edCreateProperties( EdPropertyList *properties, bool bClear )
{
	new EdProperty_Vector3D( properties, "X", right, PF_ReadOnly );
	new EdProperty_Vector3D( properties, "Y", up, PF_ReadOnly );
	new EdProperty_Vector3D( properties, "Z", look, PF_ReadOnly );
	new EdProperty_Vector3D( properties, "origin", origin );

	new EdProperty_Float( properties, "nearZ", nearZ );
	new EdProperty_Float( properties, "farZ", farZ );

	new EdProperty_Float( properties, "fovY", fovY, PF_ReadOnly );
	new EdProperty_Float( properties, "aspectRatio", aspectRatio, PF_ReadOnly );
}

/*================================
			FPSCamera
================================*/

mxDEFINE_CLASS(SCamera);
mxBEGIN_REFLECTION(SCamera)
	mxMEMBER_FIELD( view )
	mxMEMBER_FIELD( rotationScaler )
	mxMEMBER_FIELD( moveScaler )
	mxMEMBER_FIELD( totalDragTimeToZero )
	mxMEMBER_FIELD( rotationVelocity )

	// these are needed for restoring camera orientation
	mxMEMBER_FIELD( yawAngle )
	mxMEMBER_FIELD( pitchAngle )

	mxMEMBER_FIELD( numFramesToSmoothMouseData )
mxEND_REFLECTION;

//------------------------------------------------------------------------------
FLOAT SCamera::GetMinMovementVelocity()
{
	return 0.1f;
}
//------------------------------------------------------------------------------
FLOAT SCamera::GetMaxMovementVelocity()
{
	return 100.0f;
}
//------------------------------------------------------------------------------
/*static*/ FLOAT SCamera::GetDefaultMovementVelocity()
{
	return 5.0f;
}
//------------------------------------------------------------------------------
/*static*/ FLOAT SCamera::GetDefaultRotationVelocity()
{
	return 0.01f;
}
//------------------------------------------------------------------------------
SCamera::SCamera()
{
	ZERO_OUT( pressedKeys );

	rotationScaler		= GetDefaultRotationVelocity();
	moveScaler			= GetDefaultMovementVelocity();
	velocity			.SetZero();
	velocityDrag		.SetZero();
	totalDragTimeToZero	= 0.25f;
	dragTimer			= 0.0f;
	rotationVelocity	.SetZero();
	yawAngle			= 0.0f;
	pitchAngle			= 0.0f;
	mouseDelta			.SetZero();
	numFramesToSmoothMouseData = 4.0f;

	view.nearZ = 0.1f;
	view.farZ = 1e3f;

	ResetView();
}

//FPSCamera::FPSCamera( const FPSCamera& other )
//{
//	*this = other;
//}

//------------------------------------------------------------------------------
void SCamera::ResetView()
{
	view.right	.Set( 1.0f, 0.0f, 0.0f );
	view.up		.Set( 0.0f, 1.0f, 0.0f );
	view.look	.Set( 0.0f, 0.0f, 1.0f );
	view.origin	.Set( 0.0f, 1.0f, 0.0f );
}
void SCamera::SetView( const rxView& src )
{
	view = src;
}
//------------------------------------------------------------------------------
void SCamera::SetAspectRatio( FLOAT newAspectRatio )
{
	view.aspectRatio = newAspectRatio;
}
//------------------------------------------------------------------------------
static SCamera::Actions F_MapKey( EKeyCode key )
{
	switch( key )
	{
	case EKeyCode::Key_W :	return SCamera::Actions::MoveForward;
	case EKeyCode::Key_S :	return SCamera::Actions::MoveBackward;
	case EKeyCode::Key_A :	return SCamera::Actions::StrafeLeft;
	case EKeyCode::Key_D :	return SCamera::Actions::StrafeRight;
	case EKeyCode::Key_C :	return SCamera::Actions::MoveDown;
	case EKeyCode::Key_Space :	return SCamera::Actions::MoveUp;

	//case EKeyCode::Key_R :	return FPSCamera::Actions::ResetToDefaults;
	}
	return SCamera::Actions::Unknown;
}
//------------------------------------------------------------------------------
void SCamera::Update( FLOAT deltaTime )
{
	//if( pressedKeys[ResetToDefaults] )
	//{
	//	ResetView();
	//}


	// Update the camera rotation

	rotationVelocity = mouseDelta * rotationScaler;

	mouseDelta.SetZero();

	// Update the pitch & yaw angle based on mouse movement
	float fYawDelta = rotationVelocity.x;
	float fPitchDelta = rotationVelocity.y;

	// Invert pitch if requested
	bool m_bInvertPitch = 0;
	if( m_bInvertPitch ) {
		fPitchDelta = -fPitchDelta;
	}
	pitchAngle += fPitchDelta;
	yawAngle += fYawDelta;

	// Limit pitch to straight up or straight down
	pitchAngle = clampf( pitchAngle, -MX_HALF_PI, +MX_HALF_PI );


	// Make a rotation matrix based on the camera's yaw & pitch.
	float4x4	cameraRotation;
	cameraRotation = XMMatrixRotationRollPitchYaw( pitchAngle, yawAngle, 0.0f );

	const Vec3D vWorldRight = as_vec4(XMVector3TransformNormal(g_XMIdentityR0,cameraRotation)).ToVec3();
	const Vec3D vWorldUp = as_vec4(XMVector3TransformNormal(g_XMIdentityR1,cameraRotation)).ToVec3();
	const Vec3D vWorldAhead = as_vec4(XMVector3TransformNormal(g_XMIdentityR2,cameraRotation)).ToVec3();

	view.right = vWorldRight;
	view.up = vWorldUp;
	view.look = vWorldAhead;


	// Update the camera position

	bool	bIntegratePosition = 1;

	if( bIntegratePosition )
	{
		Vec3D keyboardDirection(0.0f);// Direction vector of keyboard input

		if( pressedKeys[MoveForward] )
		{
			keyboardDirection.z += 1.0f;
		}
		if( pressedKeys[MoveBackward] )
		{
			keyboardDirection.z -= 1.0f;
		}

		if( pressedKeys[StrafeLeft] )
		{
			keyboardDirection.x -= 1.0f;
		}
		if( pressedKeys[StrafeRight] )
		{
			keyboardDirection.x += 1.0f;
		}

		if( pressedKeys[MoveUp] )
		{
			keyboardDirection.y += 1.0f;
		}
		if( pressedKeys[MoveDown] )
		{
			keyboardDirection.y -= 1.0f;
		}


		Vec3D vAccel = keyboardDirection;
		const FLOAT mag = vAccel.LengthSqr();
		if( mag > VECTOR_EPSILON )
		{
			vAccel *= mxInvSqrt(mag);
		}
		else
		{
			vAccel.SetZero();
		}


		// Scale the acceleration vector
		vAccel *= moveScaler;

		bool bMovementDrag = 1;        // If true, then camera movement will slow to a stop otherwise movement is instant
		if( bMovementDrag )
		{
			// Is there any acceleration this frame?
			FLOAT lengthSq = vAccel.LengthSqr();
			if( lengthSq >= 0.0f )
			{
				// If so, then this means the user has pressed a movement key\
				// so change the velocity immediately to acceleration 
				// upon keyboard input.  This isn't normal physics
				// but it will give a quick response to keyboard input
				velocity = vAccel;
				dragTimer = totalDragTimeToZero;
				velocityDrag = vAccel / dragTimer;
			}
			else
			{
				// If no key being pressed, then slowly decrease velocity to 0
				if( dragTimer > 0 )
				{
					// Drag until timer is <= 0
					velocity -= velocityDrag * deltaTime;
					dragTimer -= deltaTime;
				}
				else
				{
					// Zero velocity
					velocity = Vec3D( 0, 0, 0 );
				}
			}
		}
		else
		{
			// No drag, so immediately change the velocity
			velocity = vAccel;
		}


		// Simple Euler method to calculate position delta
		Vec3D vPosDelta = velocity * deltaTime;

		// Transform the position delta by the camera's rotation 

		Vec3D vPosDeltaWorld = as_matrix4(cameraRotation).TransformVector(vPosDelta);

		// Move the eye position 
		view.origin += vPosDeltaWorld;
	}



	//F_DbgOutAppendText(
	//	"Pos: %.2f %.2f %.2f\t"
	//	"Look: %.2f %.2f %.2f\t"
	//	,
	//	sceneView.origin.x,	sceneView.origin.y,	sceneView.origin.z,
	//	sceneView.look.x,	sceneView.look.y,	sceneView.look.z
	//	);
}
//------------------------------------------------------------------------------
void SCamera::OnKeyPressed( EKeyCode key )
{
	//EdLogf(LL_Info,"Key pressed: %s\n",mxKeyToStr(key));
	Actions mappedKey = F_MapKey( key );
	if( mappedKey != Actions::Unknown )
	{
		pressedKeys[ mappedKey ] = true;
	}
}
//------------------------------------------------------------------------------
void SCamera::OnKeyReleased( EKeyCode key )
{
	Actions mappedKey = F_MapKey( key );
	if( mappedKey != Actions::Unknown )
	{
		pressedKeys[ mappedKey ] = false;
	}
}
//------------------------------------------------------------------------------
void SCamera::OnMouseMove( FLOAT mouseDeltaX, FLOAT mouseDeltaY )
{
	mouseDelta.x = mouseDeltaX;
	mouseDelta.y = mouseDeltaY;

	// Smooth the relative mouse data over a few frames so it isn't 
	// jerky when moving slowly at low frame rates.
	float fPercentOfNew = 1.0f / numFramesToSmoothMouseData;
	float fPercentOfOld = 1.0f - fPercentOfNew;
	mouseDelta.x = mouseDelta.x * fPercentOfOld + mouseDelta.x * fPercentOfNew;
	mouseDelta.y = mouseDelta.y * fPercentOfOld + mouseDelta.y * fPercentOfNew;
}

void SCamera::edCreateProperties( EdPropertyList *properties, bool bClear )
{
	view.edCreateProperties( properties );

	new EdProperty_Float( properties, "Rotation Speed", rotationScaler );
	new EdProperty_Float( properties, "Movement Speed", moveScaler );
	new EdProperty_Float( properties, "totalDragTimeToZero", totalDragTimeToZero );
	new EdProperty_Float( properties, "numFramesToSmoothMouseData", numFramesToSmoothMouseData );
}

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//

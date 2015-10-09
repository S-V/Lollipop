
#include <Physics_PCH.h>
#pragma hdrstop
#include <Physics.h>

#include "DemoUtil.h"


// header files for the OpenGL32 library
#include <gl\gl.h>	// main OpenGL header (OpenGL.H on Macintosh)
#include <gl\glu.h>	// utility library
//#include <gl\glaux.h>	// auxiliary

#if MX_AUTOLINK
#pragma comment( lib, "opengl32.lib" )
#pragma comment( lib, "glu32.lib" )
#endif

#include <Driver/Driver.h>
//#include <Graphics/OpenGL.h>


DemoSettings	DemoSettings::G;

//---------------------------------------------------------------------------------------------------

pxRigidBody* MakeBox(
					 const Vec3D& halfSize,
					 FLOAT density
					 )
{
	pxRigidBodyInfo	rbDesc;

	const pxVec3	halfExtents( halfSize.x, halfSize.y, halfSize.z );
	rbDesc.shape = new pxShape_Box( halfExtents );

	rbDesc.material = pxMaterial::DefaultId;

	pxRigidBody * rb = pxPhysicsSystem::Get().NewRigidBody( rbDesc );

	pxReal boxMass = 8.0f * (density * halfSize.x) * (halfSize.y * halfSize.x);

	pxMassProperties	massProps;
	pxUtil_MassPropertiesComputer::ComputeBoxVolumeMassProperties( halfExtents, boxMass, massProps );

	rb->SetMass( massProps.mass );
	rb->SetInertiaInvLocal( massProps.inertiaTensor.ToMat3x3().GetInverse() );

	return rb;
}
//---------------------------------------------------------------------------------------------------
pxRigidBody* MakeSphere(
						FLOAT radius,
						FLOAT density
						)
{
	pxRigidBodyInfo	rbDesc;
	rbDesc.shape = new pxShape_Sphere( radius );


	rbDesc.material = pxMaterial::DefaultId;

	pxRigidBody * rb = pxPhysicsSystem::Get().NewRigidBody( rbDesc );

	const FLOAT mass = ((4.0f/3.0f)*MX_PI) * (cubef(radius) * density);

	pxMassProperties	massProps;
	pxUtil_MassPropertiesComputer::ComputeSphereVolumeMassProperties( radius, mass, massProps );

	rb->SetMass( massProps.mass );
	rb->SetInertiaInvLocal( massProps.inertiaTensor.ToMat3x3().GetInverse() );

	return rb;
}
//---------------------------------------------------------------------------------------------------

void DrawShape_HalfSpace( const pxTransform& xform, const pxShape_HalfSpace* shape )
{
	Raster::G.SetWorldMatrix( Matrix4::mat4_identity );

	rrSetColor(FColor::DARK_GREY.AsRGB());

#if 1
	pxVec3 n( shape->GetPlane().GetNormal() );
	pxVec3 p, q;
	TPlaneSpace( n, p, q );

	const FLOAT sizeX = 100.0f, sizeZ = 100.0f;
	glNormal3f( n.x, n.y, n.z );
	glBegin( GL_QUADS );
	//glVertex3f( -sizeX * p.x, 0.0,  sizeZ * q.z );
	//glVertex3f(  sizeX * p.x, 0.0,  sizeZ * q.z );
	//glVertex3f(  sizeX * p.x, 0.0, -sizeZ * q.z );
	//glVertex3f( -sizeX * p.x, 0.0, -sizeZ * q.z );
	glVertex3f( -sizeX * p.x, 0.0, -sizeZ * q.z );
	glVertex3f(  sizeX * p.x, 0.0, -sizeZ * q.z );
	glVertex3f(  sizeX * p.x, 0.0,  sizeZ * q.z );
	glVertex3f( -sizeX * p.x, 0.0,  sizeZ * q.z );
	glEnd();
#else
	Assert( (pxvec3_to_vec3(shape->GetPlane().GetNormal())==pxVec3(0,1,0) && shape->GetPlane().d==0 ) );
	const FLOAT sizeX = 100.0f, sizeZ = 100.0f;
	glNormal3f(0.0, 1.0, 0.0);
	glBegin(GL_QUADS);
	glVertex3f(-sizeX, 0.0, sizeZ);
	glVertex3f(sizeX, 0.0, sizeZ);
	glVertex3f(sizeX, 0.0, -sizeZ);
	glVertex3f(-sizeX, 0.0, -sizeZ);
	glEnd();
#endif
}
//---------------------------------------------------------------------------------------------------
void DrawShape_Sphere( const pxTransform& xform, const pxShape_Sphere* shape )
{
	Matrix4 mat;
	xform.GetOpenGLMatrix(mat.ToFloatPtr());

	Raster::G.SetWorldMatrix(
		Matrix4::CreateScale(shape->GetRadius())
		* mat
		);

	rrSetColor(FColor::YELLOW.AsRGB());

	rrDrawUnitSphere();
}
//---------------------------------------------------------------------------------------------------
void DrawShape_Box( const pxTransform& xform, const pxShape_Box* shape )
{
	Matrix4 mat;
	xform.GetOpenGLMatrix(mat.ToFloatPtr());
	Raster::G.SetWorldMatrix(mat);

	rrSetColor(FColor::BLUE.AsRGB());

	const pxVec3 & halfSize = shape->GetHalfSize();
	rrDrawBox( halfSize.x, halfSize.y, halfSize.z );
}
//---------------------------------------------------------------------------------------------------
void DrawShape( const pxTransform& xform, const pxShape* shape )
{
	AssertPtr(shape);

	switch(shape->GetType())
	{

	case pxcShapeType::PX_SHAPE_HALFSPACE :
		{
			const pxShape_HalfSpace* realShape = checked_cast< const pxShape_HalfSpace* >( shape );
			DrawShape_HalfSpace( xform, realShape );
		}
		break;

	case pxcShapeType::PX_SHAPE_SPHERE :
		{
			const pxShape_Sphere* realShape = checked_cast< const pxShape_Sphere* >( shape );
			DrawShape_Sphere( xform, realShape );
		}
		break;

	case pxcShapeType::PX_SHAPE_BOX :
		{
			const pxShape_Box* realShape = checked_cast< const pxShape_Box* >( shape );
			DrawShape_Box( xform, realShape );
		}
		break;

	default:
		UnreachableX("Unknown shape type");
	}
}

void DrawObject( const pxRigidBody* obj )
{
	AssertPtr(obj);
	switch(obj->GetInternalType())
	{
	case pxcObjectType::PX_RIGID_BODY :
		{
			DrawShape( obj->GetTransform(), obj->GetShape() );
		}
		break;

	default:
		UnreachableX("Unknown object type");
	}
}

//---------------------------------------------------------------------------------------------------

void DrawContact(
				 const pxVec3& pointOnB,
				 const pxVec3& normalOnB,
				 pxReal distance,
				 const FColor& color
				 )
{
	Assert(normalOnB.IsNormalized());

	distance = clampf( distance, 0.3f, distance );

	const pxVec3 from = pointOnB;
	const pxVec3 to = pointOnB + normalOnB * distance;

	rrSetColor(color.AsRGB());

	glPointSize( 7.0f );
	glBegin( GL_POINTS );
	glVertex3fv( from.ToPtr() );
	glEnd();
	glPointSize( 1.0f );

	rrSetColor(FColor::YELLOW.AsRGB());

	glLineWidth( 3.0f );

	glBegin( GL_LINES );
	glVertex3fv( from.ToPtr() );
	glVertex3fv( to.ToPtr() );

#if 1
	const pxVec3 to2 = pointOnB - normalOnB * distance;

	glVertex3fv( from.ToPtr() );
	glVertex3fv( to2.ToPtr() );
#endif

	glEnd();

	glLineWidth( 1.0f );
}




/*================================
		GLDebugDrawer
================================*/



GLDebugDrawer::GLDebugDrawer()
:m_debugMode(0)
{

}

void	GLDebugDrawer::drawLine(const pxVec3& from,const pxVec3& to,const pxVec3& fromColor, const pxVec3& toColor)
{
	glBegin(GL_LINES);
		glColor3f(fromColor.x, fromColor.y, fromColor.z);
		glVertex3d(from.x, from.y, from.z);
		glColor3f(toColor.x, toColor.y, toColor.z);
		glVertex3d(to.x, to.y, to.z);
	glEnd();
}

void	GLDebugDrawer::drawLine(const pxVec3& from,const pxVec3& to,const pxVec3& color)
{
	drawLine(from,to,color,color);
}

void GLDebugDrawer::drawSphere (const pxVec3& p, pxReal radius, const pxVec3& color)
{
	glColor4f (color.x, color.y, color.z, pxReal(1.0f));
	glPushMatrix ();
	glTranslatef (p.x, p.y, p.z);

	int lats = 5;
	int longs = 5;

	int i, j;
	for(i = 0; i <= lats; i++) {
		pxReal lat0 = MX_PI * (-pxReal(0.5) + (pxReal) (i - 1) / lats);
		pxReal z0  = radius*sin(lat0);
		pxReal zr0 =  radius*cos(lat0);

		pxReal lat1 = MX_PI * (-pxReal(0.5) + (pxReal) i / lats);
		pxReal z1 = radius*sin(lat1);
		pxReal zr1 = radius*cos(lat1);

		glBegin(GL_QUAD_STRIP);
		for(j = 0; j <= longs; j++) {
			pxReal lng = 2 * MX_PI * (pxReal) (j - 1) / longs;
			pxReal x = cos(lng);
			pxReal y = sin(lng);

			glNormal3f(x * zr0, y * zr0, z0);
			glVertex3f(x * zr0, y * zr0, z0);
			glNormal3f(x * zr1, y * zr1, z1);
			glVertex3f(x * zr1, y * zr1, z1);
		}
		glEnd();
	}

	glPopMatrix();
}

void GLDebugDrawer::drawBox (const pxVec3& boxMin, const pxVec3& boxMax, const pxVec3& color, pxReal alpha)
{
	pxVec3 halfExtent = (boxMax - boxMin) * pxReal(0.5f);
	pxVec3 center = (boxMax + boxMin) * pxReal(0.5f);
	//glEnable(GL_BLEND);     // Turn blending On
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glColor4f (color.x, color.y, color.z, alpha);
	glPushMatrix ();
	glTranslatef (center.x, center.y, center.z);
	glScaled(2*halfExtent[0], 2*halfExtent[1], 2*halfExtent[2]);
	
	void extern glutSolidCube(double size);
	glutSolidCube(1.0);
	glPopMatrix ();
	//glDisable(GL_BLEND);
}

void	GLDebugDrawer::drawTriangle(const pxVec3& a,const pxVec3& b,const pxVec3& c,const pxVec3& color,pxReal alpha)
{
//	if (m_debugMode > 0)
	{
		const pxVec3	n=pxCross(b-a,c-a).normalized();
		glBegin(GL_TRIANGLES);		
		glColor4f(color.x, color.y, color.z,alpha);
		glNormal3d(n.x,n.y,n.z);
		glVertex3d(a.x,a.y,a.z);
		glVertex3d(b.x,b.y,b.z);
		glVertex3d(c.x,c.y,c.z);
		glEnd();
	}
}

void	GLDebugDrawer::setDebugMode(int debugMode)
{
	m_debugMode = debugMode;

}

void	GLDebugDrawer::draw3dText(const pxVec3& location,const char* textString)
{
//	glRasterPos3f(location.x,  location.y,  location.z);
	//BMF_DrawString(BMF_GetFont(BMF_kHelvetica10),textString);

	DrawText3D(location.x,  location.y,  location.z, textString, strlen(textString));
}

void	GLDebugDrawer::reportErrorWarning(const char* warningString)
{
	printf("%s\n",warningString);
}

void	GLDebugDrawer::drawContactPoint(const pxVec3& pointOnB,const pxVec3& normalOnB,pxReal distance,int lifeTime,const pxVec3& color)
{
	
	{
		pxVec3 to=pointOnB+normalOnB*distance;
		const pxVec3&from = pointOnB;
		glColor4f(color.x, color.y, color.z,1.f);
		//glColor4f(0,0,0,1.f);

		glBegin(GL_LINES);
		glVertex3d(from.x, from.y, from.z);
		glVertex3d(to.x, to.y, to.z);
		glEnd();

		
		
		char buf[12];
		int length = sprintf(buf," %d",lifeTime);
		//glRasterPos3f(from.x,  from.y,  from.z);
		//BMF_DrawString(BMF_GetFont(BMF_kHelvetica10),buf);

		DrawText3D( from.x,  from.y,  from.z, buf, length );
	}
}

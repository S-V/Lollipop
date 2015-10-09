#pragma once


class GLDebugDrawer : public pxDebugDrawer
{
	int m_debugMode;

public:

	GLDebugDrawer();


	virtual void	drawLine(const pxVec3& from,const pxVec3& to,const pxVec3& fromColor, const pxVec3& toColor);

	virtual void	drawLine(const pxVec3& from,const pxVec3& to,const pxVec3& color);

	virtual void	drawSphere (const pxVec3& p, pxReal radius, const pxVec3& color);
	virtual void	drawBox (const pxVec3& boxMin, const pxVec3& boxMax, const pxVec3& color, pxReal alpha);

	virtual void	drawTriangle(const pxVec3& a,const pxVec3& b,const pxVec3& c,const pxVec3& color,pxReal alpha);
	
	virtual void	drawContactPoint(const pxVec3& PointOnB,const pxVec3& normalOnB,pxReal distance,int lifeTime,const pxVec3& color);

	virtual void	reportErrorWarning(const char* warningString);

	virtual void	draw3dText(const pxVec3& location,const char* textString);

	virtual void	setDebugMode(int debugMode);

	virtual int		getDebugMode() const { return m_debugMode;}

};








struct DemoApp
{
	DemoApp()
	{}
	virtual ~DemoApp()
	{}

	virtual const char* GetName() const {return "unnamed";};
	virtual bool Setup() { return true; }
	virtual void Close() {}

	virtual void MoveAndDraw( FLOAT deltaSeconds ) {}
	virtual void DrawStats( int startPosY ) {}
	//virtual void OnKeyPressed( EKeyCode button ) {}
};
struct DemoSettings
{
	bool	bWireframe;
	bool	bReverseCulling;
	bool	bDrawAabbs;
	bool	bDrawContacts;
	bool	bFlatShading;

	DemoSettings()
	{
		bWireframe = 0;
		bReverseCulling = 0;
		bDrawAabbs = 1;
		bDrawContacts = 1;
		bFlatShading = 1;
	}

	static DemoSettings G;
};


FORCEINLINE const Vec3D& pxvec3_to_vec3( const pxVec3& vector ) {
	return reinterpret_cast< const Vec3D& >( vector );
}
FORCEINLINE const pxVec3 vec3_to_pxvec3( const Vec3D& vector ) {
	return pxVec3( vector.x,vector.y,vector.z );
}



pxRigidBody* MakeBox(
					 const Vec3D& halfSize = Vec3D(0.5f),
					 FLOAT density = 1.0f
					 );

pxRigidBody* MakeSphere(
						FLOAT radius = 1.0f,
						FLOAT density = 1.0f
						);


void DrawObject( const pxRigidBody* obj );


void DrawContact(
				 const pxVec3& pointOnB,
				 const pxVec3& normalOnB,
				 pxReal distance,
				 const FColor& color = FColor::RED
				 );




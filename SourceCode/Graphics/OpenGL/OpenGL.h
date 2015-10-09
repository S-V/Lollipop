#pragma once

#include <Graphics/Base/Config.h>
#include <Graphics/Base/Types.h>
#include <Graphics/Base/View.h>

MX_NAMESPACE_BEGIN

class rrFont2D
{
public:
	rrFont2D();
	~rrFont2D();

	bool Setup( HDC hDC );
	void Close();

	void Draw(
		int xPos, int yPos,
		const char *str, int length
	);

private:
	UINT CreateBitmapFont( const TCHAR *fontName, int fontHeight, HDC hDC );
	
	void ReleaseFont(unsigned int base);

public:
	UINT		m_fontListBase;
};

//---------------------------------------------------------------------------------------------------

class rrFont3D
{
public:
	rrFont3D();
	~rrFont3D();

	bool Setup( HDC hDC );
	void Close();

	void Draw(
		float xPos, float yPos, float zPos,
		const char *str, int length
	);

private:
	UINT CreateOutlineFont( const TCHAR *fontName, int fontHeight, float depth, HDC hDC );
	
	void ReleaseFont(UINT base);

public:
	GLYPHMETRICSFLOAT	gmf[256];   // holds orientation and placement info for display lists
	UINT				m_fontListBase;
};

//---------------------------------------------------------------------------------------------------

struct rrMaterial
{
	FColor	ambient;
	FColor	diffuse;
	FColor	emissive;
	FColor	specular;
	FLOAT	shininess;

public:
	rrMaterial()
	{
		ambient = FColor::DARK_GREY;
		diffuse = FColor::WHITE;
		emissive = FColor::BLACK;
		specular = FColor::WHITE;
		shininess = 0.0f;
	}
};

struct rrLight
{
	ColorRGB	ambient;
	ColorRGB	diffuse;
	ColorRGB	specular;

	Vec3D	position;
//	FLOAT	radius;
	Vec3D	direction;
	FLOAT	attenuation[3];
	FLOAT	innerCone, outerCone, fallOff;

public:
	rrLight()
	{
		ambient = FColor::BLACK.AsRGB();
		diffuse = FColor::WHITE.AsRGB();
		specular = FColor::WHITE.AsRGB();
		position.Set(0,0,0);
//		radius = 100.0f;
		direction.Set(0,0,1);
		attenuation[0] = 1.0f;
		attenuation[1] = 0.0f;
		attenuation[2] = 0.0f;
		innerCone = 0.0f;
		outerCone = 45.0f;
		fallOff = 2.0f;
	}

	void SetRadius( FLOAT radius )
	{
		attenuation[0] = 0.0f;
		attenuation[1] = (radius == 0.f) ? 0.0f : 1.0f / radius;
		attenuation[2] = 0.0f;
	}
};

//---------------------------------------------------------------------------------------------------

class Raster
{
public:
	static Raster	G;	// ugly singleton

	Raster();
	~Raster();

	bool Setup( HWND window );
	void Close();

	void BeginScene( const rxSceneView& view );
	void EndScene();

	//void GetInfo( rxRendererInfo &outInfo );

	void SetBackgroundColor( const FColor& newColor ) { mBackgroundColor = newColor; }

	void SetWorldMatrix( const Matrix4& matrix );
	void SetViewMatrix( const Matrix4& matrix );
	void SetProjectionMatrix( const Matrix4& matrix );


	void SetMaterial( const rrMaterial& material );

	enum { MAX_LIGHTS = 8 };

	void SetParallelLight( UINT index, const rrLight& light );
	void SetPointLight( UINT index, const rrLight& light );
	void SetSpotLight( UINT index, const rrLight& light );

	void ToggleLighting( bool enable );
	void ToggleLight( UINT index, bool enable );

private:
	bool LoadOpenGLExtensions();
	void SetDefaultRenderStates();
	void ResetMatrices();

	bool IsInitialized() const
	{
		return (mHWnd != nil) && mWindowDC && mRenderContext;
	}

public:

	Matrix4		mWorldMatrix;
	Matrix4		mViewMatrix;
	Matrix4		mProjMatrix;

	rrFont2D		mFont2D;
	//rrFont3D		mFont3D;

	rxViewport		mViewport;	// updated in BeginScene()

	FColor			mBackgroundColor;

	HWND				mHWnd;
	HDC					mWindowDC;		// device context
	HGLRC				mRenderContext;	// GL context
};

//---------------------------------------------------------------------------------------------------

void DrawText3D( float xPos, float yPos, float zPos, const char *str, int length );


void rrSetColor( const ColorRGB& color );
void rrSetColor( const FColor& color );

void rrSetAmbientLight( const ColorRGB& color );
void rrDrawPoint( const Vec3D& position, FLOAT size );
void rrDrawLine( const Vec3D& start, const Vec3D& end, FLOAT size );
void rrDrawCoordSystem();

void glutWireCube( double dSize );
void glutSolidCube( double dSize );

// draw a sphere of radius 1
void rrDrawUnitSphere( int sphere_quality = 1 );

// draws a box given its half extent
void rrDrawBox( float lx=0.5f, float ly=0.5f, float lz=0.5f );

inline
void rrFormatInt(
	ANSICHAR* buffer, SizeT bufferSize,
	const char* msg,
	int number
	)
{
	mxSPrintfAnsi( buffer, bufferSize,
		"%s%d", msg, number );
};

inline
void rrFormatFloat(
	ANSICHAR* buffer, SizeT bufferSize,
	const char* msg,
	float f
	)
{
	mxSPrintfAnsi( buffer, bufferSize,
		"%s%.3f", msg, f );
};

inline
void rrFormat(
	ANSICHAR* buffer, SizeT bufferSize,
	const char* msg,
	const Vec2D& vector
	)
{
	mxSPrintfAnsi( buffer, bufferSize,
		"%s%.3f, %.3f", msg, vector.x, vector.y );
};

inline
void rrFormat(
	ANSICHAR* buffer, SizeT bufferSize,
	const char* msg,
	const Vec3D& vector
	)
{
	mxSPrintfAnsi( buffer, bufferSize,
		"%s%.3f, %.3f, %.3f", msg, vector.x, vector.y, vector.z );
};

inline
void rrFormat(
	ANSICHAR* buffer, SizeT bufferSize,
	const char* msg,
	const Matrix4& m
	)
{
	mxSPrintfAnsi( buffer, bufferSize,
		"%s"
		"%.6f, %.6f, %.6f, %.6f, "
		"%.6f, %.6f, %.6f, %.6f, "
		"%.6f, %.6f, %.6f, %.6f, "
		"%.6f, %.6f, %.6f, %.6f",
		msg,
		m[0][0], m[0][1], m[0][2], m[0][3],
		m[1][0], m[1][1], m[1][2], m[1][3],
		m[2][0], m[2][1], m[2][2], m[2][3],
		m[3][0], m[3][1], m[3][2], m[3][3]
	);
};


// use this for TFixedString<LENGTH>
#define RR_FORMAT_FIXED_STRING( fixedString, msg, fmt ) \
	rrFormat( fixedString.Ptr(), fixedString.GetCapacity(), msg, fmt )

#define RR_FORMAT_FIXED_STRING_INT( fixedString, msg, fmt ) \
	rrFormatInt( fixedString.Ptr(), fixedString.GetCapacity(), msg, fmt )

#define RR_FORMAT_FIXED_STRING_FLOAT( fixedString, msg, fmt ) \
	rrFormatFloat( fixedString.Ptr(), fixedString.GetCapacity(), msg, fmt )

MX_NAMESPACE_END


#include "OpenGL_PCH.h"
#pragma hdrstop
#include <Graphics/OpenGL.h>

MX_NAMESPACE_BEGIN

//---------------------------------------------------------------------------------------------------

rrFont2D::rrFont2D()
{
	m_fontListBase = 0;
}

rrFont2D::~rrFont2D()
{
	Close();
}

bool rrFont2D::Setup( HDC hDC )
{
	const TCHAR *fontName =
	//	TEXT("Courier")
		TEXT("Verdana")
		;
	m_fontListBase = CreateBitmapFont( fontName, 16, hDC );
	if( !m_fontListBase ) {
		return false;
	}
	return true;
}

void rrFont2D::Close()
{
	ReleaseFont(m_fontListBase);
}

UINT rrFont2D::CreateBitmapFont( const TCHAR *fontName, int fontHeight, HDC hDC )
{
	HFONT hFont;     // the windows font
	UINT base;

	base = glGenLists(96);

	int fontWeight = FW_NORMAL;//can be FW_BOLD

	DWORD fontQuality = DEFAULT_QUALITY;//ANTIALIASED_QUALITY;

	if (!mxStrCmp(fontName, TEXT("symbol")))
	{
		hFont = ::CreateFont(fontHeight, 0, 0, 0, fontWeight, FALSE, FALSE, FALSE,
							SYMBOL_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS,
							fontQuality, FF_DONTCARE | DEFAULT_PITCH,
							fontName);
	}
	else
	{
		hFont = ::CreateFont(fontHeight, 0, 0, 0, fontWeight, FALSE, FALSE, FALSE,
							ANSI_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS,
							fontQuality, FF_DONTCARE | DEFAULT_PITCH,
							fontName);
	}

	if (!hFont)
		return 0;

	::SelectObject(hDC, hFont);
	::wglUseFontBitmaps( hDC, 32, 96, base );

	return base;
}

void rrFont2D::ReleaseFont(UINT base)
{
	if (base != 0)
		glDeleteLists( base, 96 );
}

void rrFont2D::Draw( int xPos, int yPos, const char *str, int length )
{
	Assert(m_fontListBase);AssertPtr(str);Assert(length>0);

	glRasterPos2i( xPos, yPos );

	glPushAttrib(GL_LIST_BIT);
		glListBase(m_fontListBase - 32);
		glCallLists(length, GL_UNSIGNED_BYTE, str);
	glPopAttrib();
}

//---------------------------------------------------------------------------------------------------

rrFont3D::rrFont3D()
{
	ZERO_OUT(gmf);
	m_fontListBase = 0;
}

rrFont3D::~rrFont3D()
{
	Close();
}

bool rrFont3D::Setup( HDC hDC )
{
	const TCHAR *fontName =
		TEXT("Courier")
	//	TEXT("Verdana")
		;
	m_fontListBase = CreateOutlineFont( fontName, 20, 0.2f, hDC );
	if( !m_fontListBase ) {
		return false;
	}
	return true;
}

void rrFont3D::Close()
{
	ReleaseFont(m_fontListBase);
}
UINT rrFont3D::CreateOutlineFont( const TCHAR *fontName, int fontHeight, float depth, HDC hDC )
{
	HFONT hFont;     // the windows font
	UINT base;

	base = glGenLists(256);

	int fontWeight = FW_NORMAL;

	if (!mxStrCmp(fontName, TEXT("symbol")))
	{
		hFont = ::CreateFont(fontHeight, 0, 0, 0, fontWeight, FALSE, FALSE, FALSE,
							SYMBOL_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS,
							ANTIALIASED_QUALITY, FF_DONTCARE | DEFAULT_PITCH,
							fontName);
	}
	else
	{
		hFont = ::CreateFont(fontHeight, 0, 0, 0, fontWeight, FALSE, FALSE, FALSE,
							ANSI_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS,
							ANTIALIASED_QUALITY, FF_DONTCARE | DEFAULT_PITCH,
							fontName);
	}

	if (!hFont)
		return 0;

	::SelectObject( hDC, hFont );
	::wglUseFontOutlines( hDC, 0, 255, base, 0.0f, depth, WGL_FONT_POLYGONS, gmf );

	return base;
}

void rrFont3D::ReleaseFont(UINT base)
{
	if( base != 0 ) {
		glDeleteLists( base, 256 );
	}
}

void rrFont3D::Draw( float xPos, float yPos, float zPos, const char *str, int length )
{
	Assert(m_fontListBase);AssertPtr(str);Assert(length>0);
/*
	float text_length = 0.0;

	// center the text
	for (int idx = 0; idx < length; idx++)    // find length of text
	{
		text_length += gmf[str[idx]].gmfCellIncX; // increase length by character's width
	}

	glTranslatef(-text_length/2.0f, yPos, zPos);
	glRotatef(m_angle, 1.0, 0.0, 0.0);
	glRotatef(m_angle, 0.0, 1.0, 0.0);
	glRotatef(m_angle, 0.0, 0.0, 1.0);
*/
	//Matrix4 xform;
	//xform.BuildTransform( Vec3D(xPos,yPos,zPos), Quat::quat_identity );
	//Raster::Get().SetWorldMatrix(xform);

	glPushAttrib(GL_LIST_BIT);
		glListBase(m_fontListBase);
		glCallLists(length, GL_UNSIGNED_BYTE, str);
	glPopAttrib();
}

//---------------------------------------------------------------------------------------------------

Raster	Raster::G;

Raster::Raster()
{
	ENSURE_ONLY_ONE_CALL;

	mWorldMatrix.SetIdentity();
	mViewMatrix.SetIdentity();
	mProjMatrix.SetIdentity();

	ZERO_OUT(mViewport);

	mBackgroundColor = FColor::BLACK;

	mWindowDC = nil;
	mRenderContext = nil;
}

Raster::~Raster()
{
	Close();
}

bool Raster::Setup( HWND window )
{
	AssertPtr(window);
	Assert( !IsInitialized() );

	BOOL bOk = false;

	mHWnd = window;

	HWND	windowHandle = window;

	//window device context
	mWindowDC = ::GetDC( windowHandle );

	// Select the format for the drawing surface.
	PIXELFORMATDESCRIPTOR pfd = 
	{ 
		sizeof(PIXELFORMATDESCRIPTOR),   // size of this pfd 
		1,                     // version number 
		PFD_DRAW_TO_WINDOW |   // support window 
		PFD_SUPPORT_OPENGL |   // support OpenGL 
		PFD_DOUBLEBUFFER,      // double buffered 
		PFD_TYPE_RGBA,         // RGBA type 
		32,                    // 32-bit color depth 
		0, 0, 0, 0, 0, 0,      // color bits ignored 
		8,                     // no alpha buffer 
		0,                     // shift bit ignored 
		0,                     // no accumulation buffer 
		0, 0, 0, 0,            // accum bits ignored 
		24,                    // 24-bit z-buffer
		8,                     // stencil buffer
		0,                     // no auxiliary buffer
		PFD_MAIN_PLANE,        // main layer 
		0,                     // reserved 
		0, 0, 0                // layer masks ignored 
	};

	// tell windows to choose a pixel format based on what we want
	int pf = ::ChoosePixelFormat( mWindowDC, &pfd );

	// now set the pixel format
	if( ::SetPixelFormat( mWindowDC, pf, &pfd ) == FALSE ) {
		mxErr("Failed to set pixel format\n");
		Close();
		return false;
	}

	// describe the pixelformat for our device context
	if( !::DescribePixelFormat( mWindowDC, pf, sizeof(PIXELFORMATDESCRIPTOR), &pfd ) )
	{
		mxErr("DescribePixelFormat failed\n");
		Close();
		return false;
	}

	mRenderContext = wglCreateContext( mWindowDC );
	if( !mRenderContext ) {
		mxErr("wglCreateContext failed\n");
		Close();
		return false;
	}

	bOk = wglMakeCurrent( mWindowDC, mRenderContext );
    if( !bOk ) {
        mxErr("wglMakeCurrent failed\n");
		Close();
		return false;
    }

	if( !LoadOpenGLExtensions() ) {
		Close();
		return false;
	}

	if( !mFont2D.Setup( mWindowDC ) ) {
		mxErr("Failed to create 2D font\n");
		Close();
		return false;
	}
/*
	if( !mFont3D.Setup( mWindowDC ) ) {
		mxErr("Failed to create 3D font\n");
		Close();
		return false;
	}
*/

	{
		mxPutf( "OpenGL version: %s\n", glGetString( GL_VERSION ) );
		mxPutf( "OpenGL renderer: %s\n", glGetString( GL_RENDERER ) );
		mxPutf( "Device vendor: %s\n", glGetString( GL_VENDOR ) );

		/*
		char	tempbuf[4096*2];
		MX_SPRINTF_ANSI( tempbuf, "OpenGL extensions:\n%s\n", glGetString( GL_EXTENSIONS ) );
		mxPut( tempbuf );
		*/
	}

	mxInitializeBase();

	Assert( IsInitialized() );
	return true;
}

void Raster::Close()
{
	if( IsInitialized() )
	{
		mFont2D.Close();
		//mFont3D.Close();
		
		if( mRenderContext ) {
			wglDeleteContext( mRenderContext );
			mRenderContext = nil;
		}
		if( mWindowDC && (mHWnd != nil) ) {
			::ReleaseDC( mHWnd, mWindowDC );
			mWindowDC = nil;
		}

		//if( fullscreen )
		//{
		//	::ChangeDisplaySettings( NULL, 0 );	// switch back to the desktop
		//	::ShowCursor( TRUE );				// show mouse pointer
		//}
		mHWnd = nil;

		mxShutdownBase();
	}
}

bool Raster::LoadOpenGLExtensions()
{
	return true;
}

void Raster::SetDefaultRenderStates()
{
	glFrontFace( GL_CW );
	glEnable( GL_CULL_FACE );
	glCullFace( GL_BACK );

	glEnable( GL_DEPTH_TEST );
	glDepthMask( GL_TRUE );
	glDepthFunc( GL_LESS );
	glDepthRange( 0.0, 1.0 );
	glClearDepth( 1.0 );
	
	glPolygonMode( GL_FRONT, GL_FILL );

	glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );

	glDisable( GL_SCISSOR_TEST );

	glDisable( GL_LIGHTING );
	for( UINT iLight = 0; iLight < MAX_LIGHTS; iLight++ ) {
		glDisable( GL_LIGHT0 + iLight );
	}
	glShadeModel( GL_FLAT );//glShadeModel( GL_SMOOTH );

	glDisable( GL_COLOR_MATERIAL );
//	glEnable( GL_COLOR_MATERIAL );
//	glColorMaterial( GL_FRONT, GL_AMBIENT_AND_DIFFUSE );

	glLightModeli( GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE );
	glLightModeli( GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE );

	glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );

	glDisable( GL_TEXTURE_2D );
	glDisable( GL_BLEND );
	glDisable( GL_ALPHA_TEST );
	glBlendFunc( GL_ONE, GL_ONE_MINUS_SRC_COLOR );

	glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );
	glHint( GL_LINE_SMOOTH_HINT, GL_NICEST );
	glHint( GL_POINT_SMOOTH_HINT, GL_NICEST );

	ResetMatrices();
}

void Raster::ResetMatrices()
{
	mWorldMatrix = Matrix4::mat4_identity;
	mViewMatrix = Matrix4::mat4_identity;
	mProjMatrix = Matrix4::mat4_identity;
	
	SetWorldMatrix( Matrix4::mat4_identity );
	SetViewMatrix( Matrix4::mat4_identity );
	SetProjectionMatrix( Matrix4::mat4_identity );
}

static void GetViewport( const mxWindow* window, rxViewport & viewport )
{
	AssertPtr(window);

	UINT left, top, right, bottom;
	window->GetPosition( left, top, right, bottom );

	viewport.x = left;
	viewport.y = top;
	viewport.width = right - left;
	viewport.height = bottom - top;
}

/*
void Raster::GetInfo( rxRendererInfo &outInfo )
{
	rxViewport	viewport;
	GetViewport( mHWnd, viewport );
	
	outInfo.displayMode.screen.width = viewport.width;
	outInfo.displayMode.screen.height = viewport.height;

	MX_UNDONE
	outInfo.displayMode.screen.isFullscreen = false;

	char buffer[MAX_STRING_CHARS];
	mxSPrintfAnsi( buffer, ARRAY_SIZE(buffer), (const char*)glGetString( GL_RENDERER ) );
	MX_ANSI_TO_UNICODE( outInfo.deviceInfo.description, buffer );

	Print( "Extensions:\n%s", glGetString( GL_EXTENSIONS ) );

	outInfo.deviceInfo.vendor = EDeviceVendor::Vendor_Unknown;
	MX_ANSI_TO_UNICODE( outInfo.deviceInfo.vendorName, (const char*)glGetString( GL_VENDOR ) );

	mxSPrintfAnsi( buffer, ARRAY_SIZE(buffer), "OpenGL %s", glGetString( GL_VERSION ) );
	MX_ANSI_TO_UNICODE( outInfo.deviceInfo.driver, buffer );

	ZERO_OUT(outInfo.deviceInfo.caps);

	outInfo.driverType = EDriverType::GAPI_OpenGL;
	outInfo.pWindowHandle = mHWnd->GetHandle();
}
*/
void Raster::BeginScene( const rxSceneView& view )
{
	Assert( IsInitialized() );

	// Setup the viewport.
	GetViewport( mHWnd, mViewport );
	glViewport( (GLint)mViewport.x, (GLint)mViewport.y, (GLsizei)mViewport.width, (GLsizei)mViewport.height );

	// Clear the screen.
	glClearColor( mBackgroundColor.R, mBackgroundColor.G, mBackgroundColor.B, mBackgroundColor.A );

	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );

	glColorMask( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );

	glEnable( GL_DEPTH_TEST );
	glDepthMask( GL_TRUE );
	glDepthFunc( GL_LESS );

	glFrontFace( GL_CW );
	glEnable( GL_CULL_FACE );
	glCullFace( GL_BACK );

	glPolygonMode( GL_FRONT, GL_FILL );

	ResetMatrices();


	const FLOAT aspect = (view.aspectRatio != 0.0f) ?
						view.aspectRatio : mViewport.GetAspectRatio();

	Matrix4  projectionMatrix;
	projectionMatrix.BuildPerspectiveLH(
		view.fovY,
		aspect,
		view.nearZ,
		view.farZ
	);
	SetProjectionMatrix( projectionMatrix );

	Matrix4  viewMatrix;
	viewMatrix.BuildLookAtLH(
		view.origin,
		view.right,
		view.up,
		view.look
	);
	SetViewMatrix( viewMatrix );
}

void Raster::EndScene()
{
	// Flush drawing commands.
	// Docs say:
	// glFinish() has the same effect as glFlush(),
	// with the addition that glFinish() will block until all commands submitted
	// have been executed.

	// Swap functions (used in double-buffered applications) automatically flush the commands,
	// so no need to call glFlush.

//	glFlush();
	glFinish();

	// Swap the backbuffer into the front buffer.
	//::SwapBuffers(wglGetCurrentDC());
	const BOOL bOk = ::SwapBuffers( mWindowDC );
	if( !bOk ) {
		mxWarn("SwapBuffers() failed.\n");
	}
}

void Raster::SetWorldMatrix( const Matrix4& matrix )
{
	mWorldMatrix = matrix;
	glMatrixMode( GL_MODELVIEW );
	
	GLfloat glmat[16];
	mul_mat4_mat4( mViewMatrix.ToFloatPtr(), mWorldMatrix.ToFloatPtr(), glmat );
	glLoadMatrixf( glmat );
}

void Raster::SetViewMatrix( const Matrix4& matrix )
{
	mViewMatrix = matrix;
	glMatrixMode( GL_MODELVIEW );
	
	GLfloat glmat[16];
	mul_mat4_mat4( mViewMatrix.ToFloatPtr(), mWorldMatrix.ToFloatPtr(), glmat );
	glLoadMatrixf( glmat );
}

void Raster::SetProjectionMatrix( const Matrix4& matrix )
{
	mProjMatrix = matrix;

	GLfloat glmat[16];
	MemCopy16( glmat, matrix.ToFloatPtr(), sizeof(glmat) );
	// Flip Z axis because OpenGL uses a right-hand coordinate system.
	glmat[12] *= -1.0f;
	glMatrixMode( GL_PROJECTION );
	glLoadMatrixf( glmat );
}

void Raster::SetMaterial( const rrMaterial& material )
{
	glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT  , (const GLfloat*) material.ambient.ToFloatPtr() );
	glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE  , (const GLfloat*) material.diffuse.ToFloatPtr() );
	glMaterialfv( GL_FRONT_AND_BACK, GL_EMISSION , (const GLfloat*) material.emissive.ToFloatPtr() );
	glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR , (const GLfloat*) material.specular.ToFloatPtr() );
	glMaterialf ( GL_FRONT_AND_BACK, GL_SHININESS, material.shininess );
}

static 
void SetLightColorAndAttenuation( UINT index, const rrLight& light )
{
	glLightfv(index, GL_DIFFUSE, (const FLOAT*)light.diffuse.ToFloatPtr());
	glLightfv(index, GL_SPECULAR, (const FLOAT*)light.specular.ToFloatPtr());
	glLightfv(index, GL_AMBIENT, (const FLOAT*)light.ambient.ToFloatPtr());

	// attenuationFactor = 1.0f / ( constant + linear * d + quadratic*(d*d) ), where d-distance
	glLightf(index, GL_CONSTANT_ATTENUATION, light.attenuation[0]);
	glLightf(index, GL_LINEAR_ATTENUATION, light.attenuation[1]);
	glLightf(index, GL_QUADRATIC_ATTENUATION, light.attenuation[2]);
}

void Raster::SetParallelLight( UINT index, const rrLight& light )
{
	Assert(IsInitialized());
	
	SetWorldMatrix(Matrix4::mat4_identity);
	
	Assert(index>=0&&index<=MAX_LIGHTS);
	index += GL_LIGHT0;

	GLfloat data[4];
	// set direction
	data[0] = -light.direction.x;
	data[1] = -light.direction.y;
	data[2] = -light.direction.z;
	data[3] = 0.0f; // 0.0f for directional light
	glLightfv( index, GL_POSITION, data );

	glLightf(index, GL_SPOT_EXPONENT, 0.0f);
	glLightf(index, GL_SPOT_CUTOFF, 180.0f);

	SetLightColorAndAttenuation(index,light);
	glEnable(index);
}

void Raster::SetPointLight( UINT index, const rrLight& light )
{
	Assert(IsInitialized());
	
	SetWorldMatrix(Matrix4::mat4_identity);

	Assert(index>=0&&index<=MAX_LIGHTS);
	index += GL_LIGHT0;

	GLfloat data[4];
	// set position
	data[0] = light.position.x;
	data[1] = light.position.y;
	data[2] = light.position.z;
	data[3] = 1.0f; // 1.0f for local light
	glLightfv( index, GL_POSITION, data );

	glLightf(index, GL_SPOT_EXPONENT, 0.0f);
	glLightf(index, GL_SPOT_CUTOFF, 180.0f);

	SetLightColorAndAttenuation(index,light);
	glEnable(index);
}

void Raster::SetSpotLight( UINT index, const rrLight& light )
{
	Assert(IsInitialized());

	SetWorldMatrix(Matrix4::mat4_identity);

	Assert(index>=0&&index<=MAX_LIGHTS);
	index += GL_LIGHT0;

	GLfloat data[4];
	data[0] = light.direction.x;
	data[1] = light.direction.y;
	data[2] = light.direction.z;
	data[3] = 0.0f;
	glLightfv(index, GL_SPOT_DIRECTION, data);

	// set position
	data[0] = light.position.x;
	data[1] = light.position.y;
	data[2] = light.position.z;
	data[3] = 1.0f; // 1.0f for local light
	glLightfv( index, GL_POSITION, data );

	glLightf(index, GL_SPOT_EXPONENT, light.fallOff);
	glLightf(index, GL_SPOT_CUTOFF, light.outerCone);

	SetLightColorAndAttenuation(index,light);
	glEnable(index);
}

void Raster::ToggleLighting( bool enable )
{
	if( enable ) {
		glEnable(GL_LIGHTING);
	} else {
		glDisable(GL_LIGHTING);
	}
}

void Raster::ToggleLight( UINT index, bool enable )
{
	Assert(index>=0&&index<=MAX_LIGHTS);
	index += GL_LIGHT0;
	if( enable ) {
		glEnable(index);
	} else {
		glDisable(index);
	}
}

//---------------------------------------------------------------------------------------------------
/*
 * Draws a wireframed cube. Code contributed by Andreas Umbach <marvin@dataway.ch>
 */
void glutWireCube( double dSize )
{
    double size = dSize * 0.5;

#   define V(a,b,c) glVertex3d( a size, b size, c size );
#   define N(a,b,c) glNormal3d( a, b, c );

    /* PWO: I dared to convert the code to use macros... */
    glBegin( GL_LINE_LOOP ); N( 1.0, 0.0, 0.0); V(+,-,+); V(+,-,-); V(+,+,-); V(+,+,+); glEnd();
    glBegin( GL_LINE_LOOP ); N( 0.0, 1.0, 0.0); V(+,+,+); V(+,+,-); V(-,+,-); V(-,+,+); glEnd();
    glBegin( GL_LINE_LOOP ); N( 0.0, 0.0, 1.0); V(+,+,+); V(-,+,+); V(-,-,+); V(+,-,+); glEnd();
    glBegin( GL_LINE_LOOP ); N(-1.0, 0.0, 0.0); V(-,-,+); V(-,+,+); V(-,+,-); V(-,-,-); glEnd();
    glBegin( GL_LINE_LOOP ); N( 0.0,-1.0, 0.0); V(-,-,+); V(-,-,-); V(+,-,-); V(+,-,+); glEnd();
    glBegin( GL_LINE_LOOP ); N( 0.0, 0.0,-1.0); V(-,-,-); V(-,+,-); V(+,+,-); V(+,-,-); glEnd();

#   undef V
#   undef N
}

/*
 * Draws a solid cube. Code contributed by Andreas Umbach <marvin@dataway.ch>
 */
void glutSolidCube( double dSize )
{
    double size = dSize * 0.5;

#   define V(a,b,c) glVertex3d( a size, b size, c size );
#   define N(a,b,c) glNormal3d( a, b, c );

    /* PWO: Again, I dared to convert the code to use macros... */
    glBegin( GL_QUADS );
        N( 1.0, 0.0, 0.0); V(+,-,+); V(+,-,-); V(+,+,-); V(+,+,+);
        N( 0.0, 1.0, 0.0); V(+,+,+); V(+,+,-); V(-,+,-); V(-,+,+);
        N( 0.0, 0.0, 1.0); V(+,+,+); V(-,+,+); V(-,-,+); V(+,-,+);
        N(-1.0, 0.0, 0.0); V(-,-,+); V(-,+,+); V(-,+,-); V(-,-,-);
        N( 0.0,-1.0, 0.0); V(-,-,+); V(-,-,-); V(+,-,-); V(+,-,+);
        N( 0.0, 0.0,-1.0); V(-,-,-); V(-,+,-); V(+,+,-); V(+,-,-);
    glEnd();

#   undef V
#   undef N
}

//---------------------------------------------------------------------------------------------------

static void drawConvex (float *_planes,unsigned int _planecount,
			float *_points,
			unsigned int _pointcount,
			unsigned int *_polygons)
{
  unsigned int polyindex=0;
  for(unsigned int i=0;i<_planecount;++i)
    {
      unsigned int pointcount=_polygons[polyindex];
      polyindex++;
      glBegin (GL_POLYGON);      
       glNormal3f(_planes[(i*4)+0],
		  _planes[(i*4)+1],
		  _planes[(i*4)+2]);
      for(unsigned int j=0;j<pointcount;++j)
	{
	  glVertex3f(_points[_polygons[polyindex]*3],
		     _points[(_polygons[polyindex]*3)+1],
		     _points[(_polygons[polyindex]*3)+2]);
	  polyindex++;
	}
      glEnd();
    }
}

// This is recursively subdivides a triangular area (vertices p1,p2,p3) into
// smaller triangles, and then draws the triangles. All triangle vertices are
// normalized to a distance of 1.0 from the origin (p1,p2,p3 are assumed
// to be already normalized). Note this is not super-fast because it draws
// triangles rather than triangle strips.

static void drawPatch (float p1[3], float p2[3], float p3[3], int level)
{
  int i;
  if (level > 0) {
    float q1[3],q2[3],q3[3];		 // sub-vertices
    for (i=0; i<3; i++) {
      q1[i] = 0.5f*(p1[i]+p2[i]);
      q2[i] = 0.5f*(p2[i]+p3[i]);
      q3[i] = 0.5f*(p3[i]+p1[i]);
    }
    float length1 = (float)(1.0/sqrt(q1[0]*q1[0]+q1[1]*q1[1]+q1[2]*q1[2]));
    float length2 = (float)(1.0/sqrt(q2[0]*q2[0]+q2[1]*q2[1]+q2[2]*q2[2]));
    float length3 = (float)(1.0/sqrt(q3[0]*q3[0]+q3[1]*q3[1]+q3[2]*q3[2]));
    for (i=0; i<3; i++) {
      q1[i] *= length1;
      q2[i] *= length2;
      q3[i] *= length3;
    }
    drawPatch (p1,q1,q3,level-1);
    drawPatch (q1,p2,q2,level-1);
    drawPatch (q1,q2,q3,level-1);
    drawPatch (q3,q2,p3,level-1);
  }
  else {
    glNormal3f (p1[0],p1[1],p1[2]);
    glVertex3f (p1[0],p1[1],p1[2]);
    glNormal3f (p2[0],p2[1],p2[2]);
    glVertex3f (p2[0],p2[1],p2[2]);
    glNormal3f (p3[0],p3[1],p3[2]);
    glVertex3f (p3[0],p3[1],p3[2]);
  }
}


// draw a sphere of radius 1

void rrDrawUnitSphere( int sphere_quality /*= 1*/ )
{
  // icosahedron data for an icosahedron of radius 1.0
# define ICX 0.525731112119133606f
# define ICZ 0.850650808352039932f
  static GLfloat idata[12][3] = {
    {-ICX, 0, ICZ},
    {ICX, 0, ICZ},
    {-ICX, 0, -ICZ},
    {ICX, 0, -ICZ},
    {0, ICZ, ICX},
    {0, ICZ, -ICX},
    {0, -ICZ, ICX},
    {0, -ICZ, -ICX},
    {ICZ, ICX, 0},
    {-ICZ, ICX, 0},
    {ICZ, -ICX, 0},
    {-ICZ, -ICX, 0}
  };

  static int index[20][3] = {
    {0, 4, 1},	  {0, 9, 4},
    {9, 5, 4},	  {4, 5, 8},
    {4, 8, 1},	  {8, 10, 1},
    {8, 3, 10},   {5, 3, 8},
    {5, 2, 3},	  {2, 7, 3},
    {7, 10, 3},   {7, 6, 10},
    {7, 11, 6},   {11, 0, 6},
    {0, 1, 6},	  {6, 1, 10},
    {9, 0, 11},   {9, 11, 2},
    {9, 2, 5},	  {7, 2, 11},
  };

  static GLuint listnum = 0;
  if (listnum==0) {
    listnum = glGenLists (1);
    glNewList (listnum,GL_COMPILE);
    glBegin (GL_TRIANGLES);
    for (int i=0; i<20; i++) {
      drawPatch (&idata[index[i][2]][0],&idata[index[i][1]][0],
		 &idata[index[i][0]][0],sphere_quality);
    }
    glEnd();
    glEndList();
  }
  glCallList (listnum);
}

// draw a capped cylinder of length l and radius r, aligned along the x axis

static int capped_cylinder_quality = 3;

static void drawCapsule (float l, float r)
{
  int i,j;
  float tmp,nx,ny,nz,start_nx,start_ny,a,ca,sa;
  // number of sides to the cylinder (divisible by 4):
  const int n = capped_cylinder_quality*4;

  l *= 0.5;
  a = float(MX_PI*2.0)/float(n);
  sa = (float) sin(a);
  ca = (float) cos(a);

  // draw cylinder body
  ny=1; nz=0;		  // normal vector = (0,ny,nz)
  glBegin (GL_TRIANGLE_STRIP);
  for (i=0; i<=n; i++) {
    glNormal3d (ny,nz,0);
    glVertex3d (ny*r,nz*r,l);
    glNormal3d (ny,nz,0);
    glVertex3d (ny*r,nz*r,-l);
    // rotate ny,nz
    tmp = ca*ny - sa*nz;
    nz = sa*ny + ca*nz;
    ny = tmp;
  }
  glEnd();

  // draw first cylinder cap
  start_nx = 0;
  start_ny = 1;
  for (j=0; j<(n/4); j++) {
    // get start_n2 = rotated start_n
    float start_nx2 =  ca*start_nx + sa*start_ny;
    float start_ny2 = -sa*start_nx + ca*start_ny;
    // get n=start_n and n2=start_n2
    nx = start_nx; ny = start_ny; nz = 0;
    float nx2 = start_nx2, ny2 = start_ny2, nz2 = 0;
    glBegin (GL_TRIANGLE_STRIP);
    for (i=0; i<=n; i++) {
      glNormal3d (ny2,nz2,nx2);
      glVertex3d (ny2*r,nz2*r,l+nx2*r);
      glNormal3d (ny,nz,nx);
      glVertex3d (ny*r,nz*r,l+nx*r);
      // rotate n,n2
      tmp = ca*ny - sa*nz;
      nz = sa*ny + ca*nz;
      ny = tmp;
      tmp = ca*ny2- sa*nz2;
      nz2 = sa*ny2 + ca*nz2;
      ny2 = tmp;
    }
    glEnd();
    start_nx = start_nx2;
    start_ny = start_ny2;
  }

  // draw second cylinder cap
  start_nx = 0;
  start_ny = 1;
  for (j=0; j<(n/4); j++) {
    // get start_n2 = rotated start_n
    float start_nx2 = ca*start_nx - sa*start_ny;
    float start_ny2 = sa*start_nx + ca*start_ny;
    // get n=start_n and n2=start_n2
    nx = start_nx; ny = start_ny; nz = 0;
    float nx2 = start_nx2, ny2 = start_ny2, nz2 = 0;
    glBegin (GL_TRIANGLE_STRIP);
    for (i=0; i<=n; i++) {
      glNormal3d (ny,nz,nx);
      glVertex3d (ny*r,nz*r,-l+nx*r);
      glNormal3d (ny2,nz2,nx2);
      glVertex3d (ny2*r,nz2*r,-l+nx2*r);
      // rotate n,n2
      tmp = ca*ny - sa*nz;
      nz = sa*ny + ca*nz;
      ny = tmp;
      tmp = ca*ny2- sa*nz2;
      nz2 = sa*ny2 + ca*nz2;
      ny2 = tmp;
    }
    glEnd();
    start_nx = start_nx2;
    start_ny = start_ny2;
  }
}

/*
// draw a cylinder of length l and radius r, aligned along the z axis

static void drawCylinder (float l, float r, float zoffset)
{
  int i;
  float tmp,ny,nz,a,ca,sa;
  const int n = 24;	// number of sides to the cylinder (divisible by 4)

  l *= 0.5;
  a = float(M_PI*2.0)/float(n);
  sa = (float) sin(a);
  ca = (float) cos(a);

  // draw cylinder body
  ny=1; nz=0;		  // normal vector = (0,ny,nz)
  glBegin (GL_TRIANGLE_STRIP);
  for (i=0; i<=n; i++) {
    glNormal3d (ny,nz,0);
    glVertex3d (ny*r,nz*r,l+zoffset);
    glNormal3d (ny,nz,0);
    glVertex3d (ny*r,nz*r,-l+zoffset);
    // rotate ny,nz
    tmp = ca*ny - sa*nz;
    nz = sa*ny + ca*nz;
    ny = tmp;
  }
  glEnd();

  // draw top cap
  glShadeModel (GL_FLAT);
  ny=1; nz=0;		  // normal vector = (0,ny,nz)
  glBegin (GL_TRIANGLE_FAN);
  glNormal3d (0,0,1);
  glVertex3d (0,0,l+zoffset);
  for (i=0; i<=n; i++) {
    if (i==1 || i==n/2+1)
      setColor (color[0]*0.75f,color[1]*0.75f,color[2]*0.75f,color[3]);
    glNormal3d (0,0,1);
    glVertex3d (ny*r,nz*r,l+zoffset);
    if (i==1 || i==n/2+1)
      setColor (color[0],color[1],color[2],color[3]);

    // rotate ny,nz
    tmp = ca*ny - sa*nz;
    nz = sa*ny + ca*nz;
    ny = tmp;
  }
  glEnd();

  // draw bottom cap
  ny=1; nz=0;		  // normal vector = (0,ny,nz)
  glBegin (GL_TRIANGLE_FAN);
  glNormal3d (0,0,-1);
  glVertex3d (0,0,-l+zoffset);
  for (i=0; i<=n; i++) {
    if (i==1 || i==n/2+1)
      setColor (color[0]*0.75f,color[1]*0.75f,color[2]*0.75f,color[3]);
    glNormal3d (0,0,-1);
    glVertex3d (ny*r,nz*r,-l+zoffset);
    if (i==1 || i==n/2+1)
      setColor (color[0],color[1],color[2],color[3]);

    // rotate ny,nz
    tmp = ca*ny + sa*nz;
    nz = -sa*ny + ca*nz;
    ny = tmp;
  }
  glEnd();
}
/*
static void drawTriangle (const float *v0, const float *v1, const float *v2, int solid)
{
  float u[3],v[3],normal[3];
  u[0] = v1[0] - v0[0];
  u[1] = v1[1] - v0[1];
  u[2] = v1[2] - v0[2];
  v[0] = v2[0] - v0[0];
  v[1] = v2[1] - v0[1];
  v[2] = v2[2] - v0[2];
  dCROSS (normal,=,u,v);
  normalizeVector3 (normal);

  glBegin(solid ? GL_TRIANGLES : GL_LINE_STRIP);
  glNormal3fv (normal);
  glVertex3fv (v0);
  glVertex3fv (v1);
  glVertex3fv (v2);
  glEnd();
}
*/
/*
void rrDrawBox( const Vec3D& halfSize )
{
	static const int indices[36] = {
		0,1,2,
		3,2,1,
		4,0,6,
		6,0,2,
		5,1,4,
		4,1,0,
		7,3,1,
		7,1,5,
		5,4,7,
		7,4,6,
		7,2,3,
		7,6,2
	};

	Vec3D vertices[8]={	
		Vec3D(halfSize[0],halfSize[1],halfSize[2]),
		Vec3D(-halfSize[0],halfSize[1],halfSize[2]),
		Vec3D(halfSize[0],-halfSize[1],halfSize[2]),	
		Vec3D(-halfSize[0],-halfSize[1],halfSize[2]),	
		Vec3D(halfSize[0],halfSize[1],-halfSize[2]),
		Vec3D(-halfSize[0],halfSize[1],-halfSize[2]),	
		Vec3D(halfSize[0],-halfSize[1],-halfSize[2]),	
		Vec3D(-halfSize[0],-halfSize[1],-halfSize[2])
	};

	glBegin (GL_TRIANGLES);
	int si=36;
	for (int i=0;i<si;i+=3)
	{
		const Vec3D& v1 = vertices[indices[i]];;
		const Vec3D& v2 = vertices[indices[i+1]];
		const Vec3D& v3 = vertices[indices[i+2]];
		Vec3D normal = (v3-v1).Cross(v2-v1);
		normal.Normalize ();
		glNormal3f(normal.x,normal.y,normal.z);
		glVertex3f (v1.x, v1.y, v1.z);
		glVertex3f (v2.x, v2.y, v2.z);
		glVertex3f (v3.x, v3.y, v3.z);

	}
	glEnd();
}
*/
//---------------------------------------------------------------------------------------------------
// draws a box given its half extent
void rrDrawBox( float lx, float ly, float lz )
{
  // sides
  glBegin (GL_TRIANGLE_STRIP);
  glNormal3f (-1,0,0);
  glVertex3f (-lx,-ly,-lz);
  glVertex3f (-lx,-ly,lz);
  glVertex3f (-lx,ly,-lz);
  glVertex3f (-lx,ly,lz);
  glNormal3f (0,1,0);
  glVertex3f (lx,ly,-lz);
  glVertex3f (lx,ly,lz);
  glNormal3f (1,0,0);
  glVertex3f (lx,-ly,-lz);
  glVertex3f (lx,-ly,lz);
  glNormal3f (0,-1,0);
  glVertex3f (-lx,-ly,-lz);
  glVertex3f (-lx,-ly,lz);
  glEnd();

  // top face
  glBegin (GL_TRIANGLE_FAN);
  glNormal3f (0,0,1);
  glVertex3f (-lx,-ly,lz);
  glVertex3f (lx,-ly,lz);
  glVertex3f (lx,ly,lz);
  glVertex3f (-lx,ly,lz);
  glEnd();

  // bottom face
  glBegin (GL_TRIANGLE_FAN);
  glNormal3f (0,0,-1);
  glVertex3f (-lx,-ly,-lz);
  glVertex3f (-lx,ly,-lz);
  glVertex3f (lx,ly,-lz);
  glVertex3f (lx,-ly,-lz);
  glEnd();
}

//---------------------------------------------------------------------------------------------------

void DrawText3D( float xPos, float yPos, float zPos, const char *str, int length )
{
	Unimplemented;
	//Raster::G.mFont3D.Draw( xPos, yPos, zPos, str, length );
}


void rrMakeGLMatrix( GLfloat gl_matrix[16], const Matrix4& srcMatrix )
{
	MemCopy16( gl_matrix, srcMatrix.Transpose().ToFloatPtr(), 16 * sizeof(srcMatrix[0]) );
}

void rrSetColor( const ColorRGB& color )
{
	glColor3f( color.R, color.G, color.B );
}
void rrSetColor( const FColor& color )
{
	glColor3f( color.R, color.G, color.B );
}

void rrSetAmbientLight( const ColorRGB& color )
{
	glLightModelfv( GL_LIGHT_MODEL_AMBIENT, color.ToFloatPtr() );
}


void rrDrawPoint( const Vec3D& position, FLOAT size )
{
	glDisable( GL_TEXTURE_2D );
	glPointSize( size );
	glBegin( GL_POINTS );
	glVertex3fv( position.ToFloatPtr() );
	glEnd();
}


void rrDrawLine( const Vec3D& start, const Vec3D& end, FLOAT size )
{
	glDisable( GL_TEXTURE_2D );
	glLineWidth( size );
	glBegin( GL_LINES );
	glVertex3fv( start.ToFloatPtr() );
	glVertex3fv( end.ToFloatPtr() );
	glEnd();
}


void rrDrawCoordSystem()
{
	glBegin( GL_LINES );
	glColor3f(	1.f, 0.f, 0.f );
	glVertex3d(	0.f, 0.f, 0.f );
	glVertex3d(	1.f, 0.f, 0.f );

	glColor3f(	0.f, 1.f, 0.f );
	glVertex3d(	0.f, 0.f, 0.f );
	glVertex3d(	0.f, 1.f, 0.f );

	glColor3f(	0.f, 0.f, 1.f );
	glVertex3d(	0.f, 0.f, 0.f );
	glVertex3d(	0.f, 0.f, 1.f );
	glEnd();
}

MX_NAMESPACE_END


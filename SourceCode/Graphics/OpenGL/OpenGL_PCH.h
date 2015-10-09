// This is a precompiled header.  Include a bunch of common stuff.
// This is kind of ugly in that it adds a bunch of dependency where it isn't needed.
// But on balance, the compile time is much lower (even incrementally) once the precompiled
// headers contain these headers.

#pragma once


//---------------------------------------------------------------

#include <Base/Base.h>

MX_USING_NAMESPACE;

#include <Graphics/Base/config.h>
#include <Graphics/Base/types.h>

#include <Driver/Driver.h>


//---------------------------------------------------------------
//	OpenGL includes.
//---------------------------------------------------------------

// header files for the OpenGL32 library
#include <gl\gl.h>	// main OpenGL header (OpenGL.H on Macintosh)
#include <gl\glu.h>	// utility library
//#include <gl\glaux.h>	// auxiliary

#if MX_AUTOLINK
	#pragma comment( lib, "opengl32.lib" )
	#pragma comment( lib, "glu32.lib" )
#endif


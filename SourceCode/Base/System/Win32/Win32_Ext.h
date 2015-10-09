/*
=============================================================================
	File:	Win32_Ext.h
	Desc:
	ToDo:	This file should eventually go away.
			Inclusion of heavy headers leads to slow compilation.
=============================================================================
*/
#pragma once

// Agner Fog's asmlib
#define MX_COMPILE_WITH_ASM_LIB		0

#define MX_COMPILE_WITH_OPENGL	0



#if MX_COMPILE_WITH_ASM_LIB

	#include <asmlib/asmlib.h>

	#if MX_AUTOLINK
		#pragma comment( lib, "alibcof32.lib" )
	#endif

#endif //MX_COMPILE_WITH_ASM_LIB


//---------------------------------------------------------------
//		Math libraries.
//---------------------------------------------------------------


//---------------------------------------------------------------
//	OpenGL includes.
//---------------------------------------------------------------

#if MX_COMPILE_WITH_OPENGL

	// header files for the OpenGL32 library
	#include <gl\gl.h>	// main OpenGL header (OpenGL.H on Macintosh)
	#include <gl\glu.h>	// utility library
	//#include <gl\glaux.h>	// auxiliary

	#if MX_AUTOLINK
		#pragma comment( lib, "opengl32.lib" )
		#pragma comment( lib, "glu32.lib" )
	#endif

#endif // MX_COMPILE_WITH_OPENGL




//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//

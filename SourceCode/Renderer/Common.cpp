/*
=============================================================================
	File:	Common.cpp
	Desc:	
=============================================================================
*/
#include "Renderer_PCH.h"
#pragma hdrstop

#include <Renderer/Common.h>
#include <Renderer/Core/Font.h>

rxStats	gfxStats;



enum { DbgFontSize = 12 };

void Dbg_DrawBackEndStats(FLOAT fps,
						  UINT x, UINT y, const FColor& color)
{
	const rxBackendStats& stats = gfxBEStats;

	UNICODECHAR buffer[ MAX_STRING_CHARS ];

	MX_SPRINTF_UNICODE( buffer,
		L"FPS %.2f\n"

		L"Batches %u, tris %u, verts %u\n"

		L"VBs %u, IBs %u\n"

		L"IL %u, PT %u\n"
		L"DS %u, BS %u, RS %u\n"
		L"VS %u, GS %u, PS %u\n"

		, fps

		, stats.numDrawCalls, stats.numTriangles, stats.numVertices

		, stats.numVBChanges, stats.numIBChanges

		, stats.numInputLayoutChanges, stats.numPrimTopologyChanges
		, stats.numDSStateChanges, stats.numBSStateChanges, stats.numRSStateChanges
		, stats.numVSChanges, stats.numGSChanges, stats.numPSChanges
		);

	DbgFont_DrawString( buffer, x, y, color.ToRGBA32(), DbgFontSize );
}

void DrawCameraStats(const rxView& view,
					 UINT x, UINT y,
					 const FColor& color)
{
	// camera/view info

	const Vec3D & pos = view.origin;
	const Vec3D & look = view.look;
	const Vec3D & up = view.up;
	const Vec3D & right = view.right;


	UNICODECHAR buffer[ MAX_STRING_CHARS ];

	MX_SPRINTF_UNICODE( buffer,
		L"Pos  (%.3f, %.3f, %.3f)\n"
		L"Look (%.3f, %.3f, %.3f)\n"
		L"Up   (%.3f, %.3f, %.3f)\n"
		L"Right(%.3f, %.3f, %.3f)\n"
		L"ZN=%f, ZF=%f\n"

		, pos.x, pos.y, pos.z
		, look.x, look.y, look.z
		, up.x, up.y, up.z
		, right.x, right.y, right.z
		, view.nearZ, view.farZ
		);

	DbgFont_DrawString( buffer, x, y, color.ToRGBA32(), DbgFontSize );
}


//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//

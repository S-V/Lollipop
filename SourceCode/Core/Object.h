/*
=============================================================================
	File:	Object.h
	Desc:	Core object class.
	Note:	notes on object reference tracking system

	two choices:
	1) integer IDs (handles)
	2) raw pointers

	i settled on option (2), because i can inspect them in debugger
	and they are faster, but occupy more memory (8 bytes on 64-bit systems)

	for managing object lifetime in the future
	may use garbage collection instead of reference counting
	(it's only used in editor mode,
	(there should be no deallocations in release mode)

	TODO:	remove this crap
=============================================================================
*/

#pragma once

#include <Base/Object/ClassDescriptor.h>
#include <Core/Core.h>
#include <Core/Editor.h>
#include <Core/Serialization.h>

#define MX_DEBUG_OBJECT_SYSTEM	(1)


mxNAMESPACE_BEGIN

//---------------------------------------------------------------------------

//enum EObjectFlags
//{
//	OF_Reachable = BIT(0)	// for mark & sweep garbage collection
//};



mxNAMESPACE_END

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//

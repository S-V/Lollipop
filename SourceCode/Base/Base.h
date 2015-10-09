/*
=============================================================================
	File:	Base.h
	Desc:	Foundation Library public header file.
=============================================================================
*/

#ifndef __MX_BASE_LIBRARY_H__
#define __MX_BASE_LIBRARY_H__

//---------------------------------------------------------------
//	Public header files.
//---------------------------------------------------------------

#include "Build/BuildDefines.h"		// Build configuration options, compile settings.
#include "Common/Helpers.h"			// Common platform-agnostic stuff.
#include "System/Platform.h"		// Platform-specific stuff.


//---------------------------------------------------------------
//	Forward declarations.
//---------------------------------------------------------------

// Platform-independent application entry point.
//
extern int mxAppMain();


mxNAMESPACE_BEGIN

	// Common types.

	class String;

	class mxOutputDevice;

	class URI;
	class mxDataStream;
	class AStreamWriter;
	class AStreamReader;
	class mxFileStream;

	template<
		typename TYPE,
		class DERIVED
	> class TArrayBase;

	template<
		class T
	> class TList;

	template<
		typename KEY,
		typename VALUE,
		class HASH_FUNC,
		class EQUALS_FUNC,
		typename SIZETYPE
	> class TMap;

	template<
		typename KEY,
		typename VALUE,
		class HASH_FUNC,
		class EQUALS_FUNC
	> class TDynaMap;

	template<
		typename VALUE,
		class HASH_FUNC
	> class TPointerMap;

	// Reflection.
	namespace Reflect
	{
		class Metadata;
	}//namespace Reflect

	// High-level concepts.
	class AObject;

mxNAMESPACE_END

//------ Common ----------------------------------------------------

#include "Common/BaseCommon.h"

#include "Common/Constants.h"		// Common constants.
#include "Common/Debug.h"			// Helpers for debugging.

	// dependencies
	#include "IO/StreamIO.h"

#include "Common/Types.h"			// Basic types.

	#include "Text/ConstCharPtr.h"

#include "Common/Integer.h"
#include "Common/Float32.h"
#include "Common/Float16.h"

#include "Common/Singleton.h"

	// dependencies
	#include "Text/StaticString.h"
	#include "Text/StackString.h"


//------ Memory management ------------------------------------------------

#include "Memory/Memory.h"
//#include "Memory/BlockAlloc/BlockAllocator.h"
//#include "Memory/BlockAlloc/DynamicBlockAlloc.h"

//------ Templates ------------------------------------------------



// Common stuff.
//#include "Templates/Templates.h"

// Smart pointers.
#include "Templates/SmartPtr/TPtr.h"
#include "Templates/SmartPtr/TAutoPtr.h"
//#include "Templates/SmartPtr/TSharedPtr.h"
#include "Templates/SmartPtr/TSmartPtr.h"
#include "Templates/SmartPtr/TReference.h"
#include "Templates/SmartPtr/ReferenceCounted.h"

// Arrays.
#include "Templates/Containers/Array/TStaticList.h"
#include "Templates/Containers/Array/TStaticArray.h"
#include "Templates/Containers/Array/TStatic2DArray.h"
#include "Templates/Containers/Array/TFixedArray.h"
#include "Templates/Containers/Array/Array.h"

// Lists.
#include "Templates/Containers/LinkedList/TLinkedList.h"
#include "Templates/Containers/LinkedList/TCircularList.h"



//------ String ----------------------------------------------------

#include "Text/StringTools.h"
#include "Text/String.h"		// String type.
//#include "Text/TextUtils.h"
//#include "Text/Token.h"
//#include "Text/Lexer.h"
//#include "Text/Parser.h"


// Input/Output system.

#include "IO/StreamIO.h"
#include "IO/Files.h"
//#include "IO/IOServer.h"
//#include "IO/URI.h"
#include "IO/DataStream.h"
//#include "IO/Archive.h"
//#include "IO/FileStream.h"
#include "IO/MemoryStream.h"
#include "IO/Log.h"

// Hash tables and maps.

	// Dependency - Math/Hashing
	#include "Math/Hashing/HashFunctions.h"

//#include "Templates/Containers/HashMap/TStringMap.h"
//#include "Templates/Containers/HashMap/BTree.h"
//#include "Templates/Containers/HashMap/RBTreeMap.h"
//#include "Templates/Containers/HashMap/TMap.h"
//#include "Templates/Containers/HashMap/TDynaMap.h"
//#include "Templates/Containers/HashMap/TKeyValue.h"
//#include "Templates/Containers/HashMap/Dictionary.h"
//#include "Templates/Containers/HashMap/TPointerMap.h"
//#include "Templates/Containers/HashMap/StringMap.h"

//#include "Templates/Containers/BitSet/BitField.h"
//#include "Templates/Containers/BitSet/BitArray.h"

//------ Math ------------------------------------------------------

// Hashing
//#include "Math/Hashing/CRC8.h"
//#include "Math/Hashing/CRC16.h"
//#include "Math/Hashing/CRC32.h"
//#include "Math/Hashing/Honeyman.h"
//#include "Math/Hashing/MD4.h"
//#include "Math/Hashing/MD5.h"

// Common
//#include "Math/Math.h"


//------ Object system ------------------------------------------------------

//#include "Object/ObjectFactory.h"	// Class factory. NOTE: must be included before 'TypeDescriptor.h' !
//#include "Object/TypeDescriptor.h"			// Run-Time Type Information.
//#include "Object/Object.h"			// Base class. NOTE: must be included after 'TypeDescriptor.h' !
//#include "Object/Message.h"			// Messaging.

#include "Object/Reflection.h"

#include "Object/StructDescriptor.h"
#include "Object/ClassDescriptor.h"
// for reflecting array types
#include "Object/ArrayDescriptor.h"
#include "Object/PointerType.h"
#include "Object/EnumType.h"
#include "Object/FlagsType.h"

//------ Miscellaneous Utilities --------------------------------------------

//#include "Util/Sorting.h"
//#include "Util/Rectangle.h"
//#include "Util/FourCC.h"
//#include "Util/Color.h"
//#include "Util/Misc.h"

mxNAMESPACE_BEGIN

//---------------------------------------------------------------
//	Don't forget to call these functions before/after
//	using anything from the base system!
//---------------------------------------------------------------

// must be called on startup by each system dependent on the base system (increments internal ref counter)
bool	mxInitializeBase();

// must be called on shutdown by each system dependent on the base system (decrements internal ref counter);
// returns 'true' if the system has really been shut down
bool	mxShutdownBase();

bool	mxBaseSystemIsInitialized();

// unsafe, terminates the app immediately
void	mxForceExit( int exitCode );

// sets the function to be called before closing the base subsystem.
// can be used to clean up resources after a fatal error has occurred
void	mxSetExitHandler( FCallback pFunc, void* pArg );

void	mxGetExitHandler( FCallback * pFunc, void ** pArg );


struct SetupBaseUtil
{
	SetupBaseUtil()
	{
		mxInitializeBase();
	}
	~SetupBaseUtil()
	{
		mxShutdownBase();
	}
};

mxNAMESPACE_END

#endif // !__MX_BASE_LIBRARY_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//

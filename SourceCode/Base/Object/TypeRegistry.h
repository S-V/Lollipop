/*
=============================================================================
	File:	TypeRegistry.h
	Desc:	TypeRegistry - central object type database,
			also serves as an object factory - creates objects by a class name.
			TypeRegistry is a singleton.
			Classes register themselves in the factory
			through the macros DECLARE_CLASS and DEFINE_CLASS.
=============================================================================
*/

#ifndef __MX_TYPE_REGISTRY_H__
#define __MX_TYPE_REGISTRY_H__

#include <Base/Templates/Containers/HashMap/TMap.h>
#include <Base/Templates/Containers/HashMap/TStringMap.h>

#include <Base/Object/ClassDescriptor.h>

mxNAMESPACE_BEGIN

// Forward declarations.
class String;

class AObject;

//
//	TypeRegistry
//	
//	This class maintains a map of
/// class names and GUIDs to factory functions.
//
class TypeRegistry : SingleInstance< TypeRegistry > {
public:

	// singleton access
	// (we cannot use the Singleton template because ctor shouldn't be accessible to Singleton).

	static TypeRegistry& Get();

	static bool IsInitialized();


	bool	ClassExists( TypeGUIDArg typeCode ) const;

	const mxClass* FindClassInfoByGuid( TypeGUIDArg typeCode ) const;

	const mxClass* FindClassInfoByName( PCSTR className ) const;

	AObject* CreateInstance( TypeGUIDArg typeCode ) const;


	void EnumerateDescendants( const mxClass& baseClass, TList<const mxClass*> &OutClasses );
	void EnumerateConcreteDescendants( const mxClass& baseClass, TList<const mxClass*> &OutClasses );

public_internal:

	// must be called after all type infos have been statically initialized
	static void			Initialize();

	// NOTE: this must be called at the end of the main function to delete the instance of TypeRegistry.
	static void			Destroy();

private:
	mxDECLARE_CLASS_ALLOCATOR(HeapGeneric,TypeRegistry);

	// constructor and destructor should be private!
	TypeRegistry();
	~TypeRegistry();

private:
	TMap< TypeGUID, const mxClass* >	mTypesById;	// for fast lookup by TypeGUID code

	mxOPTIMIZE("remove dynamic Strings");
	// TODO: fast string dictionary, binary search
	TStringMap< const mxClass* >	mTypesByName;	// for fast lookup by class name (and for detecting duplicates)
};

mxNAMESPACE_END

#endif // !__MX_TYPE_REGISTRY_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//

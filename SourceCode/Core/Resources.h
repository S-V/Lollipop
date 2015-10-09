/*
=============================================================================
	File:	ResourceSystem.h
	Desc:	Resource management system.
	ToDo:	Resource system shouldn't know about any specific resource types
			(e.g.: textures, shaders, meshes).

			Requirements:

			1.	Isolate the resource management from the other parts of the engine/app
			2.	Work with handles – manage index handles for quick access (keep resource names/ids for quick resolution of the handle); keep no  pointers (to resources) in the app
			3.	Be type safe (no texture handles are valid in the context of sounds e.g.)
			4.	Be able to load information about the resources from file (data driven design)
			5.	Be able to invalidate and restore all resources at will (device dependent resource gets invalidated on device reset or change)
			6.	Scoping of resources? Free resources by scope flag
			7.	Caching of resources. Priority. [not used] 
			8.	Build a resource manager for each type of resource (different types -different m_managers)
			9.	Centralize common operations for all m_managers in a global manager
			10.	 Be scalable and flexible – be able to implement one more type of resource; be able to add a new resource type quickly and safely 
			11.	 Maximize the code reuse
			12.	 Debugging checks 
			13.	 Statistics for resource analysis

			Making sure to keep only the necessary game assets in memory, optimizing file access,
			packaging game data, etc. are all parts of game resource management.

			You have to manage problems such as the following:
			1. Shared assets - is that brick texture being used by multiple models?
			2. Asset lifetime - is that asset you loaded 15 minutes ago no longer needed? Reference counting your assets to make sure you know when something is finished with etc
			3. In DirectX 9 if certain asset types are loaded and your graphics device gets 'lost' (this happens if you press Ctrl+Alt+Del amongst other things) - your game will need to recreate them
			4. Loading assets in advance of needing them - you couldn't build big open world games without this
			5. Bulk loading assets - We often pack lots of assets into a single file to improve loading times - seeking around the disc is very time consuming

			A good asset manager should have several modes of operation.
			These modes would most likely be separate source modules adhering to a common interface.
			The two basic modes of operation would be:
			1. Production Mode - all assets are local and stripped of all meta data
			2. Development Mode - assets are stored in a database (e.g. MySQL, etc) with additional meta data.
			The database would be a two tier system with a local database caching a shared database.
			Content creators would be able to edit and update the shared database
			and updates automatically propagated to developer / QA systems.
			It should also be possible to create placeholder content.
			Since everything is in a database, queries can be made on the database and reports generated
			to analyze the state of the production.

			You'd need a tool that can grab all the assets from the shared database
			and create the production dataset.

			See: http://altdevblogaday.com/2011/06/10/strings-redux/
			You should almost never use strings in your runtime.

			The variable length nature of strings make them slow, memory consuming and unwieldy
			(memory for them must be allocated and freed).
			If you use fixed length strings you will either use even more memory
			or annoy the content creators because they can’t make their resource names
			as descriptive as they would like too.

			For these reasons I think that strings in the runtime should be reserved for two purposes:
			1) User interface text
			2) Debugging

			In particular, you shouldn't use strings for object/resource/parameter names in the runtime. Instead use string hashes. This lets you use user friendly names (strings) in your tools and fast ints in your runtime. It is also a lot easier to use than enums. Enums require global cooperation to avoid collisions. String hashes just require that you hash into a large enough key space.
			We hash names during our data compile stage into either 32-bit or 64-bit ints depending on the risk of collision. If it is a global object name (such as the name of a texture) we use 64-bit ints. If it is a local name (such as the name of a bone in a character) we use 32-bit ints. Hash collision is considered a compile error. (It hasn’t happened yet.)
			Since user interface text should always be localized, all user interface strings are managed by the localizer. The localized text is fetched from the localizer with a string lookup key, such as “menu_file_open” (hashed to a 64-bit int of course).
			This only leaves debugging. We use formatted strings for informative assert messages when something goes wrong. Our profiler and monitoring tools use interned strings to identify data. Our game programmers use debug-prints to root out problems. Of course, non of this affects the end user, since the debugging strings are only used in debug builds.
			Hashes can be problematic when debugging. If there is an error in the resource 0x3e728af10245bc71 it is not immediately obvious that it is the object vegetation/trees/larch_3.mesh that is at fault.
			We handle this with a lookup table. When we compile our data we also create a reverse lookup table that converts from a hash value back to the original string that generated it. This table is not loaded by the runtime, but it can be accessed by our tools. So our game console, for instance, uses this table to automatically translate any hash IDs that are printed by the game.

			Updated requirements:

			1) Allow asset hot loading for fast iteration.
			2) Allow resource data streaming (background loading).

	Done:	Numeric ids are more efficient (to store,compare,hash), string ids should not be used.
			But how to generate unique ids?
			Strings are needed for mods anyway. (?)
			At least, to identify packages (resource packs) by names.
			Inside a package, all resources may have unique integer ids.
			Only references to other packages will need a fully-qualified package name + resource index in that package.

			Problem: file names (paths) are unique (uniqueness guaranteed by OS),
			but are fat, slow, clumsy and unpredictable.
			Maybe, generate (unique) hash values from file names?

	ToDo:	Make PackageGuid/FileGuid/ObjectGUID and ObjectID typesafe handles!
			Get rid of hardcoded resource types!

=============================================================================
*/

#pragma once

#ifndef __MX_CORE_RESOURCE_SYSTEM_H__
#define __MX_CORE_RESOURCE_SYSTEM_H__

//#include <Base/IO/InPlaceMemoryStream.h>
#include <Base/Object/ClassDescriptor.h>

//#include <Core/IO/IOSystem.h>
#include <Core/Kernel.h>
#include <Core/Serialization.h>
#include <Core/Object.h>

mxNAMESPACE_BEGIN

/*
-----------------------------------------------------------------------------
	EAssetType

	enum of all the possible resource types in the engine;
	mainly used to get the proper resource manager for a given resource type.

	i decided to use this enum instead of RTTI
	because it's much more efficient
	(e.g. getting resource manager by resource type is a simple table lookup,
	all types are known before running the program - no dynamic memory allocations
	and bookkeeping, not hash maps (typeId) -> (resourceMgr),
	resources load faster, etc.)
	and working with resources is somewhat easier, e.g. you can reload all resources
	of the specific type, calculate memory occupied by the given resource group, etc.

	but this system is, of course, inflexible
	- need to manually add a new resource type to this enum
	and recompile the whole app.
	that's the price for performance,
	but we're writing a real-time game for iPhone, PCs, or modern consoles,
	so performance and resource management are very important.

	in practice, new resource types are added infrequently
	so this should be no problem.
-----------------------------------------------------------------------------
*/
enum EAssetType
{
#define DECLARE_ASSET_TYPE( ENUM, NAME, INFO )	ENUM
	#include <Core/Resource/AssetTypes.inl>
#undef DECLARE_ASSET_TYPE

	Asset_MAX		// Marker. Do not use.
};

mxDECLARE_ENUM_TYPE( EAssetType, UINT32, AssetType );


	extern
	const char* EAssetType_To_Chars( EAssetType eResType );

#if MX_EDITOR
	extern
	EAssetType String_To_EAssetType( const char* szResType );
#endif // MX_EDITOR


//---------------------------------------------------------------------------


// describes memory type used to hold the resource
enum EResourceMemoryPool
{
	ResMem_SystemRAM	= BIT(0),
	ResMem_VRAM			= BIT(1),
};


//---------------------------------------------------------------------------



/*
-----------------------------------------------------------------------------
	SResourceObject

	Base class for object types that correspond to a resource file.
	Objects of this type will correspond to a single resource file on disk.

	Resources are data objects that must be loaded and managed throughout
	an application. A resource might be a mesh, a texture, or any other
	piece of data - the key thing is that they must be identified by 
	a name which is unique, must be loaded only once,
	must be managed efficiently in terms of retrieval, and they may
	also be unloadable to free memory up when they have not been used for
	a while and the memory budget is under stress.

	NOTE:
	in general, you should never cache raw pointers to resource objects,
	you should use handles!
-----------------------------------------------------------------------------
*/

// NOTE: this must be an empty base class!
// it must NOT have any virtual member functions!
// it must be movable/copyable, because memory can be moved to improve data locality/coherency
//
struct SResourceObject
{

public:
	// use this function for casting to resource types;
	// it's basically a cast from (void*)
	template< class RESOURCE >	// where RESOURCE : SResourceObject
	FORCEINLINE RESOURCE* UpCast()
	{
		return checked_cast< RESOURCE* >( this );
	}

	static inline EAssetType StaticGetResourceType()
	{
		mxDEBUG_BREAK;
		return EAssetType::Asset_Unknown;
	}

#if MX_DEBUG
	virtual ~SResourceObject() {}
#endif // MX_DEBUG
};


/*
-----------------------------------------------------------------------------
	SResourceUpdateArgs

	used for streaming
-----------------------------------------------------------------------------
*/
struct SResourceUpdateArgs
{
	F4	deltaSeconds;
	//Vec3D	cameraPosition;
	//Vec3D	cameraDirection;
	//ViewFrustum	viewFrustum;
};

/*
-----------------------------------------------------------------------------
	AResourceManager
	base class for resource managers

	NOTE: managers should register themselves automatically upon initialization.
-----------------------------------------------------------------------------
*/
class AResourceManager
{
public:

	// creates a new empty resource (allocates memory for a new resource object);
	// the resource will be loaded (deserialized) by resource database based on information from the specified resource type
	// (the resource database knows all resource file formats)
	//
	//virtual SResourceObject* CreateResource( EAssetType resourceType )
	//{
	//	mxDBG_UNIMPLEMENTED;
	//	return nil;
	//}

	virtual SResourceObject* LoadResource( SResourceLoadArgs & loadArgs )
	{
		mxUNUSED(loadArgs);
		mxDBG_UNREACHABLE;
		return nil;
	}

	virtual void DestroyResource( SResourceObject* theResource )
	{
		mxUNUSED(theResource);
		mxDBG_UNREACHABLE;
	}

	virtual void UnloadResource( SResourceObject* theResource )
	{
		mxUNUSED(theResource);
		mxDBG_UNREACHABLE;
	}

	// returns fallback resource
	virtual SResourceObject* GetDefaultResource()
	{
		mxDBG_UNREACHABLE;
		return nil;
	}

	// Deletes all resources of the specified type from the memory.

	virtual void Purge()
	{
		mxDBG_UNREACHABLE;
	}

protected:
	AResourceManager() {}
	virtual ~AResourceManager();

	PREVENT_COPY(AResourceManager);
};


/*
-----------------------------------------------------------------------------
	ResourceSystem

	global resource management system
	where object factories for different resource types can register.

	it automates creation of complex resource objects
	and their management in the memory.

	developers reference each asset individually by its GUID
	(asset - logical unit of content),
	but during export many additional resources can be generated
	(e.g. LoD/mip levels),
	and after optimizations some assets may 'disappear'.
-----------------------------------------------------------------------------
*/
mxREFACTOR("turn into namespace");

class ResourceSystem
	: public AEditable
	, SingleInstance< ResourceSystem >
{
public:

	// High-level resource management

	void SetManager( EAssetType resourceType, AResourceManager* mgr );

	// returns the resource manager corresponding to the given resource type
	AResourceManager* GetManager( EAssetType resourceType );


	// Query


	// a safe way to access resource. in case of failure the default (fallback) resource may be returned.
	//
	SResourceObject* GetResource( ObjectIDArg resourceHandle, EAssetType resourceType );


	// GetResource_NoLockNoLRUTouch()
	// used in performance-critical places (e.g. inner loops)
	//SResourceObject* GetResource_FastUnsafe( ObjectIDArg resourceIndex );


	SResourceObject* GetDefaultInstance( EAssetType resourceType );

	// Slow!
	// checks if the resource is already loaded and tries to return its cached instance;
	// in case of failure the default (fallback) resource may be returned.
	// silently returns a null pointer if the resource failed to load.
	//
	SResourceObject* GetResourceByGuid( EAssetType resourceType, ObjectGUIDArg resourceGuid );


	template< class RESOURCE >	// where RESOURCE : SResourceObject
	inline
	RESOURCE* GetResourceByGuid( ObjectGUIDArg resourceGuid )
	{
		EAssetType resourceType = RESOURCE::StaticClass();
		SResourceObject* pResource = this->GetResourceByGuid( resourceType, resourceGuid );
		return pResource->UpCast< RESOURCE >();
	}


	//ObjectIDArg IsResourceLoaded( ObjectGUIDArg resourceGuid );

	//void UnloadResource( ObjectGUIDArg resourceGuid );



	// Slow!
	ObjectGUIDArg GetResourceGuidByPointer( const void* o );
	// Slow!
	EAssetType GetResourceTypeByPointer( const void* o );


	// Slow!
	//EAssetType GetResourceTypeByPointer( const SResourceObject* pResourceObject );

	// changed function names a bit because they conflicted with Windows defines

	// locks the resource handle so that the handle cannot be purged, released or resized
	void LockResourceHandle( ObjectIDArg resourceIndex );

	void UnlockResourceHandle( ObjectIDArg resourceIndex );

	void TouchMemory( ObjectIDArg resourceIndex );

	//Acquire
	void GrabResourceHandle( ObjectIDArg resourceIndex );
	//Release
	void DropResourceHandle( ObjectIDArg resourceIndex );

public_internal:

	mxDECLARE_CLASS(ResourceSystem,AEditable);

	ResourceSystem();
	~ResourceSystem();

	void SetContentDatabase( AContentDatabase* resourceDatabase );
	AContentDatabase* GetContentDatabase();

	// called when reloading projects in the editor
	void Clear();

	virtual void Serialize( mxArchive& archive ) override;

	void Tick( const SResourceUpdateArgs& args );

public:	// Editor
	//virtual UINT edNumRows() const override;
	//virtual AEditable* edItemAt( UINT index ) override;
	//virtual UINT edIndexOf( const AEditable* child ) const override;
	//virtual const char* edToChars( UINT column ) const override;
	//virtual AEditable* edGetParent() override;

//	// these work only in editor mode if resource metadata is loaded
//	bool edGetResourceInfo( ObjectGUIDArg resourceGuid, SResourceInfo &info );
//	bool edGetFileMetadata( ObjectGUIDArg resourceGuid, SFileMetadata &info );
//	bool edSetData( ObjectGUIDArg fileGuid, EdSourceResourceData* editorData );
//	EdSourceResourceData* edGetData( ObjectGUIDArg fileGuid );
//	SResourceObject* edGetResourceByGuid( ObjectGUIDArg fileGuid );	// resource type is taken from metadata
//	ObjectGUIDArg edGetResourceGuidByName( PCHARS resourceName );
//	void edSaveAllPackages();
//
//	void edWriteResourceINI( mxTextWriter & writer );
//
//private:
//	ObjectID LoadResource( AResourcePackage* package, EAssetType resourceType, ObjectGUIDArg resourceGuid );

	void GetLoadedResources( TList<ObjectGUID> & loadedAssets );
};

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=


/*
-----------------------------------------------------------------------------
	AFilePackage

	resource package interface ('resource provider')
-----------------------------------------------------------------------------
*/

typedef UINT PakFileHandle;
enum { BadPakFileHandle = (PakFileHandle)-1 };

struct AFilePackage
{
public:	// High-level API


public:	// Low-level file access

	// fast access to file by file handle;
	// returns BadPakFileHandle if not found
	//
	virtual PakFileHandle OpenFile( ObjectGUIDArg fileGuid ) = 0;

	virtual void CloseFile( PakFileHandle file ) = 0;

	// Summary:
	//   Get the file size in bytes (uncompressed).
	// Returns:
	//   The size of the file (unpacked) by the handle
	virtual UINT GetFileSize( PakFileHandle file ) = 0;

	// Reads the file into the preallocated buffer
	virtual SizeT ReadFile( PakFileHandle file, UINT startOffset, void *buffer, UINT bytesToRead ) = 0;

	// works only in editor mode
	//virtual void UpdateFile( PakFileHandle file, const void* uncompressedData, UINT size );

	//virtual bool GetFileInfo( ResourceGuidArg fileGuid, SFileInfo &info ) = 0;
};



/*
-----------------------------------------------------------------------------
	AContentDatabase

	content/asset database,
	knows how to locate individual resource files by their GUIDs
-----------------------------------------------------------------------------
*/
class AContentDatabase
	: public AObject, public AFilePackage
{
public:
	mxDECLARE_ABSTRACT_CLASS( AContentDatabase, AObject );

	// NOTE: these functions should only be called in editor mode!

	// returns the folder to the source assets
	virtual String GetBasePath();

	// Since asset database keeps metadata about asset files,
	// you should never create, move or delete them using the file system.
	// Instead, you can use AssetDatabase.Contains, AssetDatabase.CreateAsset,
	// AssetDatabase.CreateFolder, AssetDatabase.RenameAsset, AssetDatabase.CopyAsset, AssetDatabase.ImportAsset,
	// AssetDatabase.MoveAsset, AssetDatabase.MoveAssetToTrash and AssetDatabase.DeleteAsset.

	//AssetDatabase.AssetPathToGUID 
	//AssetDatabase.GUIDToAssetPath

	// Returns the path name relative to the 'Assets' folder where the asset is stored.
	//virtual void GetAssetPath( ObjectGUIDArg resourceGuid );

	// Translate a GUID to its current asset path.
	// this is mainly used for serialization into human-readable text formats
	//
	virtual String GuidToAssetPath(
		ObjectGUIDArg resourceGuid
	) const;

	// Get the GUID for the asset at path
	//
	virtual ObjectGUIDArg AssetPathToGuid(
		const String& resourcePath
	) const;

protected:
	virtual ~AContentDatabase() {}
};





/*
-----------------------------------------------------------------------------
	SResourceLoadArgs

	parameters used for resource loading
-----------------------------------------------------------------------------
*/
class SResourceLoadArgs : public AStreamReader
{
	AFilePackage *	m_package;
	PakFileHandle	m_fileHandle;
	UINT			m_readOffset;
	UINT			m_dataSize;
	BYTE *			m_mappedData;

public:
	SResourceLoadArgs( AFilePackage* package, PakFileHandle fileHandle );
	~SResourceLoadArgs();

	virtual SizeT Read( void *pBuffer, SizeT numBytes ) override;

	SizeT GetSize() const;

	// Map stream to memory.
	BYTE* Map();
	void Unmap();
};


/*
-----------------------------------------------------------------------------
	mxAssetReferenceType

	type information for describing resource handles
-----------------------------------------------------------------------------
*/
struct mxAssetReferenceType : public mxType
{
	const AssetType		m_assetType;

public:
	inline mxAssetReferenceType( const AssetType assetType )
		: mxType( ETypeKind::Type_AssetRef, "ResourcePtr", STypeDescription::For_Type<ObjectID>() )
		, m_assetType( assetType )
	{
	}
};


/*
-----------------------------------------------------------------------------
	Resource handle

	Smart pointer type used to reference resource objects.
	Declares a type-safe handle type,
	acts like a typed resource handle, a wrapper around 'int'.

	you can't assign one handle to the next.
-----------------------------------------------------------------------------
*/
struct SResPtrBase
{
	SResourceObject *	m_pointer;

public:

	FORCEINLINE bool IsNull() const
	{
		return ( m_pointer == nil );
	}
	FORCEINLINE bool IsValid() const
	{
		return ( m_pointer != nil );
	}

	// Slow!
	ObjectGUIDArg GetGUID() const;

	// Slow!
	const String GetPath() const;

protected:
	friend class EdProperty_AssetReference;

	void Internal_SetDefault( EAssetType assetType );
	void Internal_SetPointer( EAssetType assetType, ObjectGUIDArg assetGuid );
};

template< class RESOURCE >	// where RESOURCE : SResourceObject
struct TResPtr : public SResPtrBase
{
	mxUTIL_CHECK_BASE_CLASS( RESOURCE, SResourceObject );

	typedef TResPtr<RESOURCE> ThisType;

	inline TResPtr()
	{
		m_pointer = nil;
	}
	inline TResPtr( RESOURCE* pointer )
	{
		AssertPtr( pointer );
		m_pointer = pointer;
	}
	inline explicit TResPtr( const TResPtr<RESOURCE>& other )
	{
		*this = other;
	}
	inline ~TResPtr()
	{
	}

	FORCEINLINE RESOURCE * operator -> () const
	{
		AssertPtr( m_pointer );
		return m_pointer;
	}

	FORCEINLINE RESOURCE & operator * () const
	{
		AssertPtr( m_pointer );
		return *m_pointer;
	}

	FORCEINLINE operator RESOURCE* () const
	{
		AssertPtr( m_pointer );
		return m_pointer;
	}
	FORCEINLINE operator const RESOURCE* () const
	{
		AssertPtr( m_pointer );
		return m_pointer;
	}

	FORCEINLINE bool operator == ( RESOURCE* pointer ) const
	{
		return ( m_pointer == pointer );
	}
	FORCEINLINE bool operator != ( RESOURCE* pointer ) const
	{
		return ( m_pointer != pointer );
	}
	FORCEINLINE bool operator == ( const TValidPtr<RESOURCE>& other ) const
	{
		return ( m_pointer == other.m_pointer );
	}
	FORCEINLINE bool operator != ( const TValidPtr<RESOURCE>& other ) const
	{
		return ( m_pointer != other.m_pointer );
	}


	void SetDefaultInstance()
	{
		const EAssetType assetType = this->GetAssetType();
		this->Internal_SetDefault( assetType );
	}
	void SetFromGuid( ObjectGUIDArg assetGuid )
	{
		const EAssetType assetType = this->GetAssetType();
		this->Internal_SetPointer( assetType, assetGuid );
	}

	FORCEINLINE RESOURCE* ToPtr() const
	{
		AssertPtr( m_pointer );
		return m_pointer->UpCast<RESOURCE>();
	}

public:	// Reflection / Serialization

	inline EAssetType GetAssetType() const
	{ return RESOURCE::StaticGetResourceType(); }
};

typedef TResPtr<SResourceObject> ResourceHandle;


// specialization of TypeDeducer<> for resource reference types
//
template< class RESOURCE >	// where RESOURCE : SResourceObject
struct TypeDeducer< TResPtr<RESOURCE> >
{
	static inline const mxType& GetType()
	{
		static mxAssetReferenceType	staticTypeInfo( RESOURCE::StaticGetResourceType() );
		return staticTypeInfo;
	}
	static inline ETypeKind GetTypeKind()
	{
		return ETypeKind::Type_AssetRef;
	}
};


namespace Resources
{
	// Translate a GUID to its current asset path.
	// this is mainly used for serialization into human-readable text formats
	//
	String GuidToAssetPath( ObjectGUIDArg assetGuid );

	// Get the GUID for the asset at path
	//
	ObjectGUIDArg AssetPathToGuid( const String& assetPath );

	SResourceObject* GetResource( EAssetType assetType, ObjectGUIDArg assetGuid );

}//namespace Resources

mxNAMESPACE_END

#endif // !__MX_CORE_RESOURCE_SYSTEM_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//

/*
=============================================================================
	File:	Resources.cpp
	Desc:	Resource management system.
=============================================================================
*/

#include <Core_PCH.h>
#pragma hdrstop
#include <Core.h>

#include <Core/Kernel.h>
#include <Core/Resources.h>

mxNAMESPACE_BEGIN

/*
-----------------------------------------------------------------------------
	EAssetType
-----------------------------------------------------------------------------
*/
namespace
{
	static const char* gAssetTypeNames[ EAssetType::Asset_MAX ] =
	{
#define DECLARE_ASSET_TYPE( ENUM, NAME, INFO )	NAME
	#include <Core/Resource/AssetTypes.inl>
#undef DECLARE_ASSET_TYPE
	};
}//namespace

const char* EAssetType_To_Chars( EAssetType eResType )
{
	CHK_VRET_X_IF_NOT( eResType >= 0 && eResType < NUMBER_OF(gAssetTypeNames), "" );
	return gAssetTypeNames[ eResType ];
}

#if MX_EDITOR

EAssetType String_To_EAssetType( const char* szResType )
{
	CHK_VRET_X_IF_NIL( szResType, EAssetType::Asset_Unknown );

	for( UINT i=0; i < NUMBER_OF(gAssetTypeNames); i++ )
	{
		if( mxStrEquAnsi( szResType, gAssetTypeNames[i] ) ) {
			return (EAssetType)i;
		}
	}

	Unreachable;

	return EAssetType::Asset_Unknown;
}

#endif // MX_EDITOR

/*
-----------------------------------------------------------------------------
	SResourceObject
-----------------------------------------------------------------------------
*/

/*
-----------------------------------------------------------------------------
	SResourceLoadArgs
-----------------------------------------------------------------------------
*/
SResourceLoadArgs::SResourceLoadArgs( AFilePackage* package, PakFileHandle fileHandle )
{
	AssertPtr(package);
	m_package = package;
	Assert(fileHandle != BadPakFileHandle);
	m_fileHandle = fileHandle;
	m_readOffset = 0;
	m_dataSize = package->GetFileSize( fileHandle );
	Assert(m_dataSize > 0);
	m_mappedData = nil;
}

SResourceLoadArgs::~SResourceLoadArgs()
{
	if( m_mappedData != nil )
	{
		this->Unmap();
	}
	m_package->CloseFile( m_fileHandle );
}

SizeT SResourceLoadArgs::Read( void *pBuffer, SizeT numBytes )
{
	Assert(m_mappedData == nil);
	Assert(m_readOffset < m_dataSize + numBytes);
	UINT bytesToRead = Min<UINT>( m_dataSize - m_readOffset, numBytes );

	m_package->ReadFile( m_fileHandle, m_readOffset, pBuffer, numBytes );

	m_readOffset += bytesToRead;
	return bytesToRead;
}

SizeT SResourceLoadArgs::GetSize() const
{
	return m_dataSize;
}

BYTE* SResourceLoadArgs::Map()
{
	Assert(nil == m_mappedData);
	const SizeT dataSize = m_dataSize;
	void* data = mxAllocX( EMemHeap::HeapStreaming, dataSize );
	const SizeT readSize = this->Read( data, dataSize );
	Assert( readSize == dataSize );
	(void)readSize;
	m_mappedData = c_cast(BYTE*) data;
	return m_mappedData;
}

void SResourceLoadArgs::Unmap()
{
	Assert(nil != m_mappedData);
	mxFreeX( EMemHeap::HeapStreaming, m_mappedData );
	m_mappedData = nil;
}

/*
-----------------------------------------------------------------------------
	AResourceManager
-----------------------------------------------------------------------------
*/
AResourceManager::~AResourceManager()
{
}

/*
-----------------------------------------------------------------------------
	AContentDatabase
-----------------------------------------------------------------------------
*/

String AContentDatabase::GetBasePath()
{
	mxDBG_UNREACHABLE;
	return String();
}

String AContentDatabase::GuidToAssetPath(
	ObjectGUIDArg resourceGuid
	) const
{
	if( resourceGuid.IsNull() ) {
		return String();
	}
	mxDBG_UNREACHABLE;
	return String();
}

ObjectGUIDArg AContentDatabase::AssetPathToGuid(
	const String& resourcePath
	) const
{
	if( resourcePath.IsEmpty() ) {
		return ObjectGUID();
	}
	mxDBG_UNREACHABLE;
	return ObjectGUID();
}

/*
-----------------------------------------------------------------------------
	NullContentDatabase
-----------------------------------------------------------------------------
*/
struct NullContentDatabase : public AContentDatabase
{
	// fast access to file by file handle;
	// returns BadPakFileHandle if not found
	//
	virtual PakFileHandle OpenFile( ObjectGUIDArg fileGuid ) override
	{
		mxDBG_UNREACHABLE;
		return BadPakFileHandle;
	}

	virtual void CloseFile( PakFileHandle file ) override
	{
		mxDBG_UNREACHABLE;
	}

	// Summary:
	//   Get the file size in bytes (uncompressed).
	// Returns:
	//   The size of the file (unpacked) by the handle
	virtual UINT GetFileSize( PakFileHandle file ) override
	{
		mxDBG_UNREACHABLE;
		return 0;
	}

	// Reads the file into the preallocated buffer
	virtual SizeT ReadFile( PakFileHandle file, UINT startOffset, void *buffer, UINT bytesToRead ) override
	{
		mxDBG_UNREACHABLE;
		return 0;
	}

	NullContentDatabase() {}
};

// NOTE: type can be encoded, say, in 8 bits
struct LookUpKey
{
	union
	{
		struct 
		{
			ObjectGUID	guid;	// resource GUID
			EAssetType	type;	// resource type
		};
		U8		v;
	};
};
mxSTATIC_ASSERT( sizeof LookUpKey == sizeof U8 );

template<>
struct THashTrait< LookUpKey >
{
	static FORCEINLINE UINT GetHashCode( const LookUpKey& key )
	{
		return key.v;
	}
};
template<>
struct TEqualsTrait< LookUpKey >
{
	static FORCEINLINE bool Equals( const LookUpKey& a, const LookUpKey& b )
	{
		return (a.v == b.v);
	}
};

/*
-----------------------------------------------------------------------------
	ResourceSystem
-----------------------------------------------------------------------------
*/
namespace
{
	// maps static resource GUIDs to dynamic identifiers (handles)
	typedef TMap< LookUpKey, SResourceObject* > ResourceMap;

	// resource managers by resource type
	typedef TStaticArray_InitZeroed< AResourceManager*, Asset_MAX >	ResourceLoaderMap;

	// private data
	struct ResourceSystemData
	{
		// All resources are cached as they are loaded
		// so that only one copy of each resides in memory at once.

		ResourceMap			loadedMap;	// maps resource GUIDs (static ids) to indices (dynamic ids)

		// these can create default resource instances
		ResourceLoaderMap	managers;	// pointers not owned

		// maps resource GUIDs to file streams
		TPtr< AContentDatabase >	database;

		NullContentDatabase		dummyDatabase;

		// put large structures at the end
		//StreamEngine	streamer;

	public:
		ResourceSystemData()
		{
			database = &dummyDatabase;
		}
	};

	static TPtr< ResourceSystemData >	m_data;


	static inline
	void F_InsertNewResource( const LookUpKey& key, SResourceObject* value )
	{
		AssertPtr( value );
		m_data->loadedMap.Set( key, value );
	}

	static
	SResourceObject* F_OnResourceFailedToLoad( AResourceManager* manager, EAssetType resourceType, ObjectGUIDArg resourceGuid )
	{
//#if MX_EDITOR
		const char* resourceTypeName = EAssetType_To_Chars( resourceType );
		char	tmp[32];
		resourceGuid.ToChars(tmp,NUMBER_OF(tmp));
		DBGOUT("Failed to load resource of type '%s' (GUID=%s), using fallback\n", resourceTypeName,tmp);
//#endif

		// if failed to load, return default instance

		SResourceObject* defaultInstance = manager->GetDefaultResource();
		AssertPtr(defaultInstance);
		return defaultInstance;
	}

}//namespace

mxDEFINE_CLASS(ResourceSystem);

#if MX_EDITOR
void ResourceSystem::GetLoadedResources( TList<ObjectGUID> & loadedAssets )
{
	const UINT numLoadedResources = m_data->loadedMap.NumEntries();
	loadedAssets.Reserve( numLoadedResources );
	ResourceMap::PairsArray & pairs = m_data->loadedMap.GetPairs();
	for( UINT i=0; i < numLoadedResources; i++ )
	{
		if( pairs[i].key.guid.IsValid() )
		{
			loadedAssets.Add( pairs[i].key.guid );
		}
	}
}
#endif // MX_EDITOR

ResourceSystem::ResourceSystem()
{
	m_data.ConstructInPlace();

	//m_data->entries.Reserve();
}

ResourceSystem::~ResourceSystem()
{
	//@todo: log any leaked (unreleased) resource, dump stats
	//

	m_data.Destruct();
}

void ResourceSystem::SetManager( EAssetType resourceType, AResourceManager* mgr )
{
	Assert( m_data->managers[ resourceType ] == nil );
	AssertPtr( mgr );

	m_data->managers[ resourceType ] = mgr;
}

AResourceManager* ResourceSystem::GetManager( EAssetType resourceType )
{
	return m_data->managers[ resourceType ];
}

void ResourceSystem::SetContentDatabase( AContentDatabase* resourceDatabase )
{
	if( resourceDatabase != nil )
	{
		DBGOUT("ResourceSystem::SetContentDatabase: %s\n",resourceDatabase->rttiGetTypeName());
		m_data->database = resourceDatabase;
	}
	else
	{
		m_data->database = &m_data->dummyDatabase;
	}
}

AContentDatabase* ResourceSystem::GetContentDatabase()
{
	return m_data->database;
}

void ResourceSystem::Clear()
{
	this->uiBeginRefresh();

	m_data->loadedMap.Empty();

	// NOTABUG: resource databases are core system objects, they are persistent
	//m_data->database = &m_data->dummyDatabase;

	// NOTABUG: resource managers are core system objects, they are persistent
	//m_data->managers.Empty();

	this->uiEndRefresh();
}

void ResourceSystem::Serialize( mxArchive& archive )
{
	Super::Serialize( archive );

	if( archive.IsReading() )
	{
		// auto expand tree view
		this->uiRefreshView();
	}
}

void ResourceSystem::Tick( const SResourceUpdateArgs& args )
{
	m_data->loadedMap.GrowIfNeeded();
}

SResourceObject* ResourceSystem::GetResource( ObjectIDArg resourceHandle, EAssetType resourceType )
{
	Assert( resourceHandle.IsValid() );
UNDONE;
	SResourceObject* ptr = c_cast(SResourceObject*) Kernel::LookupObject( resourceHandle );

	AssertPtr(ptr);

#if 0

	if(!PtrToBool( ptr ))
	{
		AResourceManager* manager = this->GetManager( resourceType );
		AssertPtr(manager);

		if(PtrToBool( manager ))
		{
			ptr = this->GetManager( resourceType )->GetDefaultResource();
		}
	}

#endif

	return ptr;
}

SResourceObject* ResourceSystem::GetDefaultInstance( EAssetType resourceType )
{
	LookUpKey	key;
	key.guid = ObjectGUID(_InitInvalid);
	key.type = resourceType;

	SResourceObject* cachedInstance = m_data->loadedMap.FindRef( key );
	if(PtrToBool( cachedInstance ))
	{
		return cachedInstance;
	}

	AResourceManager* manager = this->GetManager( resourceType );
	AssertPtr( manager );
	if(PtrToBool( manager ))
	{
		SResourceObject * defaultInstance = manager->GetDefaultResource();
		if(PtrToBool( defaultInstance ))
		{
			F_InsertNewResource( key, defaultInstance );
			return defaultInstance;
		}
	}

	mxErrf( "Failed to get default instance of '%s'\n",EAssetType_To_Chars( resourceType ) );

	return nil;
}

SResourceObject* ResourceSystem::GetResourceByGuid( EAssetType resourceType, ObjectGUIDArg resourceGuid )
{
	LookUpKey	key;
	key.guid = resourceGuid;
	key.type = resourceType;

	// check if the resource is already loaded

	SResourceObject* cachedInstance = m_data->loadedMap.FindRef( key );
	if(PtrToBool( cachedInstance ))
	{
		return cachedInstance;
	}

	// load and cache the resource

	SResourceObject *	newInstance = nil;

	AResourceManager* manager = this->GetManager( resourceType );
	AssertPtr( manager );
	if(PtrToBool( manager ))
	{
		TPtr< AContentDatabase > database = m_data->database;

		if( resourceGuid.IsValid() )
		{
			const PakFileHandle fileHandle = database->OpenFile( resourceGuid );
			if( fileHandle != BadPakFileHandle )
			{
				//DBGOUT("Loading '%s'.\n",this->GetContentDatabase()->GuidToAssetPath(resourceGuid).ToChars());

				SResourceLoadArgs	loadArgs( database, fileHandle );

				newInstance = manager->LoadResource( loadArgs );
			}
			else
			{
				newInstance = F_OnResourceFailedToLoad( manager, resourceType, resourceGuid );
			}
		}
		else
		{
			newInstance = F_OnResourceFailedToLoad( manager, resourceType, resourceGuid );
		}

		AssertPtr( newInstance );

		if(PtrToBool( newInstance ))
		{
			F_InsertNewResource( key, newInstance );
			return newInstance;
		}
	}

	return nil;
}

static inline
const LookUpKey* F_FindKeyByPointer( const void* o )
{
	AssertPtr(o);
	const LookUpKey* key = m_data->loadedMap.FindKeyByValue( (SResourceObject*)o );
	return key;
}

ObjectGUIDArg ResourceSystem::GetResourceGuidByPointer( const void* o )
{
	//AssertPtr(o);
	if(PtrToBool( o ))
	{
		const LookUpKey* key = F_FindKeyByPointer( o );
		AssertPtr(key);
		if(PtrToBool( key ))
		{
			return key->guid;
		}
	}
	return ObjectGUID(_InitInvalid);
}

EAssetType ResourceSystem::GetResourceTypeByPointer( const void* o )
{
	AssertPtr(o);
	if(PtrToBool( o ))
	{
		const LookUpKey* key = F_FindKeyByPointer( o );
		AssertPtr(key);
		if(PtrToBool( key ))
		{
			return key->type;
		}
	}
	return EAssetType::Asset_Unknown;
}

/*
-----------------------------------------------------------------------------
	AContentDatabase
-----------------------------------------------------------------------------
*/
mxDEFINE_ABSTRACT_CLASS( AContentDatabase );

/*
-----------------------------------------------------------------------------
	SResPtrBase
-----------------------------------------------------------------------------
*/
ObjectGUIDArg SResPtrBase::GetGUID() const
{
	const ObjectGUID assetGuid = gCore.resources->GetResourceGuidByPointer( m_pointer );
	return assetGuid;
}

const String SResPtrBase::GetPath() const
{
	const ObjectGUID assetGuid = this->GetGUID();
	return Resources::GuidToAssetPath( assetGuid );
}

void SResPtrBase::Internal_SetDefault( EAssetType assetType )
{
	m_pointer = gCore.resources->GetDefaultInstance( assetType );
}

void SResPtrBase::Internal_SetPointer( EAssetType assetType, ObjectGUIDArg assetGuid )
{
	m_pointer = Resources::GetResource( assetType, assetGuid );
}

namespace Resources
{

String GuidToAssetPath( ObjectGUIDArg assetGuid )
{
	const String assetPath = gCore.resources->GetContentDatabase()->GuidToAssetPath( assetGuid );
	return assetPath;
}

ObjectGUIDArg AssetPathToGuid( const String& assetPath )
{
	const ObjectGUID assetGuid = gCore.resources->GetContentDatabase()->AssetPathToGuid( assetPath );
	return assetGuid;
}

SResourceObject* GetResource( EAssetType assetType, ObjectGUIDArg assetGuid )
{
	SResourceObject* pObject = gCore.resources->GetResourceByGuid( assetType, assetGuid );
	return pObject;
}

}//namespace Resources

mxNAMESPACE_END

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//

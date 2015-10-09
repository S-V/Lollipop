#include <EditorSupport_PCH.h>
#pragma hdrstop

#include <Renderer/Core/Texture.h>

#include <EditorSupport/Serialization/JsonSerializationCommon.h>
#include <EditorSupport/Reflection/ReflectionUtil.h>

namespace ReflectionUtil
{

namespace
{
static void Transform(
	const Json::Value& srcValue,
	const mxType& typeInfo,
	AStreamWriter &dst
	);

static void Transform_Asset_Reference(
	const Json::Value& srcValue,
	const mxAssetReferenceType& handleType,
	AStreamWriter &dst
	);

static void Transform_Array(
	const Json::Value& srcValue,
	const mxArrayType& arrayInfo,
	AStreamWriter &dst
	);

static void Transform_Class(
	const Json::Value& srcValue,
	const mxClass& classInfo,
	AStreamWriter &dst
	);

static void Transform(const Json::Value& srcValue,
					  const mxType& typeInfo,
					  AStreamWriter &dst)
{
	const ETypeKind typeKind = typeInfo.m_kind;

	switch( typeKind )
	{
	case ETypeKind::Type_Int8 :
		{
			INT8	dstValue;
			JSON::Deserialize_Int8( srcValue, dstValue );
			dst << dstValue;
		}
		break;

	case ETypeKind::Type_UInt8 :
		{
			UINT8	dstValue;
			JSON::Deserialize_UInt8( srcValue, dstValue );
			dst << dstValue;
		}
		break;

	case ETypeKind::Type_Int16 :
		{
			INT16	dstValue;
			JSON::Deserialize_Int16( srcValue, dstValue );
			dst << dstValue;
		}
		break;

	case ETypeKind::Type_UInt16 :
		{
			UINT16	dstValue;
			JSON::Deserialize_UInt16( srcValue, dstValue );
			dst << dstValue;
		}
		break;

	case ETypeKind::Type_Int32 :
		{
			INT32	dstValue;
			JSON::Deserialize_Int32( srcValue, dstValue );
			dst << dstValue;
		}
		break;

	case ETypeKind::Type_UInt32 :
		{
			UINT32	dstValue;
			JSON::Deserialize_UInt32( srcValue, dstValue );
			dst << dstValue;
		}
		break;

	case ETypeKind::Type_Int64 :
		{
			INT64	dstValue;
			JSON::Deserialize_Int64( srcValue, dstValue );
			dst << dstValue;
		}
		break;

	case ETypeKind::Type_UInt64 :
		{
			UINT64	dstValue;
			JSON::Deserialize_UInt64( srcValue, dstValue );
			dst << dstValue;
		}
		break;

	case ETypeKind::Type_Float :
		{
			F4	dstValue;
			JSON::Deserialize_Float( srcValue, dstValue );
			dst << dstValue;
		}
		break;

	case ETypeKind::Type_Double :
		{
			F8	dstValue;
			JSON::Deserialize_Double( srcValue, dstValue );
		}
		break;

	case ETypeKind::Type_Bool :
		{
			bool	dstValue;
			JSON::Deserialize_Boolean( srcValue, dstValue );
			dst << dstValue;
		}
		break;

	case ETypeKind::Type_SimdQuad :
		{
			float4	dstValue;
			JSON::Deserialize_Float4( srcValue, dstValue );
			dst << dstValue;
		}
		break;

	case ETypeKind::Type_Vec2D :
		{
			Vec2D	dstValue;
			JSON::Deserialize_Vec2D( srcValue, dstValue );
			dst << dstValue;
		}
		break;

	case ETypeKind::Type_Vec3D :
		{
			Vec3D	dstValue;
			JSON::Deserialize_Vec3D( srcValue, dstValue );
			dst << dstValue;
		}
		break;

	case ETypeKind::Type_Vec4D :
		{
			Vec4D	dstValue;
			JSON::Deserialize_Vec4D( srcValue, dstValue );
			dst << dstValue;
		}
		break;

	case ETypeKind::Type_Matrix2 :
		{
			Matrix2	dstValue;
			JSON::Deserialize_Matrix2( srcValue, dstValue );
			dst << dstValue;
		}
		break;

	case ETypeKind::Type_Matrix3 :
		{
			Matrix3	dstValue;
			JSON::Deserialize_Matrix3( srcValue, dstValue );
			dst << dstValue;
		}
		break;

	case ETypeKind::Type_Matrix4 :
		{
			Matrix4	dstValue;
			JSON::Deserialize_Matrix4( srcValue, dstValue );
			dst << dstValue;
		}
		break;

	case ETypeKind::Type_ColorRGBA :
		{
			FColor	dstValue;
			JSON::Deserialize_ColorRGBA( srcValue, dstValue );
			dst << dstValue;
		}
		break;

	case ETypeKind::Type_String :
		{
			String	dstValue;
			JSON::Deserialize_String( srcValue, dstValue );
			dst << dstValue;
		}
		break;

#if 0
	case ETypeKind::Type_Enum :
		{
			const mxEnumType& enumInfo = typeInfo.UpCast<mxEnumType>();
			Transform_Enum( srcValue, enumInfo, dst );
		}
		break;

	case ETypeKind::Type_Flags :
		{
			const mxFlagsType& flagsType = typeInfo.UpCast<mxFlagsType>();
			Transform_Flags( srcValue, flagsType, dst );
		}
		break;


	case ETypeKind::Type_Struct :
		{
			const mxStruct& structInfo = typeInfo.UpCast<mxStruct>();
			Transform_Struct( srcValue, structInfo, dst );
		}
		break;
#endif
	case ETypeKind::Type_Class :
		{
			const mxClass& classInfo = typeInfo.UpCast<mxClass>();
			Transform_Class( srcValue, classInfo, dst );
		}
		break;
#if 0
	case ETypeKind::Type_Pointer :
		{
			const mxPointerType& pointerType = typeInfo.UpCast<mxPointerType>();
			Transform_Pointer( srcValue, pointerType, dst );
		}
		break;
#endif

	case ETypeKind::Type_AssetRef :
		{
			const mxAssetReferenceType& handleType = typeInfo.UpCast<mxAssetReferenceType>();
			return Transform_Asset_Reference( srcValue, handleType, dst );
		}
		break;

	case ETypeKind::Type_Array :
		{
			const mxArrayType& arrayInfo = typeInfo.UpCast<mxArrayType>();
			return Transform_Array( srcValue, arrayInfo, dst );
		}
		break;


	default:
		Unreachable;
	}
}

static void Transform_Parent_Class(
	const Json::Value& srcValue,
	const mxClass& parentClass,
	AStreamWriter &dst
	)
{
	if( ObjectUtil::Serializable_Class( parentClass ) )
	{
		const Json::Value parentClassValue = srcValue[ BASE_CLASS_TAG ];

		Transform_Class( parentClassValue, parentClass, dst );
	}
}

static void Transform_Class_Members(
							const Json::Value& srcValue,
							const mxClassMembers& members,
							AStreamWriter &dst
							)
{
	for( UINT iField = 0 ; iField < members.numFields; iField++ )
	{
		const mxField& field = members.fields[ iField ];

		Transform( srcValue[ field.alias ], field.type, dst );
	}
}


static void Transform_Class(
							const Json::Value& srcValue,
							const mxClass& classInfo,
							AStreamWriter &dst
							)
{
	if(0)
	{
		DBGOUT( "Class: %s\n", classInfo.m_name );
		JSON::DbgPrintValue( srcValue );
	}

	const mxClass* parentClass = classInfo.GetParent();
	if( parentClass != nil )
	{
		Transform_Parent_Class( srcValue, *parentClass, dst );
	}

	const mxClassMembers& classMembers = classInfo.GetMembers();
	Transform_Class_Members( srcValue, classMembers, dst );
}

static void Transform_Asset_Reference(
	const Json::Value& srcValue,
	const mxAssetReferenceType& handleType,
	AStreamWriter &dst
	)
{
	const String		assetPath = srcValue[ ASSET_PATH_TAG ].asCString();
	const ObjectGUID	assetGuid = Resources::AssetPathToGuid( assetPath );
	const EAssetType	assetType = String_To_EAssetType( srcValue[ ASSET_TYPE_TAG ].asCString() );
	mxUNUSED(assetType);

	//dst << (U4)assetType;
	dst << assetGuid;

	//if(MX_DEBUG)
	//{
	//	char	tmp[32];
	//	assetGuid.ToChars(tmp);
	//	DBGOUT( " TransformGUID: %s\n", tmp );
	//}
}

static void Transform_Array(
	const Json::Value& srcValue,
	const mxArrayType& arrayInfo,
	AStreamWriter &dst
	)
{
	UNDONE;
}

}//namespace

bool ConvertTextToBinary( AStreamReader& src, AStreamWriter &dst )
{
	Json::Value		rootValue;
	CHK_VRET_FALSE_IF_NOT(JSON::Read_Value_From_Stream( src, rootValue ));

	const Json::Value objectData = rootValue[ NODE_DATA_TAG ];
	const std::string className = rootValue[ NODE_TYPE_TAG ].asString();

	const mxClass* pClassInfo = TypeRegistry::Get().FindClassInfoByName( className.c_str() );
	CHK_VRET_FALSE_IF_NIL(pClassInfo);

	//{
	//	MemoryBlob			tempBlob;
	//	MemoryBlobWriter	memStream( tempBlob );

	//	Transform( objectData, *pClassInfo, memStream );

	//	dst << tempBlob;
	//}
	const TypeGUID classGuid = pClassInfo->GetTypeGuid();
	dst << classGuid;

	Transform( objectData, *pClassInfo, dst );

	return true;
}

AObject* LoadObjectFromFile( AStreamReader& src )
{
UNDONE;
return nil;
}

}//namespace ReflectionUtil



namespace TextFiles
{

static void Visit(
	const Json::Value& srcValue,
	const mxType& typeInfo,
	AObjectVisitor & visitor
	);

static void Visit_Asset_Reference(
	const Json::Value& srcValue,
	const mxAssetReferenceType& handleType,
	AObjectVisitor & visitor
	);

static void Visit_Array(
	const Json::Value& srcValue,
	const mxArrayType& arrayInfo,
	AObjectVisitor & visitor
	);

static void Visit_Class(
	const Json::Value& srcValue,
	const mxClass& classInfo,
	AObjectVisitor & visitor
	);

static void Visit(const Json::Value& srcValue,
				  const mxType& typeInfo,
				  AObjectVisitor & visitor)
{
	const ETypeKind typeKind = typeInfo.m_kind;

	switch( typeKind )
	{
	case ETypeKind::Type_Int8 :
		{
			INT8	value;
			JSON::Deserialize_Int8( srcValue, value );
			visitor.Visit_Int8( typeInfo, value );
		}
		break;

	case ETypeKind::Type_UInt8 :
		{
			UINT8	value;
			JSON::Deserialize_UInt8( srcValue, value );
			visitor.Visit_UInt8( typeInfo, value );
		}
		break;

	case ETypeKind::Type_Int16 :
		{
			INT16	value;
			JSON::Deserialize_Int16( srcValue, value );
			visitor.Visit_Int16( typeInfo, value );
		}
		break;

	case ETypeKind::Type_UInt16 :
		{
			UINT16	value;
			JSON::Deserialize_UInt16( srcValue, value );
			visitor.Visit_UInt16( typeInfo, value );
		}
		break;

	case ETypeKind::Type_Int32 :
		{
			INT32	value;
			JSON::Deserialize_Int32( srcValue, value );
			visitor.Visit_Int32( typeInfo, value );
		}
		break;

	case ETypeKind::Type_UInt32 :
		{
			UINT32	value;
			JSON::Deserialize_UInt32( srcValue, value );
			visitor.Visit_UInt32( typeInfo, value );
		}
		break;

	case ETypeKind::Type_Int64 :
		{
			INT64	value;
			JSON::Deserialize_Int64( srcValue, value );
			visitor.Visit_Int64( typeInfo, value );
		}
		break;

	case ETypeKind::Type_UInt64 :
		{
			UINT64	value;
			JSON::Deserialize_UInt64( srcValue, value );
			visitor.Visit_UInt64( typeInfo, value );
		}
		break;

	case ETypeKind::Type_Float :
		{
			F4	value;
			JSON::Deserialize_Float( srcValue, value );
			visitor.Visit_Float32( typeInfo, value );
		}
		break;

	case ETypeKind::Type_Double :
		{
			F8	value;
			JSON::Deserialize_Double( srcValue, value );
			visitor.Visit_Float64( typeInfo, value );
		}
		break;

	case ETypeKind::Type_Bool :
		{
			bool	value;
			JSON::Deserialize_Boolean( srcValue, value );
			visitor.Visit_Boolean( typeInfo, value );
		}
		break;

	case ETypeKind::Type_SimdQuad :
		{
			float4	value;
			JSON::Deserialize_Float4( srcValue, value );
			visitor.Visit_SimdQuad( typeInfo, value );
		}
		break;

	case ETypeKind::Type_Vec2D :
		{
			Vec2D	value;
			JSON::Deserialize_Vec2D( srcValue, value );
			visitor.Visit_Vec2D( typeInfo, value );
		}
		break;

	case ETypeKind::Type_Vec3D :
		{
			Vec3D	value;
			JSON::Deserialize_Vec3D( srcValue, value );
			visitor.Visit_Vec3D( typeInfo, value );
		}
		break;

	case ETypeKind::Type_Vec4D :
		{
			Vec4D	value;
			JSON::Deserialize_Vec4D( srcValue, value );
			visitor.Visit_Vec4D( typeInfo, value );
		}
		break;

	case ETypeKind::Type_Matrix2 :
		{
			Matrix2	value;
			JSON::Deserialize_Matrix2( srcValue, value );
			visitor.Visit_Matrix2( typeInfo, value );
		}
		break;

	case ETypeKind::Type_Matrix3 :
		{
			Matrix3	value;
			JSON::Deserialize_Matrix3( srcValue, value );
			visitor.Visit_Matrix3( typeInfo, value );
		}
		break;

	case ETypeKind::Type_Matrix4 :
		{
			Matrix4	value;
			JSON::Deserialize_Matrix4( srcValue, value );
			visitor.Visit_Matrix4( typeInfo, value );
		}
		break;

	case ETypeKind::Type_ColorRGBA :
		{
			FColor	value;
			JSON::Deserialize_ColorRGBA( srcValue, value );
			visitor.Visit_ColorRGBA( typeInfo, value );
		}
		break;

	case ETypeKind::Type_String :
		{
			String	value;
			JSON::Deserialize_String( srcValue, value );
			visitor.Visit_String( typeInfo, value );
		}
		break;

#if 0
	case ETypeKind::Type_Enum :
		{
			const mxEnumType& enumInfo = typeInfo.UpCast<mxEnumType>();
			Visit_Enum( srcValue, enumInfo, visitor );
		}
		break;

	case ETypeKind::Type_Flags :
		{
			const mxFlagsType& flagsType = typeInfo.UpCast<mxFlagsType>();
			Visit_Flags( srcValue, flagsType, visitor );
		}
		break;


	case ETypeKind::Type_Struct :
		{
			const mxStruct& structInfo = typeInfo.UpCast<mxStruct>();
			Visit_Struct( srcValue, structInfo, visitor );
		}
		break;
#endif
	case ETypeKind::Type_Class :
		{
			const mxClass& classInfo = typeInfo.UpCast<mxClass>();
			Visit_Class( srcValue, classInfo, visitor );
		}
		break;
#if 0
	case ETypeKind::Type_Pointer :
		{
			const mxPointerType& pointerType = typeInfo.UpCast<mxPointerType>();
			Visit_Pointer( srcValue, pointerType, visitor );
		}
		break;
#endif

	case ETypeKind::Type_AssetRef :
		{
			const mxAssetReferenceType& handleType = typeInfo.UpCast<mxAssetReferenceType>();
			return Visit_Asset_Reference( srcValue, handleType, visitor );
		}
		break;

	case ETypeKind::Type_Array :
		{
			const mxArrayType& arrayInfo = typeInfo.UpCast<mxArrayType>();
			return Visit_Array( srcValue, arrayInfo, visitor );
		}
		break;


	default:
		Unreachable;
	}
}

static void Visit_Parent_Class(
	const Json::Value& srcValue,
	const mxClass& parentClass,
	AObjectVisitor & visitor
	)
{
	if( ObjectUtil::Serializable_Class( parentClass ) )
	{
		const Json::Value parentClassValue = srcValue[ BASE_CLASS_TAG ];

		Visit_Class( parentClassValue, parentClass, visitor );
	}
}

static void Visit_Class_Members(
							const Json::Value& srcValue,
							const mxClassMembers& members,
							AObjectVisitor & visitor
							)
{
	for( UINT iField = 0 ; iField < members.numFields; iField++ )
	{
		const mxField& field = members.fields[ iField ];

		Visit( srcValue[ field.alias ], field.type, visitor );
	}
}


static void Visit_Class(
						const Json::Value& srcValue,
						const mxClass& classInfo,
						AObjectVisitor & visitor
						)
{
	const mxClass* parentClass = classInfo.GetParent();
	if( parentClass != nil )
	{
		Visit_Parent_Class( srcValue, *parentClass, visitor );
	}

	const mxClassMembers& classMembers = classInfo.GetMembers();
	Visit_Class_Members( srcValue, classMembers, visitor );
}

static void Visit_Asset_Reference(
	const Json::Value& srcValue,
	const mxAssetReferenceType& handleType,
	AObjectVisitor & visitor
	)
{
	const String		assetPath = srcValue[ ASSET_PATH_TAG ].asCString();
	const EAssetType	assetType = String_To_EAssetType( srcValue[ ASSET_TYPE_TAG ].asCString() );
	//const ObjectGUID	assetGuid = Resources::AssetPathToGuid( assetPath );
	//mxUNUSED(assetGuid);
	visitor.Visit_Asset_Reference( handleType, assetType, assetPath );
}

static void Visit_Array(
	const Json::Value& srcValue,
	const mxArrayType& arrayInfo,
	AObjectVisitor & visitor
	)
{
	Assert( srcValue.isArray() );

	const UINT numObjects = srcValue.size();

	const mxType& itemType = arrayInfo.m_elemType;

	for( Json::Value::UInt iObject = 0; iObject < numObjects; iObject++ )
	{
		const Json::Value objectValue = srcValue[ iObject ];

		Visit( objectValue, itemType, visitor );
	}
}

/*
-----------------------------------------------------------------------------
	AObjectVisitor
-----------------------------------------------------------------------------
*/

	
/*
-----------------------------------------------------------------------------
	CollectReferencedAssets
-----------------------------------------------------------------------------
*/
class CollectReferencedAssets : public AObjectVisitor
{
	StringListType &	m_usedAssets;

public:
	CollectReferencedAssets( StringListType & usedAssets )
		: m_usedAssets( usedAssets )
	{
		m_usedAssets.Empty();
	}

	virtual void Visit_Asset_Reference(
		const mxAssetReferenceType& pointerType,
		const EAssetType assetType,
		const String& assetPath
		) override
	{
		m_usedAssets.AddUnique( assetPath );
	}
};

void Collect_Referenced_Assets(
	AStreamReader& srcStream,
	const mxType& typeInfo,
	StringListType &usedAssets
	)
{
	Json::Value		rootValue;
	CHK_VRET_IF_NOT(JSON::Read_Value_From_Stream( srcStream, rootValue ));

	const Json::Value objectData = rootValue[ NODE_DATA_TAG ];
	const std::string className = rootValue[ NODE_TYPE_TAG ].asString();

	Assert(mxStrEquAnsi( typeInfo.m_name, className.c_str() ));


	CollectReferencedAssets	collectAssets( usedAssets );

	Visit( objectData, typeInfo, collectAssets );
}

}//namespace TextFiles


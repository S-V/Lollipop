#include <EditorSupport_PCH.h>
#pragma hdrstop
#include <EditorSupport.h>

#include <EditorSupport/Serialization/JsonSerializationCommon.h>

static const bool bEnableJsonComments = true;

const char* NODE_TYPE_TAG = "$TYPE";
const char* NODE_DATA_TAG = "$DATA";

const char* CLASS_NAME_TAG = "$CLASS";
const char* CLASS_DATA_TAG = "$VALUE";
const char* BASE_CLASS_TAG = "$BASE";//$SUPER
const char* ASSET_GUID_TAG = "$ASSET_GUID";
const char* ASSET_PATH_TAG = "$ASSET_PATH";
const char* ASSET_TYPE_TAG = "$ASSET_TYPE";


namespace JSON
{
	Json::Value Serialize_Int8( const INT8 intValue )
	{
		return Json::Value( (Json::Value::Int)intValue );
	}
	Json::Value Serialize_UInt8( const UINT8 uintValue )
	{
		return Json::Value( (Json::Value::UInt)uintValue );
	}

	void Deserialize_Int8( const Json::Value& srcValue, INT8 &dstValue )
	{
		CHK_VRET_IF_NOT( srcValue.isInt() );
		Assert( srcValue.asInt() < MAX_INT8 );
		dstValue = srcValue.asInt();
	}
	void Deserialize_UInt8( const Json::Value& srcValue, UINT8 &dstValue )
	{
		CHK_VRET_IF_NOT( srcValue.isUInt() );
		Assert( srcValue.asUInt() < MAX_UINT8 );
		dstValue = srcValue.asUInt();
	}

	Json::Value Serialize_Int16( const INT16 intValue )
	{
		return Json::Value( (Json::Value::Int)intValue );
	}
	Json::Value Serialize_UInt16( const UINT16 uintValue )
	{
		return Json::Value( (Json::Value::UInt)uintValue );
	}

	void Deserialize_Int16( const Json::Value& srcValue, INT16 &dstValue )
	{
		CHK_VRET_IF_NOT( srcValue.isInt() );
		Assert( srcValue.asInt() < MAX_INT16 );
		dstValue = srcValue.asInt();
	}
	void Deserialize_UInt16( const Json::Value& srcValue, UINT16 &dstValue )
	{
		CHK_VRET_IF_NOT( srcValue.isUInt() );
		Assert( srcValue.asUInt() < MAX_UINT16 );
		dstValue = srcValue.asUInt();
	}

	Json::Value Serialize_Int32( const INT32 intValue )
	{
		return Json::Value( (Json::Value::Int)intValue );
	}
	Json::Value Serialize_UInt32( const UINT32 uintValue )
	{
		return Json::Value( (Json::Value::UInt)uintValue );
	}

	void Deserialize_Int32( const Json::Value& srcValue, INT32 &dstValue )
	{
		CHK_VRET_IF_NOT( srcValue.isInt() );
		Assert( srcValue.asInt() < MAX_INT32 );
		dstValue = srcValue.asInt();
	}
	void Deserialize_UInt32( const Json::Value& srcValue, UINT32 &dstValue )
	{
		//HACK: Json Cpp saves small unsigned integers as signed ints
		if( srcValue.isInt() )
		{
			const Json::Int signedIntValue = srcValue.asInt();

			if( signedIntValue >= Json::Value::minInt
				&& signedIntValue <= Json::Value::maxInt )
			{
				dstValue = signedIntValue;
				return;
			}
		}

		CHK_VRET_IF_NOT( srcValue.isUInt() );
		Assert( srcValue.asUInt() < MAX_UINT32 );
		dstValue = srcValue.asUInt();
	}


	Json::Value Serialize_Int64( const INT64 intValue )
	{
		Signed64_Union	union64;
		union64.v = intValue;

		Json::Value	int64value;
		Serialize_Class_Members( int64value, union64 );
		return int64value;
	}
	Json::Value Serialize_UInt64( const UINT64 uintValue )
	{
		Unsigned64_Union	union64;
		union64.v = uintValue;

		Json::Value	uint64value;
		Serialize_Class_Members( uint64value, union64 );
		return uint64value;
	}

	void Deserialize_Int64( const Json::Value& srcValue, INT64 &dstValue )
	{
		Signed64_Union	union64;
		Deserialize_Class_Members( srcValue, union64 );
		dstValue = union64.v;
	}
	void Deserialize_UInt64( const Json::Value& srcValue, UINT64 &dstValue )
	{
		Unsigned64_Union	union64;
		Deserialize_Class_Members( srcValue, union64 );
		dstValue = union64.v;
	}

	Json::Value Serialize_Float( const F4 floatValue )
	{
		return Json::Value( (double)floatValue );
	}
	void Deserialize_Float( const Json::Value& srcValue, F4 &dstValue )
	{
		CHK_VRET_IF_NOT( srcValue.isDouble() );
		dstValue = srcValue.asDouble();
	}

	Json::Value Serialize_Double( const F8 doubleValue )
	{
		return Json::Value( (double)doubleValue );
	}
	void Deserialize_Double( const Json::Value& srcValue, F8 &dstValue )
	{
		CHK_VRET_IF_NOT( srcValue.isDouble() );
		dstValue = srcValue.asDouble();
	}

	Json::Value Serialize_Boolean( const bool booleanValue )
	{
		return Json::Value( booleanValue );
	}
	void Deserialize_Boolean( const Json::Value& srcValue, bool &dstValue )
	{
		CHK_VRET_IF_NOT( srcValue.isBool() );
		dstValue = srcValue.asBool();
	}

	Json::Value Serialize_Float4( const float4& vectorValue )
	{
		Vec4D v;
		v.quad = vectorValue;
		return Serialize_Vec4D( v );
	}
	void Deserialize_Float4( const Json::Value& srcValue, float4 &dstValue )
	{
		Vec4D v;
		Deserialize_Vec4D( srcValue, v );
		dstValue = v.quad;
	}

	Json::Value Serialize_Vec2D( const Vec2D& vectorValue )
	{
		Json::Value	outValue;
		outValue["X"] = Serialize_Float( vectorValue.x );
		outValue["Y"] = Serialize_Float( vectorValue.y );
		Assert(outValue.isObject());
		return outValue;
	}
	void Deserialize_Vec2D( const Json::Value& srcValue, Vec2D &dstValue )
	{
		Assert( srcValue.isObject() );
		Deserialize_Float( srcValue["X"], dstValue.x );
		Deserialize_Float( srcValue["Y"], dstValue.y );
	}

	Json::Value Serialize_Vec3D( const Vec3D& vectorValue )
	{
		Json::Value	outValue;
		outValue["X"] = Serialize_Float( vectorValue.x );
		outValue["Y"] = Serialize_Float( vectorValue.y );
		outValue["Z"] = Serialize_Float( vectorValue.z );
		Assert(outValue.isObject());
		return outValue;
	}
	void Deserialize_Vec3D( const Json::Value& srcValue, Vec3D &dstValue )
	{
		Assert( srcValue.isObject() );
		Deserialize_Float( srcValue["X"], dstValue.x );
		Deserialize_Float( srcValue["Y"], dstValue.y );
		Deserialize_Float( srcValue["Z"], dstValue.z );
	}

	Json::Value Serialize_Vec4D( const Vec4D& vectorValue )
	{
		Json::Value	outValue;
		outValue["X"] = Serialize_Float( vectorValue.x );
		outValue["Y"] = Serialize_Float( vectorValue.y );
		outValue["Z"] = Serialize_Float( vectorValue.z );
		outValue["W"] = Serialize_Float( vectorValue.w );
		Assert(outValue.isObject());
		return outValue;
	}
	void Deserialize_Vec4D( const Json::Value& srcValue, Vec4D &dstValue )
	{
		Assert( srcValue.isObject() );
		Deserialize_Float( srcValue["X"], dstValue.x );
		Deserialize_Float( srcValue["Y"], dstValue.y );
		Deserialize_Float( srcValue["Z"], dstValue.z );
		Deserialize_Float( srcValue["W"], dstValue.w );
	}

	Json::Value Serialize_Matrix2( const Matrix2& matrixValue )
	{
		Json::Value	outValue;
		outValue["Row0"] = Serialize_Vec2D( matrixValue[0] );
		outValue["Row1"] = Serialize_Vec2D( matrixValue[1] );
		Assert(outValue.isObject());
		return outValue;
	}
	void Deserialize_Matrix2( const Json::Value& srcValue, Matrix2 &dstValue )
	{
		Assert( srcValue.isObject() );
		Deserialize_Vec2D( srcValue["Row0"], dstValue[0] );
		Deserialize_Vec2D( srcValue["Row1"], dstValue[1] );
	}

	Json::Value Serialize_Matrix3( const Matrix3& matrixValue )
	{
		Json::Value	outValue;
		outValue["Column0"] = Serialize_Vec3D( matrixValue[0] );
		outValue["Column1"] = Serialize_Vec3D( matrixValue[1] );
		outValue["Column2"] = Serialize_Vec3D( matrixValue[2] );
		Assert(outValue.isObject());
		return outValue;
	}
	void Deserialize_Matrix3( const Json::Value& srcValue, Matrix3 &dstValue )
	{
		Assert( srcValue.isObject() );
		Deserialize_Vec3D( srcValue["Column0"], dstValue[0] );
		Deserialize_Vec3D( srcValue["Column1"], dstValue[1] );
		Deserialize_Vec3D( srcValue["Column2"], dstValue[2] );
	}


	Json::Value Serialize_Matrix4( const Matrix4& matrixValue )
	{
		Json::Value	outValue;
		outValue["Row0"] = Serialize_Vec4D( matrixValue[0] );
		outValue["Row1"] = Serialize_Vec4D( matrixValue[1] );
		outValue["Row2"] = Serialize_Vec4D( matrixValue[2] );
		outValue["Row3"] = Serialize_Vec4D( matrixValue[3] );
		Assert(outValue.isObject());
		return outValue;
	}
	void Deserialize_Matrix4( const Json::Value& srcValue, Matrix4 &dstValue )
	{
		Assert( srcValue.isObject() );
		Deserialize_Vec4D( srcValue["Row0"], dstValue[0] );
		Deserialize_Vec4D( srcValue["Row1"], dstValue[1] );
		Deserialize_Vec4D( srcValue["Row2"], dstValue[2] );
		Deserialize_Vec4D( srcValue["Row3"], dstValue[2] );
	}


	Json::Value Serialize_ColorRGBA( const FColor& colorValue )
	{
		Json::Value	outValue;
		outValue["R"] = Serialize_Float( colorValue.R );
		outValue["G"] = Serialize_Float( colorValue.G );
		outValue["B"] = Serialize_Float( colorValue.B );
		outValue["A"] = Serialize_Float( colorValue.A );
		Assert(outValue.isObject());
		return outValue;
	}
	void Deserialize_ColorRGBA( const Json::Value& srcValue, FColor &dstValue )
	{
		Assert( srcValue.isObject() );
		Deserialize_Float( srcValue["R"], dstValue.R );
		Deserialize_Float( srcValue["G"], dstValue.G );
		Deserialize_Float( srcValue["B"], dstValue.B );
		Deserialize_Float( srcValue["A"], dstValue.A );
	}


	Json::Value Serialize_String( const String& stringValue )
	{
		Json::Value	outValue;
		outValue = stringValue.ToChars();
		Assert(outValue.isString());
		return outValue;
	}
	void Deserialize_String( const Json::Value& srcValue, String &dstValue )
	{
		CHK_VRET_IF_NOT( srcValue.isString() );
		dstValue = srcValue.asCString();
	}


	Json::Value Serialize(
		const mxType& typeInfo,	// type of serialized object
		const void* rawMem,	// pointer to the start of the memory block
		const UINT offset	// byte offset within the structure
		)
	{
	#if MX_DEBUG_SERIALIZATION
		if( typeInfo.m_alignment ) {
			Assert(IS_ALIGNED_BY( rawMem, typeInfo.m_alignment ));
		}
	#endif // MX_DEBUG_SERIALIZATION

		Json::Value		returnValue;

		const void* objAddr = (BYTE*)rawMem + offset;

		const ETypeKind typeKind = typeInfo.m_kind;

		switch( typeKind )
		{
		case ETypeKind::Type_Int8 :
			{
				const INT8 intValue = TPODHelper< INT8 >::GetConst( objAddr );
				returnValue = Serialize_Int8( intValue );
			}
			break;

		case ETypeKind::Type_UInt8 :
			{
				const UINT8 uintValue = TPODHelper< UINT8 >::GetConst( objAddr );
				returnValue = Serialize_UInt8( uintValue );
			}
			break;

		case ETypeKind::Type_Int16 :
			{
				const INT16 intValue = TPODHelper< INT16 >::GetConst( objAddr );
				returnValue = Serialize_Int16( intValue );
			}
			break;

		case ETypeKind::Type_UInt16 :
			{
				const UINT16 uintValue = TPODHelper< UINT16 >::GetConst( objAddr );
				returnValue = Serialize_UInt16( uintValue );
			}
			break;

		case ETypeKind::Type_Int32 :
			{
				const INT32 intValue = TPODHelper< INT32 >::GetConst( objAddr );
				returnValue = Serialize_Int32( intValue );
			}
			break;

		case ETypeKind::Type_UInt32 :
			{
				const UINT32 uintValue = TPODHelper< UINT32 >::GetConst( objAddr );
				returnValue = Serialize_UInt32( uintValue );
			}
			break;

		case ETypeKind::Type_Int64 :
			{
				const INT64 intValue = TPODHelper< INT64 >::GetConst( objAddr );
				returnValue = Serialize_Int64( intValue );
			}
			break;

		case ETypeKind::Type_UInt64 :
			{
				const UINT64 uintValue = TPODHelper< UINT64 >::GetConst( objAddr );
				returnValue = Serialize_UInt64( uintValue );
			}
			break;

		case ETypeKind::Type_Float :
			{
				const F4 floatValue = TPODHelper< F4 >::GetConst( objAddr );
				returnValue = Serialize_Float( floatValue );
			}
			break;

		case ETypeKind::Type_Double :
			{
				const F8 doubleValue = TPODHelper< F8 >::GetConst( objAddr );
				returnValue = Serialize_Double( doubleValue );
			}
			break;

		case ETypeKind::Type_Bool :
			{
				const bool booleanValue = TPODHelper< bool >::GetConst( objAddr );
				returnValue = Serialize_Boolean( booleanValue );
			}
			break;

		case ETypeKind::Type_SimdQuad :
			{
				const float4 vectorValue = TPODHelper< float4 >::GetConst( objAddr );
				returnValue = Serialize_Float4( vectorValue );
			}
			break;

		case ETypeKind::Type_Vec2D :
			{
				const Vec2D& vectorValue = TPODHelper< Vec2D >::GetConst( objAddr );
				returnValue = Serialize_Vec2D( vectorValue );
			}
			break;

		case ETypeKind::Type_Vec3D :
			{
				const Vec3D& vectorValue = TPODHelper< Vec3D >::GetConst( objAddr );
				returnValue = Serialize_Vec3D( vectorValue );
			}
			break;

		case ETypeKind::Type_Vec4D :
			{
				const Vec4D& vectorValue = TPODHelper< Vec4D >::GetConst( objAddr );
				returnValue = Serialize_Vec4D( vectorValue );
			}
			break;

		case ETypeKind::Type_Matrix2 :
			{
				const Matrix2& matrixValue = TPODHelper< Matrix2 >::GetConst( objAddr );
				returnValue = Serialize_Matrix2( matrixValue );
			}
			break;

		case ETypeKind::Type_Matrix3 :
			{
				const Matrix3& matrixValue = TPODHelper< Matrix3 >::GetConst( objAddr );
				returnValue = Serialize_Matrix3( matrixValue );
			}
			break;

		case ETypeKind::Type_Matrix4 :
			{
				const Matrix4& matrixValue = TPODHelper< Matrix4 >::GetConst( objAddr );
				returnValue = Serialize_Matrix4( matrixValue );
			}
			break;

		case ETypeKind::Type_ColorRGBA :
			{
				const FColor& colorValue = TPODHelper< FColor >::GetConst( objAddr );
				returnValue = Serialize_ColorRGBA( colorValue );
			}
			break;

		case ETypeKind::Type_String :
			{
				const String& stringValue = TPODHelper< String >::GetConst( objAddr );
				returnValue = Serialize_String( stringValue );
			}
			break;

		case ETypeKind::Type_Enum :
			{
				const mxEnumType& enumInfo = typeInfo.UpCast<mxEnumType>();
				returnValue = Serialize_Enum( enumInfo, objAddr );
			}
			break;

		case ETypeKind::Type_Flags :
			{
				const mxFlagsType& flagsType = typeInfo.UpCast<mxFlagsType>();
				returnValue = Serialize_Flags( flagsType, objAddr );
			}
			break;

		case ETypeKind::Type_Struct :
			{
				const mxStruct& structInfo = typeInfo.UpCast<mxStruct>();
				returnValue = Serialize_Struct( structInfo, objAddr );
			}
			break;

		case ETypeKind::Type_Class :
			{
				const mxClass& classInfo = typeInfo.UpCast<mxClass>();
				returnValue = Serialize_Class( classInfo, objAddr );
			}
			break;

		case ETypeKind::Type_Pointer :
			{
				const mxPointerType& pointerType = typeInfo.UpCast<mxPointerType>();
				returnValue = Serialize_Pointer( pointerType, objAddr );
			}
			break;

		case ETypeKind::Type_AssetRef :
			{
				const mxAssetReferenceType& handleType = typeInfo.UpCast<mxAssetReferenceType>();
				returnValue = Serialize_AssetReference( handleType, objAddr );
			}
			break;

		case ETypeKind::Type_Array :
			{
				const mxArrayType& arrayInfo = typeInfo.UpCast<mxArrayType>();
				returnValue = Serialize_Array( arrayInfo, objAddr );
			}
			break;

		default:
			Unreachable;
		}

		if( bEnableJsonComments )
		{
			String	comment;
			comment.Format("// type: %s, size: %u, offset: %u, align: %u\n",
				typeInfo.m_name, typeInfo.m_instanceSize, offset, typeInfo.m_alignment);

			returnValue.setComment(comment, Json::CommentPlacement::commentBefore);
		}

		return returnValue;
	}//Serialize()


	Json::Value Serialize_Enum(
		const mxEnumType& enumInfo,
		const void* rawMem	
		)
	{
		Json::Value		value;

		const UINT currEnumValue = enumInfo.m_accessor.Get_Value( rawMem );

		const UINT iCurrEnumItem = enumInfo.GetItemIndexByValue( currEnumValue );
		CHK_VRET_X_IF_NOT( iCurrEnumItem != INDEX_NONE, value );

		const mxEnumType::Member* enumItems = enumInfo.m_members;
		const mxEnumType::Member& currEnumItem = enumItems[ iCurrEnumItem ];

		value = currEnumItem.alias;

		return value;
	}

	Json::Value Serialize_Flags(
		const mxFlagsType& flagsType,
		const void* rawMem	
		)
	{
		Json::Value		value;

		const UINT currValue = flagsType.m_accessor.Get_Value( rawMem );

		for( UINT iBit = 0; iBit < flagsType.m_numBits; iBit++ )
		{
			const mxFlagsType::Member& rBit = flagsType.m_bits[ iBit ];
			value[ rBit.alias ] = bool( currValue & rBit.value );
		}

		return value;
	}

	Json::Value Serialize_Struct(
		const mxStruct& structInfo,
		const void* rawMem	
		)
	{
#if MX_DEBUG_SERIALIZATION
		DBGOUT("Serialize_Struct '%s' (size %u, align %u)\n"
			, structInfo.m_name, (UINT)structInfo.m_instanceSize, (UINT)structInfo.m_alignment);
#endif // MX_DEBUG_SERIALIZATION

		Json::Value		value;

		const mxClassMembers& structMembers = structInfo.GetMembers();
		Serialize_Class_Members( value, structMembers, rawMem );

		//if( bEnableJsonComments )
		//{
		//	String	comment;
		//	comment.Format("// %s :\n", structInfo.m_name);
		//	value.setComment(comment, Json::CommentPlacement::commentBefore);
		//}

		return value;
	}

	Json::Value Serialize_Class(
		const mxClass& classInfo,
		const void* rawMem
	)
	{
#if MX_DEBUG_SERIALIZATION
		DBGOUT("Serialize_Class '%s' (size %u, align %u)\n"
			, classInfo.GetTypeName(), (UINT)classInfo.GetInstanceSize(), (UINT)classInfo.m_alignment);
#endif // MX_DEBUG_SERIALIZATION

#if MX_DEBUG_SERIALIZATION
		if( DEBUGGED_CLASS::StaticClass() == classInfo )
		{
			DEBUGGED_CLASS* pInstance = (DEBUGGED_CLASS*) rawMem;
			(void)pInstance;
			mxDEBUG_BREAK;
		}
#endif // MX_DEBUG_SERIALIZATION


		Json::Value	classValue;

		// First serialize the parent class.

		const mxClass* parentClass = classInfo.GetParent();
		if( parentClass != nil )
		{
			Serialize_Parent_Class( classValue, *parentClass, rawMem );
		}

		// Now serialize the members of this class.

		const mxClassMembers& classMembers = classInfo.GetMembers();
		Serialize_Class_Members( classValue, classMembers, rawMem );

		return classValue;
	}

	void Serialize_Parent_Class(
		Json::Value & classValue,
		const mxClass& parentClass,
		const void* rawMem
		)
	{
		if( ObjectUtil::Serializable_Class( parentClass ) )
		{
			Json::Value	parentClassValue = Serialize_Class( parentClass, rawMem );

			classValue[ BASE_CLASS_TAG ] = parentClassValue;
			//classValue[ parentClass.GetTypeName() ] = parentClassValue;
			//value[ "Super" ] = parentClassValue;

			if( bEnableJsonComments )
			{
				parentClassValue.setComment("// Base class", Json::CommentPlacement::commentBefore);
			}
		}
	}

	void Serialize_Class_Members(
		Json::Value & classValue,
		const mxClassMembers& members,
		const void* rawMem
		)
	{
		for( UINT iField = 0 ; iField < members.numFields; iField++ )
		{
			const mxField& field = members.fields[ iField ];

			classValue[ field.alias ] = Serialize( field.type, rawMem, field.offset );
		}
	}

	template< class STRUCT >
	void Serialize_Class_Members(
		Json::Value & classValue,
		const STRUCT& objRef
		)
	{
		Serialize_Class_Members( classValue, STRUCT::StaticGetReflection(), &objRef );
	}

	Json::Value Serialize_Array(
		const mxArrayType& arrayInfo,
		const void* rawMem
		)
	{
		const UINT numObjects = arrayInfo.Generic_Get_Count( rawMem );
		const void* pArrayData = arrayInfo.Generic_Get_Data( rawMem );

		const mxType& itemType = arrayInfo.m_elemType;
		const UINT itemSize = itemType.m_instanceSize;

		Json::Value	arrayValue;

		for( Json::Value::UInt iObject = 0; iObject < numObjects; iObject++ )
		{
			const void* pObject = (BYTE*)pArrayData + iObject * itemSize;

			Json::Value	objectValue = Serialize( itemType, pObject, 0/*offset*/ );

			arrayValue.append( objectValue );
		}

		Assert( arrayValue.isArray() );

		return arrayValue;
	}

	Json::Value Serialize_Pointer(
		const mxPointerType& pointerType,
		const void* pointerAddress
		)
	{
		const mxType& pointeeType = pointerType.m_pointeeType;
		Assert( pointeeType.m_kind == ETypeKind::Type_Class );

		const mxClass& pointeeClass = pointeeType.UpCast<mxClass>();
		AssertX( pointeeClass.IsDerivedFrom( AObject::StaticClass() ), "Can only serialize instances of AObjects!" );

		const AObject** pObject = c_cast(const AObject**) pointerAddress;

		const mxClass& dynamicClass = (*pObject)->rttiGetClass();

		Json::Value	objectData = Serialize( dynamicClass, *pObject, 0/*offset*/ );

		// type info is needed is needed for creating object instances during deserialization
		objectData[ CLASS_NAME_TAG ] = dynamicClass.GetTypeName();

		return objectData;
	}

	Json::Value Serialize_AssetReference(
		const mxAssetReferenceType& handleType,
		const void* pointerAddress
		)
	{
		const SResPtrBase* pResPtr = c_cast(const SResPtrBase*) pointerAddress;
		const SResourceObject* objPtr = pResPtr->m_pointer;
		mxUNUSED(objPtr);

		const ObjectGUID assetGuid = pResPtr->GetGUID();
		const EAssetType assetType = handleType.m_assetType;

		Json::Value	handleValue;
		handleValue[ ASSET_PATH_TAG ] = pResPtr->GetPath().ToChars();
		handleValue[ ASSET_TYPE_TAG ] = EAssetType_To_Chars( assetType );

		return handleValue;
	}

	//-----------------------------------------------------------------------

	void Deserialize(
		const Json::Value& srcValue,
		const mxType& typeInfo,	// type of serialized object
		void *objAddr,	// pointer to the start of the memory block
		const FieldFlags flags
		)
	{
		const ETypeKind typeKind = typeInfo.m_kind;

		switch( typeKind )
		{
		case ETypeKind::Type_Int8 :
			{
				INT8 & dstValue = TPODHelper< INT8 >::GetNonConst( objAddr );
				Deserialize_Int8( srcValue, dstValue );
			}
			break;

		case ETypeKind::Type_UInt8 :
			{
				UINT8 & dstValue = TPODHelper< UINT8 >::GetNonConst( objAddr );
				Deserialize_UInt8( srcValue, dstValue );
			}
			break;

		case ETypeKind::Type_Int16 :
			{
				INT16 & dstValue = TPODHelper< INT16 >::GetNonConst( objAddr );
				Deserialize_Int16( srcValue, dstValue );
			}
			break;

		case ETypeKind::Type_UInt16 :
			{
				UINT16 & dstValue = TPODHelper< UINT16 >::GetNonConst( objAddr );
				Deserialize_UInt16( srcValue, dstValue );
			}
			break;

		case ETypeKind::Type_Int32 :
			{
				INT32 & dstValue = TPODHelper< INT32 >::GetNonConst( objAddr );
				Deserialize_Int32( srcValue, dstValue );
			}
			break;

		case ETypeKind::Type_UInt32 :
			{
				UINT32 & dstValue = TPODHelper< UINT32 >::GetNonConst( objAddr );
				Deserialize_UInt32( srcValue, dstValue );
			}
			break;

		case ETypeKind::Type_Int64 :
			{
				INT64 & dstValue = TPODHelper< INT64 >::GetNonConst( objAddr );
				Deserialize_Int64( srcValue, dstValue );
			}
			break;

		case ETypeKind::Type_UInt64 :
			{
				UINT64 & dstValue = TPODHelper< UINT64 >::GetNonConst( objAddr );
				Deserialize_UInt64( srcValue, dstValue );
			}
			break;

		case ETypeKind::Type_Float :
			{
				F4 & dstValue = TPODHelper< F4 >::GetNonConst( objAddr );
				Deserialize_Float( srcValue, dstValue );
			}
			break;

		case ETypeKind::Type_Double :
			{
				F8 & dstValue = TPODHelper< F8 >::GetNonConst( objAddr );
				Deserialize_Double( srcValue, dstValue );
			}
			break;

		case ETypeKind::Type_Bool :
			{
				bool & dstValue = TPODHelper< bool >::GetNonConst( objAddr );
				Deserialize_Boolean( srcValue, dstValue );
			}
			break;

		case ETypeKind::Type_SimdQuad :
			{
				float4 & dstValue = TPODHelper< float4 >::GetNonConst( objAddr );
				Deserialize_Float4( srcValue, dstValue );
			}
			break;

		case ETypeKind::Type_Vec2D :
			{
				Vec2D & dstValue = TPODHelper< Vec2D >::GetNonConst( objAddr );
				Deserialize_Vec2D( srcValue, dstValue );
			}
			break;

		case ETypeKind::Type_Vec3D :
			{
				Vec3D & dstValue = TPODHelper< Vec3D >::GetNonConst( objAddr );
				Deserialize_Vec3D( srcValue, dstValue );
			}
			break;

		case ETypeKind::Type_Vec4D :
			{
				Vec4D & dstValue = TPODHelper< Vec4D >::GetNonConst( objAddr );
				Deserialize_Vec4D( srcValue, dstValue );
			}
			break;

		case ETypeKind::Type_Matrix2 :
			{
				Matrix2 & dstValue = TPODHelper< Matrix2 >::GetNonConst( objAddr );
				Deserialize_Matrix2( srcValue, dstValue );
			}
			break;

		case ETypeKind::Type_Matrix3 :
			{
				Matrix3 & dstValue = TPODHelper< Matrix3 >::GetNonConst( objAddr );
				Deserialize_Matrix3( srcValue, dstValue );
			}
			break;

		case ETypeKind::Type_Matrix4 :
			{
				Matrix4 & dstValue = TPODHelper< Matrix4 >::GetNonConst( objAddr );
				Deserialize_Matrix4( srcValue, dstValue );
			}
			break;

		case ETypeKind::Type_ColorRGBA :
			{
				FColor & dstValue = TPODHelper< FColor >::GetNonConst( objAddr );
				Deserialize_ColorRGBA( srcValue, dstValue );
			}
			break;

		case ETypeKind::Type_String :
			{
				String & dstValue = TPODHelper< String >::GetNonConst( objAddr );
				Deserialize_String( srcValue, dstValue );
			}
			break;

		case ETypeKind::Type_Enum :
			{
				const mxEnumType& enumInfo = typeInfo.UpCast<mxEnumType>();
				Deserialize_Enum( srcValue, enumInfo, objAddr );
			}
			break;

		case ETypeKind::Type_Flags :
			{
				const mxFlagsType& flagsType = typeInfo.UpCast<mxFlagsType>();
				Deserialize_Flags( srcValue, flagsType, objAddr );
			}
			break;


		case ETypeKind::Type_Struct :
			{
				const mxStruct& structInfo = typeInfo.UpCast<mxStruct>();
				Deserialize_Struct( srcValue, structInfo, objAddr );
			}
			break;

		case ETypeKind::Type_Class :
			{
				const mxClass& classInfo = typeInfo.UpCast<mxClass>();
				Deserialize_Class( srcValue, classInfo, objAddr );
			}
			break;

		case ETypeKind::Type_Pointer :
			{
				const mxPointerType& pointerType = typeInfo.UpCast<mxPointerType>();
				Deserialize_Pointer( srcValue, pointerType, objAddr );
			}
			break;

		case ETypeKind::Type_AssetRef :
			{
				const mxAssetReferenceType& handleType = typeInfo.UpCast<mxAssetReferenceType>();
				return Deserialize_Asset_Reference( srcValue, handleType, objAddr, flags );
			}
			break;

		case ETypeKind::Type_Array :
			{
				const mxArrayType& arrayInfo = typeInfo.UpCast<mxArrayType>();
				return Deserialize_Array( srcValue, arrayInfo, objAddr );
			}
			break;

		default:
			Unreachable;
		}

	#if MX_DEBUG_SERIALIZATION
		if( typeInfo.m_alignment ) {
			Assert(IS_ALIGNED_BY( rawMem, typeInfo.m_alignment ));
		}
	#endif // MX_DEBUG_SERIALIZATION
	}


	void Deserialize_Enum(
		const Json::Value& srcValue,
		const mxEnumType& enumInfo,
		void *rawMem	
		)
	{
		const UINT newValue = enumInfo.GetItemIndexByString( srcValue.asCString() );

		enumInfo.m_accessor.Set_Value( rawMem, newValue );
	}

	void Deserialize_Flags(
		const Json::Value& srcValue,
		const mxFlagsType& flagsType,
		void *rawMem	
		)
	{
		UINT newValue = 0;

		for( UINT iBit = 0; iBit < flagsType.m_numBits; iBit++ )
		{
			const mxFlagsType::Member& rBit = flagsType.m_bits[ iBit ];
			if( srcValue[ rBit.alias ].asBool() )
			{
				newValue |= rBit.value;
			}
		}

		flagsType.m_accessor.Set_Value( rawMem, newValue );
	}

	void Deserialize_Struct(
		const Json::Value& srcValue,
		const mxStruct& structInfo,
		void *rawMem	
		)
	{
#if MX_DEBUG_SERIALIZATION
		DBGOUT("Deserialize_Struct '%s' (size %u, align %u)\n"
			, structInfo.m_name, (UINT)structInfo.m_instanceSize, (UINT)structInfo.m_alignment);
#endif // MX_DEBUG_SERIALIZATION

		const mxClassMembers& structMembers = structInfo.GetMembers();
		Deserialize_Class_Members( srcValue, structMembers, rawMem );
	}

	void Deserialize_Class(
		const Json::Value& srcValue,
		const mxClass& classInfo,
		void *rawMem
	)
	{
#if MX_DEBUG_SERIALIZATION
		DBGOUT("Deserialize_Class '%s' (size %u, align %u)\n"
			, classInfo.m_name, (UINT)classInfo.m_instanceSize, (UINT)classInfo.m_alignment);
#endif // MX_DEBUG_SERIALIZATION

		const mxClass* parentClass = classInfo.GetParent();
		if( parentClass != nil )
		{
			Deserialize_Parent_Class( srcValue, *parentClass, rawMem );
		}

		const mxClassMembers& classMembers = classInfo.GetMembers();
		Deserialize_Class_Members( srcValue, classMembers, rawMem );

#if MX_DEBUG_SERIALIZATION
		if( DEBUGGED_CLASS::StaticClass() == classInfo )
		{
			DEBUGGED_CLASS* pInstance = (DEBUGGED_CLASS*) rawMem;
			(void)pInstance;
			mxDEBUG_BREAK;
		}
#endif // MX_DEBUG_SERIALIZATION
	}

	void Deserialize_Parent_Class(
		const Json::Value& srcValue,
		const mxClass& parentClass,
		void *rawMem
		)
	{
		if( ObjectUtil::Serializable_Class( parentClass ) )
		{
			const Json::Value parentClassValue =
				srcValue[ BASE_CLASS_TAG ]
				//srcValue[ parentClass.GetTypeName() ]
				;

			Deserialize_Class( parentClassValue, parentClass, rawMem );
		}
	}

	void Deserialize_Class_Members(
		const Json::Value& srcValue,
		const mxClassMembers& members,
		void *rawMem
		)
	{
		for( UINT iField = 0 ; iField < members.numFields; iField++ )
		{
			const mxField& field = members.fields[ iField ];

			void* fieldPtr = (BYTE*)rawMem + field.offset;

			Deserialize( srcValue[ field.alias ], field.type, fieldPtr, field.flags );
		}
	}

	template< class STRUCT >
	void Deserialize_Class_Members(
		const Json::Value & srcValue,
		STRUCT &objRef
		)
	{
		Deserialize_Class_Members( srcValue, STRUCT::StaticGetReflection(), &objRef );
	}

	void Deserialize_Array(
		const Json::Value& srcValue,
		const mxArrayType& arrayInfo,
		void *rawMem
		)
	{
		Assert( srcValue.isArray() );

		const UINT numObjects = srcValue.size();

		arrayInfo.Generic_Set_Count( rawMem, numObjects );

		const void* pArrayData = arrayInfo.Generic_Get_Data( rawMem );

		const mxType& itemType = arrayInfo.m_elemType;
		const UINT itemSize = itemType.m_instanceSize;

		for( Json::Value::UInt iObject = 0; iObject < numObjects; iObject++ )
		{
			void* pObject = (BYTE*)pArrayData + iObject * itemSize;
			const Json::Value objectValue = srcValue[ iObject ];

			Deserialize( objectValue, itemType, pObject, 0/*offset*/ );
		}
	}

	void Deserialize_Pointer(
		const Json::Value& srcValue,
		const mxPointerType& pointerType,
		void *pointerAddress
		)
	{
		const mxType& pointeeType = pointerType.m_pointeeType;
		CHK_VRET_IF_NOT( pointeeType.m_kind == ETypeKind::Type_Class );

		AssertX( pointeeType.UpCast<mxClass>().IsDerivedFrom( AObject::StaticClass() ),
			"Can only serialize instances of AObjects!" );

		const char* className = srcValue[ CLASS_NAME_TAG ].asCString();
		CHK_VRET_IF_NIL( className );

		const mxClass* pointeeClass = TypeRegistry::Get().FindClassInfoByName( className );
		AssertPtr( pointeeClass );
		if( pointeeClass == nil ) {
			mxErrf("No type info for class '%s'\n", className);
			return;
		}

		AObject* pNewInstance = TypeRegistry::Get().CreateInstance( pointeeClass->GetTypeGuid() );
		AssertPtr( pNewInstance );
		if( pNewInstance == nil ) {
			mxErrf("Failed to create instance of class '%s'\n", className);
			return;
		}

		Deserialize_Class( srcValue, *pointeeClass, pNewInstance );

		AObject** pObject = c_cast(AObject**) pointerAddress;
		*pObject = pNewInstance;
	}

	void Deserialize_Asset_Reference(
		const Json::Value& srcValue,
		const mxAssetReferenceType& handleType,
		void *pointerAddress,
		const FieldFlags flags
		)
	{
		const String		assetPath = srcValue[ ASSET_PATH_TAG ].asCString();
		const ObjectGUID	assetGuid = Resources::AssetPathToGuid( assetPath );
		const EAssetType	assetType = String_To_EAssetType( srcValue[ ASSET_TYPE_TAG ].asCString() );

		DBGOUT("Loading '%s': '%s'\n",EAssetType_To_Chars(assetType),assetPath.ToChars());

		SResPtrBase* pResPtr = c_cast(SResPtrBase*) pointerAddress;

		if( assetGuid.IsNull() && (flags & Field_NoDefaultInit) )
		{
			pResPtr->m_pointer = nil;
			return;
		}

		pResPtr->m_pointer = gCore.resources->GetResourceByGuid( assetType, assetGuid );
	}


	bool Read_Value_From_Stream( AStreamReader& stream, Json::Value & outValue )
	{
		MemoryBlob	fileData(EMemHeap::HeapTemp);
		CHK_VRET_FALSE_IF_NOT(Util_LoadFileToMemory( stream, fileData ));

		const char* start = c_cast(const char*) fileData.ToPtr();
		const char* end = start + fileData.GetDataSize();

		Json::Reader	reader;
		return reader.parse( start, end, outValue, true/*collectComments*/ );
	}

	void DbgPrintValue( const Json::Value& value )
	{
		const Json::Value::Members valueMembers = value.getMemberNames();
		const UINT numMembers = valueMembers.size();
		for( UINT i = 0; i < numMembers; i++ )
		{
			const Json::Value& member = valueMembers[i];
			const std::string s = member.toStyledString();
			DBGOUT( "\t%s\n", s.c_str() );
			//DBGOUT( "\t%s\n", member.c_str() );
		}
	}

};//JSON

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

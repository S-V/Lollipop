#include <Core_PCH.h>
#pragma hdrstop
#include <Core.h>

#include <Core/VectorMath.h>
#include <Core/Serialization.h>
#include <Core/Serialization/BinarySerializationCommon.h>
#include <Core/Resources.h>

namespace BinarySerialization
{
	void Deserialize(
		AStreamReader& stream,
		const mxType& typeInfo,	// type of serialized object
		void* objAddr,	// pointer to the start of the memory block
		const FieldFlags flags
		)
	{
		const ETypeKind typeKind = typeInfo.m_kind;

		switch( typeKind )
		{
		case ETypeKind::Type_Int8 :
			{
				INT8 & dstValue = TPODHelper< INT8 >::GetNonConst( objAddr );
				stream >> dstValue;
			}
			break;

		case ETypeKind::Type_UInt8 :
			{
				UINT8 & dstValue = TPODHelper< UINT8 >::GetNonConst( objAddr );
				stream >> dstValue;
			}
			break;

		case ETypeKind::Type_Int16 :
			{
				INT16 & dstValue = TPODHelper< INT16 >::GetNonConst( objAddr );
				stream >> dstValue;
			}
			break;

		case ETypeKind::Type_UInt16 :
			{
				UINT16 & dstValue = TPODHelper< UINT16 >::GetNonConst( objAddr );
				stream >> dstValue;
			}
			break;

		case ETypeKind::Type_Int32 :
			{
				INT32 & dstValue = TPODHelper< INT32 >::GetNonConst( objAddr );
				stream >> dstValue;
				DBGOUT("\t\tRead Int32: %d\n",dstValue);
			}
			break;

		case ETypeKind::Type_UInt32 :
			{
				UINT32 & dstValue = TPODHelper< UINT32 >::GetNonConst( objAddr );
				stream >> dstValue;
				DBGOUT("\t\tRead UInt32: %d\n",dstValue);
			}
			break;

		case ETypeKind::Type_Int64 :
			{
				INT64 & dstValue = TPODHelper< INT64 >::GetNonConst( objAddr );
				stream >> dstValue;
			}
			break;

		case ETypeKind::Type_UInt64 :
			{
				UINT64 & dstValue = TPODHelper< UINT64 >::GetNonConst( objAddr );
				stream >> dstValue;
			}
			break;

		case ETypeKind::Type_Float :
			{
				F4 & dstValue = TPODHelper< F4 >::GetNonConst( objAddr );
				stream >> dstValue;
				DBGOUT("\t\tRead Float32: %f\n",dstValue);
			}
			break;

		case ETypeKind::Type_Double :
			{
				F8 & dstValue = TPODHelper< F8 >::GetNonConst( objAddr );
				stream >> dstValue;
				DBGOUT("\t\tRead Float64: %lf\n",dstValue);
			}
			break;

		case ETypeKind::Type_Bool :
			{
				bool & dstValue = TPODHelper< bool >::GetNonConst( objAddr );
				stream.Unpack( dstValue );
				DBGOUT("\t\tRead bool: %d\n",dstValue);
			}
			break;

		case ETypeKind::Type_SimdQuad :
			{
				float4 & dstValue = TPODHelper< float4 >::GetNonConst( objAddr );
				stream >> dstValue;
				DBGOUT("\t\tRead SimdQuad: (%.3f, %.3f, %.3f, %.3f)\n",as_vec4(dstValue).x,as_vec4(dstValue).y,as_vec4(dstValue).z,as_vec4(dstValue).w);
			}
			break;

		case ETypeKind::Type_Vec2D :
			{
				Vec2D & dstValue = TPODHelper< Vec2D >::GetNonConst( objAddr );
				stream >> dstValue;
			}
			break;

		case ETypeKind::Type_Vec3D :
			{
				Vec3D & dstValue = TPODHelper< Vec3D >::GetNonConst( objAddr );
				stream >> dstValue;
				DBGOUT("\t\tRead Vec3D: (%.3f, %.3f, %.3f)\n",dstValue.x,dstValue.y,dstValue.z);
			}
			break;

		case ETypeKind::Type_Vec4D :
			{
				Vec4D & dstValue = TPODHelper< Vec4D >::GetNonConst( objAddr );
				stream >> dstValue;
			}
			break;

		case ETypeKind::Type_Matrix2 :
			{
				Matrix2 & dstValue = TPODHelper< Matrix2 >::GetNonConst( objAddr );
				stream >> dstValue;
			}
			break;

		case ETypeKind::Type_Matrix3 :
			{
				Matrix3 & dstValue = TPODHelper< Matrix3 >::GetNonConst( objAddr );
				stream >> dstValue;
			}
			break;

		case ETypeKind::Type_Matrix4 :
			{
				Matrix4 & dstValue = TPODHelper< Matrix4 >::GetNonConst( objAddr );
				stream >> dstValue;
			}
			break;

		case ETypeKind::Type_ColorRGBA :
			{
				FColor & dstValue = TPODHelper< FColor >::GetNonConst( objAddr );
				stream >> dstValue;
				DBGOUT("\t\tRead ColorRGBA: (%.3f, %.3f, %.3f, %.3f)\n",dstValue.R,dstValue.G,dstValue.B,dstValue.A);
			}
			break;

		case ETypeKind::Type_String :
			{
				String & dstValue = TPODHelper< String >::GetNonConst( objAddr );
				stream >> dstValue;
				DBGOUT("\t\tRead String: %s\n",dstValue.ToChars());
			}
			break;

		case ETypeKind::Type_Enum :
			{
				const mxEnumType& enumInfo = typeInfo.UpCast<mxEnumType>();
				const UINT32 enumValue = ReadUInt32( stream );
				enumInfo.m_accessor.Set_Value( objAddr, enumValue );
			}
			break;

		case ETypeKind::Type_Flags :
			{
				const mxFlagsType& flagsType = typeInfo.UpCast<mxFlagsType>();
				const UINT32 flagsValue = ReadUInt32( stream );
				flagsType.m_accessor.Set_Value( objAddr, flagsValue );
			}
			break;

		case ETypeKind::Type_Struct :
			{
				const mxStruct& structInfo = typeInfo.UpCast<mxStruct>();
				Deserialize_Struct( stream, structInfo, objAddr );
			}
			break;

		case ETypeKind::Type_Class :
			{
				const mxClass& classInfo = typeInfo.UpCast<mxClass>();
				Deserialize_Class( stream, classInfo, objAddr );
			}
			break;

		case ETypeKind::Type_Pointer :
			{
				const mxPointerType& pointerType = typeInfo.UpCast<mxPointerType>();
				Deserialize_Pointer( stream, pointerType, objAddr );
			}
			break;

		case ETypeKind::Type_AssetRef :
			{
				const mxAssetReferenceType& handleType = typeInfo.UpCast<mxAssetReferenceType>();
				return Deserialize_Asset_Reference( stream, handleType, objAddr, flags );
			}
			break;

		case ETypeKind::Type_Array :
			{
				const mxArrayType& arrayInfo = typeInfo.UpCast<mxArrayType>();
				return Deserialize_Array( stream, arrayInfo, objAddr );
			}
			break;

		default:
			Unreachable;
		}
	}//Deserialize()


	void Deserialize_Struct(
		AStreamReader &stream,
		const mxStruct& structInfo,
		void *rawMem	
		)
	{
		const mxClassMembers& structMembers = structInfo.GetMembers();
		Deserialize_Class_Members( stream, structMembers, rawMem );
	}

	void Deserialize_Class(
		AStreamReader& stream,
		const mxClass& classInfo,
		void *rawMem
	)
	{
		DBGOUT("Deserialize_Class: %s\n",classInfo.GetTypeName());

		const mxClass* parentClass = classInfo.GetParent();
		if( parentClass != nil )
		{
			Deserialize_Parent_Class( stream, *parentClass, rawMem );
		}

		const mxClassMembers& classMembers = classInfo.GetMembers();
		Deserialize_Class_Members( stream, classMembers, rawMem );
	}

	void Deserialize_Parent_Class(
		AStreamReader& stream,
		const mxClass& parentClass,
		void *rawMem
		)
	{
		if( ObjectUtil::Serializable_Class( parentClass ) )
		{
			Deserialize_Class( stream, parentClass, rawMem );
		}
	}

	void Deserialize_Class_Members(
		AStreamReader& stream,
		const mxClassMembers& members,
		void *rawMem
		)
	{
		for( UINT iField = 0 ; iField < members.numFields; iField++ )
		{
			const mxField& field = members.fields[ iField ];

			void* fieldPtr = (BYTE*)rawMem + field.offset;

			DBGOUT("\tDeserialize_Field: %s\n",field.name);

			Deserialize( stream, field.type, fieldPtr );
		}
	}

	void Deserialize_Asset_Reference(
		AStreamReader& stream,
		const mxAssetReferenceType& handleType,
		void *pointerAddress,
		const FieldFlags flags
		)
	{
		const EAssetType	assetType = handleType.m_assetType;
		const ObjectGUID	assetGuid = ReadObjectGuid( stream );

		//if(MX_DEBUG)
		//{
		//	char	tmp[32];
		//	assetGuid.ToChars(tmp);
		//	DBGOUT( "ReadGUID: %s\n", tmp );
		//}

#if MX_DEBUG_SERIALIZATION
		const String		assetPath = gCore.resources->GetContentDatabase()->GuidToAssetPath( assetGuid );
		DBGOUT("Loading '%s': '%s'\n",EAssetType_To_Chars(assetType),assetPath.ToChars());
#endif // MX_DEBUG_SERIALIZATION

		SResPtrBase* pResPtr = c_cast(SResPtrBase*) pointerAddress;

		if( assetGuid.IsNull() && (flags & Field_NoDefaultInit) )
		{
			pResPtr->m_pointer = nil;
			return;
		}

		pResPtr->m_pointer = gCore.resources->GetResourceByGuid( assetType, assetGuid );
	}


	void Deserialize_Array(
		AStreamReader& stream,
		const mxArrayType& arrayInfo,
		void *objAddr
		)
	{
		const UINT32 numObjects = ReadUInt32( stream );
		arrayInfo.Generic_Set_Count( objAddr, numObjects );

		void* pArrayData = arrayInfo.Generic_Get_Data( objAddr );

		const mxType& itemType = arrayInfo.m_elemType;
		const UINT itemSize = itemType.m_instanceSize;

		for( UINT iObject = 0; iObject < numObjects; iObject++ )
		{
			void* pObject = (BYTE*)pArrayData + iObject * itemSize;

			Deserialize( stream, itemType, pObject );
		}
	}

	void Deserialize_Pointer(
		AStreamReader &stream,
		const mxPointerType& pointerType,
		void* pointerAddress
		)
	{
		const mxType& pointeeType = pointerType.m_pointeeType;
		Assert( pointeeType.m_kind == ETypeKind::Type_Class );

		const TypeGUID classGuid = ReadTypeGuid( stream );

		const mxClass* pointeeClass = TypeRegistry::Get().FindClassInfoByGuid( classGuid );
		AssertPtr(pointeeClass);

		AObject** pObject = c_cast(AObject**) pointerAddress;

		*pObject = ObjectUtil::Create_Object_Instance( classGuid );
		AssertPtr(*pObject);

		Deserialize( stream, *pointeeClass, *pObject );
	}

}//namespace BinarySerialization

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//


#include <Core_PCH.h>
#pragma hdrstop
#include <Core.h>

#include <Core/Object.h>
#include <Core/Object/ObjectVisitor.h>
#include <Core/Serialization.h>
#include <Core/Serialization/BinarySerializationCommon.h>
#include <Core/Resources.h>
#include <Core/VectorMath.h>

mxNAMESPACE_BEGIN

/*
-----------------------------------------------------------------------------
	BinaryObjectWriter
-----------------------------------------------------------------------------
*/
BinaryObjectWriter::BinaryObjectWriter( AStreamWriter& stream )
	: m_stream( stream )
{
}

BinaryObjectWriter::~BinaryObjectWriter()
{
}

//namespace
//{
//	class BinarySerializer : public AObjectVisitor
//	{
//		AStreamWriter &	m_stream;
//
//	public:
//		BinarySerializer( AStreamWriter& stream )
//			: m_stream( stream )
//		{
//		}
//
//		void SaveObject( const void* o, const mxType& typeInfo )
//		{
//			void* nonConstPtr = const_cast< void* >( o );
//			UNDONE;
//			Reflection::ProcessFields( nonConstPtr, typeInfo, this );
//		}
//	};
//
//}//namespace

struct BinarySerialization2
{
	static void Serialize(
		AStreamWriter &stream,
		const mxType& typeInfo,	// type of serialized object
		const void* objAddr	// pointer to the start of the memory block
		)
	{
		const ETypeKind typeKind = typeInfo.m_kind;

		switch( typeKind )
		{
		case ETypeKind::Type_Int8 :
			{
				const INT8 intValue = TPODHelper< INT8 >::GetConst( objAddr );
				stream << intValue;
			}
			break;

		case ETypeKind::Type_UInt8 :
			{
				const UINT8 uintValue = TPODHelper< UINT8 >::GetConst( objAddr );
				stream << uintValue;
			}
			break;

		case ETypeKind::Type_Int16 :
			{
				const INT16 intValue = TPODHelper< INT16 >::GetConst( objAddr );
				stream << intValue;
			}
			break;

		case ETypeKind::Type_UInt16 :
			{
				const UINT16 uintValue = TPODHelper< UINT16 >::GetConst( objAddr );
				stream << uintValue;
			}
			break;

		case ETypeKind::Type_Int32 :
			{
				const INT32 intValue = TPODHelper< INT32 >::GetConst( objAddr );
				stream << intValue;
			}
			break;

		case ETypeKind::Type_UInt32 :
			{
				const UINT32 uintValue = TPODHelper< UINT32 >::GetConst( objAddr );
				stream << uintValue;
			}
			break;

		case ETypeKind::Type_Int64 :
			{
				const INT64 intValue = TPODHelper< INT64 >::GetConst( objAddr );
				stream << intValue;
			}
			break;

		case ETypeKind::Type_UInt64 :
			{
				const UINT64 uintValue = TPODHelper< UINT64 >::GetConst( objAddr );
				stream << uintValue;
			}
			break;

		case ETypeKind::Type_Float :
			{
				const F4 floatValue = TPODHelper< F4 >::GetConst( objAddr );
				stream << floatValue;
				//DBGOUT("\t\tWrite Float32: %f\n",floatValue);
			}
			break;

		case ETypeKind::Type_Double :
			{
				const F8 doubleValue = TPODHelper< F8 >::GetConst( objAddr );
				stream << doubleValue;
				//DBGOUT("\t\tWrite Float64: %lf\n",doubleValue);
			}
			break;

		case ETypeKind::Type_Bool :
			{
				const bool booleanValue = TPODHelper< bool >::GetConst( objAddr );
				stream << booleanValue;
			}
			break;

		case ETypeKind::Type_SimdQuad :
			{
				const float4 vectorValue = TPODHelper< float4 >::GetConst( objAddr );
				stream << vectorValue;
			}
			break;

		case ETypeKind::Type_Vec2D :
			{
				const Vec2D& vectorValue = TPODHelper< Vec2D >::GetConst( objAddr );
				stream << vectorValue;
			}
			break;

		case ETypeKind::Type_Vec3D :
			{
				const Vec3D& vectorValue = TPODHelper< Vec3D >::GetConst( objAddr );
				stream << vectorValue;
			}
			break;

		case ETypeKind::Type_Vec4D :
			{
				const Vec4D& vectorValue = TPODHelper< Vec4D >::GetConst( objAddr );
				stream << vectorValue;
			}
			break;

		case ETypeKind::Type_Matrix2 :
			{
				const Matrix2& matrixValue = TPODHelper< Matrix2 >::GetConst( objAddr );
				stream << matrixValue;
			}
			break;

		case ETypeKind::Type_Matrix3 :
			{
				const Matrix3& matrixValue = TPODHelper< Matrix3 >::GetConst( objAddr );
				stream << matrixValue;
			}
			break;

		case ETypeKind::Type_Matrix4 :
			{
				const Matrix4& matrixValue = TPODHelper< Matrix4 >::GetConst( objAddr );
				stream << matrixValue;
			}
			break;

		case ETypeKind::Type_ColorRGBA :
			{
				const FColor& colorValue = TPODHelper< FColor >::GetConst( objAddr );
				stream << colorValue;
			}
			break;

		case ETypeKind::Type_String :
			{
				const String& stringValue = TPODHelper< String >::GetConst( objAddr );
				stream << stringValue;
			}
			break;

		case ETypeKind::Type_Enum :
			{
				const mxEnumType& enumInfo = typeInfo.UpCast<mxEnumType>();
				const UINT32 enumValue = enumInfo.m_accessor.Get_Value( objAddr );
				stream << enumValue;
			}
			break;

		case ETypeKind::Type_Flags :
			{
				const mxFlagsType& flagsType = typeInfo.UpCast<mxFlagsType>();
				const UINT32 flagsValue = flagsType.m_accessor.Get_Value( objAddr );
				stream << flagsValue;
			}
			break;

		case ETypeKind::Type_Struct :
			{
				const mxStruct& structInfo = typeInfo.UpCast<mxStruct>();
				Serialize_Struct( stream, structInfo, objAddr );
			}
			break;

		case ETypeKind::Type_Class :
			{
				const mxClass& classInfo = typeInfo.UpCast<mxClass>();
				Serialize_Class( stream, classInfo, objAddr );
			}
			break;

		case ETypeKind::Type_Pointer :
			{
				const mxPointerType& pointerType = typeInfo.UpCast<mxPointerType>();
				Serialize_Pointer( stream, pointerType, objAddr );
			}
			break;

		case ETypeKind::Type_AssetRef :
			{
				const mxAssetReferenceType& handleType = typeInfo.UpCast<mxAssetReferenceType>();
				Serialize_AssetReference( stream, handleType, objAddr );
			}
			break;

		case ETypeKind::Type_Array :
			{
				const mxArrayType& arrayInfo = typeInfo.UpCast<mxArrayType>();
				Serialize_Array( stream, arrayInfo, objAddr );
			}
			break;

		default:
			Unreachable;
		}
	}//Serialize()

	static void Serialize_Struct(
		AStreamWriter &stream,
		const mxStruct& structInfo,
		const void* rawMem	
		)
	{
		const mxClassMembers& structMembers = structInfo.GetMembers();
		Serialize_Class_Members( stream, structMembers, rawMem );
	}

	static void Serialize_Class(
		AStreamWriter &stream,
		const mxClass& classInfo,
		const void* rawMem
	)
	{
		// First serialize the parent class.

		const mxClass* parentClass = classInfo.GetParent();
		if( parentClass != nil )
		{
			Serialize_Parent_Class( stream, *parentClass, rawMem );
		}

		// Now serialize the members of this class.

		const mxClassMembers& classMembers = classInfo.GetMembers();
		Serialize_Class_Members( stream, classMembers, rawMem );
	}

	static void Serialize_Parent_Class(
		AStreamWriter &stream,
		const mxClass& parentClass,
		const void* rawMem
		)
	{
		// the root classes don't have any serializable state
		if( parentClass != SBaseType::StaticClass() && parentClass != AObject::StaticClass() )
		{
			Serialize_Class( stream, parentClass, rawMem );
		}
	}

	static void Serialize_Class_Members(
		AStreamWriter &stream,
		const mxClassMembers& members,
		const void* rawMem
		)
	{
		for( UINT iField = 0 ; iField < members.numFields; iField++ )
		{
			const mxField& field = members.fields[ iField ];

			const void* fieldPtr = (BYTE*)rawMem + field.offset;

			Serialize( stream, field.type, fieldPtr );
		}
	}

	static void Serialize_Array(
		AStreamWriter &stream,
		const mxArrayType& arrayInfo,
		const void* rawMem
		)
	{
		const UINT32 numObjects = arrayInfo.Generic_Get_Count( rawMem );
		const void* pArrayData = arrayInfo.Generic_Get_Data( rawMem );

		stream << numObjects;

		const mxType& itemType = arrayInfo.m_elemType;
		const UINT itemSize = itemType.m_instanceSize;

		for( UINT iObject = 0; iObject < numObjects; iObject++ )
		{
			const void* pObject = (BYTE*)pArrayData + iObject * itemSize;

			Serialize( stream, itemType, pObject );
		}
	}

	static void Serialize_Pointer(
		AStreamWriter &stream,
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

		// persistent type code is needed is needed for creating object instances during deserialization
		stream << dynamicClass.GetTypeGuid();

		Serialize( stream, dynamicClass, *pObject );
	}

	static void Serialize_AssetReference(
		AStreamWriter &stream,
		const mxAssetReferenceType& handleType,
		const void* pointerAddress
		)
	{
		const SResPtrBase* pResPtr = c_cast(const SResPtrBase*) pointerAddress;

		const EAssetType assetType = handleType.m_assetType;
		const ObjectGUID assetGuid = pResPtr->GetGUID();

		//stream << (U4)assetType;
		mxUNUSED(assetType);

		WriteObjectGuid( stream, assetGuid );

		//if(MX_DEBUG)
		//{
		//	char	tmp[32];
		//	assetGuid.ToChars(tmp);
		//	DBGOUT( "SerializeGUID: %s\n", tmp );
		//}
	}

};//BinarySerialization2

void BinaryObjectWriter::Serialize( const void* o, const mxType& typeInfo )
{
	CHK_VRET_IF_NIL( o );

	//BinarySerializer	serializer( m_stream );
	//serializer.SaveObject( o, typeInfo );

	BinarySerialization2::Serialize( m_stream, typeInfo, o );
}

mxNAMESPACE_END

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//

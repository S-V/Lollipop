#include <Core_PCH.h>
#pragma hdrstop
#include <Core.h>

#include <Core/Object/ObjectVisitor.h>

namespace BinaryFiles
{

/*
-----------------------------------------------------------------------------
	AObjectVisitor
-----------------------------------------------------------------------------
*/


void AObjectVisitor::Visit_Pointer(
	const mxPointerType& pointerType,
	const void* pointerAddress
	)
{
	const mxType& pointeeType = pointerType.m_pointeeType;
	void* pObject = *(void**)pointerAddress;
	if( pObject != nil )
	{
		Visit( pointeeType, pObject, *this );
	}
}

//---------------------------------------------------------------------------

void Visit(
		   const mxType& typeInfo,
		   const void* objectPtr,
		   AObjectVisitor & visitor
		   )
{
	AssertPtr(objectPtr);

	const ETypeKind typeKind = typeInfo.m_kind;

	switch( typeKind )
	{
	case ETypeKind::Type_Int8 :
		{
			const INT8 value = TPODHelper< INT8 >::GetConst( objectPtr );
			visitor.Visit_Int8( typeInfo, value );
		}
		break;

	case ETypeKind::Type_UInt8 :
		{
			const UINT8 value = TPODHelper< UINT8 >::GetConst( objectPtr );
			visitor.Visit_UInt8( typeInfo, value );
		}
		break;

	case ETypeKind::Type_Int16 :
		{
			const INT16 value = TPODHelper< INT16 >::GetConst( objectPtr );
			visitor.Visit_Int16( typeInfo, value );
		}
		break;

	case ETypeKind::Type_UInt16 :
		{
			const UINT16 value = TPODHelper< UINT16 >::GetConst( objectPtr );
			visitor.Visit_UInt16( typeInfo, value );
		}
		break;

	case ETypeKind::Type_Int32 :
		{
			const INT32 value = TPODHelper< INT32 >::GetConst( objectPtr );
			visitor.Visit_Int32( typeInfo, value );
		}
		break;

	case ETypeKind::Type_UInt32 :
		{
			const UINT32 value = TPODHelper< UINT32 >::GetConst( objectPtr );
			visitor.Visit_UInt32( typeInfo, value );
		}
		break;

	case ETypeKind::Type_Int64 :
		{
			const INT64 value = TPODHelper< INT64 >::GetConst( objectPtr );
			visitor.Visit_Int64( typeInfo, value );
		}
		break;

	case ETypeKind::Type_UInt64 :
		{
			const UINT64 value = TPODHelper< UINT64 >::GetConst( objectPtr );
			visitor.Visit_UInt64( typeInfo, value );
		}
		break;

	case ETypeKind::Type_Float :
		{
			const F4 value = TPODHelper< F4 >::GetConst( objectPtr );
			visitor.Visit_Float32( typeInfo, value );
		}
		break;

	case ETypeKind::Type_Double :
		{
			const F8 value = TPODHelper< F8 >::GetConst( objectPtr );
			visitor.Visit_Float64( typeInfo, value );
		}
		break;

	case ETypeKind::Type_Bool :
		{
			const bool value = TPODHelper< bool >::GetConst( objectPtr );
			visitor.Visit_Boolean( typeInfo, value );
		}
		break;

	case ETypeKind::Type_SimdQuad :
		{
			const float4 value = TPODHelper< float4 >::GetConst( objectPtr );
			visitor.Visit_SimdQuad( typeInfo, value );
		}
		break;

	case ETypeKind::Type_Vec2D :
		{
			const Vec2D& value = TPODHelper< Vec2D >::GetConst( objectPtr );
			visitor.Visit_Vec2D( typeInfo, value );
		}
		break;

	case ETypeKind::Type_Vec3D :
		{
			const Vec3D& value = TPODHelper< Vec3D >::GetConst( objectPtr );
			visitor.Visit_Vec3D( typeInfo, value );
		}
		break;

	case ETypeKind::Type_Vec4D :
		{
			const Vec4D& value = TPODHelper< Vec4D >::GetConst( objectPtr );
			visitor.Visit_Vec4D( typeInfo, value );
		}
		break;

	case ETypeKind::Type_Matrix2 :
		{
			const Matrix2& value = TPODHelper< Matrix2 >::GetConst( objectPtr );
			visitor.Visit_Matrix2( typeInfo, value );
		}
		break;

	case ETypeKind::Type_Matrix3 :
		{
			const Matrix3& value = TPODHelper< Matrix3 >::GetConst( objectPtr );
			visitor.Visit_Matrix3( typeInfo, value );
		}
		break;

	case ETypeKind::Type_Matrix4 :
		{
			const Matrix4& value = TPODHelper< Matrix4 >::GetConst( objectPtr );
			visitor.Visit_Matrix4( typeInfo, value );
		}
		break;

	case ETypeKind::Type_ColorRGBA :
		{
			const FColor& value = TPODHelper< FColor >::GetConst( objectPtr );
			visitor.Visit_ColorRGBA( typeInfo, value );
		}
		break;


	case ETypeKind::Type_String :
		{
			const String& value = TPODHelper< String >::GetConst( objectPtr );
			visitor.Visit_String( typeInfo, value );
		}
		break;


	case ETypeKind::Type_Enum :
		mxDBG_UNIMPLEMENTED;
		break;

	case ETypeKind::Type_Flags :
		mxDBG_UNIMPLEMENTED;
		break;

	case ETypeKind::Type_Struct :
		{
			const mxStruct& structInfo = typeInfo.UpCast<mxStruct>();
			return Visit_Struct( structInfo, objectPtr, visitor );
		}
		break;

	case ETypeKind::Type_Class :
		{
			const mxClass& classInfo = typeInfo.UpCast<mxClass>();
			return Visit_Class( classInfo, objectPtr, visitor );
		}
		break;

	case ETypeKind::Type_Pointer :
		{
			const mxPointerType& pointerType = typeInfo.UpCast<mxPointerType>();
			return visitor.Visit_Pointer( pointerType, objectPtr );
		}
		break;

	case ETypeKind::Type_AssetRef :
		{
			const mxAssetReferenceType& handleType = typeInfo.UpCast<mxAssetReferenceType>();
			const SResPtrBase* pResPtr = (const SResPtrBase*) objectPtr;
			return visitor.Visit_Asset_Reference( handleType, *pResPtr );
		}
		break;

	case ETypeKind::Type_Array :
		{
			const mxArrayType& arrayInfo = typeInfo.UpCast<mxArrayType>();
			return Visit_Array( arrayInfo, objectPtr, visitor );
		}
		break;

	default:
		Unreachable;
	}
}

void Visit_Struct(
				  const mxStruct& structInfo,
				  const void* objectPtr,
				  AObjectVisitor & visitor
				  )
{
	const mxClassMembers& structMembers = structInfo.GetMembers();

	Visit_Class_Members( structMembers, objectPtr, visitor );
}

static void Visit_Parent_Class(
							   const mxClass& parentClass,
							   const void *objectPtr,
							   AObjectVisitor & visitor
							   )
{
	if( ObjectUtil::Serializable_Class( parentClass ) )
	{
		Visit_Class( parentClass, objectPtr, visitor );
	}
}

void Visit_Class(
				 const mxClass& classInfo,
				 const void* objectPtr,
				 AObjectVisitor & visitor
				 )
{
	const mxClass* parentClass = classInfo.GetParent();
	if( parentClass != nil )
	{
		Visit_Parent_Class( *parentClass, objectPtr, visitor );
	}

	const mxClassMembers& classMembers = classInfo.GetMembers();
	Visit_Class_Members( classMembers, objectPtr, visitor );
}

void Visit_Class_Members(
						 const mxClassMembers& members,
						 const void* objectPtr,
						 AObjectVisitor & visitor
						 )
{
	AssertPtr(objectPtr);
	for( UINT iField = 0 ; iField < members.numFields; iField++ )
	{
		const mxField& field = members.fields[ iField ];

		const void* fieldPtr = (BYTE*)objectPtr + field.offset;

		Visit( field.type, fieldPtr, visitor );
	}
}

void Visit_Array(
				 const mxArrayType& arrayInfo,
				 const void* objectPtr,
				 AObjectVisitor & visitor
				 )
{
	const U4 numObjects = arrayInfo.Generic_Get_Count( objectPtr );
	const void* pArrayData = arrayInfo.Generic_Get_Data( objectPtr );

	const mxType& itemType = arrayInfo.m_elemType;
	const UINT itemSize = itemType.m_instanceSize;

	for( UINT iObject = 0; iObject < numObjects; iObject++ )
	{
		const void* pObject = (BYTE*)pArrayData + iObject * itemSize;

		Visit( itemType, pObject, visitor );
	}
}

/*
-----------------------------------------------------------------------------
	CollectReferencedAssets
-----------------------------------------------------------------------------
*/
CollectReferencedAssets::CollectReferencedAssets( StringListType & usedAssets )
	: m_usedAssets( usedAssets )
{
	m_usedAssets.Empty();
}

void CollectReferencedAssets::Visit_Asset_Reference(
	const mxAssetReferenceType& pointerType,
	const SResPtrBase& assetReference
	)
{
	if( assetReference.IsValid() )
	{
		const String assetPath = assetReference.GetPath();
		Assert( !assetPath.IsEmpty() );

		//m_usedAssets.AddUnique( assetPath );
		if( !m_usedAssets.Contains( assetPath ) )
		{
			DBGOUT("Visit_Asset_Reference: %s\n", assetPath.ToChars());
			m_usedAssets.Add( assetPath );

			// recursively process all resources referenced by this asset

			UNDONE;
		}
	}
}

}//namespace BinaryFiles

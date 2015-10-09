/*
=============================================================================
	File:	ObjectVisitor.h
	ToDo:	Split into Visitor and ConstVisitor?
=============================================================================
*/
#pragma once

#include <Base/Object/Reflection.h>
#include <Core/Resources.h>
#include <Core/VectorMath.h>	// for float4

namespace BinaryFiles
{

/*
-----------------------------------------------------------------------------
	AObjectVisitor
-----------------------------------------------------------------------------
*/
class AObjectVisitor
{
public:
	//virtual void ProcessBytes( void* ptrData, UINT numBytes ) {}

	virtual void Visit_Int8(
		const mxType& typeInfo,
		const INT8 value
		) {}

	virtual void Visit_UInt8(
		const mxType& typeInfo,
		const UINT8 value
		) {}

	virtual void Visit_Int16(
		const mxType& typeInfo,
		const INT16 value
		) {}

	virtual void Visit_UInt16(
		const mxType& typeInfo,
		const UINT16 value
		) {}


	virtual void Visit_Int32(
		const mxType& typeInfo,
		const INT32 value
		) {}

	virtual void Visit_UInt32(
		const mxType& typeInfo,
		const UINT32 value
		) {}


	virtual void Visit_Int64(
		const mxType& typeInfo,
		const INT64 value
		) {}

	virtual void Visit_UInt64(
		const mxType& typeInfo,
		const UINT64 value
		) {}

	virtual void Visit_Float32(
		const mxType& typeInfo,
		const F4 value
		) {}

	virtual void Visit_Float64(
		const mxType& typeInfo,
		const F8 value
		) {}

	virtual void Visit_SimdQuad(
		const mxType& typeInfo,
		const float4 value
		) {}

	virtual void Visit_Boolean(
		const mxType& typeInfo,
		const bool value
		) {}

	virtual void Visit_Vec2D(
		const mxType& typeInfo,
		const Vec2D& value
		) {}

	virtual void Visit_Vec3D(
		const mxType& typeInfo,
		const Vec3D& value
		) {}

	virtual void Visit_Vec4D(
		const mxType& typeInfo,
		const Vec4D& value
		) {}

	virtual void Visit_Matrix2(
		const mxType& typeInfo,
		const Matrix2& value
		) {}

	virtual void Visit_Matrix3(
		const mxType& typeInfo,
		const Matrix3& value
		) {}

	virtual void Visit_Matrix4(
		const mxType& typeInfo,
		const Matrix4& value
		) {}

	virtual void Visit_ColorRGBA(
		const mxType& typeInfo,
		const FColor& value
		) {}

	virtual void Visit_String(
		const mxType& typeInfo,
		const String& value
		) {}

	virtual void Visit_Pointer(
		const mxPointerType& pointerType,
		const void* pointerAddress
		);

	virtual void Visit_Asset_Reference(
		const mxAssetReferenceType& pointerType,
		const SResPtrBase& assetReference
		) {};

protected:
	virtual ~AObjectVisitor() {}
};

//---------------------------------------------------------------------------

void Visit(
		   const mxType& typeInfo,
		   const void* objectPtr,
		   AObjectVisitor & visitor
		   );

void Visit_Struct(
				  const mxStruct& structInfo,
				  const void* objectPtr,
				  AObjectVisitor & visitor
				  );

void Visit_Class(
				 const mxClass& classInfo,
				 const void* objectPtr,
				 AObjectVisitor & visitor
				 );

void Visit_Class_Members(
						 const mxClassMembers& members,
						 const void* objectPtr,
						 AObjectVisitor & visitor
						 );

//void Visit_Pointer(
//				   const mxPointerType& pointerType,
//				   void* objectPtr,
//				   AObjectVisitor & visitor
//				   );

//void Visit_Asset_Reference(
//						   const mxAssetReferenceType& handleType,
//						   void* objectPtr,
//						   AObjectVisitor & visitor
//						   );

void Visit_Array(
				 const mxArrayType& arrayInfo,
				 const void* objectPtr,
				 AObjectVisitor & visitor
				 );

/*
-----------------------------------------------------------------------------
	CollectReferencedAssets
-----------------------------------------------------------------------------
*/

//typedef TList< ObjectGUID > AssetGuidsList;

class CollectReferencedAssets : public AObjectVisitor
{
	StringListType &	m_usedAssets;

public:
	CollectReferencedAssets( StringListType & usedAssets );

	virtual void Visit_Asset_Reference(
		const mxAssetReferenceType& pointerType,
		const SResPtrBase& assetReference
		) override;
};

}//namespace BinaryFiles

//@todo: refactor
// 
#pragma once

namespace ReflectionUtil
{

//UINT CalculateObjectSize();

bool ConvertTextToBinary( AStreamReader& src, AStreamWriter &dst );

AObject* LoadObjectFromFile( AStreamReader& src );

}//namespace ReflectionUtil

namespace TextFiles
{

/*
-----------------------------------------------------------------------------
	AObjectVisitor
-----------------------------------------------------------------------------
*/
class AObjectVisitor
{public:

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

	virtual void Visit_Asset_Reference(
		const mxAssetReferenceType& pointerType,
		const EAssetType assetType,
		const String& assetPath		
		) {}

protected:
	virtual ~AObjectVisitor() {}
};

void Collect_Referenced_Assets(
	AStreamReader& srcStream,
	const mxType& typeInfo,
	StringListType &usedAssets
	);

}//namespace TextFiles

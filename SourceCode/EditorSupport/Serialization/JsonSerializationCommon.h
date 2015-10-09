// JsonSerializationCommon.h
#pragma once

#include <Core/Serialization.h>

#include <EditorSupport/Serialization/JSON/json.h>


#if MX_DEBUG_SERIALIZATION

	typedef SBaseType DEBUGGED_CLASS;
	//typedef StaticModelBatch DEBUGGED_CLASS;

#endif // MX_DEBUG_SERIALIZATION

//---------------------------------------------------------------------------

extern const char* NODE_TYPE_TAG;
extern const char* NODE_DATA_TAG;

extern const char* CLASS_NAME_TAG;
extern const char* CLASS_DATA_TAG;
extern const char* BASE_CLASS_TAG;
extern const char* ASSET_GUID_TAG;
extern const char* ASSET_PATH_TAG;
extern const char* ASSET_TYPE_TAG;

//---------------------------------------------------------------------------

namespace JSON
{
	Json::Value Serialize_Int8( const INT8 intValue );
	Json::Value Serialize_UInt8( const UINT8 uintValue );

	void Deserialize_Int8( const Json::Value& srcValue, INT8 &dstValue );
	void Deserialize_UInt8( const Json::Value& srcValue, UINT8 &dstValue );

	Json::Value Serialize_Int16( const INT16 intValue );
	Json::Value Serialize_UInt16( const UINT16 uintValue );

	void Deserialize_Int16( const Json::Value& srcValue, INT16 &dstValue );
	void Deserialize_UInt16( const Json::Value& srcValue, UINT16 &dstValue );

	Json::Value Serialize_Int32( const INT32 intValue );
	Json::Value Serialize_UInt32( const UINT32 uintValue );

	void Deserialize_Int32( const Json::Value& srcValue, INT32 &dstValue );
	void Deserialize_UInt32( const Json::Value& srcValue, UINT32 &dstValue );


	Json::Value Serialize_Int64( const INT64 intValue );
	Json::Value Serialize_UInt64( const UINT64 uintValue );

	void Deserialize_Int64( const Json::Value& srcValue, INT64 &dstValue );
	void Deserialize_UInt64( const Json::Value& srcValue, UINT64 &dstValue );

	Json::Value Serialize_Float( const F4 floatValue );
	void Deserialize_Float( const Json::Value& srcValue, F4 &dstValue );

	Json::Value Serialize_Double( const F8 doubleValue );
	void Deserialize_Double( const Json::Value& srcValue, F8 &dstValue );

	Json::Value Serialize_Boolean( const bool booleanValue );
	void Deserialize_Boolean( const Json::Value& srcValue, bool &dstValue );

	Json::Value Serialize_Float4( const float4& vectorValue );
	void Deserialize_Float4( const Json::Value& srcValue, float4 &dstValue );

	Json::Value Serialize_Vec2D( const Vec2D& vectorValue );
	void Deserialize_Vec2D( const Json::Value& srcValue, Vec2D &dstValue );

	Json::Value Serialize_Vec3D( const Vec3D& vectorValue );
	void Deserialize_Vec3D( const Json::Value& srcValue, Vec3D &dstValue );

	Json::Value Serialize_Vec4D( const Vec4D& vectorValue );
	void Deserialize_Vec4D( const Json::Value& srcValue, Vec4D &dstValue );

	Json::Value Serialize_Matrix2( const Matrix2& matrixValue );
	void Deserialize_Matrix2( const Json::Value& srcValue, Matrix2 &dstValue );

	Json::Value Serialize_Matrix3( const Matrix3& matrixValue );
	void Deserialize_Matrix3( const Json::Value& srcValue, Matrix3 &dstValue );


	Json::Value Serialize_Matrix4( const Matrix4& matrixValue );
	void Deserialize_Matrix4( const Json::Value& srcValue, Matrix4 &dstValue );


	Json::Value Serialize_ColorRGBA( const FColor& colorValue );
	void Deserialize_ColorRGBA( const Json::Value& srcValue, FColor &dstValue );


	Json::Value Serialize_String( const String& stringValue );
	void Deserialize_String( const Json::Value& srcValue, String &dstValue );


	Json::Value Serialize(
		const mxType& typeInfo,	// type of serialized object
		const void* rawMem,	// pointer to the start of the memory block
		const UINT offset	// byte offset within the structure
		);


	Json::Value Serialize_Enum(
		const mxEnumType& enumInfo,
		const void* rawMem	
		);

	Json::Value Serialize_Flags(
		const mxFlagsType& flagsType,
		const void* rawMem	
		);

	Json::Value Serialize_Struct(
		const mxStruct& structInfo,
		const void* rawMem	
		);

	Json::Value Serialize_Class(
		const mxClass& classInfo,
		const void* rawMem
	);

	void Serialize_Parent_Class(
		Json::Value & classValue,
		const mxClass& parentClass,
		const void* rawMem
		);

	void Serialize_Class_Members(
		Json::Value & classValue,
		const mxClassMembers& members,
		const void* rawMem
		);

	template< class STRUCT >
	void Serialize_Class_Members(
		Json::Value & classValue,
		const STRUCT& objRef
		);

	Json::Value Serialize_Array(
		const mxArrayType& arrayInfo,
		const void* rawMem
		);

	Json::Value Serialize_Pointer(
		const mxPointerType& pointerType,
		const void* pointerAddress
		);

	Json::Value Serialize_AssetReference(
		const mxAssetReferenceType& handleType,
		const void* pointerAddress
		);

	//-----------------------------------------------------------------------

	void Deserialize(
		const Json::Value& srcValue,
		const mxType& typeInfo,	// type of serialized object
		void *objAddr,	// pointer to the start of the memory block
		const FieldFlags flags = Field_DefaultFlags
		);


	void Deserialize_Enum(
		const Json::Value& srcValue,
		const mxEnumType& enumInfo,
		void *rawMem	
		);

	void Deserialize_Flags(
		const Json::Value& srcValue,
		const mxFlagsType& flagsType,
		void *rawMem	
		);

	void Deserialize_Struct(
		const Json::Value& srcValue,
		const mxStruct& structInfo,
		void *rawMem	
		);

	void Deserialize_Class(
		const Json::Value& srcValue,
		const mxClass& classInfo,
		void *rawMem
	);

	void Deserialize_Parent_Class(
		const Json::Value& srcValue,
		const mxClass& parentClass,
		void *rawMem
		);

	void Deserialize_Class_Members(
		const Json::Value& srcValue,
		const mxClassMembers& members,
		void *rawMem
		);

	template< class STRUCT >
	void Deserialize_Class_Members(
		const Json::Value & srcValue,
		STRUCT &objRef
		);

	void Deserialize_Array(
		const Json::Value& srcValue,
		const mxArrayType& arrayInfo,
		void *rawMem
		);

	void Deserialize_Pointer(
		const Json::Value& srcValue,
		const mxPointerType& pointerType,
		void *pointerAddress
		);

	void Deserialize_Asset_Reference(
		const Json::Value& srcValue,
		const mxAssetReferenceType& handleType,
		void *pointerAddress,
		const FieldFlags flags = Field_DefaultFlags
		);

	//-----------------------------------------------------------------------

	bool Read_Value_From_Stream( AStreamReader& stream, Json::Value & outValue );

	// debugging

	void DbgPrintValue( const Json::Value& value );

};//JSON

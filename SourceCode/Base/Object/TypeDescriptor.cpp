/*
=============================================================================
	File:	TypeDescriptor.cpp
	Desc:	
=============================================================================
*/

#include <Base_PCH.h>
#pragma hdrstop
#include <Base.h>

#include <Base/Object/TypeDescriptor.h>

const char* ETypeKind_To_Chars( ETypeKind typeKind )
{
	switch( typeKind )
	{
	//case ETypeKind::Type_Unknown :

	case ETypeKind::Type_Int8 :		return "Int8";
	case ETypeKind::Type_Int16 :	return "Int16";
	case ETypeKind::Type_Int32 :	return "Int32";
	case ETypeKind::Type_Int64 :	return "Int64";

	case ETypeKind::Type_UInt8 :	return "UInt8";
	case ETypeKind::Type_UInt16 :	return "UInt16";
	case ETypeKind::Type_UInt32 :	return "UInt32";
	case ETypeKind::Type_UInt64 :	return "UInt64";

	case ETypeKind::Type_Bool :		return "Boolean";

	case ETypeKind::Type_Float :	return "Float";
	case ETypeKind::Type_Double :	return "Double";
	case ETypeKind::Type_SimdQuad :	return "SimdQuad";


	case ETypeKind::Type_Vec2D :	return "Vec2D";
	case ETypeKind::Type_Vec3D :	return "Vec3D";
	case ETypeKind::Type_Vec4D :	return "Vec4D";
	case ETypeKind::Type_Matrix2 :	return "Matrix2";
	case ETypeKind::Type_Matrix3 :	return "Matrix3";
	case ETypeKind::Type_Matrix4 :	return "Matrix4";

	case ETypeKind::Type_ColorRGBA :return "ColorRGBA";
	case ETypeKind::Type_String :	return "String";

	case ETypeKind::Type_Enum :		return "Enum";

	case ETypeKind::Type_Flags :	return "Flags";


	case ETypeKind::Type_Struct :	return "Struct";
	case ETypeKind::Type_Class :	return "Class";

	case ETypeKind::Type_Pointer :	return "Pointer";

	case ETypeKind::Type_AssetRef :	return "ResourcePtr";

	case ETypeKind::Type_Array :	return "Array";
	}

	mxDBG_UNREACHABLE;

	return "Unknown";
}

bool ETypeKind_Is_Bitwise_Serializable( const ETypeKind inTypeKind )
{
	return inTypeKind >= ETypeKind::Type_Int8
		&& inTypeKind <= ETypeKind::Type_ColorRGBA
		;
}

NO_EMPTY_FILE

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//

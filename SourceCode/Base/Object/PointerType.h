/*
=============================================================================
	File:	PointerType.h
	Desc:	
=============================================================================
*/

#pragma once

#include <Base/Object/TypeDescriptor.h>

struct mxPointerType : public mxType
{
	const mxType &	m_pointeeType;	// type of the referenced object

public:
	inline mxPointerType( const char* typeName, const STypeDescription& typeInfo, const mxType& pointeeType )
		: mxType( ETypeKind::Type_Pointer, typeName, typeInfo )
		, m_pointeeType( pointeeType )
	{
	}
};

template< typename TYPE >
struct TypeDeducer< TYPE* >
{
	static inline const mxType& GetType()
	{
		static mxPointerType staticTypeInfo(
			"RawPointer",
			STypeDescription::For_Type<TYPE*>(),
			T_DeduceTypeInfo<TYPE>()
		);
		return staticTypeInfo;
	}
	static inline ETypeKind GetTypeKind()
	{
		return ETypeKind::Type_Pointer;
	}
};

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//

// ArrayDescriptor
// 
#pragma once

#include <Base/Object/TypeDescriptor.h>

struct mxArrayType : public mxType
{
	const mxType &	m_elemType;	// type of stored elements

public:
	inline mxArrayType( const char* typeName, const STypeDescription& typeInfo, const mxType& storedItemType )
		: mxType( ETypeKind::Type_Array, typeName, typeInfo )
		, m_elemType( storedItemType )
	{
	}

	virtual UINT Generic_Get_Count( const void* pArrayObject ) const = 0;
	virtual void Generic_Set_Count( void* pArrayObject, UINT newNum ) const = 0;

	virtual void* Generic_Get_Data( void* pArrayObject ) const = 0;
	virtual const void* Generic_Get_Data( const void* pArrayObject ) const = 0;
};

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//

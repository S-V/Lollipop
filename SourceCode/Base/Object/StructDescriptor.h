#pragma once

#include <Base/Object/TypeDescriptor.h>
#include <Base/Object/Reflection.h>

/*
-----------------------------------------------------------------------------
	mxStruct

	C-style structure
	(i.e. inheritance is not allowed, only member fields are reflected)

-----------------------------------------------------------------------------
*/
struct mxStruct : public mxType
{
	const mxClassMembers &	m_members;	// reflection metadata

public:
	inline mxStruct( const char* typeName, const mxClassMembers& members, const STypeDescription& typeInfo )
		: mxType( ETypeKind::Type_Struct, typeName, typeInfo )
		, m_members( members )
	{
	}
	inline const mxClassMembers& GetMembers() const
	{
		return m_members;
	}
};


//!=- MACRO -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//!=- can only be used on C-like structs
//!=- should be placed into header files
//
#define mxDECLARE_BASIC_STRUCT( STRUCT )\
	extern mxClassMembers& PP_JOIN_TOKEN( Reflect_Struct_, STRUCT )();\
	template<>\
	struct TypeDeducer< STRUCT >\
	{\
		static inline const mxType& GetType()\
		{\
			static mxStruct staticTypeInfo(\
								mxEXTRACT_TYPE_NAME( STRUCT ),\
								PP_JOIN_TOKEN( Reflect_Struct_, STRUCT )(),\
								STypeDescription::For_Type<STRUCT>()\
							);\
			return staticTypeInfo;\
		}\
		static inline ETypeKind GetTypeKind()\
		{\
			return ETypeKind::Type_Struct;\
		}\
	};

//!--------------------------------------------------------------------------




//!=- MACRO -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//!=- can only be used on C-like structs
//!=- should be placed into source files
//
#define mxBEGIN_BASIC_STRUCT( STRUCT )\
	mxClassMembers& PP_JOIN_TOKEN( Reflect_Struct_, STRUCT )() {\
		typedef STRUCT OuterType;\
		static mxField fields[] = {\

// use mxMEMBER_FIELD*
// and mxEND_REFLECTION

//!--------------------------------------------------------------------------





//!=- MACRO -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//!=- can only be used on C-like structs
//!=- should be placed into source files
//
#define mxREFLECT_STRUCT_VIA_STATIC_METHOD( STRUCT )\
	mxClassMembers& PP_JOIN_TOKEN( Reflect_Struct_, STRUCT )() {\
	return STRUCT::StaticGetReflection();\
	}\

//!--------------------------------------------------------------------------





//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//

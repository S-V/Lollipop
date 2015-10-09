#pragma once

#include <Base/Object/TypeDescriptor.h>
#include <Base/Object/Reflection.h>

/*
-----------------------------------------------------------------------------
	mxEnumType
-----------------------------------------------------------------------------
*/
struct mxEnumType : public mxType
{
	struct Member
	{
		const char*	name;	// name of the value in the code
		const char*	alias;	// name of the value in the editor
		U4			value;
	};
	struct MemberList
	{
		const Member *	array;
		const U4		count;
	};
	struct Accessor
	{
		virtual UINT Get_Value( const void* pEnum ) const = 0;
		virtual void Set_Value( void *pEnum, UINT value ) const = 0;
	};

	const Member*	m_members;
	const UINT		m_numMembers;
	const Accessor&	m_accessor;

public:
	inline mxEnumType( const char* name, const MemberList& members, const Accessor& accessor, const STypeDescription& info )
		: mxType( ETypeKind::Type_Enum, name, info )
		, m_members( members.array )
		, m_numMembers( members.count )
		, m_accessor( accessor )
	{
	}

	UINT GetItemIndexByValue( const UINT nEnumValue ) const;
	UINT GetItemIndexByString( const char* szValue ) const;
};


//!=- MACRO -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//!=- ENUM - name of enumeration
//!=- STORAGE - type of storage
//!=- TYPEDEF - type of TEnum<ENUM,STORAGE>
//
#define mxDECLARE_ENUM( ENUM, STORAGE, TYPEDEF )\
	typedef TEnum< ENUM, STORAGE > TYPEDEF;\
	mxDECLARE_POD_TYPE( TYPEDEF );\
	extern const mxEnumType::MemberList& PP_JOIN_TOKEN( Reflect_Enum_, TYPEDEF )();\
	template<>\
	struct TypeDeducer< TYPEDEF >\
	{\
		static inline const mxType& GetType()\
		{\
			mxSTATIC_ASSERT( sizeof TYPEDEF <= sizeof UINT );\
			\
			struct EnumAccessor : public mxEnumType::Accessor\
			{\
				virtual UINT Get_Value( const void* pEnum ) const override\
				{\
					return *(const TYPEDEF*) pEnum;\
				}\
				virtual void Set_Value( void *pEnum, UINT value ) const override\
				{\
					*((TYPEDEF*) pEnum) = (ENUM) value;\
				}\
			};\
			static EnumAccessor enumAccessor;\
			static mxEnumType staticTypeInfo(\
								mxEXTRACT_TYPE_NAME( TYPEDEF ),\
								PP_JOIN_TOKEN( Reflect_Enum_, TYPEDEF )(),\
								enumAccessor,\
								STypeDescription::For_Type< TYPEDEF >()\
							);\
			return staticTypeInfo;\
		}\
		static inline ETypeKind GetTypeKind()\
		{\
			return ETypeKind::Type_Enum;\
		}\
	};

//!--------------------------------------------------------------------------




//!=- MACRO -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//!=- should be placed into source files
//
#define mxBEGIN_ENUM( FLAGS )\
	const mxEnumType::MemberList& PP_JOIN_TOKEN( Reflect_Enum_, FLAGS )() {\
		static mxEnumType::Member items[] = {\


//!=- MACRO =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//
#define mxREFLECT_BIT2( ENUM, NAME_IN_EDITOR )\
	{\
		#ENUM,\
		#NAME_IN_EDITOR,\
		ENUM\
	},


//!=- MACRO =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//
#define mxREFLECT_BIT( ENUM )\
	mxREFLECT_BIT2( ENUM, ENUM )


//!=- MACRO =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//
#define mxEND_ENUM\
		};\
		static mxEnumType::MemberList reflectionMetadata = { items, ARRAY_SIZE(items) };\
		return reflectionMetadata;\
	}


//!--------------------------------------------------------------------------


//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//

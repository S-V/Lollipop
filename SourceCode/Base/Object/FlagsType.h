#pragma once

#include <Base/Object/TypeDescriptor.h>
#include <Base/Object/Reflection.h>

/*
-----------------------------------------------------------------------------
	mxFlagsType

	TBits< ENUM, STORAGE > - 8,16,32 bits of named values.
-----------------------------------------------------------------------------
*/
struct mxFlagsType : public mxType
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

	const Member*	m_bits;
	const UINT		m_numBits;
	const Accessor&	m_accessor;

public:
	inline mxFlagsType( const char* name, const MemberList& members, const Accessor& accessor, const STypeDescription& info )
		: mxType( ETypeKind::Type_Flags, name, info )
		, m_bits( members.array )
		, m_numBits( members.count )
		, m_accessor( accessor )
	{
	}
};


//!=- MACRO -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//!=- ENUM - name of enumeration
//!=- STORAGE - type of storage
//!=- FLAGS - type of TBits<ENUM,STORAGE>
//
#define mxDECLARE_FLAGS( ENUM, STORAGE, FLAGS )\
	typedef TBits< ENUM, STORAGE > FLAGS;\
	mxDECLARE_POD_TYPE( FLAGS );\
	extern const mxFlagsType::MemberList& PP_JOIN_TOKEN( Reflect_Flags_, FLAGS )();\
	template<>\
	struct TypeDeducer< FLAGS >\
	{\
		static inline const mxType& GetType()\
		{\
			mxSTATIC_ASSERT( sizeof FLAGS <= sizeof UINT );\
			\
			struct ValueAccessor : public mxFlagsType::Accessor\
			{\
				virtual UINT Get_Value( const void* pEnum ) const override\
				{\
					return *(const FLAGS*) pEnum;\
				}\
				virtual void Set_Value( void *pEnum, UINT value ) const override\
				{\
					*((FLAGS*) pEnum) = (ENUM) value;\
				}\
			};\
			static ValueAccessor valueAccessor;\
			static mxFlagsType staticTypeInfo(\
								mxEXTRACT_TYPE_NAME( FLAGS ),\
								PP_JOIN_TOKEN( Reflect_Flags_, FLAGS )(),\
								valueAccessor,\
								STypeDescription::For_Type< FLAGS >()\
							);\
			return staticTypeInfo;\
		}\
		static inline ETypeKind GetTypeKind()\
		{\
			return ETypeKind::Type_Flags;\
		}\
	};

//!--------------------------------------------------------------------------




//!=- MACRO -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//!=- should be placed into source files
//
#define mxBEGIN_FLAGS( FLAGS )\
	const mxFlagsType::MemberList& PP_JOIN_TOKEN( Reflect_Flags_, FLAGS )() {\
		static mxFlagsType::Member items[] = {\


//!=- MACRO =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//
#define mxREFLECT_ENUM2( ENUM, NAME_IN_EDITOR )\
	{\
		#ENUM,\
		#NAME_IN_EDITOR,\
		ENUM\
	},


//!=- MACRO =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//
#define mxREFLECT_ENUM( ENUM )\
	mxREFLECT_ENUM2( ENUM, ENUM )


//!=- MACRO =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//
#define mxEND_FLAGS\
		};\
		static mxFlagsType::MemberList reflectionMetadata = { items, ARRAY_SIZE(items) };\
		return reflectionMetadata;\
	}


//!--------------------------------------------------------------------------


//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//

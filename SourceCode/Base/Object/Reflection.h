/*
=============================================================================
	File:	Reflection.h
	Desc:	Reflection - having access at runtime
			to information about the C++ classes in your program.

			Reflection is used mainly for serialization.

	Note:	This has been written in haste and is by no means complete.

	Note:	base classes are assumed to start at offset 0 (see mxFIELD_SUPER)

	ToDo:	don't leave string names in release,
			they should only be used in editor mode
=============================================================================
*/

#ifndef __MX_BASE_REFLECTION_H__
#define __MX_BASE_REFLECTION_H__


#include <Base/Object/TypeDescriptor.h>

class AObject;

// should be ptrdiff_t/size_t but an unsigned int will suffice
typedef UINT MetaOffset;

enum EFieldFlags
{
	Field_DefaultFlags = 0,

	// The field won't be initialized with default values (e.g. fallback resources).
	Field_NoDefaultInit = BIT(0)
};

typedef TBits<EFieldFlags,UINT8>	FieldFlags;

/*
-----------------------------------------------------------------------------
	mxField

	structure field definition
	,used for reflecting contained, nested objects

	@todo: don't leave name string in release exe
	@todo: could possibly get rid of 'size' field
-----------------------------------------------------------------------------
*/
struct mxField
{
	const mxType &		type;	// type of the field
	const char *		name;	// name of the variable in the code
	const char *		alias;	// name of the field in the editor
	const MetaOffset	offset;	// byte offset in the structure (relative to the immediately enclosing type)
	const FieldFlags	flags;	// combination of EFieldFlags::Field_* bits

	//const char *		annotation;
};


/*
-----------------------------------------------------------------------------
	Metadata

	reflection metadata for describing the contents of a structure
-----------------------------------------------------------------------------
*/
struct mxClassMembers
{
	const mxField *	fields;		// array of fields in the structure
	const UINT		numFields;	// number of fields in the structure

	// time stamp for version tracking
	// metadata is implemented in source files ->
	// time stamp changes when file is recompiled
	//STimeStamp	timeStamp;

public:
	static mxClassMembers	dummy;	// empty, null instance
};


//!=- MACRO =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//	mxDECLARE_REFLECTION - must be included in the declaration of a class.
//---------------------------------------------------------------------------
//
#define mxDECLARE_REFLECTION\
	public:\
		static mxClassMembers& StaticGetReflection();


//!=- MACRO =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//	mxBEGIN_REFLECTION( KLASS ) - should be placed in source file.
//---------------------------------------------------------------------------
//
#define mxBEGIN_REFLECTION( KLASS )\
	mxClassMembers& KLASS::StaticGetReflection() {\
		typedef KLASS OuterType;\
		static mxField fields[] = {\



//!=- MACRO =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//!=- takes the name of the class member variable
//!=- and its corresponding name in the editor
//
#define mxMEMBER_FIELD2( VAR, NAME_IN_EDITOR, ... )\
	{\
		T_DeduceTypeInfo( ((OuterType*)0)->VAR ),\
		#VAR,\
		#NAME_IN_EDITOR,\
		OFFSET_OF( OuterType, VAR ),\
		## __VA_ARGS__ /*flags - initialized to zero by default*/,\
	},


//!=- MACRO =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//!=- takes the name of the class member variable
//
#define mxMEMBER_FIELD( VAR )\
	mxMEMBER_FIELD2( VAR, VAR )




//!=- MACRO =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//
#define mxEND_REFLECTION\
		};\
		static mxClassMembers reflectionMetadata = { fields, ARRAY_SIZE(fields) };\
		return reflectionMetadata;\
	}

//!--------------------------------------------------------------------------




template< typename TYPE >
struct TPODHelper
{
	static FORCEINLINE void Set( void* const base, const MetaOffset offset, const TYPE& newValue )
	{
		TYPE & o = *(TYPE*) ( (BYTE*)base + offset );
		o = newValue;
	}
	static FORCEINLINE const TYPE& Get( const void*const base, const MetaOffset offset )
	{
		return *(TYPE*) ( (BYTE*)base + offset );
	}
	static FORCEINLINE TYPE& GetNonConst( void *const base, const MetaOffset offset )
	{
		return *(TYPE*) ( (BYTE*)base + offset );
	}
	//static FORCEINLINE void GetNonConst( void *const base, const UINT offset, TYPE &dstValue )
	//{
	//	dstValue = *(TYPE*) ( (BYTE*)base + offset );
	//}
	static FORCEINLINE const TYPE& GetConst( const void*const objAddr )
	{
		return *(const TYPE*) objAddr;
	}
	static FORCEINLINE TYPE& GetNonConst( void*const objAddr )
	{
		return *(TYPE*) objAddr;
	}
};


#endif // !__MX_BASE_REFLECTION_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//

/*
=============================================================================
	File:	TypeDescriptor.h
	Desc:	classes for representing C++ types
	ToDo:	don't store type names in release mode;
			could possibly get away with shorts/bytes
			for storing size, alignment, etc.
=============================================================================
*/
#pragma once

// for compile-time string hashing
#include <Base/Common/StaticStringHash.h>

// Forward declarations.
class SBaseType;
class AObject;

class mxType;
class STypeDescription;

/*
-----------------------------------------------------------------------------
	TypeGUID
-----------------------------------------------------------------------------
*/

// Unique type identifier, it's usually a hash of a class name.
// (aka 'static, persistent class GUID', it stays the same when saving/loading).
// The base class for POD structs has zero index.
// The base class for virtual classes has index 1.
//
typedef UINT32 TypeGUID;
typedef const UINT32 TypeGUIDArg;

// invalid type GUID
enum { mxNULL_TYPE_GUID	= ((TypeGUIDArg)0) };

enum { MAX_NUM_CLASSES = MAX_INT32 };

FORCEINLINE bool IsValidTypeGuid( TypeGUIDArg classId )
{
	return classId != mxNULL_TYPE_GUID;
}

mxIMPLEMENT_FUNCTION_READ_SINGLE(TypeGUID,ReadTypeGuid);
mxIMPLEMENT_FUNCTION_WRITE_SINGLE(TypeGUID,WriteTypeGuid);


// Dynamic type identifier
//
typedef UINT32 TypeID;
typedef const UINT32 TypeIDArg;

// invalid type ID
enum { mxNULL_TYPE_ID	= ((TypeID)-1) };

// see: http://www.altdevblogaday.com/2011/10/10/making-your-own-type-id-is-fun/#comment-332144776
//
// Assuming that unsigned int is the same as the pointer type on the platform 
// this is guaranteed to return a unique value for each instantiation.
// 
// The only caveat to this is that the actual value of the ID will almost
// certainly not remain constant over multiple executions of the exe.
//
// this generates a typeID for each type that instantiates the template
template< typename TYPE >
static TypeID T_GenerateTypeId()
{
    // I wasn't sure if the char would take up > 1 byte because of alignment.
    // With VS2010 on win32 they take up exactly 1 byte each.
	static char iAddressOfThisIsTheTypeId;
	return reinterpret_cast< TypeID >( &iAddressOfThisIsTheTypeId );
}






//!=- MACRO -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// macros for extracting type name string and type id at compile-time
//
#define mxEXTRACT_TYPE_NAME( TYP )		#TYP

//!=- MACRO -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// uses a compile-time string hashing function
//
#define mxEXTRACT_TYPE_GUID( TYP )		StringId( #TYP ).GetHash()

//!--------------------------------------------------------------------------



// put any application-specific data here:
struct SPerTypeUserData
{
	//void * userData;
};


/*
-----------------------------------------------------------------------------
	ETypeKind

	An enumeration of all possible kinds of types.

	NOTE: the order is important (optimized for type checks) !
	NOTE: if you change this, make sure to update
		ETypeKind_Is_Bitwise_Serializable()
-----------------------------------------------------------------------------
*/
enum ETypeKind
{
	Type_Unknown = 0,	// the null type, the bad type

	// Plain old data types
	//
	// Primitive (fundamental) types
	//Type_Byte,
	//Type_UByte,
	Type_Int8,		// signed 8-bit integer
	Type_UInt8,		// unsigned 8-bit integer
	Type_Int16,		// signed 16-bit integer
	Type_UInt16,	// unsigned 16-bit integer
	Type_Int32,		// signed 32-bit integer
	Type_UInt32,	// unsigned 32-bit integer
	Type_Int64,		// signed 64-bit integer
	Type_UInt64,	// unsigned 64-bit integer

	Type_Bool,		// native 'bool' type (usually, 1 byte in size)

	Type_Float,		// 32-bit (single-precision) floating point
	Type_Double,	// 64-bit (double-precision) floating point
	Type_SimdQuad,	// 4-component SIMD vector (float4/vec4)

	// Built-in POD types
	Type_Vec2D,		// 2 floats
	Type_Vec3D,		// 3 floats (12-bytes)
	Type_Vec4D,		// 4 floats (16-bytes)
	Type_Matrix2,	// 2x2 matrix
	Type_Matrix3,	// 3x3 matrix
	Type_Matrix4,	// 4x4 matrix
	Type_ColorRGBA,	// 4 floats (16-bytes)

	// Built-in complex types
	Type_String,	// dynamic String (slow, bloated & unpredictable)

	// Enumerations
	Type_Enum,

	// Bit masks
	Type_Flags,


	// Complex types

	// Aggregate types
	Type_Struct,	// C-style struct (only member fields, no inheritance, no vtable, etc)
	Type_Class,		// C++ struct or class (single inheritance, ctors/dtors, vtable, etc)

	Type_Pointer,	// generic pointer, but only pointers to descendants of AObject are supported (raw pointers -> unclear ownership)
	//Type_ClassPtr,	// pointer to a polymorphic object (derived from AObject)

	Type_AssetRef,	// asset reference (for saving to/loading from persistent storage)
	//Type_ResourceHandle,
	//Type_AssetGUID,		// asset reference (for saving to/loading from persistent storage)
	//Type_ObjectPtr,		// pointer to a polymorphic object (avoid like the plague!)
	//Type_ObjectList,	// array of pointers to polymorphic objects (pure evil - brings non-determinism!)

	Type_Array,		// generic array of values

	Type_MAX	// Must be last! Don't use!
};

mxDECLARE_ENUM_TYPE( ETypeKind, UINT, TypeKind );

// returns true if the type is bitwise-copyable (can be serialized via reading/writing bytes)
bool ETypeKind_Is_Bitwise_Serializable( const ETypeKind inTypeKind );



// parameters for initializing mxType structure
//
struct STypeDescription
{
	UINT				size;	// size of object of this class, in bytes
	UINT				alignment;	// the alignment of this data type (zero for abstract classes)
	//bool				isPointer;	// true if the type is a pointer

public:
	inline STypeDescription(ENoInit)
	{}

	inline STypeDescription()
	{
		size = 0;
		alignment = EFFICIENT_ALIGNMENT;
		//typeInfo.isPointer = false;
	}

	template< typename TYPE >
	static inline STypeDescription For_Type()
	{
		STypeDescription	typeInfo(_NoInit);

		typeInfo.size = sizeof TYPE;
		typeInfo.alignment = mxALIGNMENT( TYPE );
		//typeInfo.isPointer = is_pointer< TYPE >::VALUE;

		return typeInfo;
	}
};


/*
-----------------------------------------------------------------------------
	mxType

	this is a base class for providing information about C++ types

	NOTE: everything should be statically allocated
-----------------------------------------------------------------------------
*/
struct mxType
{
	const char *		m_name;	// pointer to static string - name of this type
	const ETypeKind		m_kind;
	const UINT			m_instanceSize;	// size of object of this type, in bytes
	const UINT			m_alignment;	// the alignment of this data type

	SPerTypeUserData *	m_userData;

	//const char *		m_alias;	// name of this type in editor

public:
	inline mxType( const ETypeKind typeKind, const char* typeName, const STypeDescription& typeInfo )
		: m_name( typeName )
		, m_kind( typeKind )
		, m_instanceSize( typeInfo.size )
		, m_alignment( typeInfo.alignment )
		//, m_alias( typeName )
	{
		m_userData = nil;
	}

	virtual ~mxType()
	{}

	template< class DERIVED >
	inline const DERIVED& UpCast() const
	{
		const DERIVED* pDERIVED = checked_cast< const DERIVED* >( this );
		return *pDERIVED;
	}
};


// helper class for extracting type descriptors
// (via partial template specialization)
//
template< typename TYPE >
struct TypeDeducer
{
	static inline const mxType& GetType()
	{
		// Compile-time assert: Type not implemented
		//Error__Failed_To_Deduce_Type_Info;
		//return nil;
	
		// by default, assume it's a struct
		return TYPE::StaticClass();
	}
	static inline ETypeKind GetTypeKind()
	{
		// Compile-time assert: Type not implemented
		Error__Failed_To_Deduce_Type_Info;
		return ETypeKind::Type_Unknown;
	}
};


// a templated function to get the type descriptor for the given type
//
template< typename TYPE >
inline const mxType& T_DeduceTypeInfo()
{
	return TypeDeducer< TYPE >::GetType();
}

template< typename TYPE >
inline const mxType& T_DeduceTypeInfo( const TYPE& value )
{
	mxUNUSED(value);
	return T_DeduceTypeInfo< TYPE >();
}

template< typename TYPE >
inline ETypeKind T_DeduceTypeKind()
{
	return TypeDeducer< TYPE >::GetTypeKind();
}





/*
-----------------------------------------------------------------------------
	mxBuiltInType

	represents a built-in type
-----------------------------------------------------------------------------
*/
template< typename TYPE >
struct mxBuiltInType : public mxType
{
public:
	inline mxBuiltInType( const ETypeKind typeKind, const char* typeName )
		: mxType( typeKind, typeName, STypeDescription::For_Type< TYPE >() )
	{
	}
};

//!=- MACRO -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
#define mxDECLARE_BUILTIN_TYPE( TYPE, KIND )\
	template<>\
	inline const mxType& T_DeduceTypeInfo< TYPE >()\
	{\
		static mxBuiltInType< TYPE >	staticTypeInfo( KIND, mxEXTRACT_TYPE_NAME( TYPE ) );\
		return staticTypeInfo;\
	}\
	template<>\
	inline ETypeKind T_DeduceTypeKind< TYPE >()\
	{\
		return KIND;\
	}
//!--------------------------------------------------------------------------

mxDECLARE_BUILTIN_TYPE( INT8,	ETypeKind::Type_Int8 );
mxDECLARE_BUILTIN_TYPE( UINT8, ETypeKind::Type_UInt8 );
mxDECLARE_BUILTIN_TYPE( INT16,	ETypeKind::Type_Int16 );
mxDECLARE_BUILTIN_TYPE( UINT16, ETypeKind::Type_UInt16 );
mxDECLARE_BUILTIN_TYPE( INT32,	ETypeKind::Type_Int32 );
mxDECLARE_BUILTIN_TYPE( UINT32, ETypeKind::Type_UInt32 );
mxDECLARE_BUILTIN_TYPE( INT64,	ETypeKind::Type_Int64 );
mxDECLARE_BUILTIN_TYPE( UINT64, ETypeKind::Type_UInt64 );
mxDECLARE_BUILTIN_TYPE( float,	ETypeKind::Type_Float );
mxDECLARE_BUILTIN_TYPE( double, ETypeKind::Type_Double );
mxDECLARE_BUILTIN_TYPE( bool,	ETypeKind::Type_Bool );

//#undef mxDECLARE_BUILTIN_TYPE


//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//

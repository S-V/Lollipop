/*
=============================================================================
	File:	
	Desc:
	ToDo:	strip class names from the executable in release version
=============================================================================
*/
#pragma once

#include <Base/Object/TypeDescriptor.h>
#include <Base/Object/Reflection.h>

// Forward declarations.
class SBaseType;
class AObject;



/*
=============================================================================
	Object factory
=============================================================================
*/

// function prototypes (used for creating object instances by type information)

// allocates memory and calls constructor
typedef void* F_CreateObject();
// constructs the object in-place (used to init vtables)
typedef void F_ConstructObject( void* objMem );
//F_DestroyObject
typedef void F_DestructObject( void* objMem );


//
// Object factory function generators,
// wrapped in the common 'friend' class.
// this is wrapped in a struct rather than a namespace
// to allow access to private/protected class members
// (via 'friend' keyword).
//
struct TypeHelper
{
	template< typename TYPE >
	static inline void* CreateObjectTemplate()
	{
		return new TYPE();
	}
	template< typename TYPE >
	static inline void ConstructObjectTemplate( void* objMem )
	{
		new (objMem) TYPE();
	}
	template< typename TYPE >
	static inline void DestructObjectTemplate( void* objMem )
	{
		((TYPE*)objMem)->TYPE::~TYPE();
	}
};



//
//	contains some common parameters for initializing mxClass structure
//
struct SClassDescription : public STypeDescription
{
	F_CreateObject *	creator;	// 'create' function, null for abstract classes
	F_ConstructObject *	constructor;// 'create in place' function, null for abstract classes
	F_DestructObject *	destructor;

public:
	inline SClassDescription()
	{
		creator = nil;
		constructor = nil;
		destructor = nil;
	}

	inline SClassDescription(ENoInit)
		: STypeDescription(_NoInit)
	{}

	template< typename KLASS >
	static SClassDescription For_Class_With_Default_Ctor()
	{
		SClassDescription	classInfo(_NoInit);
		classInfo.Collect_Common_Properties_For_Class< KLASS >();

		classInfo.alignment = mxALIGNMENT( KLASS );

		classInfo.creator = TypeHelper::CreateObjectTemplate< KLASS >;
		classInfo.constructor = TypeHelper::ConstructObjectTemplate< KLASS >;
		classInfo.destructor = TypeHelper::DestructObjectTemplate< KLASS >;

		return classInfo;
	}

	template< typename KLASS >
	static SClassDescription For_Abstract_Class()
	{
		SClassDescription	classInfo(_NoInit);
		classInfo.Collect_Common_Properties_For_Class< KLASS >();

		classInfo.alignment = 0;

		classInfo.creator = nil;
		classInfo.constructor = nil;
		classInfo.destructor = TypeHelper::DestructObjectTemplate< KLASS >;

		return classInfo;
	}

private:
	template< typename KLASS >
	inline void Collect_Common_Properties_For_Class()
	{
		this->size = sizeof KLASS;
		//this->isPointer = is_pointer< TYPE >::VALUE;
	}
};

/*
-----------------------------------------------------------------------------
	mxClass

	this is a base class for providing information about C++ classes

	NOTE: only single inheritance class hierarchies are supported!
-----------------------------------------------------------------------------
*/
class mxClass : public mxType
{
public:
	PCSTR		GetTypeName() const;	// GetTypeName, because GetClassName is defined in Windows headers.
	TypeGUIDArg	GetTypeGuid() const;

	// Returns the size of a single instance of the class, in bytes.
	SizeT		GetInstanceSize() const;

	SPerTypeUserData* GetUserData();

	const mxClassMembers& GetMembers() const;

	const mxClass *	GetParent() const;

	bool	IsDerivedFrom( const mxClass& other ) const;
	bool	IsDerivedFrom( TypeGUIDArg typeCode ) const;
	bool	IsDerivedFrom( PCSTR className ) const;

	bool	operator == ( const mxClass& other ) const;
	bool	operator != ( const mxClass& other ) const;

	// Returns 'true' if this type inherits from the given type.
	template< class KLASS >	// where KLASS : AObject
	inline bool IsA() const
	{
		return this->IsDerivedFrom( KLASS::StaticClass() );
	}

	// Returns 'true' if this type is the same as the given type.
	template< class KLASS >	// where KLASS : AObject
	inline bool Is() const
	{
		return this == &KLASS::StaticClass();
	}

	bool IsAbstract() const;
	bool IsConcrete() const;

	F_CreateObject *	GetCreator() const;
	F_ConstructObject *	GetConstructor() const;
	F_DestructObject *	GetDestructor() const;

	AObject* CreateInstance() const;

public_internal:

	// these constructors are wrapped in macros
	mxClass(
		const char* className,
		const TypeGUIDArg classGuid,
		const mxClass* const parentClass,
		const SClassDescription& classInfo,
		const mxClassMembers& reflectionMetadata
		);

private:
	const TypeGUID		m_guid;	// unique type identifier

	const mxClass * const	m_parent;	// base class for this class

	const mxClassMembers &	m_members;	// reflected members of this class (not including inherited members)

	F_CreateObject *	m_creator;
	F_ConstructObject *	m_constructor;
	F_DestructObject *	m_destructor;

private:
	friend class TypeRegistry;

	// these are used for building the linked list of registered classes
	// during static initialization

	mxClass *		m_next;

	// this is the head of the singly linked list of all class descriptors
	static mxClass *	m_head;

private:
	NO_COPY_CONSTRUCTOR( mxClass );
	NO_ASSIGNMENT( mxClass );
};

#include <Base/Object/ClassDescriptor.inl>


/*
=============================================================================
	
	Macros for declaring and implementing type information

=============================================================================
*/





//!=- MACRO -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// NOTE: 'virtual' is omitted intentionally in order to avoid vtbl placement in 'non-virtual' classes.
//
#define mxDECLARE_CLASS_COMMON( KLASS, BASE_KLASS )\
	NO_COMPARES(KLASS);\
	mxUTIL_CHECK_BASE_CLASS( KLASS, BASE_KLASS );\
	private:\
		static mxClass ms_staticTypeInfo;\
		friend class TypeHelper;\
	public:\
		FORCEINLINE static mxClass& StaticClass() { return ms_staticTypeInfo; };\
		FORCEINLINE /*virtual*/ mxClass& rttiGetClass() const { return ms_staticTypeInfo; }\
		typedef KLASS ThisType;\
		/*typedef BASE_KLASS BaseType;*/\
		typedef BASE_KLASS Super;\



//== MACRO =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//	This macro must be included in the definition of any subclass of AObject (usually in header files).
//	This must be used on single inheritance concrete classes only!
//---------------------------------------------------------------------------
//
#define mxDECLARE_CLASS( KLASS, BASE_KLASS )\
	mxDECLARE_CLASS_COMMON( KLASS, BASE_KLASS );\



//== MACRO =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//	This macro must be included in the definition of any abstract subclass of AObject (usually in header files).
//	This must be used on single inheritance abstract classes only!
//---------------------------------------------------------------------------
//
#define mxDECLARE_ABSTRACT_CLASS( KLASS, BASE_KLASS )\
	mxDECLARE_CLASS_COMMON( KLASS, BASE_KLASS );\
	NO_ASSIGNMENT(KLASS);\
	mxUTIL_CHECK_ABSTRACT_CLASS( KLASS );




//== MACRO =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//	mxDEFINE_CLASS must be included in the implementation of any subclass of AObject (usually in source files).
//---------------------------------------------------------------------------
//
#define mxDEFINE_CLASS( KLASS )\
	mxClass	KLASS::ms_staticTypeInfo(\
					mxEXTRACT_TYPE_NAME( KLASS ), mxEXTRACT_TYPE_GUID( KLASS ),\
					&Super::StaticClass(),\
					SClassDescription::For_Class_With_Default_Ctor< KLASS >(),\
					KLASS::StaticGetReflection()\
					);


//@todo: mxDEFINE_CLASS_NO_DEFAULT_CTOR and mxDEFINE_ABSTRACT_CLASS are the same

//== MACRO =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//	mxDEFINE_CLASS_NO_DEFAULT_CTOR must be included in the implementation of any subclass of AObject (usually in source files).
//---------------------------------------------------------------------------
//
#define mxDEFINE_CLASS_NO_DEFAULT_CTOR( KLASS )\
	mxClass	KLASS::ms_staticTypeInfo(\
					mxEXTRACT_TYPE_NAME( KLASS ), mxEXTRACT_TYPE_GUID( KLASS ),\
					&Super::StaticClass(),\
					SClassDescription::For_Abstract_Class< KLASS >(),\
					KLASS::StaticGetReflection()\
					);

//== MACRO =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//	mxDEFINE_ABSTRACT_CLASS must be included in the implementation of any abstract subclass of AObject (usually in source files).
//---------------------------------------------------------------------------
//
#define mxDEFINE_ABSTRACT_CLASS( KLASS )\
	mxClass	KLASS::ms_staticTypeInfo(\
					mxEXTRACT_TYPE_NAME( KLASS ), mxEXTRACT_TYPE_GUID( KLASS ),\
					&Super::StaticClass(),\
					SClassDescription::For_Abstract_Class< KLASS >(),\
					KLASS::StaticGetReflection()\
					);


//---------------------------------------------------------------------------
// Helper macros used for type checking
//---------------------------------------------------------------------------

//!=- MACRO -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// compile-time inheritance test for catching common typing errors
//
#define mxUTIL_CHECK_BASE_CLASS( KLASS, BASE_KLASS )\
	private:\
		static void PP_JOIN_TOKEN(Check_,\
						PP_JOIN_TOKEN(KLASS,\
							PP_JOIN_TOKEN(_Is_Derived_From_, BASE_KLASS)\
						)\
					)()\
		{\
			BASE_KLASS* base;\
			KLASS* derived = static_cast<KLASS*>( base );\
			(void)derived;\
		}\
	public:


//!=- MACRO -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// all abstract classes must derived from AObject
//
#define mxUTIL_CHECK_ABSTRACT_CLASS( KLASS )\
	private:\
		static void PP_JOIN_TOKEN( Abstract_Class_,\
						PP_JOIN_TOKEN( KLASS, _Must_Derive_From_AObject )\
					)()\
		{\
			AObject* base;\
			KLASS* derived = static_cast<KLASS*>( base );\
			(void)derived;\
		}\
	public:



//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//

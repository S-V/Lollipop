// Dynamic properties used by the editor
//
#pragma once

#include <Core/Editor.h>

#include <Core/Resources.h>

mxNAMESPACE_BEGIN

/*
-----------------------------------------------------------------------------
	APropertyEditor
	its functions are overridden by gui editor widgets
-----------------------------------------------------------------------------
*/
class APropertyEditor
{
public:
	virtual ~APropertyEditor();

	// reads data from the model and writes it to the editor widget.
	virtual void SetEditorData() = 0;

	// reads the contents of the editor, and writes it to the model.
	virtual void SetModelData() = 0;

protected:
	APropertyEditor( AEditableProperty* theProperty );

private:
	TPtr< AEditableProperty >	m__property;
};

enum EPropertyFlags
{
	PF_ReadOnly = BIT(0),	// Property is not editable in the editor.

	PF_DefaultFlags = 0,	// default value
};
typedef TBits< EPropertyFlags, U4 >	PropertyFlags;

/*
-----------------------------------------------------------------------------
	AProperty

	Abstract property
-----------------------------------------------------------------------------
*/
class AEditableProperty : public AEditableRefCounted
{
	mxDECLARE_ABSTRACT_CLASS( AEditableProperty, AEditable );

	typedef TRefPtr<AEditableProperty>	Ref;

	virtual ~AEditableProperty();

public:
	virtual UINT edNumRows() const override;
	virtual AEditable* edGetParent() override;
	virtual const char* edToChars( UINT column ) const override;
	virtual const char* edGetName() const override;

	inline void SetPropertyFlags( PropertyFlags flags )
	{
		m_flags = flags;
	}

	inline bool isReadOnly() const
	{
		return m_flags & PF_ReadOnly;
	}

public:
	// pointer to editor widget
	// to avoid if/switch constructs in favor of virtual function calls
	TPtr< APropertyEditor >	editor;

protected:
	AEditableProperty( const char* name, EdPropertyList* parent );

protected:
	PropertyFlags		m_flags;	// EPropertyFlags
	String				m_name;
	TPtr< AEditable >	m_parent;
};

/*
-----------------------------------------------------------------------------
	EdPropertyList
-----------------------------------------------------------------------------
*/
class EdPropertyList : public AEditableProperty
	, public TArrayBase< AEditableProperty*, EdPropertyList >
{
	mxDECLARE_CLASS( EdPropertyList, AEditableProperty );

public:
	EdPropertyList( const char* name = "EdPropertyList", EdPropertyList* parent = nil )
		: AEditableProperty( name, parent )
	{
	}

	virtual ~EdPropertyList()
	{
		this->Clear();
	}

	void Clear()
	{
		m_properties.DestroyAndEmpty();
	}

	void Add( AEditableProperty* p )
	{
		m_properties.Add( p );
	}

public:	//=== TArrayBase

	UINT Num() const
	{
		return m_properties.Num();
	}
	UINT GetCapacity() const
	{
		return m_properties.GetCapacity();
	}
	AEditableProperty** ToPtr()
	{
		return c_cast(AEditableProperty**) m_properties.ToPtr();
	}
	const AEditableProperty* const * ToPtr() const
	{
		return c_cast(AEditableProperty**) m_properties.ToPtr();
	}

public:	//==- AEditable

	virtual UINT edNumRows() const override;
	virtual AEditable* edGetParent() override;
	virtual AEditable* edItemAt( UINT index ) override;
	virtual UINT edIndexOf( const AEditable* child ) const override;
	virtual const char* edToChars( UINT column ) const override;

	inline void SetParent( AEditable* parent )
	{
		m_parent = parent;
	}

protected:
	EdPropertyList( EdPropertyList* parent, const char* name )
		: AEditableProperty( name, parent )
	{
	}

private:
	TList< TRefPtr< AEditableProperty > >	m_properties;
};

/*
-----------------------------------------------------------------------------
	RWPropertyTemplate< T >
-----------------------------------------------------------------------------
*/
template< typename TYPE, typename RET_TYPE = TYPE, typename ARG_TYPE = const TYPE& >
struct RWPropertyTemplate : public AEditableProperty
{
	typedef TYPE PropertyValType;
	typedef RET_TYPE PropertyRetType;
	typedef ARG_TYPE PropertyArgType;

	typedef TCallback< PropertyRetType () >		GetterType;
	typedef TCallback< void (PropertyArgType) >	SetterType;


	RWPropertyTemplate( EdPropertyList* parent, const char* name )
		: AEditableProperty( name, parent )
	{}

	virtual void Set( PropertyArgType newValue ) = 0;
	virtual PropertyRetType Get() const = 0;

#if 0
	virtual PropertyRetType GetMinimum() const { return -MAX_INT32; }
	// NOTABUG: don't make returned values too big - it confuses Qt.
	virtual PropertyRetType GetMaximum() const { return MAX_INT32; }

	virtual PropertyRetType GetSingleStepSize() const { return (PropertyRetType)1; }

	virtual bool IsValid( PropertyArgType n ) const { return true; }
#endif

	FORCEINLINE void SetPOD( void* const ptr, PropertyArgType newValue )
	{
		TYPE & o = *(TYPE*) ( (BYTE*)ptr );
		o = newValue;
	}
	FORCEINLINE PropertyRetType GetPOD( const void*const ptr )
	{
		return *(TYPE*) ( (BYTE*)ptr );
	}
};

/*
-----------------------------------------------------------------------------
	EdProperty_Bool
-----------------------------------------------------------------------------
*/
class EdProperty_Bool : public RWPropertyTemplate< bool >
{
	mxDECLARE_CLASS( EdProperty_Bool, AEditableProperty );

public:
	EdProperty_Bool( EdPropertyList* parent, const char* name )
		: RWPropertyTemplate( parent, name )
	{}
};

/*
-----------------------------------------------------------------------------
	EdProperty_Int32
-----------------------------------------------------------------------------
*/
class EdProperty_Int32 : public RWPropertyTemplate< S4 >
{
	mxDECLARE_CLASS( EdProperty_Int32, AEditableProperty );

public:
	EdProperty_Int32( EdPropertyList* parent, const char* name )
		: RWPropertyTemplate( parent, name )
	{}

	virtual S4 GetMinimum() const { return -MAX_INT32; }
	// NOTABUG: don't make returned values too big - it confuses Qt.
	virtual S4 GetMaximum() const { return MAX_INT32; }

	virtual S4 GetSingleStepSize() const { return 1; }

	virtual bool IsValid( S4 n ) const { return true; }
};

/*
-----------------------------------------------------------------------------
	EdProperty_UInt32
-----------------------------------------------------------------------------
*/
class EdProperty_UInt32 : public AEditableProperty
{
	mxDECLARE_CLASS( EdProperty_UInt32, AEditableProperty );

public:
	typedef U4 PropertyValType;
	typedef U4 PropertyRetType;
	typedef U4 PropertyArgType;

	typedef TCallback< PropertyRetType () >		GetterType;
	typedef TCallback< void (PropertyArgType) >	SetterType;

	EdProperty_UInt32(
		EdPropertyList* parent, const char* name,
		PropertyValType & theValue,
		PropertyFlags flags = PF_DefaultFlags
		)
		: Super( name, parent )
	{
		m_ptr = &theValue;

		m_getter = ConstMemberCallbackFactory0< PropertyRetType, ThisType >().MakeCallback< &ThisType::GetPOD >( *this );
		m_setter = GetCallbackFactory( &ThisType::SetPOD ).MakeCallback< &ThisType::SetPOD >( *this );

		m_flags = flags;
	}


	//template
	//<
	//	class KLASS,
	//	typename PropertyRetType (KLASS::*GETTER)() const
	//>
	//EdProperty_UInt32(
	//	bool dummy,
	//	EdPropertyList* parent, const char* name,
	//	const KLASS * theObject
	//	)
	//	: Super( name, parent )
	//{
	//	m_getter = nil;//ConstMemberCallbackFactory0< PropertyRetType, KLASS >().MakeCallback< GETTER >( theObject );
	//	m_setter = nil;

	//	m_flags = PF_ReadOnly;
	//}

	// you must call Bind() after this ctor
	EdProperty_UInt32(
		EdPropertyList* parent, const char* name
		)
		: Super( name, parent )
	{
		m_flags = PF_ReadOnly;
	}

	template
	<
		class KLASS,
		typename PropertyRetType (KLASS::*GETTER)() const
	>
	void Bind( const KLASS& theObject )
	{
		m_getter = ConstMemberCallbackFactory0< PropertyRetType, KLASS >().MakeCallback< GETTER >( theObject );

		m_flags = PF_ReadOnly;
	}

	//template
	//<
	//	class KLASS,
	//	typename PropertyRetType (KLASS::*GETTER)() const,
	//	typename void (KLASS::*SETTER)(PropertyArgType)
	//>
	//EdProperty_UInt32(
	//	EdPropertyList* parent, const char* name,
	//	const KLASS * theObject
	//	)
	//	: Super( name, parent )
	//{
	//	m_getter = ConstMemberCallbackFactory0< PropertyRetType, KLASS >().MakeCallback< GETTER >( theObject );
	//	m_setter = GetCallbackFactory( SETTER ).MakeCallback< SETTER >( theObject );
	//}



	PropertyRetType Get() const
	{
		Assert(m_getter);
		return m_getter();
	}
	void Set( PropertyArgType newValue ) const
	{
		Assert(m_setter);
		if( m_setter != nil )
		{
			m_setter( newValue );
		}
	}

	virtual PropertyRetType GetMinimum() const { return 0; }
	// NOTABUG: don't make returned values too big - it confuses Qt.
	virtual PropertyRetType GetMaximum() const { return MAX_INT32; }

	virtual PropertyRetType GetSingleStepSize() const { return 1; }

	virtual bool IsValid( PropertyArgType n ) const { return true; }

private:
	PropertyRetType GetPOD() const
	{
		return *m_ptr;
	}
	void SetPOD( PropertyArgType newValue )
	{
		*m_ptr = newValue;
	}

private:
	GetterType	m_getter;
	SetterType	m_setter;
	TPtr<PropertyValType>	m_ptr;
};

/*
-----------------------------------------------------------------------------
	EdProperty_Float
-----------------------------------------------------------------------------
*/
class EdProperty_Float : public AEditableProperty
{
	mxDECLARE_CLASS( EdProperty_Float, AEditableProperty );

public:
	typedef F4 PropertyValType;
	typedef F4 PropertyRetType;
	typedef F4 PropertyArgType;

	typedef TCallback< PropertyRetType () >		GetterType;
	typedef TCallback< void (PropertyArgType) >	SetterType;


	EdProperty_Float(
		EdPropertyList* parent, const char* name,
		PropertyValType & theValue,
		PropertyFlags flags = PF_DefaultFlags
		)
		: Super( name, parent )
	{
		m_ptr = &theValue;

		m_getter = ConstMemberCallbackFactory0< PropertyRetType, ThisType >().MakeCallback< &ThisType::GetPOD >( *this );
		m_setter = GetCallbackFactory( &ThisType::SetPOD ).MakeCallback< &ThisType::SetPOD >( *this );

		m_flags = flags;
	}

#if 0
	template
	<
		class KLASS,
		typename PropertyRetType (KLASS::*GETTER)() const,
		typename void (KLASS::*SETTER)(PropertyArgType)
	>
	EdProperty_Float(
		EdPropertyList* parent, const char* name,
		const KLASS * theObject
		)
		: Super( name, parent )
	{
		m_getter = ConstMemberCallbackFactory0< PropertyRetType, KLASS >().MakeCallback< GETTER >( theObject );
		m_setter = GetCallbackFactory( SETTER ).MakeCallback< SETTER >( theObject );
	}
#endif

	virtual PropertyRetType Get() const
	{
		return m_getter();
	}
	virtual void Set( PropertyArgType newValue )
	{
		m_setter( newValue );
	}

	// FLT_MAX confuses GUI framework
	//virtual F32 GetMinimum() const { return -FLT_MAX; }
	//virtual F32 GetMaximum() const { return +FLT_MAX; }
	virtual F4 GetMinimum() const { return -9999.0f; }
	virtual F4 GetMaximum() const { return +9999.0f; }

	virtual F4 GetSingleStepSize() const { return 1.0f; }

	virtual bool IsValid( F4 n ) const { return true; }

	PropertyRetType GetPOD() const
	{
		return *m_ptr;
	}
	void SetPOD( PropertyArgType newValue )
	{
		*m_ptr = newValue;
	}

protected:
	EdProperty_Float(
		EdPropertyList* parent, const char* name
		)
		: Super( name, parent )
	{}

private:
	GetterType	m_getter;
	SetterType	m_setter;
	TPtr<PropertyValType>	m_ptr;
};

/*
-----------------------------------------------------------------------------
	EdProperty_Vector3D
-----------------------------------------------------------------------------
*/
class EdProperty_Vector3D : public AEditableProperty
{
	mxDECLARE_CLASS( EdProperty_Vector3D, AEditableProperty );

public:
	typedef Vec3D PropertyValType;
	typedef Vec3D PropertyRetType;
	typedef const Vec3D& PropertyArgType;

	typedef TCallback< PropertyRetType () >		GetterType;
	typedef TCallback< void (PropertyArgType) >	SetterType;

	EdProperty_Vector3D(
		EdPropertyList* parent, const char* name,
		PropertyValType & theValue,
		PropertyFlags flags = PF_DefaultFlags
		)
		: Super( name, parent )
	{
		m_ptrPOD = &theValue;

		m_flags = flags;
	}

	virtual PropertyRetType Get() const
	{
		return *m_ptrPOD;
	}
	virtual void Set( PropertyArgType newValue )
	{
		*m_ptrPOD = newValue;
	}

protected:
	EdProperty_Vector3D(
		EdPropertyList* parent, const char* name
		)
		: Super( name, parent )
	{}

private:
	TPtr<PropertyValType>	m_ptrPOD;
};

/*
-----------------------------------------------------------------------------
	EdProperty_Quat
-----------------------------------------------------------------------------
*/
class EdProperty_Quat : public AEditableProperty
{
	mxDECLARE_CLASS( EdProperty_Quat, AEditableProperty );

public:
	typedef Quat PropertyValType;
	typedef Quat PropertyRetType;
	typedef const Quat& PropertyArgType;

	typedef TCallback< PropertyRetType () >		GetterType;
	typedef TCallback< void (PropertyArgType) >	SetterType;

	EdProperty_Quat( EdPropertyList* parent, const char* name )
		: Super( name, parent )
	{}

	virtual PropertyRetType Get() const = 0;
	virtual void Set( PropertyArgType newValue ) = 0;
};

/*
-----------------------------------------------------------------------------
	EdProperty_Array
-----------------------------------------------------------------------------
*/
class EdProperty_Array : public EdPropertyList
{
	mxDECLARE_CLASS( EdProperty_Array, EdPropertyList );

public:
	virtual ~EdProperty_Array()
	{
	}

#if 0
	template< class TYPE >
	EdProperty_Array( EdPropertyList* parent, const char* name, TList< TYPE > & theArray )
		: Super( name, parent )
	{
		for( UINT iProperty = 0; iProperty < theArray.Num(); iProperty++ )
		{
			TYPE & aProperty = theArray[ iProperty ];
			aProperty.edCreateProperties( *this );
		}
	}

	template< class TYPE >
	EdProperty_Array( EdPropertyList* parent, const char* name, TFixedArray< TYPE > & theArray )
		: Super( name, parent )
	{
		for( UINT iProperty = 0; iProperty < theArray.Num(); iProperty++ )
		{
			TYPE & aProperty = theArray[ iProperty ];
			aProperty.edCreateProperties( *this );
		}
	}
#endif

	template< typename TYPE, typename SIZETYPE >
	EdProperty_Array( EdPropertyList* parent, const char* name, TArrayBase< TYPE, SIZETYPE > & a )
		: Super( name, parent )
	{
		TYPE* propertiesArray = a.ToPtr();
		for( UINT iProperty = 0; iProperty < a.Num(); iProperty++ )
		{
			TYPE & aProperty = propertiesArray[ iProperty ];
			aProperty.edCreateProperties( *this );
		}
	}
};




/*
-----------------------------------------------------------------------------
	EdProperty_String
-----------------------------------------------------------------------------
*/
class EdProperty_String : public AEditableProperty
{
	mxDECLARE_CLASS( EdProperty_String, AEditableProperty );

public:
	typedef const char* PropertyValType;
	typedef const char* PropertyRetType;
	typedef const char* PropertyArgType;

	template< UINT SIZE >
	EdProperty_String(
		EdPropertyList* parent,
		const char* name,
		TStackString<SIZE>& theString,
		PropertyFlags flags = 0
		)
		: Super( name, parent )
	{
		typedef TStackString<SIZE> ClassType;

		m_setter = GetCallbackFactory( &ClassType::SetString ).MakeCallback< &ClassType::SetString >( theString );
		m_getter = ConstMemberCallbackFactory0< PropertyRetType, ClassType >().MakeCallback< &ClassType::ToChars >( theString );

		m_flags = flags;
	}

	virtual void Set( PropertyArgType newValue )
	{
		m_setter( newValue );
	}
	virtual PropertyRetType Get() const
	{
		return m_getter();
	}

protected:
	EdProperty_String(
		EdPropertyList* parent,
		const char* name,
		PropertyFlags flags = PF_ReadOnly
		)
		: Super( name, parent )
	{
		m_flags = flags;
	}

private:
	TCallback< void (PropertyArgType) >	m_setter;
	TCallback< PropertyRetType (void) >	m_getter;
};

/*
-----------------------------------------------------------------------------
	EdProperty_ClassGuid
-----------------------------------------------------------------------------
*/
class EdProperty_ClassGuid : public AEditableProperty
{
	mxDECLARE_CLASS( EdProperty_ClassGuid, AEditableProperty );

public:
	EdProperty_ClassGuid(
		EdPropertyList* parent,
		const char* name,
		TypeGUIDArg typeId
		)
		: Super( name, parent )
		, m_typeId( typeId )
	{
		m_flags = PF_ReadOnly;
	}

	const char* rttiGetTypeName() const
	{
		if( m_typeId != mxNULL_TYPE_GUID ) {
			return TypeRegistry::Get().FindClassInfoByGuid( m_typeId )->GetTypeName();
		}
		return "Unknown Class";
	}

private:
	const TypeGUID	m_typeId;
};


/*
-----------------------------------------------------------------------------
	EdProperty_FileTime
-----------------------------------------------------------------------------
*/
class EdProperty_FileTime : public AEditableProperty
{
	mxDECLARE_CLASS( EdProperty_FileTime, AEditableProperty );

public:
	EdProperty_FileTime(
		EdPropertyList* parent,
		const char* name,
		const FileTime& fileTime
		)
		: Super( name, parent )
		, m_fileTime( fileTime )
	{
		m_flags = PF_ReadOnly;
	}

	const FileTime& Get() const
	{
		return m_fileTime;
	}

private:
	const FileTime &	m_fileTime;
};

/*
-----------------------------------------------------------------------------
	EdProperty_ColorRGBA
-----------------------------------------------------------------------------
*/
class FColor;
class EdProperty_ColorRGBA : public AEditableProperty
{
	mxDECLARE_CLASS( EdProperty_ColorRGBA, AEditableProperty );

public:
	typedef const FColor& PropertyType;

	EdProperty_ColorRGBA(
		EdPropertyList* parent,
		const char* name,
		FColor& theColorRGBA,
		PropertyFlags flags = 0
		)
		: Super( name, parent )
		, m_value( theColorRGBA )
	{
		m_flags = flags;
	}

	void Set( const FColor& newValue );
	const FColor& Get() const;

private:
	FColor &	m_value;
};


/*
-----------------------------------------------------------------------------
	Property_32BitHexId
-----------------------------------------------------------------------------
*/
class Property_32BitHexId : public AEditableProperty
{
	mxDECLARE_CLASS( Property_32BitHexId, AEditableProperty );

public:
	typedef U4 PropertyType;

	Property_32BitHexId(
		EdPropertyList* parent,
		const char* name,
		const PropertyType& id
		)
		: Super( name, parent )
		, m_id( id )
	{
		m_flags = PF_ReadOnly;
	}

	const PropertyType& Get() const
	{
		return m_id;
	}

private:
	const PropertyType &	m_id;
};



/*
-----------------------------------------------------------------------------
	EdProperty_VoidPointer
-----------------------------------------------------------------------------
*/
class EdProperty_VoidPointer : public AEditableProperty
{
	mxDECLARE_CLASS( EdProperty_VoidPointer, AEditableProperty );

public:
	typedef const void* PropertyValType;
	typedef const void* PropertyRetType;
	typedef const void* PropertyArgType;

	typedef TCallback< PropertyRetType () >		GetterType;
	typedef TCallback< void (PropertyArgType) >	SetterType;

	EdProperty_VoidPointer(
		EdPropertyList* parent, const char* name
		)
		: Super( name, parent )
	{
		m_flags = PF_ReadOnly;
	}

	template
	<
		class KLASS,
		typename PropertyRetType (KLASS::*GETTER)() const
	>
	void Bind( const KLASS& theObject )
	{
		m_getter = ConstMemberCallbackFactory0< PropertyRetType, KLASS >().MakeCallback< GETTER >( theObject );

		m_flags = PF_ReadOnly;
	}

	PropertyRetType Get() const
	{
		Assert(m_getter);
		return m_getter();
	}

private:
	PropertyRetType GetPOD() const
	{
		return *m_ptr;
	}
	void SetPOD( PropertyArgType newValue )
	{
		*m_ptr = newValue;
	}

private:
	GetterType	m_getter;
	TPtr<PropertyValType>	m_ptr;
};


/*
-----------------------------------------------------------------------------
	EdProperty_AssetReference

	is an object property
	representing a (generic) reference to some resource
-----------------------------------------------------------------------------
*/
class EdProperty_AssetReference : public AEditableProperty
{
	mxDECLARE_CLASS( EdProperty_AssetReference, AEditableProperty );

public:

	template< class RESOURCE >
	EdProperty_AssetReference(
		TResPtr<RESOURCE>& resourceHandle,
		const char* name,
		EdPropertyList* parent
	)
		: Super( name, parent )
		, m_resourceType( resourceHandle.GetAssetType() )
		, m_resourceHandle( resourceHandle )
	{
	}

	SResPtrBase& GetHandle();
	SResourceObject* Get() const;
	void SetDefaultInstance();

	void Set( ObjectGUIDArg assetGuid );
	void Set( SResourceObject* newValue );

	EAssetType GetAssetType();

public:
	// drag & drop support: resources can be dropped onto resource ref editor widget
	virtual bool edOnItemDropped( AEditable* pDroppedItem ) override;

private:
	const EAssetType		m_resourceType;
	TValidRef<SResPtrBase>	m_resourceHandle;
};



/*
-----------------------------------------------------------------------------
	EdProperty_Enum
-----------------------------------------------------------------------------
*/
class EdProperty_Enum : public AEditableProperty
{
	mxDECLARE_CLASS( EdProperty_Enum, AEditableProperty );

public:
	typedef const UINT PropertyRetType;
	typedef const UINT PropertyArgType;


	EdProperty_Enum(
		EdPropertyList* parent,
		const mxEnumType& enumType
	);

	virtual PropertyRetType Get() const = 0;
	virtual void Set( PropertyArgType newEnumValue ) = 0;

	const mxEnumType& GetEnumReflection() const
	{
		return m_enumType;
	}
	const mxEnumType::Member& GetCurrEnumItem() const
	{
		const mxEnumType& enumType = this->GetEnumReflection();
		const UINT currItemValue = this->Get();
		const UINT currItemIndex = enumType.GetItemIndexByValue(currItemValue);
		Assert(currItemIndex != INDEX_NONE);
		return enumType.m_members[ currItemIndex ];
	}

protected:
	const mxEnumType &	m_enumType;
};

class EdProperty_Enum_Concrete : public EdProperty_Enum
{
	void *	m_enumPtr;

public:
	typedef EdProperty_Enum Super;

	EdProperty_Enum_Concrete(
		EdPropertyList* parent,
		const mxEnumType& enumType,
		void * pEnum
	)
		: Super( parent, enumType )
	{
		m_enumPtr = pEnum;
	}

	virtual PropertyRetType Get() const override
	{
		return m_enumType.m_accessor.Get_Value( m_enumPtr );
	}
	virtual void Set( PropertyArgType newEnumValue ) override
	{
		m_enumType.m_accessor.Set_Value( m_enumPtr, newEnumValue );
	}
};

//---------------------------------------------------------------------------

#define MxProperty( PARENT, OBJECT_KLASS, PROPERTY_KLASS, NAME, GETTER, SETTER, ... )\
{\
	class Property##GETTER##SETTER : public PROPERTY_KLASS\
	{\
		OBJECT_KLASS &	m_propertyObject;\
	public:\
		Property##GETTER##SETTER( EdPropertyList* parent, const char* name, OBJECT_KLASS& propertyObject )\
			: PROPERTY_KLASS( parent, name )\
			, m_propertyObject( propertyObject )\
		{\
		}\
		virtual void Set( PropertyArgType newValue ) override\
		{\
			m_propertyObject.SETTER( newValue );\
		}\
		virtual PropertyRetType Get() const override\
		{\
			return m_propertyObject.GETTER();\
		}\
	};\
	new Property##GETTER##SETTER( PARENT, NAME, *this );\
}

//---------------------------------------------------------------------------


#define MxEnumPropertyX( OBJECT_KLASS, ENUM_REFLECTION, GETTER, SETTER )\
{\
	class Property##GETTER##SETTER : public EdProperty_Enum\
	{\
		OBJECT_KLASS &	m_propertyObject;\
	public:\
		Property##GETTER##SETTER( EdPropertyList* parent, const mxEnumType& enumType, OBJECT_KLASS& propertyObject )\
			: EdProperty_Enum( parent, enumType )\
			, m_propertyObject( propertyObject )\
		{\
		}\
		virtual void Set( PropertyArgType newValue ) override\
		{\
			m_propertyObject.SETTER( newValue );\
		}\
		virtual PropertyRetType Get() const override\
		{\
			return m_propertyObject.GETTER();\
		}\
	};\
	new Property##GETTER##SETTER( properties, ENUM_REFLECTION, *this );\
}
//---------------------------------------------------------------------------

#define MxPropertyArray( NAME, MEMBER )\
{\
	new EdProperty_Array( properties, NAME, MEMBER );\
}


//---------------------------------------------------------------------------






#define mxIMPLEMENT_COMMON_PROPERTIES( className )\
	FORCEINLINE const void* property_GetObjectAddress() const\
	{ return this; }\
	FORCEINLINE SizeT property_GetObjectSize() const\
	{ return sizeof *this; }


#define mxCOLLECT_COMMON_PROPERTIES( className, propertyCollectorPtr )\
	(new EdProperty_UInt32( propertyCollectorPtr, "["#className":Size]" ))\
		->Bind< class className, &className::property_GetObjectSize >( *this );\
	(new EdProperty_VoidPointer( propertyCollectorPtr, "["#className":Addr]" ))\
		->Bind< class className, &className::property_GetObjectAddress >( *this );\
	;






//struct EdProperty_Struct : public AEditableProperty
//{
//	mxDECLARE_CLASS( EdProperty_Struct, AEditableProperty );
//
//	EdProperty_UInt32(
//		EdPropertyList* parent, const char* name,
//		const mxClass& typeInfo,
//		PropertyFlags flags = PF_DefaultFlags
//		)
//		: Super( name, parent )
//	{
//		m_ptr = &theValue;
//
//		m_getter = ConstMemberCallbackFactory0< PropertyRetType, ThisType >().MakeCallback< &ThisType::GetPOD >( *this );
//		m_setter = GetCallbackFactory( &ThisType::SetPOD ).MakeCallback< &ThisType::SetPOD >( *this );
//
//		m_flags = flags;
//	}
//};

struct EdAssetGuidContainer : public AEditable
{
	ObjectGUID		assGuid;

public:
	mxDECLARE_CLASS( EdAssetGuidContainer, AEditable );
};

mxNAMESPACE_END

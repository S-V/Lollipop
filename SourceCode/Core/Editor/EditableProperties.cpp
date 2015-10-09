// Dynamic properties used by the editor
//
#include <Core_PCH.h>
#pragma hdrstop
#include <Core.h>

#include <Base/Util/Color.h>
#include <Core/Editor/EditableProperties.h>


mxNAMESPACE_BEGIN


/*
-----------------------------------------------------------------------------
	APropertyEditor
-----------------------------------------------------------------------------
*/
APropertyEditor::APropertyEditor( AEditableProperty* theProperty )
{
	AssertPtr( theProperty );
	//Assert( theProperty->editor == nil );
	theProperty->editor = this;

	m__property = theProperty;
}

APropertyEditor::~APropertyEditor()
{
	//Assert( m__property->editor == this );
	//m__property->editor = nil;
}

/*
-----------------------------------------------------------------------------
	AProperty
-----------------------------------------------------------------------------
*/
mxDEFINE_ABSTRACT_CLASS( AEditableProperty );

AEditableProperty::AEditableProperty( const char* name, EdPropertyList* parent )
	: m_name( name )
	, m_parent( parent )
{
	if( parent != nil )
	{
		parent->Add( this );
	}

	m_flags = PF_DefaultFlags;

	//DBGOUT("Creating property '%s' of class '%s'\n",
	//	m_name,this->rttiGetClass().GetTypeName());
}

AEditableProperty::~AEditableProperty()
{
	//DBGOUT("Destroying property '%s' of class '%s'\n",
	//	m_name,this->rttiGetClass().GetTypeName());
}

UINT AEditableProperty::edNumRows() const
{
	return 0;
}

AEditable* AEditableProperty::edGetParent()
{
	return m_parent;
}

const char* AEditableProperty::edToChars( UINT column ) const
{
	return m_name;
}

const char* AEditableProperty::edGetName() const
{
	return m_name;
}

/*
-----------------------------------------------------------------------------
	EdPropertyList
-----------------------------------------------------------------------------
*/
mxDEFINE_CLASS( EdPropertyList );

UINT EdPropertyList::edNumRows() const
{
	return m_properties.Num();
}
AEditable* EdPropertyList::edGetParent()
{
	return AEditableProperty::edGetParent();
}
AEditable* EdPropertyList::edItemAt( UINT index )
{
	return m_properties.At(index);
}
UINT EdPropertyList::edIndexOf( const AEditable* child ) const
{
	for( UINT iProperty = 0; iProperty < m_properties.Num(); iProperty++ )
	{
		if( m_properties[ iProperty ] == child )
		{
			return iProperty;
		}
	}
	return INDEX_NONE;
}
const char* EdPropertyList::edToChars( UINT column ) const
{
	return AEditableProperty::edToChars(column);
}

/*
-----------------------------------------------------------------------------
	EdProperty_Bool
-----------------------------------------------------------------------------
*/
mxDEFINE_CLASS_NO_DEFAULT_CTOR( EdProperty_Bool );

/*
-----------------------------------------------------------------------------
	EdProperty_UInt32
-----------------------------------------------------------------------------
*/
mxDEFINE_CLASS_NO_DEFAULT_CTOR( EdProperty_UInt32 );

/*
-----------------------------------------------------------------------------
	EdProperty_Float
-----------------------------------------------------------------------------
*/
mxDEFINE_CLASS_NO_DEFAULT_CTOR( EdProperty_Float );

/*
-----------------------------------------------------------------------------
	EdProperty_Vector3D
-----------------------------------------------------------------------------
*/
mxDEFINE_CLASS_NO_DEFAULT_CTOR( EdProperty_Vector3D );

/*
-----------------------------------------------------------------------------
	EdProperty_Quat
-----------------------------------------------------------------------------
*/
mxDEFINE_CLASS_NO_DEFAULT_CTOR( EdProperty_Quat );

/*
-----------------------------------------------------------------------------
	EdProperty_Array
-----------------------------------------------------------------------------
*/
mxDEFINE_CLASS_NO_DEFAULT_CTOR( EdProperty_Array );


mxDEFINE_CLASS_NO_DEFAULT_CTOR( EdProperty_String );
mxDEFINE_CLASS_NO_DEFAULT_CTOR( EdProperty_ClassGuid );
mxDEFINE_CLASS_NO_DEFAULT_CTOR( EdProperty_FileTime );
mxDEFINE_CLASS_NO_DEFAULT_CTOR( Property_32BitHexId );
mxDEFINE_CLASS_NO_DEFAULT_CTOR( EdProperty_VoidPointer );

/*
-----------------------------------------------------------------------------
	EdProperty_AssetReference
-----------------------------------------------------------------------------
*/
mxDEFINE_CLASS_NO_DEFAULT_CTOR( EdProperty_AssetReference );

SResPtrBase& EdProperty_AssetReference::GetHandle()
{
	return m_resourceHandle.ToRef();
}

SResourceObject* EdProperty_AssetReference::Get() const
{
	return m_resourceHandle.ToRef().m_pointer;
}

void EdProperty_AssetReference::SetDefaultInstance()
{
	m_resourceHandle.ToRef().Internal_SetDefault( m_resourceType );
}

void EdProperty_AssetReference::Set( ObjectGUIDArg assetGuid )
{
	m_resourceHandle.ToRef().Internal_SetPointer( m_resourceType, assetGuid );
}

void EdProperty_AssetReference::Set( SResourceObject* newValue )
{
	CHK_VRET_IF_NIL(newValue);

	m_resourceHandle.ToRef().m_pointer = newValue;
}

EAssetType EdProperty_AssetReference::GetAssetType()
{
	return m_resourceType;
}

bool EdProperty_AssetReference::edOnItemDropped( AEditable* pDroppedItem )
{
	AssertPtr(pDroppedItem);

	EdAssetGuidContainer* pAssRef = SafeCast< EdAssetGuidContainer >( pDroppedItem );
	if( pAssRef != nil )
	{
		m_resourceHandle.ToRef().Internal_SetPointer( m_resourceType, pAssRef->assGuid );
		return true;
	}

	return false;
}

/*
-----------------------------------------------------------------------------
	EdProperty_ColorRGBA
-----------------------------------------------------------------------------
*/
mxDEFINE_CLASS_NO_DEFAULT_CTOR( EdProperty_ColorRGBA );

void EdProperty_ColorRGBA::Set( const FColor& newValue )
{
	m_value = newValue;
}
const FColor& EdProperty_ColorRGBA::Get() const
{
	return m_value;
}

/*
-----------------------------------------------------------------------------
	EdProperty_Enum
-----------------------------------------------------------------------------
*/
mxDEFINE_CLASS_NO_DEFAULT_CTOR( EdProperty_Enum );

EdProperty_Enum::EdProperty_Enum(
	EdPropertyList* parent,
	const mxEnumType& enumType
	)
	: Super( enumType.m_name, parent )
	, m_enumType( enumType )
{
}

mxDEFINE_CLASS( EdAssetGuidContainer );

mxNAMESPACE_END

NO_EMPTY_FILE

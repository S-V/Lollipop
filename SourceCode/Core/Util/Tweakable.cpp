#include <Core_PCH.h>
#pragma hdrstop
#include <Core.h>

#include <Core/Editor/EditableProperties.h>
#include "Tweakable.h"

#if MX_EDITOR

#define IMPLEMENT_TWEAKABLE_POD_PROPERTY( TheTypeName, TheBaseClassName )\
class PodProperty_##TheTypeName : public TheBaseClassName\
{\
	TheTypeName *	m_ptr;\
public:\
	PodProperty_##TheTypeName( TheTypeName* ptr, const char* name )\
		: TheBaseClassName( nil, name )\
	{\
		m_ptr = ptr;\
	}\
	virtual void Set( TheBaseClassName::PropertyArgType newValue )\
	{\
		*m_ptr = newValue;\
	}\
	virtual TheBaseClassName::PropertyRetType Get() const\
	{\
		return *m_ptr;\
	}\
	virtual AEditable* edGetParent() const\
	{\
		return gCore.tweaks;\
	}\
};

IMPLEMENT_TWEAKABLE_POD_PROPERTY(bool,EdProperty_Bool);
IMPLEMENT_TWEAKABLE_POD_PROPERTY(F4,EdProperty_Float);


TweakUtil::TweakUtil()
{
}

TweakUtil::~TweakUtil()
{

}

bool TweakUtil::Tweak_Bool( bool * var, const char* expr, const char* file, int line )
{
	AEditableProperty* pExisting = m_varsByPtr.FindRef( var );
	if (pExisting == nil)
	{
		AEditableProperty* pNewEntry = new PodProperty_bool( var, expr );

		m_varsByPtr.Set( var, pNewEntry );

		gCore.editor->Notify_ObjectModified( this );
	}
	return *var;
}

F4 TweakUtil::Tweak_Float32( F4 * var, const char* expr, const char* file, int line )
{
	AEditableProperty* pExisting = m_varsByPtr.FindRef( var );
	if (pExisting == nil)
	{
		AEditableProperty* pNewEntry = new PodProperty_F4( var, expr );

		m_varsByPtr.Set( var, pNewEntry );

		gCore.editor->Notify_ObjectModified( this );
	}
	return *var;
}

UINT TweakUtil::edNumRows() const
{
	return m_varsByPtr.NumEntries();
}
AEditable* TweakUtil::edGetParent()
{
	return nil;
}
AEditable* TweakUtil::edItemAt( UINT index )
{
	return m_varsByPtr.GetPairs()[index].value;
}
UINT TweakUtil::edIndexOf( const AEditable* child ) const
{
	const AEditableProperty* pVar = checked_cast< const AEditableProperty* >( child );
	return m_varsByPtr.FindKeyIndex( pVar );
}
const char* TweakUtil::edToChars( UINT column ) const
{
	return "Tweakable variables";
}
void TweakUtil::edCreateProperties( EdPropertyList *properties, bool bClear )
{
	properties->Clear();
	const UINT numItems = m_varsByPtr.NumEntries();
	for (UINT iProperty = 0; iProperty < numItems; iProperty++)
	{
		properties->Add( m_varsByPtr.GetPairs()[iProperty].value );
	}
}

#endif // MX_EDITOR


NO_EMPTY_FILE

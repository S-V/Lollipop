#include <Base_PCH.h>
#pragma hdrstop
#include <Base.h>

#include <Base/Object/EnumType.h>

UINT mxEnumType::GetItemIndexByValue( const UINT nEnumValue ) const
{
	for( UINT iEnumItem = 0; iEnumItem < m_numMembers; iEnumItem++ )
	{
		const Member& rEnumItem = m_members[ iEnumItem ];
		if( rEnumItem.value == nEnumValue )
		{
			return iEnumItem;
		}
	}
	mxDBG_UNREACHABLE;
	return INDEX_NONE;
}

UINT mxEnumType::GetItemIndexByString( const char* szValue ) const
{
	CHK_VRET_X_IF_NIL( szValue, -1 );
	for( UINT iEnumItem = 0; iEnumItem < m_numMembers; iEnumItem++ )
	{
		const Member& rEnumItem = m_members[ iEnumItem ];
		if( mxStrEquAnsi( rEnumItem.alias, szValue ) )
		{
			return iEnumItem;
		}
	}
	mxDBG_UNREACHABLE;
	return -1;
}

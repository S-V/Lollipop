/*
=============================================================================
	File:	Components.cpp
	Desc:	
=============================================================================
*/

#include <Core_PCH.h>
#pragma hdrstop
#include <Core.h>

// for resetting EMemHeap::ResourceData
#include <Base/Memory/Memory_Private.h>

#include <Core/Entity/System.h>
#include <Core/Entity/Components.h>

mxNAMESPACE_BEGIN

mxDEFINE_ABSTRACT_CLASS(ASubsystem);



#if 0//MX_EDITOR

//---------------------------------------------------------------------------
mxDEFINE_CLASS(EdComponentView,AEditableRefCounted);

EdComponentView::EdComponentView( AEditableRefCounted* parent )
{
	m_parent = parent;
}

EdComponentView::~EdComponentView()
{

}

AEditable* EdComponentView::edGetParent()
{
	return m_parent;
}

void EdComponentView::Serialize( mxArchive & archive )
{
	Super::Serialize(archive);

	archive && m_parent;
}

#endif // MX_EDITOR

mxNAMESPACE_END

NO_EMPTY_FILE

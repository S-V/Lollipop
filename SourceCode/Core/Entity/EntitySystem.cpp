/*
=============================================================================
	File:	System.cpp
	Desc:	Entity management system.
=============================================================================
*/

#include <Core_PCH.h>
#pragma hdrstop
#include <Core.h>

// for resetting EMemHeap::ResourceData
#include <Base/Memory/Memory_Private.h>

#include <Core/Entity/System.h>


mxNAMESPACE_BEGIN

namespace
{
	struct SCompChain
	{
		SCompLink	m_head;

	public:
		SCompChain()
		{
			ZERO_OUT( m_head );
		}
		void Link( EComponentType type, ComponentHandle )
		{
UNDONE;
		}
	};

	struct SEntityData
	{
		//ASubsystem* sys;
		//TList<AEntityAspect>	aspects;

		UINT nextFreeSlot;

	public:
		SEntityData() {
			//sys = nil;
			nextFreeSlot = INDEX_NONE;
		}
	};
	struct PrivateData
	{
		UINT	firstFreeSlot;
		TList<SEntityData>	entities;

	public:
		PrivateData()
		{
			firstFreeSlot = INDEX_NONE;
		}
	};

	static TBlob16<PrivateData>	gPrivateData;

	#define gData	gPrivateData.Get()

}//namespace

void EntitySystem_Setup()
{
	mxPut("Initializing Entity system.\n");

	gPrivateData.Construct();
}

void EntitySystem_Close()
{
	gPrivateData.Destruct();
}

EntityId Entity_CreateNew()
{
	if( gData.firstFreeSlot != INDEX_NONE )
	{
		const EntityId newEntityId = gData.firstFreeSlot;

		SEntityData& newEntityData = gData.entities.Add();
		{
			newEntityData.nextFreeSlot = INDEX_NONE;
		}

		gData.firstFreeSlot = INDEX_NONE;

		return newEntityId;
	}
	else
	{
		const EntityId newEntityId = gData.entities.Num();

		SEntityData& newEntityData = gData.entities.Add();
		{
			newEntityData.nextFreeSlot = INDEX_NONE;
		}

		return newEntityId;
	}
}

void Entity_Destroy( EntityId existingEntity )
{
	SEntityData& theEntityData = gData.entities[ existingEntity ];
	{
		theEntityData.nextFreeSlot = existingEntity;
	}
}



void Entity_AttachAspect( EntityId theEntity, AEntityAspect* newAspect )
{
UNDONE;
}

void Entity_RemoveAspect()
{
UNDONE;
}




#if 0//MX_EDITOR

//---------------------------------------------------------------------------
mxDEFINE_CLASS(EdEntityView,AEditableRefCounted);

EdEntityView::EdEntityView()
	: TRefCountedObjectList
{

}

EdEntityView::~EdEntityView()
{

}

//---------------------------------------------------------------------------
mxDEFINE_CLASS(EdEntityList,AEditableRefCounted);

EdEntityList::EdEntityList()
{

}

EdEntityList::~EdEntityList()
{

}


AEditable* EdEntityList::edGetParent()
{
	return m_containingWorld;
}

//---------------------------------------------------------------------------
EdEntityView* Entity_GetEditorProxy( EntityId existingEntity )
{
Unimplemented;
return nil;
}

#endif // MX_EDITOR


mxNAMESPACE_END

NO_EMPTY_FILE

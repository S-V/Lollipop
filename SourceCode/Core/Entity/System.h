/*
=============================================================================
	File:	System.h
	Desc:	Entity management system.
			Entity System (ES) is a technique for building computer games,
			with advantages in terms of: performance, extensibility, game-design.
	Note:	

	See:
	http://www.insomniacgames.com/dynamic-component-system/
	http://www.insomniacgames.com/assets/filesadynamiccomponentarchitectureforhighperformancegameplay.pdf
	http://t-machine.org/index.php/2007/09/03/entity-systems-are-the-future-of-mmog-development-part-1/
	http://scottbilas.com/games/dungeon-siege/

	http://cowboyprogramming.com/2007/01/05/evolve-your-heirachy/
	http://gamedev.fh-hagenberg.at/wiki/index.php?title=Componentbased_entity_systems
	http://www.oddrosemedia.com/wiki/index.php?title=Articles
	http://robertwrose.com/2007/02/component-oriented-programming.html
	http://entity-systems.wikidot.com/


	A great place to start for an intro to data-oriented design is the following presentation:
	http://seven-degrees-of-freedom.blogspot.com/2009/12/pitfalls-of-object-oriented-programming.html
	http://gamesfromwithin.com/great-presentation-on-data-oriented-design
	[PDF] http://research.scee.net/files/presentations/gcapaustralia09/Pitfalls_of_Object_Oriented_Programming_GCAP_09.pdf
	http://gamearchitect.net/2008/06/01/an-anatomy-of-despair-aggregation-over-inheritance/

	and the discussions here:
	http://solid-angle.blogspot.com/2010/02/musings-on-data-oriented-design.html
	http://gamesfromwithin.com/data-oriented-design

	and those slides offer a pretty good summary:
	http://www.slideshare.net/DICEStudio/introduction-to-data-oriented-design

	Next, these articles and slides might be of use:
	http://molecularmusings.wordpress.com/2011/11/03/adventures-in-data-oriented-design-part-1-mesh-data-3/
	http://stackoverflow.com/questions/1641580/what-is-data-oriented-design
	http://www.slideshare.net/DICEStudio/culling-the-battlefield-data-oriented-design-in-practice
	http://gamesfromwithin.com/data-oriented-design-now-and-in-the-future
	http://gamesfromwithin.com/managing-data-relationships
	http://gamesfromwithin.com/the-always-evolving-coding-style
	http://www.gamasutra.com/view/news/38484/An_Example_In_DataOriented_Design_Sound_Parameters.php
	http://defragdev.com/blog/?p=570
	http://www.asawicki.info/news_1422_data-oriented_design_-_links_and_thoughts.html
=============================================================================
*/

#pragma once

#include <Core/Serialization.h>
#include <Core/Object.h>
#include <Core/Entity/Components.h>


mxNAMESPACE_BEGIN

// run-time entity identifier
typedef UINT EntityId;

enum { NULL_ENTITY_ID = INDEX_NONE };

/*
-----------------------------------------------------------------------------
	AEntity

	abstract base class for entity classes
-----------------------------------------------------------------------------
*/
//struct AEntity
//{
//
//};

/*
-----------------------------------------------------------------------------
	EntitySystem
-----------------------------------------------------------------------------
*/

void EntitySystem_Setup();
void EntitySystem_Close();

// creates an empty entity
EntityId Entity_CreateNew();
void Entity_Destroy( EntityId existingEntity );

void Entity_AddComponent( AComponent* newComponent );
void Entity_RemoveComponent();

void Entity_AttachAspect( EntityId theEntity, AEntityAspect* newAspect );
void Entity_RemoveAspect();


#if 0// MX_EDITOR

class EdEntityList;

// EdEntityProxy - editor representation of entity (MVC)
struct EdEntityView : public TRefCountedObjectList< EdComponentView >
{
	mxDECLARE_CLASS(EdEntityView,AEditableRefCounted);

	EdEntityView();
	~EdEntityView();

public:
	TPtr<EdEntityList>	m_parent;
};

struct EdEntityList : public TRefCountedObjectList< EdEntityView >
{
	mxDECLARE_CLASS(EdEntityList,AEditableRefCounted);

	EdEntityList();
	~EdEntityList();

	virtual AEditable* edGetParent() override;

public:
	TPtr<AEditable>		m_containingWorld;
};

EdEntityView* Entity_GetEditorProxy( EntityId existingEntity );

#endif // MX_EDITOR


mxNAMESPACE_END

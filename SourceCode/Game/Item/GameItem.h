/*
=============================================================================
	File:	GameItem.h
	Desc:	Item system.
=============================================================================
*/
#pragma once

#include <Core/Object.h>

/*
=============================================================================

  Items the player can pick up or use.

=============================================================================
*/

struct gmItem : public mxObject
{
	MX_DECLARE_ABSTRACT_CLASS(gmItem,mxObject);

	virtual ~gmItem() {}
};

struct gmInventory
{
	enum { MAX_ITEMS = 16 };
	enum { MAX_WEAPONS = 16 };

	TStaticList< gmItem::Ref, MAX_ITEMS >	items;

	TStaticList< gmItem::Ref, MAX_WEAPONS >	weapons;
};

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//

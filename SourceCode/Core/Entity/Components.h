/*
=============================================================================
	File:	Components.h
	Desc:	Entity component system.
=============================================================================
*/

#pragma once

#include <Core/Serialization.h>
#include <Core/Object.h>

mxNAMESPACE_BEGIN

// property types are hardcoded for speed
enum EComponentType
{
	Comp_RenderMesh,
	Comp_RigidBody,

	Comp_MAX
};

// max. 256 component types should be enough
#define COMPONENT_TYPE_BITS	3

// max. 8096 components in each subsystem should be enough
#define COMPONENT_UID_BITS	13

CHECK_STORAGE_BITS( COMPONENT_TYPE_BITS, Comp_MAX );

/*
-----------------------------------------------------------------------------
	AComponent

	domain-specific component
-----------------------------------------------------------------------------
*/
struct AComponent
{
	// empty to avoid vtbl overhead, etc.
	// use casts from void* type.
};

//NOTE: only COMPONENT_UID_BITS are available
typedef UINT ComponentHandle;

enum { NullComponentHandle = INDEX_NONE };

// this structure is used to iterate in a circular list of components
struct SCompLink
{
	BITFIELD thisCompType : COMPONENT_TYPE_BITS;
	BITFIELD thisCompIndex : COMPONENT_UID_BITS;	// unique id of this component in the corresponding subsystem
	BITFIELD nextCompType : COMPONENT_TYPE_BITS;
	BITFIELD nextCompIndex : COMPONENT_UID_BITS;
};

mxSTATIC_ASSERT( sizeof SCompLink == sizeof U4 );

/*
-----------------------------------------------------------------------------
	ASubsystem
-----------------------------------------------------------------------------
*/
struct ASubsystem : public AEditable
{
	mxDECLARE_ABSTRACT_CLASS(ASubsystem,AEditable);

	virtual void Serialize( mxArchive & archive ) {}

public:	// Editor

protected:
	ASubsystem() {}
	virtual ~ASubsystem() {}
};




#if MX_EDITOR

class EdCompGroup;
class EdCompGroupList;

enum ECompGroup
{
	CompGroup_Unknown = 0,

	CompGroup_Lights,
	CompGroup_StaticModel,
	CompGroup_SkinnedModel,

	CompGroup_MAX
};

struct EdComponentView : public AEditableRefCounted
{
	mxDECLARE_CLASS(EdComponentView,AEditableRefCounted);

	EdComponentView( AEditableRefCounted* parent = nil );
	~EdComponentView();

	virtual ECompGroup GetTypeEnum() const
	{
		return CompGroup_Unknown;
	}

	virtual AEditable* edGetParent() override;
	virtual void Serialize( mxArchive & archive ) override;

private:
	TPtr<AEditableRefCounted>	m_parent;
};


#endif // MX_EDITOR


mxNAMESPACE_END

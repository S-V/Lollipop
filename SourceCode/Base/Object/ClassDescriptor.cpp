/*
=============================================================================
	File:	mxClass.cpp
	Desc:	Classes for run-time type checking
			and run-time instancing of objects.
=============================================================================
*/

#include <Base_PCH.h>
#pragma hdrstop
#include <Base.h>

#include <Base/Object/AObject.h>
#include <Base/Object/TypeRegistry.h>
#include <Base/Object/ClassDescriptor.h>


/*================================
		mxClass
================================*/

// static
mxClass* mxClass::m_head = nil;

mxClass::mxClass(
	const char* className,
	const TypeGUIDArg classGuid,
	const mxClass* const parentClass,
	const SClassDescription& classInfo,
	const mxClassMembers& reflectionMetadata
	)
	: mxType( ETypeKind::Type_Class, className, classInfo )
	, m_guid( classGuid )
	, m_parent( parentClass )
	, m_members( reflectionMetadata )
{
	AssertPtr( className );
	Assert( m_parent != this );	// but parentType can be NULL for root classes

	m_creator = classInfo.creator;
	m_constructor = classInfo.constructor;
	m_destructor = classInfo.destructor;

	// Insert this object into the linked list
	m_next = m_head;
	m_head = this;
}

bool mxClass::IsDerivedFrom( const mxClass& other ) const
{
	for ( const mxClass * current = this; current != nil; current = current->GetParent() )
	{
		if ( current->m_name == other.m_name )
		{
			return true;
		}
	}
	return false;
}

bool mxClass::IsDerivedFrom( TypeGUIDArg typeCode ) const
{
	Assert(typeCode != mxNULL_TYPE_GUID);
	for ( const mxClass * current = this; current != nil; current = current->GetParent() )
	{
		if ( current->GetTypeGuid() == typeCode )
		{
			return true;
		}
	}
	return false;
}

bool mxClass::IsDerivedFrom( PCSTR className ) const
{
	AssertPtr(className);
	for ( const mxClass * current = this; current != nil; current = current->GetParent() )
	{
		if ( mxStrEquAnsi( m_name, className ) )
		{
			return true;
		}
	}
	return false;
}

AObject* mxClass::CreateInstance() const
{
	Assert( this->IsConcrete() );

	F_CreateObject* pCreateFunction = this->GetCreator();
	AssertPtr( pCreateFunction );

	AObject* pObjectInstance = (AObject*) (*pCreateFunction)();
	AssertPtr( pObjectInstance );

	return pObjectInstance;
}

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//

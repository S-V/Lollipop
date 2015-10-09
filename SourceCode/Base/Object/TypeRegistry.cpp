/*
=============================================================================
	File:	TypeRegistry.cpp
	Desc:	Object factory for run-time class instancing and type information.
	ToDo:	check inheritance depth and warn about pathologic cases?
=============================================================================
*/

#include <Base_PCH.h>
#pragma hdrstop
#include <Base.h>

#include <Base/Object/AObject.h>
#include <Base/Object/TypeRegistry.h>

mxNAMESPACE_BEGIN

/*
-----------------------------------------------------------------------------
	TypeRegistry
-----------------------------------------------------------------------------
*/
//
//	Singleton.
//
namespace
{
	static TPtr< TypeRegistry >	TheFactory;
}

//
//	TypeRegistry::GetInstance
//
TypeRegistry& TypeRegistry::Get()
{
	return *TheFactory;
}

bool TypeRegistry::IsInitialized()
{
	return TheFactory != nil;
}

void TypeRegistry::Initialize()
{
	//DBG_TRACE_CALL;
	Assert(TheFactory == nil);

	if ( TheFactory == nil )
	{
		TheFactory.ConstructInPlace();

		// Calculate number of types.
		UINT	numTypes = 0;
		{
			mxClass* curr = mxClass::m_head;

			while( PtrToBool(curr) )
			{
				mxClass* next = curr->m_next;

				++numTypes;

				curr = next;
			}
		}

		DBGOUT("TypeRegistry::Initialize: %u types\n",(UINT)numTypes);

		TheFactory->mTypesById.Reserve( numTypes );
		TheFactory->mTypesByName.Reserve( numTypes );


		// Register types.
		{
			mxClass* curr = mxClass::m_head;
			UINT classId = 0;

			while( PtrToBool(curr) )
			{
				mxClass* next = curr->m_next;

				DBGOUT("Register class %s (ID=%u, GUID=0x%x)\n",
					curr->GetTypeName(), classId, (UINT)curr->GetTypeGuid() );

				Assert( ! TheFactory->mTypesById.Contains( curr->GetTypeGuid() ) );
				TheFactory->mTypesById.Set( curr->GetTypeGuid(), curr );



				Assert( ! TheFactory->mTypesByName.Contains( curr->GetTypeName() ) );
				TheFactory->mTypesByName.Set( curr->GetTypeName(), curr );


				curr = next;

				classId++;
			}
		}

	}
}

void TypeRegistry::Destroy()
{
	if ( nil != TheFactory )
	{
		TheFactory.Destruct();
	}
}

TypeRegistry::TypeRegistry()
	: mTypesById( _NoInit )
	, mTypesByName( _NoInit )
{
}

TypeRegistry::~TypeRegistry()
{
}

bool TypeRegistry::ClassExists( TypeGUIDArg typeCode ) const
{
	return mTypesById.Contains( typeCode );
}

const mxClass* TypeRegistry::FindClassInfoByGuid( TypeGUIDArg typeCode ) const
{
	const mxClass* typeInfo = mTypesById.FindRef( typeCode );
	AssertPtr(typeInfo);
	return typeInfo;
}

const mxClass* TypeRegistry::FindClassInfoByName( PCSTR className ) const
{
	const mxClass* typeInfo = mTypesByName.FindRef( className );
	AssertPtr(typeInfo);
	return typeInfo;
}

AObject* TypeRegistry::CreateInstance( TypeGUIDArg typeCode ) const
{
	AObject* pObjectInstance = ObjectUtil::Create_Object_Instance( typeCode );
	AssertPtr( pObjectInstance );
	return pObjectInstance;
}

void TypeRegistry::EnumerateDescendants( const mxClass& baseClass, TList<const mxClass*> &OutClasses )
{
	mxClass* curr = mxClass::m_head;

	while( PtrToBool(curr) )
	{
		mxClass* next = curr->m_next;

		if( curr->IsDerivedFrom( baseClass ) )
		{
			OutClasses.Add( curr );
		}

		curr = next;
	}
}

void TypeRegistry::EnumerateConcreteDescendants( const mxClass& baseClass, TList<const mxClass*> &OutClasses )
{
	mxClass* curr = mxClass::m_head;

	while( PtrToBool(curr) )
	{
		mxClass* next = curr->m_next;

		if( curr->IsDerivedFrom( baseClass ) && curr->IsConcrete() )
		{
			OutClasses.Add( curr );
		}

		curr = next;		
	}
}

mxNAMESPACE_END

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//

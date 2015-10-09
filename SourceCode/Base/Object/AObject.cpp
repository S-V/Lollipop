/*
=============================================================================
	File:	Object.cpp
	Desc:	Common base class.
=============================================================================
*/

#include <Base_PCH.h>
#pragma hdrstop
#include <Base.h>

#include <Base/Object/AObject.h>

mxNAMESPACE_BEGIN


/*================================
		SBaseType
================================*/

/*static*/
mxClass SBaseType::ms_staticTypeInfo(
	"SBaseType", mxNULL_TYPE_GUID,
	nil,
	SClassDescription(),
	SBaseType::StaticGetReflection()
);

/*================================
			AObject
================================*/

/*static*/
mxClass	AObject::ms_staticTypeInfo( 
	"AObject", mxNULL_TYPE_GUID+1,
	nil,
	SClassDescription(),
	AObject::StaticGetReflection()
);

AObject::~AObject()
{
}

namespace ObjectUtil
{
	AObject* Create_Object_Instance( const mxClass& classInfo )
	{
		CHK_VRET_NIL_IF_NOT( classInfo.IsConcrete() );

		AObject* pNewInstance = classInfo.CreateInstance();
		AssertPtr( pNewInstance );

		return pNewInstance;
	}
	AObject* Create_Object_Instance( TypeGUIDArg classGuid )
	{
		const mxClass* pClassInfo = TypeRegistry::Get().FindClassInfoByGuid( classGuid );
		CHK_VRET_NIL_IF_NIL(pClassInfo);

		AObject* pNewInstance = Create_Object_Instance( *pClassInfo );

		return pNewInstance;
	}

}//namespace ObjectUtil

mxNAMESPACE_END

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//

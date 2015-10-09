/*
=============================================================================
	File:	pxCollideable.cpp
	Desc:
=============================================================================
*/
#include <Physics_PCH.h>
#pragma hdrstop
#include <Physics.h>

/*================================
		pxCollideable
================================*/

void pxCollideable::_ctor( const pxCollideableInfo& desc )
{
	Assert( desc.isOk() );

	m_transform.SetIdentity();

	m_broadphaseProxy = INDEX_NONE;
	m_collisionShape = desc.shape;

	//m_ccdHitFraction = 1.0f;

	SetMaterial( desc.material );

	m_material = pxMaterial::DefaultId;
}

void pxCollideable::_dtor()
{
	m_collisionShape.SetInvalid();
}

pxShape* pxCollideable::GetShape()
{
	return m_collisionShape.ToPtr();
}

const pxShape* pxCollideable::GetShape() const
{
	return m_collisionShape.ToPtr();
}

void pxCollideable::SetShape( pxShape::Handle newShape )
{
	m_collisionShape = newShape;
}

void pxCollideable::SetMaterial( pxMaterial::Handle newMaterial )
{
	m_material = newMaterial;
}

#if MX_EDITOR

void pxCollideable::edCreateProperties( EdPropertyCollector & outProperties )
{
	MX_COLLECT_COMMON_PROPERTIES( pxCollideable, &outProperties );
}

#endif // MX_EDITOR

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//

/*
=============================================================================
	File:	pxCollisionAgent.cpp
	Desc:
=============================================================================
*/
#include <Physics_PCH.h>
#pragma hdrstop
#include <Physics.h>

/*
-----------------------------------------------------------------------------
	pxEmptyCollisionAgent
-----------------------------------------------------------------------------
*/
pxEmptyCollisionAgent::pxEmptyCollisionAgent( const pxCollisionAgentCreationInfo& cInfo )
	: pxCollisionAgent(cInfo)
{}

pxEmptyCollisionAgent::~pxEmptyCollisionAgent()
{}

void pxEmptyCollisionAgent::ProcessCollision(
	pxCollideable* objA, pxCollideable* objB,
	const pxProcessCollisionInput& input,
	pxProcessCollisionOutput & result
)
{
	Assert(objA != objB);

//	const pxcShapeType shapeTypeA = oA->m_collisionShape.GetType();
//	const pxcShapeType shapeTypeB = oB->m_collisionShape.GetType();

	//DBGOUT("No collision: %s vs %s\n",
	//	pxDbgGetShapeName(objA->GetShape()->GetType()),
	//	pxDbgGetShapeName(objB->GetShape()->GetType())
	//);
}

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//

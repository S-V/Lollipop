/*
=============================================================================
	File:	pxCollisionDispatcher.cpp
	Desc:
=============================================================================
*/
#include <Physics_PCH.h>
#pragma hdrstop
#include <Physics.h>

#include <Physics/Collide/NearPhase/pxSpherePlaneAgent.h>
#include <Physics/Collide/NearPhase/pxSphereSphereAgent.h>
#include <Physics/Collide/NearPhase/pxConvexVsConcave.h>

/*================================
		pxCollisionDispatcher
================================*/

pxCollisionDispatcher::pxCollisionDispatcher( pxUInt maxPairs )
{
	m_manifoldsPool.Setup( MX_EFFICIENT_SIZE(sizeof(pxContactManifold)), 1024 );

	m_manifoldsPtrArray.Reserve( 1024 );

	ZERO_OUT(m_collisionMatrix);

	SizeT maxAgentClassSize = this->_RegisterAllCollisionAgents();

	m_agentsPool.Setup( maxAgentClassSize, 1024 );
}

pxCollisionDispatcher::~pxCollisionDispatcher() {
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

template< class AGENT >
pxCollisionAgent::F_CreateAgent *	T_GetCollisionAgentFactory()
{
	struct AgentFactory
	{
		static FORCEINLINE
		pxCollisionAgent* ConstructAgent( const pxCollisionAgentCreationInfo& cInfo )
		{
			return new( cInfo.memory ) AGENT( cInfo );
		}
	};

	return &AgentFactory::ConstructAgent;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

template< class AGENT >
void T_RegisterCollisionAgent( CollisionMatrixType& collisionMatrix )
{
	for( UINT iRow = 0; iRow < pxcShapeType::PX_SHAPE_MAX; iRow++ )
	{
		for( UINT iColumn = 0; iColumn < pxcShapeType::PX_SHAPE_MAX; iColumn++ )
		{
			//Assert( iRow < pxcShapeType::PX_SHAPE_MAX && iColumn < pxcShapeType::PX_SHAPE_MAX );

			const pxcShapeType shapeA	= static_cast< pxcShapeType >( iRow );
			const pxcShapeType shapeB	= static_cast< pxcShapeType >( iColumn );

			if( AGENT::HandlesCollisionShapes( shapeA, shapeB ) )
			{
				AssertX( collisionMatrix[ shapeA ][ shapeB ].createFunc == nil, "colliders can only be set once");
				AssertX( collisionMatrix[ shapeB ][ shapeA ].createFunc == nil, "colliders can only be set once");

				//if( collisionMatrix[ shapeA ][ shapeB ].createFunc == nil )
				{
					collisionMatrix[ shapeA ][ shapeB ].createFunc = T_GetCollisionAgentFactory<AGENT>();
					collisionMatrix[ shapeA ][ shapeB ].agentClassSize = sizeof AGENT;
					collisionMatrix[ shapeA ][ shapeB ].swapBodies = 0;
				}
				//if( collisionMatrix[ shapeB ][ shapeA ].createFunc == nil )
				{
					collisionMatrix[ shapeB ][ shapeA ].createFunc = T_GetCollisionAgentFactory<AGENT>();
					collisionMatrix[ shapeB ][ shapeA ].agentClassSize = sizeof AGENT;
					collisionMatrix[ shapeB ][ shapeA ].swapBodies = 1;
				}
			}
		}
	}
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

SizeT pxCollisionDispatcher::_RegisterAllCollisionAgents()
{
	//***********************************************************

	T_RegisterCollisionAgent< pxSpherePlaneAgent >( m_collisionMatrix );
	T_RegisterCollisionAgent< pxSphereSphereAgent >( m_collisionMatrix );
//	T_RegisterCollisionAgent< pxSphereBoxAgent >( m_collisionMatrix );
//	T_RegisterCollisionAgent< pxBoxPlaneAgent >( m_collisionMatrix );
	T_RegisterCollisionAgent< pxConvexVsStaticBspAgent >( m_collisionMatrix );

	//***********************************************************

	// Calculate maximum element size, big enough to fit any collision algorithm in the memory pool.

	SizeT maxAgentSize = 0;
	SizeT minAgentSize = SizeT(-1);

	// Fill the remaining holes with empty collision agent.

	for( UINT iRow = 0; iRow < pxcShapeType::PX_SHAPE_MAX; iRow++ )
	{
		for( UINT iColumn = 0; iColumn < pxcShapeType::PX_SHAPE_MAX; iColumn++ )
		{
			pxColliderEntry & handler = m_collisionMatrix[ iRow ][ iColumn ];

			if( handler.createFunc == nil )
			{
				handler.createFunc		= T_GetCollisionAgentFactory<pxEmptyCollisionAgent>();
				handler.agentClassSize	= sizeof pxEmptyCollisionAgent;
				handler.swapBodies		= 0;
			}

			maxAgentSize = Max<SizeT>( maxAgentSize, handler.agentClassSize );
			minAgentSize = Min<SizeT>( minAgentSize, handler.agentClassSize );
		}
	}

	DBGOUT("max. agent size: %d, min. agent size: %d\n",
		maxAgentSize, minAgentSize);

	maxAgentSize = MX_EFFICIENT_SIZE(maxAgentSize);
	return maxAgentSize;
}

void pxCollisionDispatcher::AfterNewPairAdded( pxCollisionPair* newPair )
{
	Assert( nil == newPair->agent );
	//if( PX_DEBUG )
	//{
	//	ANSICHAR	buf[512];
	//	pxDbgDumpPairInfo(newPair->oA,newPair->oB,buf);
	//	DBGOUT("adding pair: %s\n",buf);
	//}
	pxCollideable* oA = newPair->oA;
	pxCollideable* oB = newPair->oB;
	newPair->agent = this->CreateCollisionAgent( oA, oB );
}

void pxCollisionDispatcher::BeforePairRemoved( pxCollisionPair* oldPair )
{
	Assert( nil != oldPair->agent );
	//if( PX_DEBUG )
	//{
	//	ANSICHAR	buf[512];
	//	pxDbgDumpPairInfo(oldPair->oA,oldPair->oB,buf);
	//	DBGOUT("destroying pair: %s\n",buf);
	//}
	this->DestroyCollisionAgent( oldPair->agent );
}

pxCollisionAgent* pxCollisionDispatcher::CreateCollisionAgent( pxCollideable* oA, pxCollideable* oB )
{
	PX_ASSERT_PTR2(oA,oB);

	// avoid touching collision shape objects
	//const pxcShapeType shapeTypeA = oA->GetShape()->GetType();
	//const pxcShapeType shapeTypeB = oB->GetShape()->GetType();
	const pxcShapeType shapeTypeA = oA->m_collisionShape.GetType();
	const pxcShapeType shapeTypeB = oB->m_collisionShape.GetType();

	pxColliderEntry & entry = m_collisionMatrix[ shapeTypeA ][ shapeTypeB ];
	Assert(entry.isOk());

	pxCollideable * o0;
	pxCollideable * o1;

	PX_OPTIMIZE("branchless swap based on condition");
#if 0
	o0 = oA;
	o1 = oB;
	if( entry.swapBodies ) {
		TSwap( o0, o1 );
	}
#else
	o0 = entry.swapBodies ? oB : oA;
	o1 = entry.swapBodies ? oA : oB;
#endif

	pxCollisionAgentCreationInfo	cInfo(_NoInit);
	{
		// allocate memory for collision agent
		cInfo.memory = m_agentsPool.GetNew( /*entry.agentClassSize*/ );
		cInfo.oA = o0;
		cInfo.oB = o1;
	}

	return entry.createFunc( cInfo );
}

void pxCollisionDispatcher::DestroyCollisionAgent( pxCollisionAgent* agent )
{
	AssertPtr(agent);
	agent->~pxCollisionAgent();
	m_agentsPool.Free( agent );
}

pxContactManifold* pxCollisionDispatcher::CreateContactManifold()
{
	PX_STATS(gPhysStats.numContactManifolds++);

	void * mem = m_manifoldsPool.GetNew();
	pxContactManifold * newManifold = new (mem) pxContactManifold(_NoInit);
	{
		newManifold->numPoints = 0;
		newManifold->internalIndex = m_manifoldsPtrArray.Num();
		newManifold->oA = nil;
		newManifold->oB = nil;
	}
	m_manifoldsPtrArray.Add( newManifold );

	return newManifold;
}

void pxCollisionDispatcher::ReleaseContactManifold( pxContactManifold* manifold )
{
	PX_STATS(gPhysStats.numContactManifolds--);

	const UINT findIndex = manifold->internalIndex;
	const UINT arraySize = m_manifoldsPtrArray.Num();
	Assert( findIndex < arraySize );

	pxContactManifold** manifoldsPtrArray = m_manifoldsPtrArray.ToPtr();

	manifoldsPtrArray[ findIndex ]->Clear();

	m_manifoldsPtrArray.RemoveAt_Fast( findIndex );

	manifoldsPtrArray[ findIndex ]->internalIndex = findIndex;
}

void pxCollisionDispatcher::Collide( pxContactCache & contacts )
{
	contacts.Clear();

	const UINT numPairs = this->NumPairs();
	pxCollisionPair* pairs = this->GetPairs();

	//@todo: this can be done in parallel

	UINT	totalNumContacts = 0;

	for( UINT iPair = 0; iPair < numPairs; iPair++ )
	{
		pxCollisionPair & pair = pairs[ iPair ];

		AssertPtr(pair.agent);

		pxProcessCollisionInput		input;
		//{
		//	input.oA = pair.oA;
		//	input.oB = pair.oB;
		//}

		pxProcessCollisionOutput	result;

		pair.agent->ProcessCollision( pair.oA, pair.oB, input, result );

		totalNumContacts += result.numContacts;
	}

	contacts.manifolds = m_manifoldsPtrArray.ToPtr();
	contacts.numManifolds = m_manifoldsPtrArray.Num();
	contacts.totalNumContacts = totalNumContacts;
}

void pxCollisionDispatcher::Clear()
{
	Super::Clear();
}

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//

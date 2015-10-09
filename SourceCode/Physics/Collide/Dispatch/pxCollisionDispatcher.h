/*
=============================================================================
	File:	pxCollisionDispatcher.h
	Desc:	
=============================================================================
*/

#ifndef __PX_COLLISION_DISPATCHER_H__
#define __PX_COLLISION_DISPATCHER_H__

//
//	Handlers for broad-phase collision detection events.
//
typedef void (*F_OnPairAddedCallback)	( pxCollisionPair& pair, pxCollisionDispatcher& dispatcher );
typedef void (*F_OnPairRemovedCallback)	( pxCollisionPair& pair, pxCollisionDispatcher& dispatcher );

// collider function (called if contact caching is not used)
//typedef void F_Collide( pxCollideable* objA, pxCollideable* objB, const pxProcessCollisionInput& input, pxProcessCollisionOutput & result );

// pxColliderEntry is a single entry in collision matrix
//
struct pxColliderEntry
{
	pxCollisionAgent::F_CreateAgent *	createFunc;
	//F_Collide *	collideFunc;
	pxU4	agentClassSize;
	B4		swapBodies;	// 1 -> reverse body A and body B

public:
	inline bool isOk() const {
		return (createFunc != nil)
			&& agentClassSize
			;
	}
};

typedef pxColliderEntry	CollisionMatrixType[ pxcShapeType::PX_SHAPE_MAX ][ pxcShapeType::PX_SHAPE_MAX ];



//
//	pxCollisionDispatcher
//
// This class is used to find the correct function which performs collision detection between pairs of shapes.
// It's based on the following rules:
//  - Each shapes has a type
//  - For each pair of types, there must be a function registered to handle this pair of types
//
class pxCollisionDispatcher
	: public pxSimplePairCache< pxCollisionDispatcher >
	, SingleInstance< pxCollisionDispatcher >
{
	PX_DECLARE_CLASS_ALLOCATOR( pxCollisionAgent, PX_MEMORY_COLLISION_DISPATCH );

public:
	typedef pxSimplePairCache Super;

	pxCollisionDispatcher( pxUInt maxPairs = 1024 );

	~pxCollisionDispatcher();

	//--- Broad-phase collision detection --------------------------------------

	void AfterNewPairAdded( pxCollisionPair* newPair );
	void BeforePairRemoved( pxCollisionPair* oldPair );

	//--- Narrow-phase collision detection --------------------------------------

	// Performs a narrow-phase collision detection and generates contacts.
	void Collide( pxContactCache & contacts );

	void Clear();

	pxCollisionAgent* CreateCollisionAgent( pxCollideable* oA, pxCollideable* oB );

	void DestroyCollisionAgent( pxCollisionAgent* agent );


	// Contact point cache

	pxContactManifold* CreateContactManifold();

	void ReleaseContactManifold( pxContactManifold* manifold );

	UINT NumManifolds() const { return m_manifoldsPtrArray.Num(); }

	pxContactManifold** GetManifoldsPtr() { return m_manifoldsPtrArray.ToPtr(); }

	pxContactManifold& GetManifold( UINT index )
	{
		return *(m_manifoldsPtrArray[ index ]);
	}

private:
	// called in ctor, returns max. agent size in bytes
	SizeT _RegisterAllCollisionAgents();

private: PREVENT_COPY(pxCollisionDispatcher);
private:
	mxPoolAlloc			m_manifoldsPool;
	mxPoolAlloc			m_agentsPool;

	TList< pxContactManifold* >	m_manifoldsPtrArray;

	// collision matrix for invoking proper near-phase callbacks
	MX_ALIGN_16( CollisionMatrixType	m_collisionMatrix );
};


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

#if PX_DEBUG

	template< UINT NUM_CHARS >
	void pxDbgDumpPairInfo(
						   const pxCollideable* oA, const pxCollideable* oB,
						   char (&outBuffer)[NUM_CHARS]
	)
	{
		const pxcShapeType shapeTypeA = oA->m_collisionShape.GetType();
		const pxcShapeType shapeTypeB = oB->m_collisionShape.GetType();

		MX_SPRINTF_ANSI( outBuffer, "('%s' vs '%s')",
			pxDbgGetShapeName( shapeTypeA ),
			pxDbgGetShapeName( shapeTypeB )
			);
	}

#endif // PX_DEBUG


#endif // !__PX_COLLISION_DISPATCHER_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//

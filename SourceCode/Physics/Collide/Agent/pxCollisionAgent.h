/*
=============================================================================
	File:	pxCollisionAgent.h
	Desc:	Collision agents perform narrowphase collision detection
			between collision shapes.
=============================================================================
*/

#ifndef __PX_COLLISION_AGENT_H__
#define __PX_COLLISION_AGENT_H__

//
// pxCollisionAgentCreationInfo - holds info for creating new collision agents.
//
class pxCollisionAgentCreationInfo
{
public:
	void * memory;	// memory allocated for the agent
	pxCollideable *	oA;
	pxCollideable *	oB;

public:
	FORCEINLINE pxCollisionAgentCreationInfo(ENoInit)
	{}

	FORCEINLINE pxCollisionAgentCreationInfo()
		: memory(nil), oA(nil), oB(nil)
	{}

	PX_INLINE bool isOk() const {
		return (memory && oA && oB )
			;
	}
};

//
// pxProcessCollisionInput
//
class pxProcessCollisionInput {
public:
	//pxCollideable *	RESTRICT_PTR( oA );
	//pxCollideable *	RESTRICT_PTR( oB );
};

//
// pxProcessCollisionOutput
//
class pxProcessCollisionOutput {
public:
	pxUInt	numContacts;

public:
	FORCEINLINE pxProcessCollisionOutput()
		: numContacts( 0 )
	{}
	FORCEINLINE pxProcessCollisionOutput(ENoInit)
	{}
};

// Time-Of-Impact calculation
struct pxToiInput
{
	pxCollideable *	RESTRICT_PTR( oA );
	pxCollideable *	RESTRICT_PTR( oB );
};
struct pxToiOutput
{
	pxReal		ccdHitFraction;
};

/*
-----------------------------------------------------------------------------
	pxCollisionAgent (aka Collision Arbiter/Algorithm)
	- is the base class for all collision agents,
	which are used for pair-wise narrow-phase collision detection between shapes.
-----------------------------------------------------------------------------
*/
class pxCollisionAgent
{
	PX_DECLARE_CLASS_ALLOCATOR( pxCollisionAgent, PX_MEMORY_COLLISION_AGENT );

public:

	// Performs narrowphase collision detection.
	//
	PX_VIRTUAL void ProcessCollision(
		pxCollideable* objA, pxCollideable* objB,
		const pxProcessCollisionInput& input,
		pxProcessCollisionOutput & result
	) = 0;

#if 0
	// Checks whether the two objects are penetrating.
	//
	virtual void TestOverlap(
		pxCollideable* objA, pxCollideable* objB,
		pxCollisionResult &result
	) = 0;

	// Get the closest distances between two objects.
	//
	virtual void GetClosestPoints(
		pxCollideable* objA, pxCollideable* objB,
		pxCollisionResult &result
	) = 0;
#endif

	// Estimates Time Of Impact - reports time of impact (the first time of contact (TOC))
	// between two objects in range [0..1].
	//
	virtual pxReal CalculateTOI( const pxToiInput& input, pxToiOutput &output )
	{return 1.0f;}

public:	// Creation/Destruction

	// used for creating new collision agents
	typedef pxCollisionAgent* F_CreateAgent( const pxCollisionAgentCreationInfo& cInfo );

public:	// Serialization/Relocation
	virtual void ProcessPointers( pxPointerRelocator* relocator ) {}

protected:
	friend class pxCollisionDispatcher;

	FORCEINLINE pxCollisionAgent( const pxCollisionAgentCreationInfo& cInfo )
	{
	}

	FORCEINLINE virtual ~pxCollisionAgent()
	{}

protected:
	//
};



/*
-----------------------------------------------------------------------------
	pxEmptyCollisionAgent
-----------------------------------------------------------------------------
*/
class pxEmptyCollisionAgent : public pxCollisionAgent
{
public:
	pxEmptyCollisionAgent( const pxCollisionAgentCreationInfo& cInfo );
	~pxEmptyCollisionAgent();

	void ProcessCollision(
		pxCollideable* objA, pxCollideable* objB,
		const pxProcessCollisionInput& input,
		pxProcessCollisionOutput & result
	);

	static bool HandlesCollisionShapes( pxcShapeType shapeA, pxcShapeType shapeB )
	{
		return true;
	}
};



/*
-----------------------------------------------------------------------------
	pxOneManifoldCollisionAgent
-----------------------------------------------------------------------------
*/
template< class SHAPE_A, class SHAPE_B >
class pxOneManifoldCollisionAgent : public pxCollisionAgent
{
public:
	FORCEINLINE pxOneManifoldCollisionAgent( const pxCollisionAgentCreationInfo& cInfo )
		: pxCollisionAgent( cInfo )
	{
		m_manifold = Physics::GetCollisionDispatcher()->CreateContactManifold();

		m_manifold->oA = cInfo.oA;
		m_manifold->oB = cInfo.oB;

		//GetShapes( cInfo.oA, cInfo.oB, m_shapeA, m_shapeB );
	}

	FORCEINLINE ~pxOneManifoldCollisionAgent()
	{
		Physics::GetCollisionDispatcher()->ReleaseContactManifold( m_manifold );
	}

	FORCEINLINE static void GetShapes(
		pxCollideable* objA, pxCollideable* objB,
		SHAPE_A *& shapeA, SHAPE_B *& shapeB
		)
	{
		shapeA = objA->GetShape()->UpCast< SHAPE_A >();
		shapeB = objB->GetShape()->UpCast< SHAPE_B >();
	}

protected:
	pxContactManifold *	m_manifold;
	//SHAPE_A *	m_shapeA;
	//SHAPE_B *	m_shapeB;
};


#endif // !__PX_COLLISION_AGENT_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//

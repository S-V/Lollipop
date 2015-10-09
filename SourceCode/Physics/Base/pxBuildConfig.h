/*
=============================================================================
	File:	BuildConfig.h
	Desc:	Compile settings.
=============================================================================
*/

#ifndef __PX_BUILD_CONFIG_H__
#define __PX_BUILD_CONFIG_H__

#define PX_DEBUG	MX_DEBUG

#if PX_DEBUG
	#define PX_DBG_CODE( x )	x
#else
	#define PX_DBG_CODE( x )
#endif//PX_DEBUG

#define PX_ENABLE_DEBUG_DRAW	(1)

#define PX_USE_CHEATS	1

#define PX_OPTIMIZE( msg );
#define PX_OPT_IDEA( msg );
#define PX_BRANCH( msg );

#define PX_HACK( msg );

#define PX_WHY( msg )

// prefer scalar code over vectorized code - pack data for FPU, not for SSE units (saves memory)
#define PX_FORCE_FPU	0

#define PX_INLINE	FORCEINLINE

#define PX_VIRTUAL virtual


//-------------------------------------------------------------
//	Memory management.
//-------------------------------------------------------------

#if PX_DEBUG
	#define PX_DEBUG_MEMORY	1
#else
	#define PX_DEBUG_MEMORY	0
#endif

// Memory allocators. Modify these to use your own allocator.
//
void* pxNew( SizeT numBytes );
void pxFree( void* ptr );


enum PX_MEMORY_CLASS
{
	PX_MEMORY_DEFAULT = EMemHeap::DefaultHeap,

	PX_MEMORY_COLLISION = EMemHeap::HeapPhysics,
	PX_MEMORY_COLLISION_BROADPHASE = EMemHeap::HeapPhysics,
	PX_MEMORY_COLLISION_NARROWPHASE = EMemHeap::HeapPhysics,
	PX_MEMORY_COLLISION_AGENT = EMemHeap::HeapPhysics,

	PX_MEMORY_COLLISION_DISPATCH = EMemHeap::HeapPhysics,

	PX_MEMORY_DYNAMICS = EMemHeap::HeapPhysics,

	PX_MEMORY_CONSTRAINT_SOLVER = EMemHeap::HeapPhysics,
};

#define PX_DECLARE_POD_ALLOCATOR( typeName, memoryClass )
#define PX_DECLARE_CLASS_ALLOCATOR( typeName, memoryClass )
#define PX_DECLARE_NON_POD_CLASS_ALLOCATOR( typeName, memoryClass )


#if PX_DEBUG
	#define PX_ASSERT_PTR2(ptr1,ptr2)	{AssertPtr(ptr1); AssertPtr(ptr2); Assert(ptr1!=ptr2);}
#else//!PX_DEBUG
	#define PX_ASSERT_PTR2(ptr1,ptr2)
#endif

//-------------------------------------------------------------
//	Profiling.
//-------------------------------------------------------------

// Make sure it's not enabled in production version!
//#define	PX_PROFILE	MX_PROFILE
#define	PX_PROFILE		MX_PROFILE

//-------------------------------------------------------------
//	Global physics stats.
//-------------------------------------------------------------

// Make sure it's not enabled in production version!
//#define	PX_COLLECT_STATISTICS	PX_DEBUG
#define	PX_COLLECT_STATISTICS	1

#if PX_COLLECT_STATISTICS

	//
	//	pxGlobalStats
	//
	class pxGlobalStats {
	public:
		mxTimer		timer;	// should be reset upon each frame

		//collision detection
		unsigned long	collisionDetectionMs;
			//pxBroadphaseStats
			unsigned long	broadphaseMs;
			unsigned long	addedPairs, searchedPairs, removedPairs;
			//pxNarrowphaseStats
			unsigned long	narrowphaseMs;
			unsigned long	numContactManifolds;

		//pxSolverStats
		unsigned long	solveConstraintsMs;

		// integration
		unsigned long	integrateMs;

	public:

		unsigned long CalcTotalTime() const
		{
			return 0
				+ collisionDetectionMs
				+ solveConstraintsMs
				+ integrateMs
				;
		}

		void Reset()
		{
			timer.Reset();

			collisionDetectionMs = 0;
			
			broadphaseMs = 0;
			addedPairs = 0;	searchedPairs = 0;	removedPairs = 0;
			
			narrowphaseMs = 0;
			numContactManifolds = 0;

			solveConstraintsMs = 0;

			integrateMs = 0;
		}

		//returns total time elapsed since last call to reset(), in milliseconds
		unsigned long GetElapsedTime()
		{
			return timer.GetTimeMilliseconds();
		}

		pxGlobalStats()
		{
			Reset();
		}
	};

	extern pxGlobalStats	gPhysStats;

	//
	//	pxTimeCounter
	//
	class pxTimeCounter
	{
	public:
		unsigned long &	counter;
		unsigned long	startTime;

	public:
		pxTimeCounter( unsigned long & val )
			: counter( val )
			, startTime( gPhysStats.GetElapsedTime() )
		{
		}
		~pxTimeCounter()
		{
			counter += (gPhysStats.GetElapsedTime() - startTime);
		}
	};

	#define PX_STATS(x) x

	#define PX_SCOPED_COUNTER(value)		pxTimeCounter	___counter___(value);

#else

	#define PX_STATS(x)					NOOP
	#define PX_SCOPED_COUNTER(value)	NOOP

#endif//PX_COLLECT_STATISTICS



//-------------------------------------------------------------
//	Limits.
//-------------------------------------------------------------

#define PX_WORLD_BITS	4
#define PX_WORLDS_MAX	((1<<PX_WORLD_BITS)-1)

#define PX_WORLD_ENTITY_BITS	(32-PX_WORLD_BITS)
#define PX_WORLD_ENTITY_MAX		((1<<PX_WORLD_ENTITY_BITS)-1)

// contains world & entity handles
//
union pxWorldEntityInfo
{
	struct  
	{
		BITFIELD	worldId : PX_WORLD_BITS;	// id of the containing world
		BITFIELD	entityId : PX_WORLD_ENTITY_BITS;	// id of this object
	};
	U4	v;
};

#endif // !__PX_BUILD_CONFIG_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//

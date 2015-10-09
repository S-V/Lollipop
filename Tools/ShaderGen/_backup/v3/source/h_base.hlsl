// file: base.fxh
// included by most shaders. contains common macros, types, functions, constants and variables

// See: http://www.spuify.co.uk/?p=138
#pragma pack_matrix( row_major )

//-------------------------------------------------------------

// Avoids flow control constructs.
#define MX_UNROLL											[unroll]
// Gives preference to flow control constructs.
#define MX_LOOP												[loop]
// Performs branching by using control flow instructions like jmp and label.
#define MX_BRANCH											[branch]
// Performs branching by using the cnd instructions.
#define MX_FLATTEN											[flatten]
// Executes the conditional part of an if statement when the condition is true for all threads on which the current shader is running.
#define MX_IFALL											[ifAll]
// Executes the conditional part of an if statement when the condition is true for any thread on which the current shader is running.
#define MX_IFANY											[ifAny]


/** Performs branching by using control flow instructions like jmp and label, only for Xbox. */
#if XBOX
#define BRANCH_XBOX	[branch]
#else
#define BRANCH_XBOX
#endif

/** Performs branching by using control flow instructions like jmp and label, only for PS3. */
#if PS3
#define BRANCH_PS3	[branch]
#else
#define BRANCH_PS3
#endif


#if XBOX
/** Optimizes the specified HLSL code independently of the surrounding code. */
#define ISOLATE											[isolate]
#define NOEXPRESSIONOPTIMIZATIONS						[noExpressionOptimizations]
#else
/** Not supported on other platforms */
#define ISOLATE
#define NOEXPRESSIONOPTIMIZATIONS						
#endif


//-------------------------------------------------------------
//	Markers (notes to the developer).
//-------------------------------------------------------------
//
//	Pros: bookmarks are not needed;
//		  easy to "Find All References" and no comments needed.
//	Cons: changes to the source file require recompiling;
//		  time & data have to be inserted manually.
//
#define MX_NOTE( message_string )
#define MX_TODO( message_string )
#define MX_REFACTOR( message_string )
#define MX_BUG( message_string )
#define MX_FIXME( message_string )
#define MX_HACK( message_string )
#define MX_OPTIMIZE( message_string )
#define MX_WARNING( message_string )
#define MX_REMOVE_THIS
#define MX_UNDONE
#define MX_SWIPED( message_string )
#define MX_UNSAFE
#define MX_TIDY_UP_FILE
#define MX_TEMP
// 'to be documented'
#define MX_TBD

#define WHY( message_string )

//-------------------------------------------------------------------
//	Misc helpers & constants
//-------------------------------------------------------------------

#define	M_PI		3.1415926535897932384626433832795
#define	M_INV_PI	0.31830988618379067154f
#define	M_E			2.71828182845904523536f

//-------------------------------------------------------------------
//	Math helpers
//-------------------------------------------------------------------

float squaref( float x ) { return (x * x); }
float cubef( float x ) { return (x * x * x); }

float sum( float3 v ) { return (v.x + v.y + v.z); }
float average3( float3 v ) { return (1.0f / 3.0f) * sum(v); }





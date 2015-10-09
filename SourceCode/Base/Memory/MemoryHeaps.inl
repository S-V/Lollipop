
//------- DECLARE_MEMORY_HEAP( enum, description ) ------------------------

DECLARE_MEMORY_HEAP( HeapGeneric,	"General purpose" ),

DECLARE_MEMORY_HEAP( HeapStreaming,	"Special heap for stream data like memory streams, zip file streams, etc." ),

//DECLARE_MEMORY_HEAP( HeapGeometry,	"Geometry held in main memory ( e.g. raw vertex buffers )." ),
DECLARE_MEMORY_HEAP( HeapSceneData,	"Scene graph structures, scene entities, etc." ),

DECLARE_MEMORY_HEAP( HeapGraphics,	"Low-level graphics." ),
DECLARE_MEMORY_HEAP( HeapRenderer,	"Render system." ),
DECLARE_MEMORY_HEAP( HeapPhysics,	"Physics system." ),
DECLARE_MEMORY_HEAP( HeapAudio,		"Sound system." ),
DECLARE_MEMORY_HEAP( HeapNetwork,	"Network system." ),

DECLARE_MEMORY_HEAP( HeapString,	"String data should be allocated from specialized string pools." ),
//DECLARE_MEMORY_HEAP( HeapScript,	"Scripting system (material scripts, UI, cinematics, etc)." ),

#if MX_PLATFORM == Platform_PS3
DECLARE_MEMORY_HEAP( HeapScratchpad,"Temporary storage for quick calculations." ),
#endif

// Make sure that data is freed exactly in reverse order of allocation.
//DECLARE_MEMORY_HEAP( HeapTempStack,	"Fast stack-based memory manager for quick temporary allocations." ),

DECLARE_MEMORY_HEAP( HeapEditor,	"In-game editor." ),
/*
DECLARE_MEMORY_HEAP
(
	ResourceData,

	"Fast stack allocator for resource data.\n"
	"It's used to quickly provide temporary storage when loading raw resource data.\n"
	"Memory is managed internally by the engine."
),
*/
//DECLARE_MEMORY_HEAP( HeapEditorData,	"Fast stack allocator for editor resources, doesn't free memory." ),

DECLARE_MEMORY_HEAP( HeapTemp,		"General-purpose memory manager for temporary allocations." ),


DECLARE_MEMORY_HEAP( HeapProcess,	"System memory manager for internal use by the engine." ),

#if MX_ENABLE_DEBUG_HEAP
DECLARE_MEMORY_HEAP( HeapDebug,		"Special memory heap for debugging memory leaks/buffer overruns/etc." ),
#endif // MX_DEBUG


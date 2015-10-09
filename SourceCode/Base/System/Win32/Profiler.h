/*
=============================================================================
	File:	Profiler.h
	Desc:	A simple hierarchical profiler.
			Swiped from Bullet / OGRE / Game Programming Gems 3
			( original code by Greg Hjelstrom & Byon Garrabrant).
=============================================================================
*/
#pragma once
#ifndef __MX_PROFILER_H__
#define __MX_PROFILER_H__

mxNAMESPACE_BEGIN

//--------------------------------------------------------------------------------------
// Profiling/instrumentation support
//--------------------------------------------------------------------------------------

//
//	ScopedCycleCounter
//
//	NOTE: results are not reliable on multi-processor systems.
//
struct ScopedCycleCounter
{
	UINT &		value;
	UINT		startCycles;

	FORCEINLINE
	ScopedCycleCounter( UINT& theCounter )
		: value( theCounter )
		, startCycles( GetCPUCycles() )
	{}

	FORCEINLINE
	~ScopedCycleCounter()
	{
		UINT currCycles = GetCPUCycles();
		value += currCycles - startCycles;
	}
};



#if MX_ENABLE_PROFILING

//
//	mxProfileNode - A node in the Profile Hierarchy Tree.
//
class mxProfileNode {
public:
		mxProfileNode( const char * name, mxProfileNode * parent );
		~mxProfileNode( void );

	mxProfileNode *		Get_Sub_Node( const char * name );

	mxProfileNode *		Get_Parent( void )		{ return Parent; }
	mxProfileNode *		Get_Sibling( void )		{ return Sibling; }
	mxProfileNode *		Get_Child( void )		{ return Child; }

	void				CleanupMemory();
	void				Reset( void );
	void				Call( void );
	bool				Return( void );

	const char *		Get_Name( void )			{ return Name; }
	int					Get_Total_Calls( void )		{ return TotalCalls; }
	float				Get_Total_Time( void )		{ return TotalTime; }

protected:
	const char *	Name;
	int				TotalCalls;
	float				TotalTime;
	unsigned long int			StartTime;
	int				RecursionCounter;

	mxProfileNode *	Parent;
	mxProfileNode *	Child;
	mxProfileNode *	Sibling;
};

///An iterator to navigate through the tree
class mxProfileIterator
{
public:
	// Access all the children of the current parent
	void				First(void);
	void				Next(void);
	bool				Is_Done(void);
	bool                Is_Root(void) { return (CurrentParent->Get_Parent() == 0); }

	void				Enter_Child( int index );		// Make the given child the new parent
	void				Enter_Largest_Child( void );	// Make the largest child the new parent
	void				Enter_Parent( void );			// Make the current parent's parent the new parent

	// Access the current child
	const char *	Get_Current_Name( void )			{ return CurrentChild->Get_Name(); }
	int				Get_Current_Total_Calls( void )		{ return CurrentChild->Get_Total_Calls(); }
	float			Get_Current_Total_Time( void )		{ return CurrentChild->Get_Total_Time(); }

	// Access the current parent
	const char *	Get_Current_Parent_Name( void )			{ return CurrentParent->Get_Name(); }
	int				Get_Current_Parent_Total_Calls( void )	{ return CurrentParent->Get_Total_Calls(); }
	float			Get_Current_Parent_Total_Time( void )	{ return CurrentParent->Get_Total_Time(); }

protected:

	mxProfileNode *	CurrentParent;
	mxProfileNode *	CurrentChild;

	mxProfileIterator( mxProfileNode * start );
	friend	class		mxProfileManager;
};


///The Manager for the Profile system
class	mxProfileManager {
public:
	static	void				Start_Profile( const char * name );
	static	void				Stop_Profile( void );

	static	void				CleanupMemory(void)		{ Root.CleanupMemory(); }

	static	void				Reset( void );
	static	void				Increment_Frame_Counter( void );
	static	int					Get_Frame_Count_Since_Reset( void )		{ return FrameCounter; }
	static	float				Get_Time_Since_Reset( void );

	static	mxProfileIterator *	Get_Iterator( void )	{ return new mxProfileIterator( &Root ); }
	static	void				Release_Iterator( mxProfileIterator * iterator ) { delete( iterator ); }

	static void	dumpRecursive( mxProfileIterator* profileIterator, int spacing, class mxOutputDevice* logger );

	static void	dumpAll( class mxOutputDevice* logger );

private:
	static	mxProfileNode			Root;
	static	mxProfileNode *			CurrentNode;
	static	int						FrameCounter;
	static	unsigned long int					ResetTime;
};


///ProfileSampleClass is a simple way to profile a function's scope
///Use the mxPROFILE_SCOPE macro at the start of scope to time
class	mxProfileSample {
public:
	mxProfileSample( const char * name )
	{ 
		mxProfileManager::Start_Profile( name ); 
	}

	~mxProfileSample( void )					
	{ 
		mxProfileManager::Stop_Profile(); 
	}
};


	//
	// Profiling instrumentation macros
	//

	#define	mxPROFILE_SCOPE( name )			mxProfileSample __profile( name )

	#define	mxPROFILE_FUNCTION				mxProfileSample __profile( __FUNCTION__ )

	#define	mxPROFILE_SCOPE_BEGIN( name )	{ mxProfileSample __profile( name )
	#define mxPROFILE_SCOPE_END				}

	#define mxPROFILE_INCREMENT_FRAME_COUNTER	mxProfileManager::Increment_Frame_Counter()

#else // ifndef MX_ENABLE_PROFILING

	#define	mxPROFILE_SCOPE( name )

	#define	mxPROFILE_FUNCTION

	#define	mxPROFILE_SCOPE_BEGIN( name )
	#define mxPROFILE_SCOPE_END

	#define mxPROFILE_INCREMENT_FRAME_COUNTER

#endif // ifndef MX_ENABLE_PROFILING

mxNAMESPACE_END

#endif // ! __MX_PROFILER_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//

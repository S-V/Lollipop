/*
=============================================================================
	File:	Profiler.cpp
	Desc:	A simple hierarchical profiler.
			Swiped from Bullet ( Game Programming Gems 3 )
			( original code by Greg Hjelstrom & Byon Garrabrant).
=============================================================================
*/
#include <Base_PCH.h>
#pragma hdrstop
#include <Base.h>

NO_EMPTY_FILE;

mxNAMESPACE_BEGIN

#if MX_ENABLE_PROFILING


local_ mxTimer g_ProfileClock;

inline void Profile_Get_Ticks(unsigned long int * ticks)
{
	*ticks = g_ProfileClock.GetTimeMicroseconds();
}

inline float Profile_Get_Tick_Rate(void)
{
//	return 1000000.f;
	return 1000.f;

}

/***************************************************************************************************
**
** mxProfileNode
**
***************************************************************************************************/

/***********************************************************************************************
 * INPUT:                                                                                      *
 * name - pointer to a static string which is the name of this profile node                    *
 * parent - parent pointer                                                                     *
 *                                                                                             *
 * WARNINGS:                                                                                   *
 * The name is assumed to be a static pointer, only the pointer is stored and compared for     *
 * efficiency reasons.                                                                         *
 *=============================================================================================*/
mxProfileNode::mxProfileNode( const char * name, mxProfileNode * parent ) :
	Name( name ),
	TotalCalls( 0 ),
	TotalTime( 0 ),
	StartTime( 0 ),
	RecursionCounter( 0 ),
	Parent( parent ),
	Child( NULL ),
	Sibling( NULL )
{
	Reset();
}


void	mxProfileNode::CleanupMemory()
{
	delete ( Child);
	Child = NULL;
	delete ( Sibling);
	Sibling = NULL;
}

mxProfileNode::~mxProfileNode( void )
{
	delete ( Child);
	delete ( Sibling);
}


/***********************************************************************************************
 * INPUT:                                                                                      *
 * name - static string pointer to the name of the node we are searching for                   *
 *                                                                                             *
 * WARNINGS:                                                                                   *
 * All profile names are assumed to be static strings so this function uses pointer compares   *
 * to find the named node.                                                                     *
 *=============================================================================================*/
mxProfileNode * mxProfileNode::Get_Sub_Node( const char * name )
{
	// Try to find this sub node
	mxProfileNode * child = Child;
	while ( child ) {
		if ( child->Name == name ) {
			return child;
		}
		child = child->Sibling;
	}

	// We didn't find it, so add it
	
	mxProfileNode * node = new mxProfileNode( name, this );
	node->Sibling = Child;
	Child = node;
	return node;
}


void	mxProfileNode::Reset( void )
{
	TotalCalls = 0;
	TotalTime = 0.0f;
	

	if ( Child ) {
		Child->Reset();
	}
	if ( Sibling ) {
		Sibling->Reset();
	}
}


void	mxProfileNode::Call( void )
{
	TotalCalls++;
	if (RecursionCounter++ == 0) {
		Profile_Get_Ticks(&StartTime);
	}
}


bool	mxProfileNode::Return( void )
{
	if ( --RecursionCounter == 0 && TotalCalls != 0 ) { 
		unsigned long int time;
		Profile_Get_Ticks(&time);
		time-=StartTime;
		TotalTime += (float)time / Profile_Get_Tick_Rate();
	}
	return ( RecursionCounter == 0 );
}


/***************************************************************************************************
**
** mxProfileIterator
**
***************************************************************************************************/
mxProfileIterator::mxProfileIterator( mxProfileNode * start )
{
	CurrentParent = start;
	CurrentChild = CurrentParent->Get_Child();
}


void	mxProfileIterator::First(void)
{
	CurrentChild = CurrentParent->Get_Child();
}


void	mxProfileIterator::Next(void)
{
	CurrentChild = CurrentChild->Get_Sibling();
}


bool	mxProfileIterator::Is_Done(void)
{
	return CurrentChild == NULL;
}


void	mxProfileIterator::Enter_Child( int index )
{
	CurrentChild = CurrentParent->Get_Child();
	while ( (CurrentChild != NULL) && (index != 0) ) {
		index--;
		CurrentChild = CurrentChild->Get_Sibling();
	}

	if ( CurrentChild != NULL ) {
		CurrentParent = CurrentChild;
		CurrentChild = CurrentParent->Get_Child();
	}
}


void	mxProfileIterator::Enter_Parent( void )
{
	if ( CurrentParent->Get_Parent() != NULL ) {
		CurrentParent = CurrentParent->Get_Parent();
	}
	CurrentChild = CurrentParent->Get_Child();
}


/***************************************************************************************************
**
** mxProfileManager
**
***************************************************************************************************/

mxProfileNode		mxProfileManager::Root( "Root", NULL );
mxProfileNode *		mxProfileManager::CurrentNode = &mxProfileManager::Root;
int					mxProfileManager::FrameCounter = 0;
unsigned long int	mxProfileManager::ResetTime = 0;


/***********************************************************************************************
 * mxProfileManager::Start_Profile -- Begin a named profile                                    *
 *                                                                                             *
 * Steps one level deeper into the tree, if a child already exists with the specified name     *
 * then it accumulates the profiling; otherwise a new child node is added to the profile tree. *
 *                                                                                             *
 * INPUT:                                                                                      *
 * name - name of this profiling record                                                        *
 *                                                                                             *
 * WARNINGS:                                                                                   *
 * The string used is assumed to be a static string; pointer compares are used throughout      *
 * the profiling code for efficiency.                                                          *
 *=============================================================================================*/
void	mxProfileManager::Start_Profile( const char * name )
{
	if (name != CurrentNode->Get_Name()) {
		CurrentNode = CurrentNode->Get_Sub_Node( name );
	} 
	
	CurrentNode->Call();
}


/***********************************************************************************************
 * mxProfileManager::Stop_Profile -- Stop timing and record the results.                       *
 *=============================================================================================*/
void	mxProfileManager::Stop_Profile( void )
{
	// Return will indicate whether we should back up to our parent (we may
	// be profiling a recursive function)
	if (CurrentNode->Return()) {
		CurrentNode = CurrentNode->Get_Parent();
	}
}


/***********************************************************************************************
 * mxProfileManager::Reset -- Reset the contents of the profiling system                       *
 *                                                                                             *
 *    This resets everything except for the tree structure.  All of the timing data is reset.  *
 *=============================================================================================*/
void	mxProfileManager::Reset( void )
{ 
	g_ProfileClock.Reset();
	Root.Reset();
    Root.Call();
	FrameCounter = 0;
	Profile_Get_Ticks(&ResetTime);
}


/***********************************************************************************************
 * mxProfileManager::Increment_Frame_Counter -- Increment the frame counter                    *
 *=============================================================================================*/
void mxProfileManager::Increment_Frame_Counter( void )
{
	FrameCounter++;
}


/***********************************************************************************************
 * mxProfileManager::Get_Time_Since_Reset -- returns the elapsed time since last reset         *
 *=============================================================================================*/
float mxProfileManager::Get_Time_Since_Reset( void )
{
	unsigned long int time;
	Profile_Get_Ticks(&time);
	time -= ResetTime;
	return (float)time / Profile_Get_Tick_Rate();
}

void mxProfileManager::dumpRecursive( mxProfileIterator* profileIterator, int spacing, class mxOutputDevice* logger )
{
	profileIterator->First();
	if (profileIterator->Is_Done())
		return;

	float accumulated_time=0,parent_time = profileIterator->Is_Root() ? mxProfileManager::Get_Time_Since_Reset() : profileIterator->Get_Current_Parent_Total_Time();
	int i;
	int frames_since_reset = mxProfileManager::Get_Frame_Count_Since_Reset();
	for (i=0;i<spacing;i++)	logger->Logf( ELogLevel::LL_Info, ".");
	logger->Logf(ELogLevel::LL_Info,"----------------------------------\n");
	for (i=0;i<spacing;i++)	logger->Logf( ELogLevel::LL_Info, ".");
	logger->Logf( ELogLevel::LL_Info, "Profiling: %s (total running time: %.3f ms) ---\n",	profileIterator->Get_Current_Parent_Name(), parent_time );
	float totalTime = 0.f;


	const float EPSILON = 1e-7f;

	
	int numChildren = 0;
	
	for (i = 0; !profileIterator->Is_Done(); i++,profileIterator->Next())
	{
		numChildren++;
		float current_total_time = profileIterator->Get_Current_Total_Time();
		accumulated_time += current_total_time;
		float fraction = parent_time > EPSILON ? (current_total_time / parent_time) * 100 : 0.f;
		{
			for (unsigned j=0;j<spacing;j++)	logger->Logf( ELogLevel::LL_Info, ".");
		}
		logger->Logf( ELogLevel::LL_Info, "%d -- %s (%.2f %%) :: %.3f ms / frame (%d calls)\n",i, profileIterator->Get_Current_Name(), fraction,(current_total_time / (double)frames_since_reset),profileIterator->Get_Current_Total_Calls());
		totalTime += current_total_time;
		//recurse into children
	}

	if (parent_time < accumulated_time)
	{
		logger->Logf( ELogLevel::LL_Info, "what's wrong\n");
	}
	for (i=0;i<spacing;i++)	logger->Logf( ELogLevel::LL_Info, ".");
	logger->Logf( ELogLevel::LL_Info, "%s (%.3f %%) :: %.3f ms\n", "Unaccounted:",parent_time > EPSILON ? ((parent_time - accumulated_time) / parent_time) * 100 : 0.f, parent_time - accumulated_time);
	
	for (i=0;i<numChildren;i++)
	{
		profileIterator->Enter_Child(i);
		dumpRecursive(profileIterator,spacing+3,logger);
		profileIterator->Enter_Parent();
	}
}

void mxProfileManager::dumpAll( class mxOutputDevice* logger )
{
	AssertPtr(logger);
	mxProfileIterator* profileIterator = 0;
	profileIterator = mxProfileManager::Get_Iterator();

	dumpRecursive(profileIterator,0,logger);

	mxProfileManager::Release_Iterator(profileIterator);
}

#endif // ! MX_ENABLE_PROFILING

mxNAMESPACE_END

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//

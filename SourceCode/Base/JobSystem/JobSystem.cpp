/*
=============================================================================
File:	JobSystem.cpp
Desc:	
=============================================================================
*/

#include <Base_PCH.h>
#pragma hdrstop
#include <Base.h>

#include <JobSystem/JobSystem.h>
#include <JobSystem/Win32/ThreadPoolWin32.h>

enum { MAXJOBTHREADS = 32 };
enum { MAXJOBS = 256 };

AsyncJobQueue::SInitArgs::SInitArgs()
{
	numThreads = 0;
	maxJobs = 0;
	threadStackSize = 0;
	threadWorkspaceSize = 0;
}

AsyncJobQueue* AsyncJobQueue::StaticCreate()
{
	UNDONE;
	return nil;
}

NO_EMPTY_FILE;

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//

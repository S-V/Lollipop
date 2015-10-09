/*
=============================================================================
	File:	FileWatcher.h
	Desc:
=============================================================================
*/
mxSWIPED("lifted and modified from Vapor3D");
/**
	Main header for the FileWatcher class. Declares all implementation
	classes to reduce compilation overhead.

	@author James Wynn
	@date 4/15/2009

	Copyright (c) 2009 James Wynn (james@jameswynn.com)
	Copyright (c) 2010 vapor3D

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in
	all copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
	THE SOFTWARE.
*/

#ifndef __MX_TOOL_UTIL_FILE_WATCHER_H__
#define __MX_TOOL_UTIL_FILE_WATCHER_H__

//@todo: move this file to tools code

mxNAMESPACE_BEGIN

// Type for a watch id
typedef U4 WatchID;

/** 
 * Actions to listen for. Rename will send two events, one for
 * the deletion of the old file, and one for the creation of the
 * new file.
 */
enum EFileWacherEvent
{
	FileEvent_Create,// Sent when a file is created
	FileEvent_Modify,// Sent when a file is modified
	FileEvent_Delete,// Sent when a file is deleted
	FileEvent_Rename,// Sent when a file is renamed
};

//-----------------------------------//

/**
 * FileWatchEvent represents an event caused by a watched file.
 * This data will be sent when a file notification is sent from the
 * OS back to the engine. You can hook the delegate in the FileWatcher
 * class to get these events. These can be used to implement assets
 * live updating, so when an asset changes it will be reloaded.
 */

//-----------------------------------//

struct FileWatcherListener
{
	virtual ~FileWatcherListener() {}

	// directory - name of folder being watched (e.g.: "D:\assets\")
	// filename - path to file located in the watched folder (e.g.: "test\cube.mesh")
	virtual void onFileCreated( const mxChar* filename, const mxChar* directory, const WatchID watchId );
	virtual void onFileDeleted( const mxChar* filename, const mxChar* directory, const WatchID watchId );
	virtual void onFileChanged( const mxChar* filename, const mxChar* directory, const WatchID watchId );
	virtual void onFileRenamed( const mxChar* oldFilename, const mxChar* newFilename, const mxChar* directory, const WatchID watchId );

public:
	static FileWatcherListener	defaultInstance;
};

/** 
 * Listens to files and directories and dispatches events
 * to notify the parent program of the changes.
 */

class FileWatcher
{
public:
	// Fired up when the watcher gets notified by the OS.
	TPtr< FileWatcherListener >	listener;

public:

	mxDECLARE_CLASS_ALLOCATOR(EMemHeap::DefaultHeap,FileWatcher);

	FileWatcher()
	{
		this->listener = &FileWatcherListener::defaultInstance;
	}

	virtual ~FileWatcher()
	{}

	// Updates the watcher. Must be called often.
	virtual void update() = 0;

	// Add a directory watch
	virtual WatchID addWatch(const mxChar* directory) = 0;

	// Remove a directory watch. This is a brute force search O(nlogn).
	virtual void removeWatch(const mxChar* directory) = 0;

	// Remove a directory watch. This is a map lookup O(logn).
	virtual void removeWatch(WatchID watchid) = 0;

	virtual const mxChar* getWatchedFolder(WatchID watchid) const
	{
		mxDBG_UNIMPLEMENTED;
		return L"";
	}

public:
	static FileWatcher* New();

	PREVENT_COPY(FileWatcher)
};

mxNAMESPACE_END

#endif // !__MX_TOOL_UTIL_FILE_WATCHER_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//

/*
=============================================================================
	File:	FileWatcher.cpp
	Desc:
=============================================================================
*/

#include <EditorSupport_PCH.h>
#pragma hdrstop
#include <EditorSupport.h>

#include <Base/Templates/Containers/HashMap/TMap.h>
#include <Base/Text/TextBuffer.h>

#include <EditorSupport/Util/FileWatcher.h>

mxNAMESPACE_BEGIN

mxSWIPED("lifted and modified from Vapor3D");


//const String Actions::getString( Actions::Enum action )
//{
//	switch(action)
//	{
//	case Actions::Added:
//		return "Added";
//	case Actions::Deleted:
//		return "Deleted";
//	case Actions::Modified:
//		return "Modified";
//	case Actions::Renamed:
//		return "Renamed";
//	default:
//		return "(unknown)";
//	}
//}


/*
-----------------------------------------------------------------------------
	FileWatcherListener
-----------------------------------------------------------------------------
*/

//static
FileWatcherListener	FileWatcherListener::defaultInstance;

void FileWatcherListener::onFileCreated( const mxChar* filename, const mxChar* directory, const WatchID watchId )
{
	DBGOUT( "Created File '%s' in '%s'\n", mxTO_ANSI(filename), mxTO_ANSI(directory) );
}

void FileWatcherListener::onFileDeleted( const mxChar* filename, const mxChar* directory, const WatchID watchId )
{
	DBGOUT( "Deleted File '%s' in '%s'\n", mxTO_ANSI(filename), mxTO_ANSI(directory) );
}

void FileWatcherListener::onFileChanged( const mxChar* filename, const mxChar* directory, const WatchID watchId )
{
	DBGOUT( "Changed File '%s' in '%s'\n", mxTO_ANSI(filename), mxTO_ANSI(directory) );
}

void FileWatcherListener::onFileRenamed( const mxChar* oldFilename, const mxChar* newFilename, const mxChar* directory, const WatchID watchId )
{
	DBGOUT( "Renamed File from '%s' to '%s' in '%s'\n", mxTO_ANSI(oldFilename), mxTO_ANSI(newFilename), mxTO_ANSI(directory) );
}


struct WatchStruct;

/// type for a map from WatchID to WatchStruct pointer
typedef TMap<WatchID, WatchStruct*> WatchMap;

//-----------------------------------//

/// Implementation for Win32 based on ReadDirectoryChangesW.
/// @class FileWatcherWin32
class FileWatcherWin32 : public FileWatcher
{
public:

	FileWatcherWin32();
	virtual ~FileWatcherWin32();

	/// Add a directory watch
	/// @exception FileNotFoundException Thrown when the requested directory does not exist
	virtual WatchID addWatch(const mxChar* directory ) override;

	/// Remove a directory watch. This is a brute force lazy search O(nlogn).
	virtual void removeWatch(const mxChar* directory) override;

	/// Remove a directory watch. This is a map lookup O(logn).
	void removeWatch(WatchID watchid) override;

	/// Updates the watcher. Must be called often.
	virtual void update() override;

	virtual const mxChar* getWatchedFolder(WatchID watchid) const override;

	/// Handles the action
	void handleAction(WatchStruct* watch, const mxChar* filename, U4 action);

private:

	/// Map of WatchID to WatchStruct pointers
	WatchMap mWatches;

	/// The last watchid
	WatchID mLastWatchID;
};



class FileWatcherWin32;

typedef FixedTextBuffer< mxChar, MAX_PATH >	DirName;

/// Internal watch data
struct WatchStruct
{
	OVERLAPPED	mOverlapped;
	HANDLE		mDirHandle;
	BYTE		mBuffer[32 * 1024];
	LPARAM		lParam;
	DWORD		mNotifyFilter;
	bool		mStopNow;
	DirName		mDirName;
	WatchID		mWatchid;

	FileWatcherWin32* mWatcher;
};

//-----------------------------------//

#pragma region Internal Functions

// forward decl
bool RefreshWatch( WatchStruct* pWatch, bool _clear = false );

//-----------------------------------//

//HACK: WatchCallback() is called asynchronously and sometimes it accesses 'deleted' Watch structs (and causes memory corruption errors)
// it won't accept WatchStruct* pointers if the number of watches is zero.
static UINT numWatches = 0;

/// Unpacks events and passes them to a user defined callback.
static void CALLBACK WatchCallback(DWORD dwErrorCode, DWORD dwNumberOfBytesTransfered, LPOVERLAPPED lpOverlapped)
{
	TCHAR szFile[MAX_PATH];
	PFILE_NOTIFY_INFORMATION pNotify;
	WatchStruct* pWatch = (WatchStruct*) lpOverlapped;
	size_t offset = 0;

	if(dwNumberOfBytesTransfered == 0)
		return;

	if( numWatches == 0) {
		return;
	}

	if (dwErrorCode == ERROR_SUCCESS)
	{
		do
		{
			pNotify = (PFILE_NOTIFY_INFORMATION) &pWatch->mBuffer[offset];
			offset += pNotify->NextEntryOffset;

#if defined(UNICODE)
			{
				lstrcpynW(szFile, pNotify->FileName,
					std::min(MAX_PATH, static_cast<int>( pNotify->FileNameLength / sizeof(WCHAR) + 1) ));
			}
#else
			{
				int count = WideCharToMultiByte(CP_ACP, 0, pNotify->FileName,
					pNotify->FileNameLength / sizeof(WCHAR),
					szFile, MAX_PATH - 1, nullptr, nullptr);
				szFile[count] = TEXT('\0');
			}
#endif

			pWatch->mWatcher->handleAction(pWatch, szFile, pNotify->Action);

		} while (pNotify->NextEntryOffset != 0);
	}

	if (!pWatch->mStopNow)
	{
		RefreshWatch(pWatch);
	}
}

//-----------------------------------//

/// Refreshes the directory monitoring.
bool RefreshWatch(WatchStruct* pWatch, bool _clear)
{
	return ::ReadDirectoryChangesW(
		pWatch->mDirHandle,	// HANDLE hDirectory
		pWatch->mBuffer,	// LPVOID lpBuffer
		sizeof(pWatch->mBuffer),	// DWORD nBufferLength
		TRUE,	// BOOL bWatchSubtree
		pWatch->mNotifyFilter,
		nullptr,	// LPDWORD lpBytesReturned
		&pWatch->mOverlapped,	// LPOVERLAPPED lpOverlapped
		_clear ? nil : &WatchCallback	// LPOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine
		) != 0;
}

//-----------------------------------//

/// Stops monitoring a directory.
void DestroyWatch(WatchStruct* pWatch)
{
	if(!pWatch) return;

	pWatch->mStopNow = TRUE;

	// Cancels all pending input and output (I/O) operations
	// that are issued by the calling thread for the specified file.
	// The function does not cancel I/O operations that other threads issue for a file handle.
	// If the function succeeds, the return value is nonzero. 
	//
	//const BOOL bCancelIoResult = CancelIo(pWatch->mDirHandle);
	//(void)bCancelIoResult;

	RefreshWatch(pWatch, true);

	if (!HasOverlappedIoCompleted(&pWatch->mOverlapped))
	{
		SleepEx(5, TRUE);
	}

	CloseHandle(pWatch->mOverlapped.hEvent);
	CloseHandle(pWatch->mDirHandle);

	HeapFree(GetProcessHeap(), 0, pWatch);

	Assert(numWatches > 0);
	--numWatches;
}

//-----------------------------------//

/// Starts monitoring a directory.
//
WatchStruct* CreateWatch( LPCTSTR szDirectory, DWORD mNotifyFilter )
{
	WatchStruct* pWatch;
	size_t ptrsize = sizeof(*pWatch);
	pWatch = static_cast<WatchStruct*>(HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, ptrsize));

	pWatch->mDirHandle = CreateFile(
		szDirectory,
		FILE_LIST_DIRECTORY,
		FILE_SHARE_READ | FILE_SHARE_DELETE | FILE_SHARE_WRITE,
		nullptr,	// security attributes
		OPEN_EXISTING,
		FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
		nullptr
		);

	if (pWatch->mDirHandle != INVALID_HANDLE_VALUE)
	{
		pWatch->mOverlapped.hEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
		pWatch->mNotifyFilter = mNotifyFilter;

		if (RefreshWatch(pWatch))
		{
			++numWatches;
			return pWatch;
		}
		else
		{
			CloseHandle(pWatch->mOverlapped.hEvent);
			CloseHandle(pWatch->mDirHandle);
		}
	}

	HeapFree(GetProcessHeap(), 0, pWatch);
	return nullptr;
}

#pragma endregion

//-----------------------------------//

FileWatcherWin32::FileWatcherWin32()
	: mLastWatchID(1)
{}

//-----------------------------------//

FileWatcherWin32::~FileWatcherWin32()
{
	WatchMap::Iterator iter( mWatches );

	while( iter )
	{
		DestroyWatch( iter.Value() );

		++iter;
	}

	mWatches.Clear();
}

//-----------------------------------//

WatchID FileWatcherWin32::addWatch(const mxChar* directory)
{
	WatchID watchid = ++mLastWatchID;

	WatchStruct* watch = CreateWatch( directory, FILE_NOTIFY_CHANGE_LAST_WRITE
		| FILE_NOTIFY_CHANGE_SIZE | FILE_NOTIFY_CHANGE_FILE_NAME);

	if( !watch )
	{
		mxErrf( "CreateWatch() failed, directory: %s", mxTO_ANSI(directory) );
		return 0;
	}

	watch->mWatchid = watchid;
	watch->mWatcher = this;

	const size_t strLen = (mxStrLen(directory)) *sizeof(directory[0]);
	Assert( strLen < sizeof(watch->mDirName));

	watch->mDirName.SetNum(strLen + 1);
	MemCopy(watch->mDirName.ToChars(), directory, strLen );
	watch->mDirName[strLen] = 0;

	mWatches.Set(watchid, watch);

	return watchid;
}

//-----------------------------------//

void FileWatcherWin32::removeWatch(const mxChar* directory)
{
	WatchMap::Iterator iter( mWatches );

	while( iter )
	{
		if( mxStrEqu( directory, iter.Value()->mDirName.ToChars() ) )
		{
			removeWatch( iter.Key() );
			return;
		}
		++iter;
	}
}

//-----------------------------------//

void FileWatcherWin32::removeWatch(WatchID watchid)
{
	WatchStruct* watch = mWatches.FindRef(watchid);
	if(PtrToBool(watch))
	{
		mWatches.Remove(watchid);

		DestroyWatch(watch);

	}
}

const mxChar* FileWatcherWin32::getWatchedFolder(WatchID watchid) const
{
	WatchStruct* watch = mWatches.FindRef(watchid);
	if(PtrToBool(watch))
	{
		return watch->mDirName.ToChars();
	}
	return L"";
}

//-----------------------------------//

void FileWatcherWin32::update()
{
	::MsgWaitForMultipleObjectsEx(0, nullptr, 0, QS_ALLINPUT, MWMO_ALERTABLE);
}

//-----------------------------------//

void FileWatcherWin32::handleAction(WatchStruct* watch, const mxChar* filename, U4 action)
{
	Assert( listener.IsValid() );

	const mxChar* dirName = watch->mDirName.ToChars();
	const WatchID watchId = watch->mWatchid;

	static mxChar	oldFileName[ 256 ];

	switch(action)
	{
	case FILE_ACTION_ADDED:
		this->listener->onFileCreated(filename,dirName,watchId);
		break;

	case FILE_ACTION_REMOVED:
		this->listener->onFileDeleted(filename,dirName,watchId);
		break;

	case FILE_ACTION_MODIFIED:
		this->listener->onFileChanged(filename,dirName,watchId);
		break;

	// The file was renamed, and this is the old name.
	// If the new name resides within the directory being monitored,
	// the client will also receive the FILE_ACTION_RENAMED_NEW_NAME.
	case FILE_ACTION_RENAMED_OLD_NAME:
		ZERO_OUT( oldFileName );
		wcscpy_s( oldFileName, filename );
		break;

	// The file was renamed, and this is the new name.
	// If the old name resides within the directory being monitored,
	// the client will also receive the FILE_ACTION_RENAME_OLD_NAME.
	case FILE_ACTION_RENAMED_NEW_NAME:
		this->listener->onFileRenamed(oldFileName,filename,dirName,watchId);
		break;

	default:
		Unreachable;
	};
}

FileWatcher* FileWatcher::New()
{
	return new FileWatcherWin32();
}

mxNAMESPACE_END

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//

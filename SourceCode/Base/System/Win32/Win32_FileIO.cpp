/*
=============================================================================
	File:	Win32_FileIO.cpp
	Desc:	File system.
=============================================================================
*/
#include <Base_PCH.h>
#pragma hdrstop
#include <Base.h>

#include <sys/stat.h> 


#if !MX_PLATFORM_WIN32
	#error Unsupported platform!
#endif


mxNAMESPACE_BEGIN

//------------------------------------------------------------------------------

const char* mxFileAccessModeToStr( EAccessMode accessMode )
{
	switch(accessMode)
	{
	case EAccessMode::ReadAccess :	return "ReadAccess";
	case EAccessMode::WriteAccess :	return "WriteAccess";
	case EAccessMode::AppendAccess :	return "AppendAccess";
	case EAccessMode::ReadWriteAccess :	return "ReadWriteAccess";
	}
	Unreachable;
	return NULL_STRING;
}

mxSWIPED("Nebula3");
//------------------------------------------------------------------------------

FileHandle FS_OpenFile(
	const char* path,
	EAccessMode accessMode,
	EAccessPattern accessPattern )
{
	const char * nativePath = path;

	DWORD access = 0;
	DWORD disposition = 0;
	DWORD shareMode = 0;
	DWORD flagsAndAttributes = 0;

	switch (accessMode)
	{
	case EAccessMode::ReadAccess:
		access = GENERIC_READ;            
		disposition = OPEN_EXISTING;
		shareMode = FILE_SHARE_READ | FILE_SHARE_WRITE;
		break;

	case EAccessMode::WriteAccess:
		access = GENERIC_WRITE;
		disposition = CREATE_ALWAYS;
		shareMode = FILE_SHARE_READ;
		break;

	case EAccessMode::ReadWriteAccess:
	case EAccessMode::AppendAccess:
		access = GENERIC_READ | GENERIC_WRITE;
		disposition = OPEN_ALWAYS;
		shareMode = FILE_SHARE_READ | FILE_SHARE_WRITE;
		break;
	}

	switch (accessPattern)
	{
	case EAccessPattern::Random:
		flagsAndAttributes |= FILE_FLAG_RANDOM_ACCESS;
		break;

	case EAccessPattern::Sequential:
		flagsAndAttributes |= FILE_FLAG_SEQUENTIAL_SCAN;
		break;
	}

	// open/create the file
	FileHandle handle = ::CreateFileA(
		nativePath,              // lpFileName
		access,                  // dwDesiredAccess
		shareMode,               // dwShareMode
		0,                       // lpSecurityAttributes
		disposition,             // dwCreationDisposition,
		flagsAndAttributes,      // dwFlagsAndAttributes
		NULL                     // hTemplateFile
	);
	if( handle != INVALID_HANDLE_VALUE )
	{
		// in append mode, we need to seek to the end of the file
		if (EAccessMode::AppendAccess == accessMode)
		{
			::SetFilePointer( handle, 0, NULL, FILE_END );
		}
		return handle;
	}
	else
	{
		return InvalidFileHandle;
	}
}

//------------------------------------------------------------------------------

void FS_CloseFile( FileHandle handle )
{
	Assert(InvalidFileHandle != handle);
	::CloseHandle( handle );
}

//------------------------------------------------------------------------------

FileSize FS_WriteFile( FileHandle handle, const void* buf, FileSize numBytes )
{
	Assert(InvalidFileHandle != handle);
	Assert(buf != 0);
	Assert(numBytes > 0);
	DWORD bytesWritten;
	BOOL result = ::WriteFile(handle, buf, numBytes, &bytesWritten, NULL);
	if ((0 == result) || ((DWORD)numBytes != bytesWritten))
	{
		mxErr("WriteFile() failed!");
	}
	return bytesWritten;
}

//------------------------------------------------------------------------------
/**
    Read data from a file, returns number of bytes read.
*/

FileSize FS_ReadFile( FileHandle handle, void* buf, FileSize numBytes )
{
    Assert(InvalidFileHandle != handle);
    Assert(buf != 0);
	Assert(numBytes > 0);
	DWORD bytesRead;
	BOOL result = ::ReadFile( handle, buf, numBytes, &bytesRead, NULL );
	Assert(bytesRead > 0);
	if (0 == result)
	{
		mxErr("ReadFile() failed!");
	}
	return bytesRead;
}

//------------------------------------------------------------------------------
/**
    Seek in a file.
*/

bool FS_SeekFile( FileHandle handle, FileOffset offset, ESeekOrigin orig )
{
	Assert(InvalidFileHandle != handle);
	DWORD moveMethod;
	switch (orig)
	{
	case ESeekOrigin::Begin:
		moveMethod = FILE_BEGIN;
		break;
	case ESeekOrigin::Current:
		moveMethod = FILE_CURRENT;
		break;
	case ESeekOrigin::End:
		moveMethod = FILE_END;
		break;
	default:
		// can't happen
		Unreachable;
		moveMethod = FILE_BEGIN;
		break;
	}
	LARGE_INTEGER	distanceToMove;
	distanceToMove.LowPart = offset;
	distanceToMove.HighPart = 0;
	return ::SetFilePointerEx( handle, distanceToMove, NULL, moveMethod ) == TRUE;
}

//------------------------------------------------------------------------------
/**
    Get current position in file.
*/

FilePosition FS_TellFilePos( FileHandle handle )
{
    Assert(InvalidFileHandle != handle);
	return ::SetFilePointer( handle, 0, NULL, FILE_CURRENT );
}

//------------------------------------------------------------------------------
/**
    Flush unwritten data to file.
*/

void FS_Flush( FileHandle handle )
{
    Assert(InvalidFileHandle != handle);
	::FlushFileBuffers( handle );
}

//------------------------------------------------------------------------------
/**
    Returns true if current position is at end of file.
*/

bool FS_Eof( FileHandle handle )
{
    Assert(InvalidFileHandle != handle);
	DWORD fpos = ::SetFilePointer( handle, 0, NULL, FILE_CURRENT );
    DWORD size = ::GetFileSize( handle, NULL );

    // NOTE: THE '>=' IS NOT A BUG!!!
    return fpos >= size;
}

//------------------------------------------------------------------------------
/**
    Returns the size of a file in bytes.
*/

FileSize FS_GetFileSize( FileHandle handle )
{
    Assert(InvalidFileHandle != handle);
    return ::GetFileSize( handle, NULL );
}

/**
    Return true if a file exists.
*/

bool
FS_FileExists( const char* path )
{
#if 1
	const char * nativePath = path;

	const DWORD fileAttrs = ::GetFileAttributesA( nativePath );
	if( (-1 != fileAttrs)
		&& (0 == (FILE_ATTRIBUTE_DIRECTORY & fileAttrs)) )
	{
		return true;
	}
	else
	{
		return false;
	}
#else
	// simply check for file existence
	return ( ::_access( path, 0 ) == 0 );
#endif
}

bool FS_PathExists( const char* path )
{
	// GetFileAttributes() works with both files and directories.
	//return ( ::GetFileAttributesA( path ) != INVALID_FILE_ATTRIBUTES );

	// for some reason, GetFileAttributesA() didn't worked,
	// this works ok:
	struct stat st;
	return stat( path, &st ) == 0;
}

bool FS_FileOrPathExists( const char* path )
{
	// GetFileAttributes() works with both files and directories.
	return ( ::GetFileAttributesA( path ) != INVALID_FILE_ATTRIBUTES );
}

bool FS_GetFileTimeStamp( FileHandle handle, FileTime &outTimeStamp )
{
	FILETIME	ftWrite;
	VRET_FALSE_IF_NOT( ::GetFileTime( handle, NULL, NULL, &ftWrite ) );

	outTimeStamp = ftWrite;

	return true;
}

bool FS_GetFileCreationTime( FileHandle handle, FileTime &outCreationTime )
{
	FILETIME	ftCreation;
	VRET_FALSE_IF_NOT( ::GetFileTime( handle, &ftCreation, NULL, NULL ) );

	outCreationTime = ftCreation;

	return true;
}

BOOL FS_GetFileDateTime( FileHandle handle, mxDateTime &outDateTime )
{
	FILETIME	ftCreate, ftAccess, ftWrite,
				ftLocal;

	VRET_FALSE_IF_NOT( ::GetFileTime( handle, &ftCreate, &ftAccess, &ftWrite ) );
	VRET_FALSE_IF_NOT( ::FileTimeToLocalFileTime( &ftWrite, &ftLocal ) );

	SYSTEMTIME fileSysTime;
	VRET_FALSE_IF_NOT( ::FileTimeToSystemTime( &ftLocal, &fileSysTime ) );

	outDateTime.year = fileSysTime.wYear - 1;
	outDateTime.month = fileSysTime.wMonth - 1;
	outDateTime.day = fileSysTime.wDay - 1;
	outDateTime.hour = fileSysTime.wHour;
	outDateTime.minute = fileSysTime.wMinute;
	outDateTime.second = fileSysTime.wSecond;

	return TRUE;
}

mxSWIPED("MSDN");
BOOL FS_GetLastWriteTimeString( FileHandle hFile, LPTSTR lpszString, DWORD dwSize )
{
	FILETIME ftCreate, ftAccess, ftWrite;
	SYSTEMTIME stUTC, stLocal;
	DWORD dwRet;

	// Retrieve the file times for the file.
	if (!GetFileTime(hFile, &ftCreate, &ftAccess, &ftWrite))
	{
		mxDEBUG_BREAK;
		return FALSE;
	}

	// Convert the last-write time to local time.
	FileTimeToSystemTime(&ftWrite, &stUTC);
	SystemTimeToTzSpecificLocalTime(NULL, &stUTC, &stLocal);

	// Build a string showing the date and time.
	dwRet = StringCchPrintf(
		lpszString, dwSize, 
		TEXT("%02d/%02d/%d  %02d:%02d"),
		stLocal.wMonth, stLocal.wDay, stLocal.wYear,
		stLocal.wHour, stLocal.wMinute
	);

	return ( S_OK == dwRet ) ? TRUE : FALSE;
}


BOOL FS_Win32_FileTimeStampToDateTimeString( const FILETIME& fileTime, char *outString, UINT numChars )
{
	DWORD dwRet;

	FILETIME	ftLocal;
	VRET_FALSE_IF_NOT( ::FileTimeToLocalFileTime( &fileTime, &ftLocal ) );

	SYSTEMTIME fileSysTime;
	VRET_FALSE_IF_NOT( ::FileTimeToSystemTime( &ftLocal, &fileSysTime ) );

	// Build a string showing the date and time.
	dwRet = ::StringCchPrintfA(
		outString, numChars, 
		("%02d.%02d.%d  %02d:%02d"),
		fileSysTime.wMonth, fileSysTime.wDay, fileSysTime.wYear,
		fileSysTime.wHour, fileSysTime.wMinute
	);
	if( S_OK == dwRet )
		return TRUE;
	else return FALSE;
}

mxUInt64 FS_GetDriveFreeSpace( const char* path )
{
	LARGE_INTEGER freeBytesAvailableToCaller;// bytes on disk available to caller
	LARGE_INTEGER totalNumberOfBytes;	// bytes on disk
	LARGE_INTEGER totalNumberOfFreeBytes;	// free bytes on disk

	::GetDiskFreeSpaceExA(
		path,
		(PULARGE_INTEGER )&freeBytesAvailableToCaller,
		(PULARGE_INTEGER )&totalNumberOfBytes,
		(PULARGE_INTEGER )&totalNumberOfFreeBytes
	);

	//return *(SizeT*)&totalNumberOfFreeBytes;
	return always_cast< mxUInt64, LARGE_INTEGER >( totalNumberOfFreeBytes );
}

bool FS_AisNewerThanB( const FileTime& a, const FileTime& b )
{
	return a > b;
}

bool FS_IsFileNewer( const char* fileNameA, const FileTime& lastTimeWhenModified )
{
	HANDLE hFile;

	hFile = ::CreateFileA( fileNameA, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
	if( INVALID_HANDLE_VALUE == hFile ) {
		mxDEBUG_BREAK;
		return true;
	}
#if 0
	BY_HANDLE_FILE_INFORMATION fi;
	::GetFileInformationByHandle( hFile, &fi );
	::CloseHandle( hFile );

	LONG result = ::CompareFileTime( &fi.ftLastWriteTime, &lastTimeWhenModified );
#else
	FILETIME	ftWrite;
	::GetFileTime( hFile, NULL, NULL, &ftWrite );
	::CloseHandle( hFile );
/*
	values returned by CompareFileTime() and their meaning:
	-1 - First file time is earlier than second file time.
	0 - First file time is equal to second file time.
	1 - First file time is later than second file time.
*/
	LONG result = ::CompareFileTime( &ftWrite, &lastTimeWhenModified.time );
#endif
	return (result == 1);
}

bool FS_IsFileANewerThanFileB( const char* fileNameA, const char* fileNameB )
{
	HANDLE hFile;
	BY_HANDLE_FILE_INFORMATION fi1, fi2;

	hFile = ::CreateFileA( fileNameA, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
	if( INVALID_HANDLE_VALUE == hFile ) {
		mxDEBUG_BREAK;
		return true;
	}
	::GetFileInformationByHandle( hFile, &fi1 );
	::CloseHandle( hFile );

	hFile = ::CreateFileA( fileNameB, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
	if( INVALID_HANDLE_VALUE == hFile ) {
		mxDEBUG_BREAK;
		return true;
	}
	::GetFileInformationByHandle( hFile, &fi2 );
	::CloseHandle( hFile );

	LONG result = ::CompareFileTime( &fi1.ftLastWriteTime, &fi2.ftLastWriteTime );
	return (result == 1);
}

bool FS_EraseFile( const char* fileName )
{
	DEVOUT("Deleting file '%s'\n",fileName);
	return ::DeleteFileA( fileName ) == TRUE;
}

bool FS_MakeDirectory( const char* pathName )
{
	DEVOUT("Creating directory '%s'\n",pathName);
	return ::CreateDirectoryA( pathName, NULL ) == TRUE;
//	if (::GetLastError() != ERROR_ALREADY_EXISTS) return false;
}

FileTime FileTime::CurrentTime()
{
	SYSTEMTIME	sysTime;
	::GetSystemTime( & sysTime );
	FILETIME	fileTime;
	BOOL ret = ::SystemTimeToFileTime( &sysTime, & fileTime );
	Assert_NZ(ret);
	return FileTime( fileTime );
}

bool FS_SetCurrentDirectory( const char* dir )
{
	return TRUE == ::SetCurrentDirectoryA( dir );
}

UINT FS_GetCurrentDirectory( char *dir, UINT numChars )
{
	AssertPtr(dir);
	Assert_GZ(numChars);
	// If the function succeeds, the return value specifies the number of characters that are written to the buffer, not including the terminating null character.
	// If the function fails, the return value is zero.
	return (UINT)::GetCurrentDirectoryA( numChars, dir );
}

// fills the buffer with strings that specify valid drives in the system
// e.g. buffer = "CDEF" and numDrives = 4
bool GetDriveLetters( char *buffer, int bufferSize, UINT &numDrives )
{
	Assert(bufferSize > 0);
	numDrives = 0;

	char	tmp[256];
	MemZero(tmp, sizeof tmp);

	// GetLogicalDriveStringsA() fills the buffer with strings
	// that specify valid drives in the system
	// e.g. "C:\" "D:\" "E:\"
	if( GetLogicalDriveStringsA( sizeof(tmp), tmp ) )
	{
		char* p = tmp;
		while( *p && (p - tmp < bufferSize) )
		{
			buffer[ numDrives++ ] = *p;
			p += 4; // sizeof("C:\\ ") is 4
		}
		return true;
	}
	return false;
}


EOSFileType FS_GetFileType( const char *filename )
{
#if WIN32
	DWORD result = ::GetFileAttributesA(filename);

	if(result == INVALID_FILE_ATTRIBUTES)
		return FS_FILETYPE_NONE;

	if(result & FILE_ATTRIBUTE_DIRECTORY)
		return FS_FILETYPE_DIRECTORY;

	return FS_FILETYPE_FILE;
#else
	struct stat buf;

	if (stat (path,&buf) == -1)
		return FS_FILETYPE_NONE;

	if(S_ISDIR(buf.st_mode))
		return FS_FILETYPE_DIRECTORY;

	return FS_FILETYPE_FILE;
#endif
}

//AStreamReader & operator >> ( AStreamReader& archive, FileTime& o )
//{
//	return archive.Unpack( o );
//}
//AStreamWriter & operator << ( AStreamWriter& archive, const FileTime& o )
//{
//	return archive.Pack( o );
//}

mxSWIPED("http://winmerge.svn.sourceforge.net/svnroot/winmerge/trunk/Src/files.h");

#define LogErrorString(x,...)

/**
 * @brief Open file as memory-mapped file.
 * @param [in,out] fileData Memory-mapped file's info.
 * @return TRUE if opening succeeded, FALSE otherwise.
 */
BOOL FS_OpenFileMapped(MAPPEDFILEDATA *fileData)
{
        DWORD dwProtectFlag = 0;
        DWORD dwMapAccess = 0;
        DWORD dwOpenAccess = 0;
        DWORD dwFileSizeHigh = 0;
        DWORD dwSharedMode = FILE_SHARE_READ;
        HANDLE hTemplateFile = NULL; // for creating new file
        BOOL bSuccess = TRUE;

        if (fileData->bWritable)
        {
                dwProtectFlag = PAGE_READWRITE;
                dwMapAccess = FILE_MAP_ALL_ACCESS;
                dwOpenAccess = GENERIC_READ | GENERIC_WRITE;
        }
        else
        {
                dwProtectFlag = PAGE_READONLY;
                dwMapAccess = FILE_MAP_READ;
                dwOpenAccess = GENERIC_READ;
        }

        fileData->hFile = CreateFile(fileData->fileName,
                dwOpenAccess, dwSharedMode, NULL, fileData->dwOpenFlags,
                FILE_ATTRIBUTE_NORMAL, hTemplateFile);

        if (fileData->hFile == INVALID_HANDLE_VALUE)
        {
                bSuccess = FALSE;
                LogErrorString(Fmt(_T("CreateFile(%s) failed in FS_OpenFileMapped: %s")
                        , fileData->fileName, GetSysError(GetLastError()).c_str()));
        }
        else
        {
                if (fileData->dwSize == 0)
                {
                        fileData->dwSize = GetFileSize(fileData->hFile,
                                 &dwFileSizeHigh);
                        if (fileData->dwSize == 0xFFFFFFFF || dwFileSizeHigh)
                        {
                                fileData->dwSize = 0;
                                bSuccess = FALSE;
                        }
                }
        }

        if (bSuccess)
        {
                if (fileData->dwSize == 0 && dwFileSizeHigh == 0)
                        // Empty file (but should be accepted anyway)
                        return bSuccess;

                fileData->hMapping = CreateFileMapping(fileData->hFile,
                                NULL, dwProtectFlag, 0, fileData->dwSize, NULL);
                if (!fileData->hMapping)
                {
                        bSuccess = FALSE;
                        LogErrorString(Fmt(_T("CreateFileMapping(%s) failed: %s")
                                , fileData->fileName, GetSysError(GetLastError()).c_str()));
                }
                else
                {
                        fileData->pMapBase = MapViewOfFile(fileData->hMapping,
                                dwMapAccess, 0, 0, 0);
                        if (!fileData->pMapBase)
                        {
                                bSuccess = FALSE;
                                LogErrorString(Fmt(_T("MapViewOfFile(%s) failed: %s")
                                        , fileData->fileName, GetSysError(GetLastError()).c_str()));
                        }
                }
        }

        if (!bSuccess)
        {
                UnmapViewOfFile(fileData->pMapBase);
                fileData->pMapBase = NULL;
                CloseHandle(fileData->hMapping);
                fileData->hMapping = NULL;
                CloseHandle(fileData->hFile);
                fileData->hFile = NULL;
        }
        return bSuccess;
}

/**
 * @brief Close memory-mapped file.
 * @param [in, out] fileData Memory-mapped file's info.
 * @param [in] newSize New size for the file.
 * @param [in] flush Flush buffers before closing the file.
 * @return TRUE if closing succeeded without errors, FALSE otherwise.
 */
BOOL FS_CloseFileMapped(MAPPEDFILEDATA *fileData, DWORD newSize, BOOL flush)
{
        BOOL bSuccess = TRUE;

        if (fileData->pMapBase)
        {
                UnmapViewOfFile(fileData->pMapBase);
                fileData->pMapBase = NULL;
        }

        if (fileData->hMapping)
        {
                CloseHandle( fileData->hMapping );
                fileData->hMapping = NULL;
        }

        if (newSize != 0xFFFFFFFF)
        {
                SetFilePointer(fileData->hFile, newSize, NULL, FILE_BEGIN);
                SetEndOfFile(fileData->hFile);
        }

        if (flush)
                FlushFileBuffers(fileData->hFile);

        if (fileData->hFile)
        {
                CloseHandle(fileData->hFile);
                fileData->hFile = NULL;
        }
        return bSuccess;
}

/**
 * @brief Checks if file is read-only on disk.
 * Optionally returns also if file exists.
 * @param [in] file Full path to file to check.
 * @param [in, out] fileExists If non-NULL, function returns if file exists.
 * @return TRUE if file is read-only, FALSE otherwise.
 */
BOOL FS_IsFileReadOnly(const TCHAR* file, BOOL *fileExists /*=NULL*/)
{
        struct _stati64 fstats = {0};
        BOOL bReadOnly = FALSE;
        BOOL bExists = FALSE;

        if (_tstati64(file, &fstats) == 0)
        {
                bExists = TRUE;

                if ((fstats.st_mode & _S_IWRITE) == 0)
                        bReadOnly = TRUE;
        }
        else
                bExists = FALSE;

        if (fileExists != NULL)
                *fileExists = bExists;

        return bReadOnly;
}

/**
 * @brief Update file's modification time.
 * @param [in] info Contains filename, path and file times to update.
 */
//void FS_UpdateFileTime(const DiffFileInfo & info)
//{
//        String path = paths_ConcatPath(info.path, info.filename);
//        _utimbuf times = {0};
//
//        times.modtime = info.mtime;
//        _tutime(path.c_str(), &times);
//}

mxNAMESPACE_END

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//

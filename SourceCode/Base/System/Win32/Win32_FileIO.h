/*
=============================================================================
	File:	Win32_FileIO.h
	Desc:	File system.
	Note:	Don't use low-level file management functions directly.
=============================================================================
*/
#pragma once

// System-dependent path separator character used for constructing platform-dependent path names.
#define PATHSEPARATOR_CHAR				'/'

#define PATHSEPARATOR_CHAR2				'\\'	// <= this works only on Windows

// System-dependent path separator character represented as a string.
#define PATHSEPARATOR_STR				"/"

#define PATHSEPARATOR_STR2				"\\"	// <= this works only on Windows

#define DRIVE_SEPARATOR_CHAR			':'

#define FILE_EXT_SEPARATOR_CHAR			'.'

#define NEWLINE_CHAR					'\n'
#define NEWLINE_STR						"\r\n"

// end-of-string marker, null terminator
#define NULL_CHAR						'\0'

#define NULL_STRING						""

#ifndef FILE_ATTRIBUTE_NORMAL 
	#define FILE_ATTRIBUTE_NORMAL 0x00000080
#endif

//---------------------------------------------------------------

mxNAMESPACE_BEGIN

class AStreamReader;
class AStreamWriter;

//------------------------------------------------------------
//
//	File system wrappers.
//
//------------------------------------------------------------

// Maximum length a file system path name can have.
//enum { MAX_PATH_CHARS = 256 };	// <= it's MAX_PATH (usually 260) on Win32
enum { FS_MAX_PATH = 256 };	// should be enough

// Maximum length a pure file name can have.
enum { MAX_FILENAME_CHARS = 64 };	// <= 127 on Win32


//------------------------------------------------------------

//
//	FileHandle
//
typedef HANDLE	FileHandle;

const FileHandle InvalidFileHandle = ((HANDLE) INVALID_HANDLE_VALUE);

FORCEINLINE bool FS_IsValid( FileHandle hFile ) { return hFile != InvalidFileHandle; }

typedef mxLong	FilePosition;	// must be signed!
typedef mxLong	FileOffset;		// must be signed!
typedef mxLong	FileSize;		// must be signed!

enum EAccessMode
{
    ReadAccess,		// opens a file only if it exists
    WriteAccess,	// overwrites existing file (always creates a new file)
    AppendAccess,	// appends to existing file
    ReadWriteAccess,// always creates a new file ('re-write')
};

const char* mxFileAccessModeToStr( EAccessMode accessMode );

enum EAccessPattern
{
    Random,
    Sequential,
};
enum ESeekOrigin
{
    Begin,
    Current,
    End,
};

//------------------------------------------------------------------------------

// returns InvalidFileHandle in case of failure
//
FileHandle FS_OpenFile(
	const char* path,
	EAccessMode accessMode,
	EAccessPattern accessPattern = EAccessPattern::Sequential
);

void FS_CloseFile( FileHandle handle );

// Writes data to the specified file.
FileSize FS_WriteFile( FileHandle handle, const void* buf, FileSize numBytes );

// Reads data from the specified file.
FileSize FS_ReadFile( FileHandle handle, void* buf, FileSize numBytes );

// Moves the file pointer of the specified file.
bool FS_SeekFile( FileHandle handle, FileOffset offset, ESeekOrigin orig );

FilePosition FS_TellFilePos( FileHandle handle );

void FS_Flush( FileHandle handle );

bool FS_Eof( FileHandle handle );

// Retrieves the size of the specified file, in bytes.
FileSize FS_GetFileSize( FileHandle handle );

bool FS_FileExists( const char* file );
bool FS_PathExists( const char* path );//FS_DirectoryExists
bool FS_FileOrPathExists( const char* path );


struct FileTime
{
	FILETIME time;

public:

	FORCEINLINE FileTime()
	{
		time.dwLowDateTime = 0;
		time.dwHighDateTime = 0;
	}
	FORCEINLINE FileTime( DWORD lo, DWORD hi )
	{
		time.dwLowDateTime = lo;
		time.dwHighDateTime = hi;
	}
	FORCEINLINE FileTime( const FILETIME& ft )
	{
		time = ft;
	}
/*
	values returned by CompareFileTime() and their meaning:
	-1 - First file time is earlier than second file time.
	0 - First file time is equal to second file time.
	1 - First file time is later than second file time.
*/
	FORCEINLINE friend bool operator == ( const FileTime& a, const FileTime& b )
	{
		return (0 == ::CompareFileTime( &(a.time), &(b.time)) );
	}
	FORCEINLINE friend bool operator != ( const FileTime& a, const FileTime& b )
	{
		return (0 != ::CompareFileTime( &(a.time), &(b.time)) );
	}
	FORCEINLINE friend bool operator > ( const FileTime& a, const FileTime& b )
	{
		return (1 == ::CompareFileTime( &(a.time), &(b.time)) );
	}
	FORCEINLINE friend bool operator < ( const FileTime& a, const FileTime& b )
	{
		return (-1 == ::CompareFileTime( &(a.time), &(b.time)) );
	}

	FORCEINLINE void operator = ( const FILETIME& platformTimeStamp )
	{
		this->time = platformTimeStamp;
	}


	//--------------------------------------------------------------//
	//	Serializers.
	//--------------------------------------------------------------//

	//friend AStreamReader & operator >> ( AStreamReader& archive, FileTime& o );
	//friend AStreamWriter & operator << ( AStreamWriter& archive, const FileTime& o );

public:
	static FileTime CurrentTime();
};


// retrieves the time stamp of last modification
bool FS_GetFileTimeStamp( FileHandle handle, FileTime &outTimeStamp );

bool FS_GetFileCreationTime( FileHandle handle, FileTime &outCreationTime );

// returns 'false' in case of failure
BOOL FS_GetFileDateTime( FileHandle handle, mxDateTime &outDateTime );

// GetLastWriteTime - Retrieves the last-write time and converts
//                    the time to a string
//
// Return value - TRUE if successful, FALSE otherwise
// hFile      - Valid file handle
// lpszString - Pointer to buffer to receive string
// dwSize     - Number of elements in the buffer
BOOL FS_GetLastWriteTimeString( FileHandle hFile, LPTSTR lpszString, DWORD dwSize );

BOOL FS_Win32_FileTimeStampToDateTimeString( const FILETIME& fileTime, char *outString, UINT numChars );

// Returns the amount of free drive space in the specified path, in bytes.
mxUInt64 FS_GetDriveFreeSpace( const char* path );

// useful for reloading modified files
bool FS_AisNewerThanB( const FileTime& a, const FileTime& b );
bool FS_IsFileNewer( const char* fileNameA, const FileTime& lastTimeWhenModified );
bool FS_IsFileANewerThanFileB( const char* fileNameA, const char* fileNameB );

bool FS_EraseFile( const char* fileName );
bool FS_MakeDirectory( const char* pathName );

// Changes the current directory for the current process.
bool FS_SetCurrentDirectory( const char* dir );

// 'dir' - a pointer to the buffer that receives the current directory string. This null-terminated string specifies the absolute path to the current directory.
// 'numChars' - size of the output buffer in characters. The buffer length must include room for a terminating null character.
// If the function succeeds, the return value specifies the number of characters that are written to the buffer, not including the terminating null character.
// If the function fails, the return value is zero.
UINT FS_GetCurrentDirectory( char *dir, UINT *numChars );

// fills the buffer with strings that specify valid drives in the system
// e.g. buffer = "CDEF" and numDrives = 4
bool GetDriveLetters( char *buffer, int bufferSize, UINT &numDrives );

inline bool FS_EnsurePathExists( PCHARS folderName )
{
	FS_MakeDirectory( folderName );
	return FS_PathExists( folderName );
}

FORCEINLINE bool IsPathSeparator( char c )
{
	return c == PATHSEPARATOR_CHAR
		|| c == PATHSEPARATOR_CHAR2;
}

enum EOSFileType {
	FS_FILETYPE_NONE,
	FS_FILETYPE_FILE,
	FS_FILETYPE_DIRECTORY
};
EOSFileType FS_GetFileType( const char *filename );

mxSWIPED("http://winmerge.svn.sourceforge.net/svnroot/winmerge/trunk/Src/files.h");
/**
 * @brief Memory-mapped file information
 * When memory-mapped file is created, related information is
 * stored to this structure.
 */
struct MAPPEDFILEDATA
{
        TCHAR fileName[_MAX_PATH];
        BOOL bWritable;
        DWORD dwOpenFlags;              // CreateFile()'s dwCreationDisposition
        DWORD dwSize;
        HANDLE hFile;
        HANDLE hMapping;
        LPVOID pMapBase;
};

BOOL FS_OpenFileMapped(MAPPEDFILEDATA *fileData);
BOOL FS_CloseFileMapped(MAPPEDFILEDATA *fileData, DWORD newSize, BOOL flush);
BOOL FS_IsFileReadOnly(const TCHAR* file, BOOL *fileExists = NULL);

mxNAMESPACE_END


mxSWIPED("OOOII, file: oStdio.h");

// _____________________________________________________________________________
// Environment API

enum oSYSPATH
{
	oSYSPATH_CWD, // current working directory
	oSYSPATH_APP, // application directory (path where exe is)
	oSYSPATH_TMP, // platform temporary directory
	oSYSPATH_SYS, // platform system directory
	oSYSPATH_OS, // platform installation directory
	oSYSPATH_DEV, // current project development root directory
	oSYSPATH_COMPILER_INCLUDES, // location of compiler includes
	oSYSPATH_DESKTOP, // platform current user desktop
	oSYSPATH_DESKTOP_ALLUSERS, // platform shared desktop
	oSYSPATH_P4ROOT, // current user's Perforce workspace root (requires P4PORT and P4USER env vars to be set)
};

// Return the full path to one of the types of system paths enumerated by oSYSPATH
bool oGetSysPath(char* _StrSysPath, size_t _SizeofStrSysPath, oSYSPATH _SysPath);

// Returns the name of this computer
bool oGetHostname(char* _Hostname, size_t _SizeofHostName);

inline const char* oGetHostname() { static char MX_THREAD_LOCAL buf[512]; oGetHostname(buf, sizeof(buf)); return buf; }

// Returns a string indicating the host, process id and thread id of where this
// function was called from.
bool oGetExecutionPath(char* _ExecutionPath, size_t _SizeofExecutionPath);

// Wrapper for use assert/debug macros ONLY because it's returning temp memory
inline const char* oGetExecutionPath() { static char MX_THREAD_LOCAL buf[512]; oGetExecutionPath(buf, sizeof(buf)); return buf; }

// Returns the full path to the name of the file that is executing in this 
// process
bool oGetExePath(char* _ExePath, size_t _SizeofExePath);

// Sets the current working directory. Use oGetSysPath to get CWD
bool oSetCWD(const char* _CWD);

// Accessors for the environment variables passed into this process
bool oSetEnvironmentVariable(const char* _Name, const char* _Value);
bool oGetEnvironmentVariable(char* _Value, size_t _SizeofValue, const char* _Name);
template<size_t size> inline bool oGetEnvironmentVariable(char (&_Value)[size], const char* _Name) { return oGetEnvironmentVariable(_Value, size, _Name); }

// Fills _StrEnvironment with all environment variables delimited by '\n'
bool oGetEnvironmentString(char* _StrEnvironment, size_t _SizeofStrEnvironment);

bool oSysGUIUsesGPUCompositing();

#if 0
// Set a time (preferably in the future) when a sleeping system should wake up
// Obviously a wakeup API cannot be called while the computer is system, so
// before using platform API to put the system to sleep, set up when it is to 
// reawake.
bool oScheduleWakeupAbsolute(time_t _AbsoluteTime, oFUNCTION<void()> _OnWake);
bool oScheduleWakeupRelative(unsigned int _TimeFromNowInMilliseconds, oFUNCTION<void()> _OnWake);
#endif
// Allow system to go to sleep (probably to default behavior), but more usefully
// setting this to false will prevent a computer going to sleep and leave it in
// a server-like mode (ES_AWAYMODE_REQUIRED on Windows) for long processes such
// as video processing.
void oSysAllowSleep(bool _Allow);

class oScopedDisableSystemSleep
{
	oScopedDisableSystemSleep() { oSysAllowSleep(false); }
	~oScopedDisableSystemSleep() { oSysAllowSleep(true); }
};

// _____________________________________________________________________________
// Templated-on-size versions of the above API

template<size_t size> inline bool oGetSysPath(char (&_StrSysPath)[size], oSYSPATH _SysPath) { return oGetSysPath(_StrSysPath, size, _SysPath); }
template<size_t size> inline bool oGetHostname(char (&_Hostname)[size]) { return oGetHostname(_Hostname, size); }
template<size_t size> inline bool oGetExePath(char (&_StrExePath)[size]) { return oGetExePath(_StrExePath, size); }
template<size_t size> inline bool oExecute(const char* _CommandLine, char (&_StrStdout)[size] , int* _pExitCode = 0, unsigned int _ExecutionTimeout = oINFINITE_WAIT) { return oExecute(_CommandLine, _StrStdout, size, _pExitCode, _ExecutionTimeout); }
template<size_t size> inline bool oGetEnvironmentString(char (&_StrEnvironment)[size]) { return oGetEnvironmentString(_pEnvironment, size); }


mxSWIPED("OOOII, file: oPath.h");

inline bool oIsFileSeparator(char c) { return c == '\\' || c == '/'; }
inline bool oIsUNCPath(const char* _Path) { return _Path && oIsFileSeparator(*_Path) && oIsFileSeparator(*(_Path+1)); }
inline bool oIsFullPath(const char* _Path) { return _Path && *_Path && *(_Path+1) == ':'; }

// Return pointer to file base
const char* oGetFilebase(const char* _Path);
char* oGetFilebase(char* _Path);
errno_t oGetFilebase(char* _Filebase, size_t _SizeofFilebase, const char* _Path);

// Return pointer to file extension (i.e. the '.' character)
// if there is no extension, this returns the pointer to the 
// nul terminator of the string.
const char* oGetFileExtension(const char* _Path);
char* oGetFileExtension(char* _Path);

// Return pointer to the volume name
inline const char* oGetVolumeName(const char* _Path) { const char* p = _Path; if (oIsUNCPath(p)) p += 2; return p; }
inline char* oGetVolumeName(char* _Path) { char* p = _Path; if (oIsUNCPath(p)) p += 2; return p; }

// Inserts or replaces the current extension with the specified one
errno_t oReplaceFileExtension(char* _Path, size_t _SizeofPath, const char* _Extension);

// Removes the right-most file or dir name
char* oTrimFilename(char* _Path);
char* oTrimFileExtension(char* _Path);

// Ensures a backslash or forward slash is the rightmost char
// returns new length of path
errno_t oEnsureFileSeparator(char* _Path, size_t _SizeofPath);

// Converts . and .. into something meaningful and replaces fseps with the specified one.
// _CleanedPath and _SourcePath can be the same pointer.
errno_t oCleanPath(char* _CleanedPath, size_t _SizeofCleanedPath, const char* _SourcePath, char _FileSeparator = '/');

// Returns the path to a .txt file with the name of the current exe 
// concatenated with the (optionally) specified suffix and a sortable timestamp 
// in the filename to ensure uniqueness.
errno_t oGetLogFilePath(char* _StrDestination, size_t _SizeofStrDestination, const char* _ExeSuffix = 0);

// Standard Unix/MS-DOS style wildcard matching
bool oMatchesWildcard(const char* _Wildcard, const char* _Path);

#if 0
// Fills path if the relative path exists when one of the paths in search_path 
// is prepended. The format of _SearchPath is paths separated by a semi-colon. 
// This also takes a _DotPath, which is what to append if relpath begin with a 
// "./". NOTE: If you want _RelativePath as-is to be found, include "." in 
// the search path.
bool oFindInPath(char* _ResultingFullPath, size_t _SizeofResultingFullPath, const char* _SearchPath, const char* _RelativePath, const char* _DotPath, oPATH_EXISTS_FUNCTION _PathExists);

// Find a file in the specified system path
bool oFindInSysPath(char* _ResultingFullPath, size_t _SizeofResultingFullPath, oSYSPATH _SysPath, const char* _RelativePath, const char* _DotPath, oPATH_EXISTS_FUNCTION _PathExists);

// Searches all system and environment paths, as well as extraSearchPath which 
// is a string of paths delimited by semi-colons. _RelativePath is the filename/
// partial path to be matched against the various prefixes to get a full path.
bool oFindPath(char* _ResultingFullPath, size_t _SizeofResultingFullPath, const char* _RelativePath, const char* _DotPath, const char* _ExtraSearchPath, oPATH_EXISTS_FUNCTION _PathExists);
#endif
//returns the index to one after the last path separator where the previous part of the path is the same between the 2 arguments.
size_t oCommonPath(const char* _path1, const char* _path2);

//takes a full path, and creates a path relative to the reference path.
void oMakeRelativePath(char* _relativePath, const char* _fullPath, const char* _referencePath);

// _____________________________________________________________________________
// Templated-on-size versions of the above functions

template<size_t size> inline errno_t oGetFilebase(char (&_Filebase)[size], const char* _Path) { return oGetFilebase(_Filebase, size, _Path); }
template<size_t size> inline errno_t oReplaceFileExtension(char (&_Path)[size], const char* _Extension) { return oReplaceFileExtension(_Path, size, _Extension); }
template<size_t size> inline errno_t oEnsureFileSeparator(char (&_Path)[size]) { return oEnsureFileSeparator(_Path, size); }
template<size_t size> inline errno_t oCleanPath(char (&_SizeofCleanedPath)[size], const char* _SourcePath, char _FileSeparator = '/') { return oCleanPath(_SizeofCleanedPath, size, _SourcePath, _FileSeparator); }
template<size_t size> inline errno_t oGetLogFilePath(char (&_StrDestination)[size], const char* _ExeSuffix = 0) { return oGetLogFilePath(_StrDestination, size, _ExeSuffix); }
#if 0
template<size_t size> inline bool oFindInPath(char (&_ResultingFullPath)[size], const char* _SearchPath, const char* _RelativePath, const char* _DotPath, oPATH_EXISTS_FUNCTION _PathExists) { return oFindInPath(_ResultingFullPath, size, _SearchPath, _RelativePath, _DotPath, _PathExists); }
template<size_t size> inline bool oFindInSysPath(char(&_ResultingFullPath)[size], oSYSPATH _SysPath, const char* _RelativePath, const char* _DotPath, oPATH_EXISTS_FUNCTION _PathExists) { return oFindInSysPath(_ResultingFullPath, size, _SysPath, _RelativePath, _DotPath, _PathExists); }
template<size_t size> inline bool oFindPath(char (&_ResultingFullPath)[size], const char* _RelativePath, const char* _DotPath, const char* _ExtraSearchPath, oPATH_EXISTS_FUNCTION _PathExists) { return oFindPath(_ResultingFullPath, size, _RelativePath, _DotPath, _ExtraSearchPath, _PathExists); }
#endif
//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//

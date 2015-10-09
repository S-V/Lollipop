#pragma once

#include <Base/Text/StringTools.h>

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
template< UINT NUM_CHARS >
inline
bool F_ExtractFileBase(
					   PCHARS inFilePath,
					   ANSICHAR (&outFileBase)[NUM_CHARS]
)
{
	AssertPtr(inFilePath);	VRET_FALSE_IF_NOT(inFilePath != nil);

	V_FileBase( inFilePath, outFileBase, NUMBER_OF(outFileBase) );

	return true;
}
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
template< UINT NUM_CHARS >
bool F_ComposeFilePath(
					   PCHARS inOldFilePath,
					   PCHARS inOldFileName,
					   PCHARS inNewExtension,
					   ANSICHAR (&outNewFilePath)[NUM_CHARS]
)
{
	AssertPtr(inOldFilePath);	VRET_FALSE_IF_NOT(inOldFilePath != nil);
	AssertPtr(inOldFileName);	VRET_FALSE_IF_NOT(inOldFileName != nil);
	AssertPtr(inNewExtension);	VRET_FALSE_IF_NOT(inNewExtension != nil);

	// Extract the file base.
	ANSICHAR	fileBase[ FS_MAX_PATH ];
	V_FileBase( inOldFileName, fileBase, NUMBER_OF(fileBase) );

	// Extract the file path only.
	ANSICHAR	purePath[ FS_MAX_PATH ];
	if( !V_ExtractFilePath( inOldFilePath, purePath, NUMBER_OF(purePath) ) )
	{
		mxStrCpyAnsi( purePath, inOldFilePath );
	}

	// Compose a new file path with the given extension.
	V_ComposeFileName( inOldFilePath, fileBase, outNewFilePath, NUM_CHARS );
	V_SetExtension( outNewFilePath, inNewExtension, NUM_CHARS );

	return true;
}
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
template< UINT NUM_CHARS >
bool F_ComposeFilePath(
					   PCHARS inOldFilePath,
					   PCHARS inNewExtension,
					   ANSICHAR (&outNewFilePath)[NUM_CHARS]
)
{
	AssertPtr(inOldFilePath);	VRET_FALSE_IF_NOT(inOldFilePath != nil);
	AssertPtr(inNewExtension);	VRET_FALSE_IF_NOT(inNewExtension != nil);

	// Extract the file base.
	ANSICHAR	fileBase[ FS_MAX_PATH ];
	V_FileBase( inOldFilePath, fileBase, NUMBER_OF(fileBase) );

	// Extract the file path only.
	ANSICHAR	purePath[ FS_MAX_PATH ];
	if( !V_ExtractFilePath( inOldFilePath, purePath, NUMBER_OF(purePath) ) )
	{
		mxStrCpyAnsi( purePath, inOldFilePath );
	}

	// Compose a new file path with the given extension.
	V_ComposeFileName( purePath, fileBase, outNewFilePath, NUM_CHARS );
	V_SetExtension( outNewFilePath, inNewExtension, NUM_CHARS );

	return true;
}
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
template< UINT NUM_CHARS >
bool F_ExtractPathOnly(
					   PCHARS inFilePath,
					   ANSICHAR (&outPurePath)[NUM_CHARS]
)
{
	return V_ExtractFilePath( inFilePath, outPurePath, NUMBER_OF(outPurePath) );
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

/*
 *
 * DirectoryWalkerWin32.h
 * 
 * 
*/

template< typename ARG_TYPE >
struct DirWalkerBase
{
	// if returns true, then the given directory will be visited
	virtual bool edEnterFolder( PCHARS folder, ARG_TYPE args )
	{
		DBGOUT("Entering folder: '%s'\n", folder);
		return true;
	}
	virtual void edLeaveFolder( PCHARS folder, ARG_TYPE args )
	{
		DBGOUT("Leaving folder: '%s'\n", folder);
	}
	// accepts file name without path
	virtual void edProcessFile( PCHARS fileName, ARG_TYPE args )
	{
		DBGOUT("Processing file: '%s'\n", fileName);
	}
};

template< class WALKER, typename ARG_TYPE >
void Win32_ProcessDirectoryRecursive( PCHARS rootPath, WALKER & walker, ARG_TYPE userData )
{
	AssertPtr(rootPath);

	//const bool bVisitFolders = false;

	mxPathName	fullPath( rootPath );

	ANSICHAR	tmp[ FS_MAX_PATH ];
	MX_SPRINTF_ANSI( tmp, "%s*.*", fullPath.ToChars() );

	WIN32_FIND_DATAA	findFileData;
	const HANDLE fh = ::FindFirstFileA( tmp, &findFileData );

	if( fh == INVALID_HANDLE_VALUE ) {
		// commented out because this line gets executed when the folder is empty
		//mxErrf("FindFirstFile('%s') failed\n",tmp);
		return;
	}

	do
	{
		// skip self
		if(mxStrEquAnsi( findFileData.cFileName, (".") ))
		{
			continue;
		}
		if(mxStrEquAnsi( findFileData.cFileName, ("..") ))
		{
			continue;
		}

		// skip hidden file
		if( findFileData.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN )
		{
			continue;
		}

		// if this is directory
		if( findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
		{
			StackString	childPath( fullPath );
			childPath.Append( findFileData.cFileName );
			F_NormalizePath( childPath );

			if( walker.edEnterFolder( childPath.ToChars(), userData ) )
			{
				Win32_ProcessDirectoryRecursive( childPath.ToChars(), walker, userData );
			}
			walker.edLeaveFolder( childPath.ToChars(), userData );
		}
		else
		{
			if(0)
			{
				StackString	fullFilePath( fullPath.ToChars(), fullPath.Length() );
				F_NormalizePath( fullFilePath );
				fullFilePath.Append( findFileData.cFileName );

				walker.edProcessFile( fullFilePath.ToChars(), userData );
			}
			else
			{
				walker.edProcessFile( findFileData.cFileName, userData );
			}
		}
	}
	while( ::FindNextFileA( fh , &findFileData ) != 0 );
}


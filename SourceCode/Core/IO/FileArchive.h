/*
=============================================================================
	File:	FileArchive.h
	Desc:	Base class of file archives. Subclasses of this class implemented support
			for specific archive formats, like zip.
	Todo:	integer file ids instead of strings
=============================================================================
*/

#pragma once

#include <Core/Object.h>
#include <Core/Serialization.h>
//#include <Core/IO/FileId.h>

mxNAMESPACE_BEGIN

/*
-----------------------------------------------------------------------------
	FileArchive

	Base class of file archives. Subclasses of this class implemented support
	for specific archive formats, like zip.
-----------------------------------------------------------------------------
*/
class FileArchive
	: public AEditableRefCounted
{
public:
	mxDECLARE_CLASS_ALLOCATOR(EMemHeap::HeapStreaming,FileArchive);
	mxDECLARE_ABSTRACT_CLASS(FileArchive,AEditableRefCounted);

	typedef TRefPtr<FileArchive> Ref;

	virtual const char* GetURI() const = 0;

	virtual bool IsOpen() const = 0;

	/// Opens a file for synchronous operations.
	/// NOTE: A nullptr is returned if no device for opening the file could be found.

	virtual mxDataStream* OpenReader( const char* filePath ) = 0;
	virtual mxDataStream* OpenWriter( const char* filePath ) = 0;

public:
	virtual void Serialize( mxArchive& archive );

public:
	const char* edToChars( UINT column ) const;
	AEditable* edGetParent();

protected:
	FileArchive();
	virtual ~FileArchive();
};


mxNAMESPACE_END

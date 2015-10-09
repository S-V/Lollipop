/*
=============================================================================
	File:	IOSystem.h
	Desc:	Global file system.
	Note:
	
		Requirements:
	
		Fast Access Times - The VFS should provide performance comparable with the native Win32 API Functions.
	By using few, huge files instead of lots of small ones, we could even improve the performance further.

		Few Archives Instead Of Lots Of small Files - The VFS should be able to handle so-called Archives,
	i.e. files that contain lots of other files. This comes in handy, because if we use only one file,
	we need only one File Handle, and so we won't exhaust too many System Resources.
	Another advantage is - especially for professional Game Projects - that it's not so easy to parse our file format.

		Debugging Features - Although Archives are a big pro for us, they are not the perfect solution
	for the development and debugging process since you can't quickly access files.
	Imagine an image isn't display correctly on the screen and you want to check if the artist didn't save the file properly or if your code doesn't work: You would have to decompress the Archive File, look at the Image, perhaps fix the Image and compress everything again.

		Pluggable Encryption and Compression - Pluggable Encryption and Compression (PEC)
	is a really cool feature of our VFS. The VFS user can not only choose between several PEC-Plugins,
	he can even select more than one and determine the order of execution.
	For instance, you could create an Archive File for the BMP-Images with the PEC-Plugin "RLE-Compression"
	and another Archive File for TXT-Files with the PEC-Plugins "ZIP-Compression" and "RSA-Encoding".

		Security - We will store a MD5 key within each Archive file so that manipulation of the Archive Files
	will be detected instantly.

		Multiple Root Paths - This is important especially for professional games, because you could
	for instance set the Root Paths to both the Installation Directory of the Game and the CD-ROM/DVD.


	from Nebula 3:
	The central server object of the IO subsystem offers the following
	services:

	* associate stream classes with URI schemes
	* create the right stream object for a given URI
	* transparent (ZIP) archive support
	* path assign management
	* global file system manipulation and query methods

=============================================================================
*/

#pragma once

#include <Core/IO/FileId.h>
#include <Core/IO/FileArchive.h>
#include <Core/Object.h>
#include <Core/Serialization.h>

mxNAMESPACE_BEGIN

/*
-----------------------------------------------------------------------------
	IOSystem
-----------------------------------------------------------------------------
*/
class IOSystem
	: public TRefCountedObjectList< FileArchive >
	, SingleInstance< IOSystem >
{
public:
	mxDECLARE_CLASS_ALLOCATOR(EMemHeap::DefaultHeap,IOSystem);

	IOSystem();

	void Mount( FileArchive* fileSystem );
	void Unmount( FileArchive* fileSystem );

	/// Opens a file for synchronous operations.
	/// NOTE: A nullptr is returned if no device for opening the file could be found.
	mxOPTIMIZE("reduce memory allocations");

	mxDataStream* OpenReader( const char* filePath );
	mxDataStream* OpenWriter( const char* filePath );

	FileArchive* FindArchive( const char* pURL );

public:
	void Serialize( mxArchive& archive );

public:
	virtual const char* edToChars( UINT column ) const override;
	virtual AEditable* edGetParent() override;
};

mxNAMESPACE_END


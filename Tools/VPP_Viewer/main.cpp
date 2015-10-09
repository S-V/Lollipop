#include "stdafx.h"
#pragma hdrstop



#include <Base/Util/LogUtil.h>
#include <Base/Util/FourCC.h>
#include <Core/Serialization.h>
#include <Core/Util/Timer.h>


#include <EditorSupport/Serialization/TextSerializer.h>
#include <EditorSupport/ContentPipeline/DevAssetManager.h>
#pragma comment( lib, "EditorSupport.lib" )


//#include "model.h"
//#include "level.h"
//
//

//const char* SRC_FILE = "D:/games/Red Faction/audio.vpp";
//const char* DEST_DIR = "R:/audio";

const char* SRC_FILE = "D:/games/Red Faction/user_maps/multi/ctf-wl-assault.vpp";
const char* DEST_DIR = "R:/temp";

const char* VPP_FILE_EXT = ".vpp";

// used for filenames, etc.
//typedef char* PCHARS;
typedef const char* PCHARS;




// 2048 bytes
enum { VPP_BLOCK_ALIGNMENT = 0x800 };

FORCEINLINE SizeT VPP_CalcAlignedOffset( SizeT unaligned )//, SizeT alignment )
{
	SizeT alignedOffset = 0;
	alignedOffset = (unaligned + (VPP_BLOCK_ALIGNMENT-1)) & (~(VPP_BLOCK_ALIGNMENT-1));
	return alignedOffset;
}

//MX_REFACTOR("");
//FORCEINLINE void VPP_SeekFile( FileReader & file )
//{
//	const SizeT currOffset = file.Tell();
//	const SizeT alignedOffset = VPP_CalcAlignedOffset( currOffset );
//
//	file.Seek( alignedOffset );
//}

#pragma pack (push,1)
struct VPP_File_Header
{
	union
	{
		struct
		{
			FourCC	fourCC;			//! always CE 0A 89 51
			U4		version;		//? always 01 00 00 00
			U2		numEntries;		//! number of entries
			U2		pad;			//? always 0
			U4		totalSize;		//? total file size, in bytes
		};
		struct
		{
			BYTE	d[ 0x800 ];	// 2048 bytes
		};
	};
};
#pragma pack (pop)
MX_DECLARE_POD_TYPE(VPP_File_Header);



#pragma pack (push,1)
struct ResouceFileName
{
	// pure file name with extension padded with nulls
	ANSICHAR	d[ 0x40 ];	// 64 bytes

public:
	inline U4 GetSize() const
	{
		// the last four bytes are file size
		const UINT offset = 60;
		const BYTE* pSize = (BYTE*)this + offset;
		return *(U4*)pSize;
	}
	inline PCHARS ToChars() const
	{
		return d;
	}
};
#pragma pack (pop)
MX_DECLARE_POD_TYPE(ResouceFileName);




struct VPP_File
{
	VPP_File_Header		m_header;

	TList< ResouceFileName >	m_names;

	TList< BYTE >		m_buffer;

public:
	VPP_File()
	{
		ZERO_OUT( m_header );
	}
	VPP_File( FileReader& file )
	{
		DataReader	reader( file );

		// read header (2048 bytes)
		//file.Skip( sizeof VPP_File_Header );
		reader & m_header;

		Assert( file.Tell() == sizeof VPP_File_Header );

		// read file names

		const UINT numEntries = m_header.numEntries;

		DBGOUT("%u entries in VPP file.\n",numEntries);

		m_names.Reserve( numEntries );
		for( UINT iEntry = 0; iEntry < numEntries; iEntry++ )
		{
			ResouceFileName & newResourceName = m_names.Add();
			reader & newResourceName;

			DBGOUT("Read string: %s (%u bytes)\n",
				newResourceName.ToChars(),newResourceName.GetSize());
		}

#if 1
		for( UINT iEntry = 0; iEntry < numEntries; iEntry++ )
		{
			const ResouceFileName & resourceName = m_names[ iEntry ];

			const SizeT resourceSize = resourceName.GetSize();

			m_buffer.Reserve( resourceSize );

			// Calculate aligned offset of the resource chunk.

			{
				const SizeT currOffset = file.Tell();
				const SizeT alignedOffset = VPP_CalcAlignedOffset( currOffset );

				if( mxStrEquAnsi( resourceName.ToChars(), "paper1.v3m" ) )
				{
					(void)alignedOffset;
					MX_DEBUG_BREAK;
				}
			}
		}
#endif
	}

#if 0
	SizeT GetOffsetOfEntry( PCHARS fileName )
	{
		const UINT numEntries = m_header.numEntries;

		SizeT accumOffset = 2048;
		for( UINT iEntry = 0; iEntry < numEntries; iEntry++ )
		{
			const ResouceFileName & resourceName = m_names[ iEntry ];

			const SizeT resourceSize = resourceName.GetSize();

			const SizeT currOffset = accumOffset;
			const SizeT alignedOffset = VPP_CalcAlignedOffset( currOffset );
Assert(alignedOffset != 0);
			// Calculate aligned offset of the resource chunk.

			if( mxStrEquAnsi( resourceName.ToChars(), fileName ) )
			{
				return alignedOffset;
			}

			accumOffset += alignedOffset;
		}

		MX_DEBUG_BREAK;
		return 0;
	}
#endif
	void Unpack( FileReader& file, const char* destFolder )
	{
		DataReader	reader( file );

		// read header (2048 bytes)
		//file.Skip( sizeof VPP_File_Header );
		reader & m_header;

		Assert( file.Tell() == sizeof VPP_File_Header );

		// read file names

		const UINT numEntries = m_header.numEntries;

		DBGOUT("%u entries in VPP file.\n",numEntries);

		m_names.Reserve( numEntries );
		for( UINT iEntry = 0; iEntry < numEntries; iEntry++ )
		{
			ResouceFileName & newResourceName = m_names.Add();
			reader & newResourceName;

			DBGOUT("Read string: %s (%u bytes)\n",
				newResourceName.ToChars(),newResourceName.GetSize());
		}


		for( UINT iEntry = 0; iEntry < numEntries; iEntry++ )
		{
			const ResouceFileName & resourceName = m_names[ iEntry ];

			String	destFileName( destFolder );
			destFileName += resourceName.ToChars();

			const SizeT resourceSize = resourceName.GetSize();

			m_buffer.Reserve( resourceSize );

			// Calculate aligned offset of the resource chunk.

			//VPP_SeekFile( file );
			{
				const SizeT currOffset = file.Tell();
				const SizeT alignedOffset = VPP_CalcAlignedOffset( currOffset );

				file.Seek( alignedOffset );
			}

			FileWriter	destFile( destFileName.ToChars() );
			file.Read( m_buffer.ToPtr(), resourceSize );
			destFile.Write( m_buffer.ToPtr(), resourceSize );
		}
	}
};

bool ExtractData( const char* filePath, const char* destPath )
{
	String	srcFileName( filePath );
	String	destFolderName( destPath );

	FileReader	srcFile( srcFileName.ToChars() );
	if( !srcFile.IsOpen() ) {
		return false;
	}

	if( !FS_EnsurePathExists( destPath ) ) {
		return false;
	}

	VPP_File	vppFile;
	vppFile.Unpack( srcFile, destFolderName );

	return true;
}







//struct TransformComp
//{
//	float4x4	mat;
//	TStackString<16>	name;
//};
//
//struct TransformCompUtil
//{
//	static void OnObjectAdded( TransformComp & o, const ObjectHandle newObjID )
//	{
//
//	}
//	static void OnObjectRemoved( TransformComp & o )
//	{
//
//	}
//	static void OnObjectMoved( TransformComp & o )
//	{
//		//
//	}
//};






int mxAppMain()
{
	SetupBaseUtil	setupBase;

	FileLogUtil		fileLog;
	SetupCoreUtil	setupCore;




#if 0
	ExtractData(
		"D:/games/Red Faction [2001]/user_maps/multi/dm-OfficeLobbyFinal.vpp"
		,"R:/temp/"
		);
#endif



#if 0
	FileReader	file(
		"D:/games/Red Faction [2001]/meshes.vpp"
	);

	VPP_File	vppFile( file );

//	SizeT paper1_v3m_offset = vppFile.GetOffsetOfEntry("paper1.v3m");

	MX_DEBUG_BREAK;

#endif

#if 0
	FileReader	file(
		//"D:/dev/RF_PC/data/meshes/paper1.v3m"
		//"D:/dev/RF_PC/data/meshes/Com_CtrlPanel01_Corpse.v3m"
		//"D:/dev/RF_PC/data/meshes/Fighter02.v3m" //<- aabb offset should be less by 2 or 4 floats?

		"D:/dev/RF_PC/data/meshes/paper1.v3m"
	);

	const SizeT fileSize = file.GetSize();

	TList<BYTE>	buffer;
	buffer.SetNum(fileSize);

	file.Read(buffer.ToPtr(),fileSize);



	IndexData	indexData;
	FindIndices_16_Bit(buffer.ToPtr(),fileSize,indexData);



	RF_Mesh* pMesh = (RF_Mesh*)buffer.ToPtr();

	UINT s = sizeof RF_Vertex;
	UINT off = OFFSET_OF(RF_Mesh,verts);

	(void)s;(void)pMesh;
	MX_DEBUG_BREAK;

#endif


#if 0
	const char* fileName =
		//"D:/dev/RF_PC/data/meshes/paper1.v3m"
		//"D:/dev/RF_PC/data/meshes/Com_CtrlPanel01_Corpse.v3m"
		//"D:/dev/RF_PC/data/meshes/Fighter02.v3m" //<- aabb offset should be less by 2 or 4 floats?

		"D:/dev/RF_PC/data/meshes/bucket1.v3m"
		//"D:/dev/RF_PC/data/meshes/minifridge_corpse.v3m"
		//"D:/dev/RF_PC/data/meshes/powerup_SniperClip.v3m"
		//"D:/dev/RF_PC/data/meshes/veg_dangling1.v3m"
		//"D:/dev/RF_PC/data/meshes/Disk.v3m"
		//"D:/dev/RF_PC/data/meshes/Weapon_RocketLauncher.v3m"
		//"D:/dev/RF_PC/data/meshes/minifridge1.v3m"
		//"D:/dev/RF_PC/data/meshes/meds.v3m"
		;

	FileReader	file( fileName );

	Assert(file.IsOpen());

	v3m_model	model;

	DEVOUT("Loading model '%s'.\n", fileName);

	model.Load( file );

	model.Dbg_Print();
#endif



#if 0
	FileReader	file(
		"D:/dev/RF_PC/data/multiplayer_maps/ctf-wl-assault/ctf-wl-assault.rfl"
		//"D:/dev/RF_PC/data/levels1/L1S1.rfl"
		//"D:/dev/RF_PC/data/levels3/L18S3.rfl"
	);

	Assert(file.IsOpen());

	Level	level;

	level.Load( file );
#endif



#if 0
	FileReader	file(
		//"D:/dev/RF_PC/data/meshes/paper1.v3m"
		//"D:/dev/RF_PC/data/meshes/Com_CtrlPanel01_Corpse.v3m"
		//"D:/dev/RF_PC/data/meshes/Fighter02.v3m" //<- aabb offset should be less by 2 or 4 floats?

		"D:/dev/RF_PC/data/meshes/paper1.v3m"
	);

	const SizeT fileSize = file.GetSize();

	//file.Read(buffer.ToPtr(),fileSize);


#endif







	//if (0)
	//{
	//	TDynHandleMgr<TransformComp,TransformCompUtil>	dynHndlMgr;
	//
	//	bool bSerialize = true;
	//
	//	if( bSerialize )
	//	{
	//		TransformComp&	t0 = *dynHndlMgr.Add();	t0.name = "Hi there0";
	//		TransformComp&	t1 = *dynHndlMgr.Add();	t0.name = "t1";
	//		TransformComp&	t2 = *dynHndlMgr.Add();	t0.name = "t2";
	//		for (int i=0; i< 32; i++)
	//		{
	//			TransformComp& rTc = *dynHndlMgr.Add();
	//			rTc.name.Format("tt[%d]",i);
	//		}
	//	}
	//	else
	//	{
	//		//for (int i=0; i < dynHndlMgr.Num(); i++)
	//		//{
	//		//	TransformComp& rTc = *dynHndlMgr.Add();
	//		//	rTc.name.Format("tt[%d]",i);
	//		//}
	//	}
	//}



	//MX_DEBUG_BREAK;

	return 0;
}

MX_APPLICATION_ENTRY_POINT

BYTE gBuf[mxMEBIBYTE];


#pragma once

class mxAssetReferenceType;

namespace BinarySerialization
{


	//-----------------------------------------------------------------------

	void Deserialize(
		AStreamReader& stream,
		const mxType& typeInfo,	// type of serialized object
		void* objAddr,	// pointer to the start of the memory block
		const FieldFlags flags = Field_DefaultFlags
		);

	void Deserialize_Struct(
		AStreamReader &stream,
		const mxStruct& structInfo,
		void *rawMem	
		);

	void Deserialize_Class(
		AStreamReader& stream,
		const mxClass& classInfo,
		void *rawMem
	);

	void Deserialize_Parent_Class(
		AStreamReader& stream,
		const mxClass& parentClass,
		void *rawMem
		);

	void Deserialize_Class_Members(
		AStreamReader& stream,
		const mxClassMembers& members,
		void *rawMem
		);

	void Deserialize_Asset_Reference(
		AStreamReader& stream,
		const mxAssetReferenceType& handleType,
		void *pointerAddress,
		const FieldFlags flags = Field_DefaultFlags
		);

	void Deserialize_Array(
		AStreamReader& stream,
		const mxArrayType& arrayInfo,
		void *objAddr
		);

	void Deserialize_Pointer(
		AStreamReader &stream,
		const mxPointerType& pointerType,
		void* pointerAddress
		);

	/*
	-----------------------------------------------------------------------------
		mxArchiveHeader
	-----------------------------------------------------------------------------
	*/
	struct mxArchiveHeader
	{
		mxSessionInfo	session;

		mxArchiveHeader()
		{
		}
		bool Matches( const mxArchiveHeader& other ) const
		{
			return mxSessionInfo::AreCompatible( session, other.session );
		}
	};
	mxDECLARE_POD_TYPE(mxArchiveHeader);

	//---------------------------------------------------------------------------


	enum { DEFAULT_HASH_TABLE_SIZE = 64 };

	//---------------------------------------------------------------------------



}//namespace BinarySerialization

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//

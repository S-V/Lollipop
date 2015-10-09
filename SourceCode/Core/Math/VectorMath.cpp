/*
=============================================================================
	File:	VectorMath.cpp
	Desc:
=============================================================================
*/

#include <Core_PCH.h>
#pragma hdrstop
#include <Core.h>

#include <Core/VectorMath.h>

mxBEGIN_BASIC_STRUCT(XMFLOAT2)
	mxMEMBER_FIELD(x)
	mxMEMBER_FIELD(y)
mxEND_REFLECTION

mxBEGIN_BASIC_STRUCT(XMFLOAT3)
	mxMEMBER_FIELD(x)
	mxMEMBER_FIELD(y)
	mxMEMBER_FIELD(z)
mxEND_REFLECTION

mxBEGIN_BASIC_STRUCT(XMFLOAT4)
	mxMEMBER_FIELD(x)
	mxMEMBER_FIELD(y)
	mxMEMBER_FIELD(z)
	mxMEMBER_FIELD(w)
mxEND_REFLECTION

mxBEGIN_BASIC_STRUCT(float4x4)
	mxMEMBER_FIELD(r[0])
	mxMEMBER_FIELD(r[1])
	mxMEMBER_FIELD(r[2])
	mxMEMBER_FIELD(r[3])
mxEND_REFLECTION

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//

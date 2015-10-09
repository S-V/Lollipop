/*
=============================================================================
	File:	SeparableSSS.h
	Desc:	Screen-Space Subsurface Scattering
=============================================================================
*/
#pragma once


class SeparableSSS
{
public:
	SeparableSSS();
	~SeparableSSS();

	void Go( const RenderTarget& mainRT, const DepthStencil& mainDS );

};//SeparableSSS

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//

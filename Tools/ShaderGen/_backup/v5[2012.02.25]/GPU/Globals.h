/*
=============================================================================
	File:	Globals.h
	Desc:	Global renderer constants (GPU constants manager).
=============================================================================
*/
#pragma once

#ifndef __MX_GLOBAL_SHADER_VARS_H__
#define __MX_GLOBAL_SHADER_VARS_H__

class rxViewport;
class rxSceneContext;

namespace GPU
{

	void UpdatePerFrameConstants( const rxViewport& viewport, FLOAT globalTimeInSeconds );
	void UpdatePerViewConstants( const rxSceneContext& sceneContext );
	void UpdatePerObjectConstants( mat4_carg worldMatrix );

}//namespace GPU



#endif // !__MX_GLOBAL_SHADER_VARS_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//

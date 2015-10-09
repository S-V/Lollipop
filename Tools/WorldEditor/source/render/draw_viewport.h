#pragma once

class EdSceneViewport;
class World;

namespace Rendering
{
	// this function must be called inside BeginScene()/EndScene()
	//
	void Draw_Viewport( const EdSceneViewport& viewport, World& world );

}//namespace Rendering


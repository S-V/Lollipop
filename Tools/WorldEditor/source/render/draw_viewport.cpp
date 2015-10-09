#include "stdafx.h"

#include <Engine/Worlds.h>

#include <Renderer/GPU/Main.hxx>
#include <Renderer/Util/BatchRenderer.h>
#include <Renderer/Core/Font.h>

#include "editor_system.h"
#include "editors/world_editor.h"
#include "render/viewports.h"
#include "render/draw_viewport.h"

namespace Rendering
{
	static
	void F_Draw_Debug_Info( const rxView& view )
	{
		BatchRenderer & batchRenderer = gRenderer.GetDrawHelper();

		DbgFont_BeginRendering();

		const FLOAT deltaSeconds = (DOUBLE)gfxBEStats.lastFrameRenderTime * 1e-6f;

		mxUNDONE;
		const FLOAT fps = 1.0f
			//fpsCounter.CalculateFPS(deltaSeconds) / 5.0f
			;

		Dbg_DrawBackEndStats( fps, 10, 10 );
		DrawCameraStats( view, 1, 100 );

		DbgFont_EndRendering();
	}

	static void F_Invoke_Callbacks_Draw_Background( BatchRenderer & batchRenderer, const EdDrawContext& context )
	{
		rxGPU_MARKER(Draw_Backround);
		batchRenderer.SetState(GPU::SolidNoCullNoZTestNoClipNoBlend);
		EdSystem::Get().Event_DrawBackground( context );
		batchRenderer.Flush();
	}
	static void F_Invoke_Callbacks_Draw_Normal( BatchRenderer & batchRenderer, const EdDrawContext& context )
	{
		rxGPU_MARKER(Draw_Normal);
		batchRenderer.SetState(GPU::Default);
		EdSystem::Get().Event_Draw( context );
		batchRenderer.Flush();
	}
	static void F_Invoke_Callbacks_Draw_Foreground( BatchRenderer & batchRenderer, const EdDrawContext& context )
	{
		rxGPU_MARKER(Draw_Foreground);
		batchRenderer.SetState(GPU::SolidNoCullNoZTestNoClipNoBlend);
		EdSystem::Get().Event_DrawForeground( context );
		batchRenderer.Flush();
	}

	static void F_Render_Hit_Proxies( const EdSceneViewport& viewport, const rxSceneContext& sceneContext, World& world )
	{
		rxGPU_MARKER(Draw_Hit_Proxies);

		HitTesting & hitTesting = GetHitTesting();
		BatchRenderer & batchRenderer = gRenderer.GetDrawHelper();


		hitTesting.Reset();
		hitTesting.Begin( viewport.GetRenderViewport() );
		batchRenderer.SetHitTesting( &hitTesting );


		world.m_editor->Draw_Hit_Proxies( viewport, sceneContext );

		{
			EdDrawContext	drawContext( viewport, sceneContext, batchRenderer, hitTesting );
			
			F_Invoke_Callbacks_Draw_Background( batchRenderer, drawContext );

			F_Invoke_Callbacks_Draw_Normal( batchRenderer, drawContext );

			F_Invoke_Callbacks_Draw_Foreground( batchRenderer, drawContext );
		}

		batchRenderer.SetHitTesting( nil );
		hitTesting.End();
	}



	static void F_Draw_Editor_Stuff( const EdSceneViewport& viewport, const rxSceneContext& sceneContext, World& world )
	{
		rxGPU_MARKER(Draw_Editor_Stuff);


		ID3D11DeviceContext* pD3DContext = GetD3DDeviceContext();

		BatchRenderer & batchRenderer = gRenderer.GetDrawHelper();

		batchRenderer.SetTransform( sceneContext.viewProjectionMatrix );


		// restore main render target and depth-stencil

		viewport.GetRenderViewport().Set( pD3DContext );

		{
			EdDrawContext	drawContext( viewport, sceneContext, batchRenderer, AHitTesting::NullObject );

			F_Invoke_Callbacks_Draw_Background( batchRenderer, drawContext );

			world.m_editor->Draw_Editor_Stuff( viewport, sceneContext );

			// Draw editor grid and coordinate axes.
			batchRenderer.SetState(GPU::Default);
			batchRenderer.DrawInfiniteGrid( sceneContext.GetOrigin(), 0.0f, FColor::GRAY );
			batchRenderer.DrawAxes();
			batchRenderer.Flush();

			F_Invoke_Callbacks_Draw_Normal( batchRenderer, drawContext );

			F_Invoke_Callbacks_Draw_Foreground( batchRenderer, drawContext );
		}

		batchRenderer.Flush();
	}

	// this function must be called inside BeginScene()/EndScene()
	//
	void Draw_Viewport( const EdSceneViewport& viewport, World& world )
	{
		rxRenderWorld& renderWorld = world.GetRenderWorld();

		const rxView& view = viewport.GetView();

		rxSceneContext	context( view, renderWorld );



		BatchRenderer & batchRenderer = gRenderer.GetDrawHelper();
	
		batchRenderer.SetTransform( context.viewProjectionMatrix );


		// first render hit proxies into special off-screen render target
		// (uses the main depth-stencil surface)
		{
			F_Render_Hit_Proxies( viewport, context, world );
		}


		// clear the main depth-buffer

		ID3D11DeviceContext* pD3DContext = GetD3DDeviceContext();

		const DepthStencil & mainDS = viewport.GetRenderViewport().mainDS;
		pD3DContext->ClearDepthStencilView( mainDS.pDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f,	0 );



		// render the main scene

		gRenderer.DrawScene( context );


		// render editor stuff and debug info

		F_Draw_Editor_Stuff( viewport, context, world );

		F_Draw_Debug_Info( view );
	}

}//namespace Rendering

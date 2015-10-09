#include "stdafx.h"
#include "content_manager.h"
#include <QtGui/QApplication>

//TEST
#include <EditorSupport/EditorSupport.h>
#include <EditorSupport/AssetPipeline/AssetProcessor.h>
#include <EditorSupport/AssetPipeline/DevAssetManager.h>
#include <EditorSupport/Serialization/TextSerializer.h>
#pragma comment( lib, "EditorSupport.lib" )

#include <Graphics/Graphics_DX11.h>
#pragma comment( lib, "Graphics.lib" )

#include <Renderer/Materials/Phong.h>
#include <Renderer/Materials/SubsurfaceScattering.h>
#pragma comment( lib, "Renderer.lib" )




int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	SetupCoreUtil	setupCore;
	InitEditorUtil	setupEditor;
	AppSettings		settings;



	Material_SSS::StaticClass();



	ContentManager w;

	w.show();

	return a.exec();
}

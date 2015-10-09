#include "stdafx.h"
#include "material_editor.h"
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
#pragma comment( lib, "Renderer.lib" )




int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	SetupBaseUtil	setupBaseSys;
	SetupCoreUtil	setupCoreSys;

	{
		PhongMaterial	testMaterial;
		FileWriter		file("R:/test.json");
		
		SObjectPtr		wrapper;
		wrapper.o = &testMaterial;

		TextObjectWriter	serializer;
		
		//serializer.SaveObject( wrapper );
		serializer.SaveObject( testMaterial );

		serializer.WriteAllToStream( file );

		mxDEBUG_BREAK;
	}


	InitEditorUtil	setupEditor;
	AppSettings		settings;


	MaterialEditor w;

	w.show();

	return a.exec();
}

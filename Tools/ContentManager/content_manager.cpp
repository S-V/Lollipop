#include "stdafx.h"

#include <EditorSupport/EditorSupport.h>

#include <QtSupport/qt_common.h>
#include <QtSupport/property_editor.h>

#include "content_manager.h"
#include "create_asset_db_dialog.h"

static const char* APP_SETTINGS_FILE_NAME = "ContentManager.cfg";
static const char* APP_LAYOUT_FILE_NAME = "ContentManager.layout";


/*
-----------------------------------------------------------------------------
	AppSettings
-----------------------------------------------------------------------------
*/
AppSettings::AppSettings()
{
	pathToContentDb = "";

	pathToSrcAssets = "0SrcAssets";
	pathToIntAssets = "0IntAssets";
	pathToBinAssets = "0BinAssets";

	{
		FileReader	file( APP_SETTINGS_FILE_NAME );
		if( file.IsOpen() )
		{
			TextReader	serializer( file );
			this->Serialize( serializer );
		}
	}
}

AppSettings::~AppSettings()
{
	{
		FileWriter	file( APP_SETTINGS_FILE_NAME );
		if( file.IsOpen() )
		{
			TextWriter	serializer;
			this->Serialize( serializer );

			serializer.WriteAllToStream( file );
		}
	}
}

void AppSettings::Serialize( ATextSerializer & serializer )
{
	serializer.Serialize_String( "pathToAssetDb", pathToContentDb );

	serializer.Serialize_String( "pathToSrcAssets", pathToSrcAssets );
	serializer.Serialize_String( "pathToIntAssets", pathToIntAssets );
	serializer.Serialize_String( "pathToBinAssets", pathToBinAssets );
}

void AppSettings::NormalizePaths()
{
	F_NormalizePath( pathToContentDb );

	F_NormalizePath( pathToSrcAssets );
	F_NormalizePath( pathToIntAssets );
	F_NormalizePath( pathToBinAssets );
}

/*
-----------------------------------------------------------------------------
	ContentManager
-----------------------------------------------------------------------------
*/
ContentManager::ContentManager(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags)
	, m_recentFiles( *this )
{
	ui.setupUi(this);
	m_recentFiles.CreateMenus( ui.menuFile );

	this->addDockWidget( Qt::DockWidgetArea::BottomDockWidgetArea, &m_logWidget );

	// Read layout
	{
		QFile		file( APP_LAYOUT_FILE_NAME );
		file.open(QIODevice::ReadOnly);
		if( file.isOpen() )
		{
			QDataStream	stream( &file );
			SerializeMainWindow_GeometryAndState( stream, this );
		}
	}

	ui.actionRebuild->setEnabled(false);

	connect(
		ui.actionNew, SIGNAL(triggered()),
		this, SLOT(slot_CreateNewAssetDatabase())
		);
	connect(
		ui.actionOpen, SIGNAL(triggered()),
		this, SLOT(slot_OpenExistingAssetDatabase())
		);
	connect(
		ui.actionRebuild, SIGNAL(triggered()),
		this, SLOT(slot_RebuildCurrentAssetDatabase())
		);
	connect(
		ui.actionExit, SIGNAL(triggered()),
		this, SLOT(close())
		);
}

ContentManager::~ContentManager()
{
	if( this->IsOpen() )
	{
		m_assetDb.SaveToFile( m_assetDbFilePath );
	}

	// Save layout
	{
		QFile		file( APP_LAYOUT_FILE_NAME );
		file.open(QIODevice::WriteOnly);
		if( file.isOpen() )
		{
			QDataStream	stream( &file );
			SerializeMainWindow_GeometryAndState( stream, this );
		}
	}
}

void ContentManager::slot_CreateNewAssetDatabase()
{
	QFileDialog dialog( this, "Create a new Asset Database - Specify Destination File" );
	{
		dialog.setViewMode(QFileDialog::Detail);
		dialog.setFileMode(QFileDialog::AnyFile);
		dialog.setAcceptMode(QFileDialog::AcceptSave);
	}
	dialog.setFilter("Resource Database Files (*.rdb)");	// ASSET_DB_FILE_EXTENSION
	dialog.setDirectory(AppSettings::Get().pathToContentDb.ToChars());
	dialog.selectFile("_file_index.rdb");

	if( dialog.exec() )
	{
		const QString directory = dialog.directory().path();
		if( directory.isEmpty() ) {
			return;
		}

		m_assetDbFilePath = QtSupport::F_Get_Selected_File_Name( dialog );

		Create_Asset_Db_Dialog	createAssetDbDialog(this);
		if( createAssetDbDialog.exec() )
		{
			DevAssetManager::SCreateArgs	cInfo;

			cInfo.pathToSrcAssets = createAssetDbDialog.GetPathToSrcAssets();
			cInfo.pathToIntAssets = createAssetDbDialog.GetPathToIntAssets();
			cInfo.pathToBinAssets = createAssetDbDialog.GetPathToBinAssets();


			m_logWidget.ClearLog();


			// Create a new resource database.

			m_assetDb.CreateNew( cInfo );
			m_assetDb.SaveToFile( m_assetDbFilePath );

			gCore.resources->SetContentDatabase( &m_assetDb );

			ui.actionRebuild->setEnabled(true);
		}

		AppSettings::Get().pathToContentDb = directory.toAscii().data();
		F_NormalizePath( AppSettings::Get().pathToContentDb );
	}
}

//@todo: use QtSupport::F_OpenDialog_Select_Asset_DB_File ?
void ContentManager::slot_OpenExistingAssetDatabase()
{
	QFileDialog dialog( this, "Open an existing Asset Database - Select File" );
	{
		dialog.setViewMode(QFileDialog::Detail);
		dialog.setFileMode(QFileDialog::ExistingFile);
		dialog.setAcceptMode(QFileDialog::AcceptOpen);
	}
	dialog.setFilter("Resource Database Files (*.rdb)");	// ASSET_DB_FILE_EXTENSION
	dialog.setDirectory(AppSettings::Get().pathToContentDb.ToChars());
	dialog.selectFile("_file_index.rdb");

	if( dialog.exec() )
	{
		const String	assetDbFileName = QtSupport::F_Get_Selected_File_Name( dialog );
		if( assetDbFileName.IsEmpty() ) {
			return;
		}

		this->OpenAssetDatabase( assetDbFileName );
	}
}

bool ContentManager::OpenAssetDatabase( const char* filePath )
{
	m_logWidget.ClearLog();

	const bool bOk = m_assetDb.OpenExisting( filePath );
	if( bOk )
	{
		m_assetDbFilePath = filePath;

		m_recentFiles.Add( filePath );

		gCore.resources->SetContentDatabase( &m_assetDb );

		ui.actionRebuild->setEnabled(true);
	}
	else
	{
		m_assetDbFilePath.Empty();

		m_recentFiles.Remove( filePath );
	}

	return bOk;
}

bool ContentManager::OpenRecentFile( const char* fileName )
{
	return this->OpenAssetDatabase( fileName );
}

void ContentManager::slot_RebuildCurrentAssetDatabase()
{
	Assert( this->IsOpen() );
	m_assetDb.Refresh();
	m_assetDb.SaveToFile( m_assetDbFilePath );
}

bool ContentManager::IsOpen() const
{
	return !m_assetDbFilePath.IsEmpty();
}

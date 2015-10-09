/*
=============================================================================
	File:	project_manager.cpp
	Desc:	Editor project management.
=============================================================================
*/
#include "stdafx.h"

#include <Base/IO/StreamIO.h>
#include <Base/Templates/Containers/HashMap/TMap.h>
#include <Base/Text/Lexer.h>
#include <Base/Util/PathUtils.h>

#include <EditorSupport/EditorSupport.h>
#include <EditorSupport/AssetPipeline/AssetProcessor.h>
#include <EditorSupport/AssetPipeline/DevAssetManager.h>
#include <EditorSupport/Serialization/TextSerializer.h>
#include <EditorSupport/Serialization/JSON/json.h>

#include "app.h"
#include "main_window_frame.h"
#include "resource_browser.h"
#include "world_editor.h"
#include "project_manager.h"

// including dot (e.g. ".proj")
static ConstCharPtr	INI_FILE_EXTENSION(".ini");

static const char* PROJ_LEVEL_FILE_EXTENSION = ".level.json";

//-----------------------------------------------------------------------------
// file path is used to locate .INI and archive files
template< UINT BUF_SIZE >
bool F_Compose_File_Path(PCHARS szProjFilePath, PCHARS fileExtensionWithDot,
						ANSICHAR (&destBuf)[BUF_SIZE])
{
	AssertPtr( szProjFilePath );
	AssertPtr( destBuf );
	StaticAssert( BUF_SIZE > 0 );

	// pure name (e.g. "MyProject")
	ANSICHAR	projName[ 128 ];
	V_FileBase( szProjFilePath, projName, NUMBER_OF(projName) );

	// path relative to exe (e.g. "Projects/MyProject/")
	ANSICHAR	projFilePath[ FS_MAX_PATH ];
	VRET_FALSE_IF_NOT( V_ExtractFilePath( szProjFilePath, projFilePath, NUMBER_OF(projFilePath) ) );

	// path to .INI file relative to exe (e.g. "Projects/MyProject/MyProject.INI")
	ANSICHAR	outFilePath[ FS_MAX_PATH ];
	V_ComposeFileName( projFilePath, projName, outFilePath, NUMBER_OF(outFilePath) );
	V_SetExtension( outFilePath, fileExtensionWithDot, NUMBER_OF(outFilePath) );

	mxStrCpyNAnsi( destBuf, outFilePath, NUMBER_OF(destBuf) );

	return true;
}

//-----------------------------------------------------------------------------

#if 0
static
bool F_Parse_INI_and_Load_Packages( PCHARS szProjFilePath )
{
	AssertPtr( szProjFilePath );

	// Parse .INI file and load resource packages.

	ANSICHAR	iniFilePath[ FS_MAX_PATH ];
	VRET_FALSE_IF_NOT( F_Compose_File_Path( szProjFilePath, iniFilePath, NUMBER_OF(iniFilePath) ) );

	FileReader	iniFileReader( iniFilePath, FileRead_NoErrors );

	if( !iniFileReader.IsOpen() ) {
		mxWarnf("Failed to load config file '%s'\n",iniFilePath);
		return false;
	}

	// 1 KiB
	enum { MAX_INI_SIZE = 1*mxKIBIBYTE};

	const UINT fileSize = iniFileReader.GetSize();

	if( fileSize > MAX_INI_SIZE )
	{
		mxWarnf("Config file '%s' is too big\n",iniFilePath);
		return false;
	}

	ANSICHAR	buffer[ MAX_INI_SIZE ];
	MemZero( buffer, sizeof buffer );
	iniFileReader.Read( buffer, fileSize );

	idLexer		lexer( buffer, fileSize, iniFilePath, LEXFL_ALLOWPATHNAMES|LEXFL_NOSTRINGCONCAT );
	idToken		token;
	while( lexer.ReadToken( &token ) )
	{
		if( token == "[" ) {
			lexer.ExpectAnyToken( &token );
			lexer.ExpectTokenString("]");
			continue;
		}
		if( token.type == TT_STRING ) {
			UNDONE;
			//F_MountExistingPackage( token.ToChars() );
			continue;
		} else {
			lexer.Error("Expected a string, but got '%s'\n",token.ToChars());
		}
		return false;
	}//while (ReadToken)

	return true;
}
#endif




/*
-----------------------------------------------------------------------------
	EdProject
-----------------------------------------------------------------------------
*/
EdProject::EdProject()
{
}
//-----------------------------------------------------------------------------
EdProject::EdProject( const EdProjectCreationSettings& settings )
{
	m_absolutePath = settings.absolutePath.toAscii().data();
	if( !FS_FileOrPathExists( m_absolutePath ) )
	{
		FS_MakeDirectory( m_absolutePath );
	}

	m_fullFilePath = ComposeFullFilePath( settings.absolutePath, settings.name ).toAscii().data();

	ConvertToQString( m_name, settings.name );
}
//-----------------------------------------------------------------------------
EdProject::~EdProject()
{
	EdSystem::Get().Event_ProjectUnloaded();

	gEngine.Reset();
}
//-----------------------------------------------------------------------------
bool EdProject::Save()
{
	mxPutf("Saving project '%s'\n",m_name.ToChars());

	// serialize engine environment
	{
		ANSICHAR	levelFilePath[FS_MAX_PATH];
		F_Compose_File_Path( m_fullFilePath, PROJ_LEVEL_FILE_EXTENSION, levelFilePath );

		FileWriter	fileStream( levelFilePath );
		if( !fileStream.IsOpen() ) {
			return false;
		}

		TextObjectWriter	serializer;

		SEngineSaveArgs		saveArgs;
		saveArgs.serializer = &serializer;

		gEngine.SaveState( saveArgs );

		// write comment
		// @todo: project build options, engine version, etc.
		{
			const UINT numWorlds = Global_Level_Editor::Get().Num();

			String		comment;
			comment.Format("// Level file for \"%s\" (%u world(s))\n//",
				m_name.ToChars(), numWorlds );

			serializer.SetRootComment( comment );
		}

		serializer.WriteAllToStream( fileStream );
	}


	// Serialize project data
	{
		FileWriter	fileStream( m_fullFilePath );
		if( !fileStream.IsOpen() ) {
			return false;
		}

		TextWriter	serializer;
		{
			{
				serializer.Serialize_String( "Path_To_Asset_Database", m_assetDbFilePath );

				if( !m_assetDbFilePath.IsEmpty() )
				{
					m_assetMgr.SaveToFile( m_assetDbFilePath );
				}
			}
			EdSystem::Get().Event_SerializeProjectData( serializer );
		}
		serializer.WriteAllToStream( fileStream );
	}

	return true;
}
//-----------------------------------------------------------------------------
bool EdProject::Load()
{
	mxPutf("Loading project '%s'\n",m_name.ToChars());

	// deserialize project data
	{
		FileReader	fileStream( m_fullFilePath );
		if( !fileStream.IsOpen() ) {
			return false;
		}

		TextObjectReader	serializer( fileStream );
		{
			{
				serializer.Serialize_String( "Path_To_Asset_Database", m_assetDbFilePath );
				if( !m_assetDbFilePath.IsEmpty() )
				{
					this->LoadAssetDatabase( m_assetDbFilePath );
				}
			}
			EdSystem::Get().Event_SerializeProjectData( serializer );
		}		
	}


	// deserialize engine environment
	{
		ANSICHAR	levelFilePath[FS_MAX_PATH];
		F_Compose_File_Path( m_fullFilePath, PROJ_LEVEL_FILE_EXTENSION, levelFilePath );

		FileReader	fileStream( levelFilePath );
		if( !fileStream.IsOpen() ) {
			return false;
		}

		TextObjectReader	serializer( fileStream );

		SEngineLoadArgs		loadArgs;
		loadArgs.serializer = &serializer;
		if( m_assetMgr.IsOpened() ) {
			loadArgs.assetDb = &m_assetMgr;
		}

		gEngine.LoadState( loadArgs );
	}

	EdSystem::Get().Event_EngineLoaded();

	return true;
}

void EdProject::LoadAssetDatabase( const char* assetDbFilePath )
{
	AssertPtr(assetDbFilePath);
	VRET_IF_NIL(assetDbFilePath);

	m_assetDbFilePath = assetDbFilePath;

	m_assetMgr.OpenExisting( m_assetDbFilePath );

	gCore.resources->SetContentDatabase( &m_assetMgr );

	EdSystem::Get().Event_AssetDbChanged( m_assetMgr.GetPathToSrcAssets() );
}

//-----------------------------------------------------------------------------
//static
QString EdProject::ComposeFullFilePath(const QString& absolutePath, const QString& projectName)
{
	QString	fullFilePath( absolutePath );

	if(!fullFilePath.endsWith('/'))
	{
		fullFilePath.append('/');
	}

	fullFilePath.append( projectName );

	if(!fullFilePath.endsWith(ED_PROJECT_FILE_EXTENSION))
	{
		fullFilePath.append(ED_PROJECT_FILE_EXTENSION);
	}

	return fullFilePath;
}

MX_REMOVE_OLD_CODE
//void EdProject::MoundResourceArchives() 
//{
//	F_Parse_INI_and_Load_Packages( this->fullFilePath.toAscii().data() );
//}

/*
-----------------------------------------------------------------------------
	EdProjectManager
-----------------------------------------------------------------------------
*/
EdProjectManager::EdProjectManager()
	: m_recentProjects(*this)
{
}
//-----------------------------------------------------------------------------
EdProjectManager::~EdProjectManager()
{

}
//-----------------------------------------------------------------------------
void EdProjectManager::PreInit()
{
	this->CreateActions();
	this->CreateMenus();
	this->ConnectSigSlots();

#if MX_DEVELOPER
	MX_CONNECT_THIS( EdSystem::Get().Event_AppInitialized, EdProjectManager, OpenMostRecentlyUsedFile );
#endif//MX_DEVELOPER
}
//-----------------------------------------------------------------------------
void EdProjectManager::SetDefaultValues()
{
}
//-----------------------------------------------------------------------------
void EdProjectManager::PostInit()
{
}
//-----------------------------------------------------------------------------
void EdProjectManager::OpenMostRecentlyUsedFile()
{
	if( m_recentProjects.Num() > 0 )
	{
		const String firstFile = m_recentProjects.GetFirst();
		this->OpenRecentFile( firstFile );
	}
}
//-----------------------------------------------------------------------------
void EdProjectManager::Shutdown()
{
	this->SaveCurrentProject();
	this->CloseCurrentProject();
}
//-----------------------------------------------------------------------------
void EdProjectManager::CreateActions()
{
	//QObject* parent = GetMainFrame();

	m_newProjectAct.setText( TR("&New") );
	m_newProjectAct.setShortcut(QKeySequence::New);
	m_newProjectAct.setStatusTip(TR("Creates a new project"));

	m_openProjectAct.setText( TR("&Open") );
	m_openProjectAct.setShortcut(QKeySequence::Open);
	m_openProjectAct.setStatusTip(TR("Opens an existing project"));

	m_saveProjectAct.setText( TR("Save Project") );
	m_saveProjectAct.setShortcut(QKeySequence::Save);
	m_saveProjectAct.setStatusTip(TR("Saves the current project"));

	m_closeProjectAct.setText( TR("&Close") );
	m_closeProjectAct.setShortcut(QKeySequence::Close);
	m_closeProjectAct.setStatusTip(TR("Closes the current project"));

	m_buildProjectAct.setText( TR("Build Project") );
	m_buildProjectAct.setShortcut( QKeySequence( Qt::Key_F7 ) );

	m_publishProjectAct.setText( TR("Publish Project") );
	m_publishProjectAct.setShortcut( QKeySequence( Qt::Key_F8 ) );
}
//-----------------------------------------------------------------------------
void EdProjectManager::CreateMenus()
{
	EdMenus& menus = EdApp::Get().menus;
	//QMenuBar* menuBar = menus.menuBar;
	{
		menus.fileMenu->addAction( &m_newProjectAct );
		menus.fileMenu->addAction( &m_openProjectAct );
		menus.fileMenu->addAction( &m_saveProjectAct );
		menus.fileMenu->addAction( &m_closeProjectAct );
		menus.fileMenu->addSeparator();
	}
	{
		menus.buildMenu->addAction( &m_buildProjectAct );
		menus.buildMenu->addAction( &m_publishProjectAct );
	}
}
//-----------------------------------------------------------------------------
void EdProjectManager::ConnectSigSlots()
{
	connect(&m_newProjectAct, SIGNAL(triggered()), this, SLOT(CreateNewProject()));
	connect(&m_openProjectAct, SIGNAL(triggered()), this, SLOT(OpenExistingProject()));
	connect(&m_saveProjectAct, SIGNAL(triggered()), this, SLOT(SaveCurrentProject()));
	connect(&m_closeProjectAct, SIGNAL(triggered()), this, SLOT(CloseCurrentProject()));

	connect(&m_buildProjectAct, SIGNAL(triggered()), this, SLOT(slot_BuildProject()));
	connect(&m_publishProjectAct, SIGNAL(triggered()), this, SLOT(slot_PublishProject()));

	EditorApp::ValidOnlyWhenProjectIsLoaded(&m_closeProjectAct);
	EditorApp::ValidOnlyWhenProjectIsLoaded(&m_saveProjectAct);

	EditorApp::ValidOnlyWhenProjectIsLoaded(&m_buildProjectAct);
}
//-----------------------------------------------------------------------------
void EdProjectManager::CreateNewProject()
{
	this->MaybeSaveCurrentProject();

	NewProjectWizard wizard;

	if( QDialog::Accepted == wizard.exec() )
	{
		EdProjectCreationSettings	settings;
		wizard.getSettings( settings );

		this->CreateNewProject( settings );
	}
}
//-----------------------------------------------------------------------------
void EdProjectManager::OpenExistingProject()
{
	this->MaybeSaveCurrentProject();

	QFileDialog dialog;
	dialog.setFileMode(QFileDialog::ExistingFile);
	dialog.setNameFilter(TR("Project files (*"ED_PROJECT_FILE_EXTENSION")"));

	static QString	prevDirectory = EdSystem::Get().settings.pathToProjects;
	dialog.setDirectory(prevDirectory);

	if (dialog.exec())
	{
		prevDirectory = dialog.directory().path();

		QStringList	files = dialog.selectedFiles();
		Assert(files.count()==1);

		this->OpenProjectFile( files.first().toAscii().data() );
	}
}
//-----------------------------------------------------------------------------
void EdProjectManager::SaveCurrentProject()
{
	if( m_currentProject != nil )
	{
		const String fileName = m_currentProject->GetFileName();

		//DBGOUT("Saving project to file '%s'\n",fileName.ToChars());

		if( m_currentProject->Save() )
		{
			m_recentProjects.Add( fileName );
		}
		else
		{
			QMessageBox::critical(
				EditorApp::GetMainFrame(),
				tr("Error"),
				tr("Failed to save project '%1' to disk").arg(m_currentProject->GetFileName())
			);

			EditorApp::GetStatusBar()->showMessage(
				tr("Failed to save project '%1' to disk").arg(m_currentProject->GetFileName())
			);
		}
	}
}
//-----------------------------------------------------------------------------
void EdProjectManager::CloseCurrentProject()
{
	this->MaybeSaveCurrentProject();

	if( m_currentProject != nil )
	{
		DBGOUT("Closing current project '%s'\n",m_currentProject->GetName());
		m_currentProject = nil;
	}
}
//-----------------------------------------------------------------------------
void EdProjectManager::slot_BuildProject()
{
	if( m_currentProject != nil )
	{
		mxPutf("Building project: '%s', output directory: '%s'\n",
			m_currentProject->GetName(), m_currentProject->GetPathName()
			);

		ANSICHAR	szAbsProjPath[FS_MAX_PATH];
		V_strcpy( szAbsProjPath, m_currentProject->GetPathName() );

		ANSICHAR	szProjFileName[FS_MAX_PATH];
		F_ComposeFilePath( szAbsProjPath, m_currentProject->GetName(), ".game", szProjFileName );

		FileWriter		file( szProjFileName );
		VRET_IF_NOT(file.IsOpen());

UNDONE;
		//ArchiveWriter	archive( file );

		//if( gEngine.SaveToFile( archive ) )
		//{
		//}
		//else
		//{
		//	QMessageBox::critical(
		//		GetMainFrame(),
		//		tr("Error"),
		//		tr("Failed to build project '%1'").arg(m_currentProject->GetFileName())
		//		);
		//}
	}
}
//-----------------------------------------------------------------------------
void EdProjectManager::slot_PublishProject()
{
	Unimplemented_Checked;
	//if( m_currentProject != nil )
	//{
	//	mxPutf("Publishing project: '%s', output directory: '%s'\n",
	//		m_currentProject->GetName(), m_currentProject->GetPathName()
	//		);

	//	//NullStreamWriter	nullStreamWriter;

	//	MemoryBlob			rawSerializedData(EMemHeap::HeapTemp);
	//	MemoryBlobWriter	streamWriter( rawSerializedData );

	//	// maps old resource GUIDs to new ones
	//	typedef TMap< ObjectGUID, UFileGuid >	ReMapResourceGUIDs;


	//	// collects used resources, serializes source data with new resource GUIDs
	//	//
	//	class ArchiveTagUsed : public ArchiveWriter
	//	{
	//		ReMapResourceGUIDs	m_oldToNew;

	//	public:
	//		typedef ArchiveWriter Super;

	//		ArchiveTagUsed( mxStreamWriter& stream )
	//			: Super( stream )
	//		{
	//			mxPutf("\n=== Gathering all referenced resources...\n");

	//		}
	//		const ReMapResourceGUIDs& GetUsedResources() const
	//		{
	//			return m_oldToNew;
	//		}
	//		virtual void SerializeMemory( void* ptr, SizeT size ) override
	//		{
	//			Super::SerializeMemory( ptr, size );
	//		}
	//		virtual void SerializeResourceGuid( ObjectGUID & resourceGuid ) override
	//		{
	//			//if( resourceGuid.v != MX_NULL_RESOURCE_GUID )
	//			//{
	//			//	UFileGuid* pFileGuid = m_oldToNew.Find( resourceGuid.v );

	//			//	if( nil == pFileGuid )
	//			//	{
	//			//		const UINT newResourceIndex = m_oldToNew.NumEntries();
	//			//		const UFileGuid newResourceGuid( mxDEFAULT_PACKAGE_GUID, newResourceIndex );

	//			//		//SFileMetadata	fileMetadata;
	//			//		//gCore.resources->edGetFileMetadata( resourceGuid.v, fileMetadata );
	//			//		//DBGOUT("Adding '%s' (%u bytes).\n", fileMetadata.name.ToChars(), (UINT)fileMetadata.fileSize);

	//			//		//DBGOUT("!> [%x] -> [%x]\n", resourceGuid.v, newResourceGuid.resourceGuid );

	//			//		pFileGuid = &m_oldToNew.Set( resourceGuid.v, newResourceGuid );
	//			//	}

	//			//	AssertPtr( pFileGuid );

	//			//	resourceGuid.v = pFileGuid->resourceGuid;
	//			//}

	//			DBGOUT("Writing resource GUID [%x]\n", resourceGuid.v );

	//			Super::SerializeResourceGuid( resourceGuid );
	//		}
	//		// serialization of non-POD objects
	//		virtual void SerializePtr( mxObject *& o ) override
	//		{
	//			Super::SerializePtr( o );
	//		}
	//		// registers a top-level object
	//		virtual void InsertRootObject( mxObject* o ) override
	//		{
	//			Super::InsertRootObject( o );
	//		}
	//	};


	//	ArchiveTagUsed	collectUsedResources( streamWriter );
	//	gEngine.SaveToFile( collectUsedResources );


	//	class MyFileEnumerator : public OptimizedPakFile::FileEnumerator
	//	{
	//		const ReMapResourceGUIDs &	m_resourceMap;

	//	public:
	//		MyFileEnumerator( const ReMapResourceGUIDs& resourceMap )
	//			: m_resourceMap( resourceMap )
	//		{
	//		}
	//		virtual UINT NumFiles() const override
	//		{
	//			return m_resourceMap.NumEntries();
	//		}
	//		virtual UINT GetFileSize( UINT iResourceFile ) const override
	//		{
	//			const ReMapResourceGUIDs::Pair& p = m_resourceMap.GetPairs()[ iResourceFile ];

	//			const ObjectGUID resourceGuid = p.key;
	//			Assert(IsValidResourceGuid( resourceGuid ));

	//			AResourcePackage* package = gCore.resources->GetPackageByResourceGuid( resourceGuid );
	//			AssertPtr( package );
	//			VRET_X_IF_NIL( package, 0 );

	//			UINT	packageGuid, fileIndex;
	//			DecomposeFileGuid( resourceGuid, packageGuid, fileIndex );

	//			return package->GetFileSize( fileIndex );
	//		}
	//		virtual void ReadFile( UINT iResourceFile, void *pBuffer ) override
	//		{
	//			const ReMapResourceGUIDs::Pair& p = m_resourceMap.GetPairs()[ iResourceFile ];

	//			const ObjectGUID resourceGuid = p.key;

	//			AResourcePackage* package = gCore.resources->GetPackageByResourceGuid( resourceGuid );
	//			AssertPtr( package );
	//			VRET_IF_NIL( package );

	//			UINT	packageGuid, fileIndex;
	//			DecomposeFileGuid( resourceGuid, packageGuid, fileIndex );

	//			const UINT numBytesToRead = package->GetFileSize( fileIndex );

	//			package->ReadFile( fileIndex, 0, pBuffer, numBytesToRead );
	//		}

	//	private:
	//		AResourcePackage* GetPackageByFileIndex( UINT fileIndex ) const
	//		{
	//			const ReMapResourceGUIDs::Pair& p = m_resourceMap.GetPairs()[ fileIndex ];

	//			const ObjectGUID resourceGuid = p.key;

	//			AResourcePackage* package = gCore.resources->GetPackageByResourceGuid( resourceGuid );
	//			AssertPtr( package );
	//			return package;
	//		}
	//	};

	//	MyFileEnumerator	resourceEnumerator( collectUsedResources.GetUsedResources() );


	//	// compose names of output directories

	//	ANSICHAR	szAbsProjPath[FS_MAX_PATH];
	//	V_strcpy( szAbsProjPath, m_currentProject->GetPathName() );


	//	// create an optimized package file which will store only the actually used resources
	//	{
	//		ANSICHAR	szPakFileName[FS_MAX_PATH];
	//		F_ComposeFilePath( szAbsProjPath, "Data", ".xxx", szPakFileName );

	//		OptimizedPakFile	resourceBundle( &resourceEnumerator, szPakFileName );
	//	}

	//	// create a scene file
	//	{
	//		ANSICHAR	szGameFileName[FS_MAX_PATH];
	//		F_ComposeFilePath( szAbsProjPath, m_currentProject->GetName(), ".xxx", szGameFileName );

	//		DBGOUT("Creating scene file '%s'\n", szGameFileName);

	//		FileWriter		file( szGameFileName );

	//		file.Write( rawSerializedData.ToPtr(), rawSerializedData.GetDataSize() );
	//	}

	//	//ArchiveWriter	archive( file );

	//	//if( gEngine.SaveToFile( archive, szPakFileName ) )
	//	//{
	//	//}
	//	//else
	//	//{
	//	//	QMessageBox::critical(
	//	//		GetMainFrame(),
	//	//		tr("Error"),
	//	//		tr("Failed to build project '%1'").arg(m_currentProject->fullFilePath)
	//	//		);
	//	//}
	//}
}
//-----------------------------------------------------------------------------
void EdProjectManager::CreateNewProject( const EdProjectCreationSettings& settings )
{
	this->MaybeSaveCurrentProject();
	this->CloseCurrentProject();

	DBGOUT("Creating a new project: '%s'\n",settings.name.toAscii().data());

	m_currentProject = new EdProject( settings );
	EdSystem::Get().Event_ProjectLoaded();
}
//-----------------------------------------------------------------------------
bool EdProjectManager::OpenProjectFile( const String& filePath )
{
	// check if such project is already loaded
	if( m_currentProject != nil )
	{
		if( filePath == m_currentProject->GetFileName() )
		{
			return true;
		}
	}

	this->MaybeSaveCurrentProject();
	this->CloseCurrentProject();

	const QString strFilePath = filePath.ToChars();

	QFileInfo	fileInfo( strFilePath );
	if( !fileInfo.exists() )
	{
		const int fiveSeconds = 5000;
		EditorApp::GetStatusBar()->showMessage(
			QString("Failed to open project %1").arg( strFilePath ),fiveSeconds);

		if( m_recentProjects.Find( filePath ) )
		{
			//	QMessageBox::StandardButton ret =
			//		QMessageBox::warning(
			//		this, tr("Application"),
			//		tr("The project cannot be opened.\n"
			//		"Do you want to remove it from the recent files list?"),
			//		QMessageBox::Ok | QMessageBox::Cancel
			//		);
			//	if( QMessageBox::Ok == ret )
			//	{
			//		this->recentFiles.Remove(fileName.toAscii().data());
			//	}

			m_recentProjects.Remove( filePath );
		}

		return false;
	}


	DBGOUT("Loading project from file '%s'\n",filePath.ToChars());

	EdProjectCreationSettings	settings;
	settings.name = fileInfo.fileName();
	settings.absolutePath = fileInfo.dir().path();

	m_currentProject = new EdProject( settings );
	m_currentProject->Load();

	EdSystem::Get().Event_ProjectLoaded();

	return true;

}
//-----------------------------------------------------------------------------
void EdProjectManager::MaybeSaveCurrentProject()
{
#if 0
	if( m_currentProject != nil )
	{
		//if( m_currentProject->isDirty )
		{
			QMessageBox::StandardButton ret =
				QMessageBox::warning(
					GetMainFrame(),
					TR("Application"),

					//tr("The document has been modified.\n"
					//"Do you want to save your changes?"),
					TR("Do you want to save the project?"),

					QMessageBox::Save | QMessageBox::Discard //| QMessageBox::Cancel
				);

			if( QMessageBox::Save == ret )
			{
				SaveCurrentProject();
			}
			//else if( QMessageBox::Cancel == ret )
			//{
			//	return false;
			//}

			//m_currentProject->isDirty = false;
		}
	}
#endif
}
//-----------------------------------------------------------------------------

void EdProjectManager::SerializeAddData( ATextSerializer & serializer )
{
	if( m_currentProject != nil )
	{
		m_recentProjects.Add( m_currentProject->GetPathName() );
	}

	m_recentProjects.Serialize( serializer );

	if( serializer.IsLoading() )
	{
		EdMenus& menus = EdApp::Get().menus;
		m_recentProjects.CreateMenus( menus.fileMenu );
	}
}

void EdProjectManager::SerializeWidgetLayout( QDataStream & stream )
{
	//DBG_TRACE_CALL;
}
//-----------------------------------------------------------------------------
bool EdProjectManager::OpenRecentFile( const char* fileName )
{
	const bool bOk = this->OpenProjectFile( fileName );

	const QString strFilePath( fileName );

	if( !bOk )
	{
		const int fiveSeconds = 5000;
		EditorApp::GetStatusBar()->showMessage(
			tr("Failed to open project %1").arg( strFilePath ), fiveSeconds );
	}

	return bOk;
}
//-----------------------------------------------------------------------------
EdProject* EdProjectManager::GetCurrentProject()
{
	return m_currentProject.get_ptr();
}
//-----------------------------------------------------------------------------
bool EdProjectManager::IsProjectLoaded() const
{
	return m_currentProject != nil;
}

/*
-----------------------------------------------------------------------------
	NewProjectWizard
-----------------------------------------------------------------------------
*/

NewProjectWizard::NewProjectWizard(QWidget *parent)
	: QWizard( parent )
{
	ProjectInfoPage* projInfoPage = new ProjectInfoPage(this);
	this->addPage(projInfoPage);
	projInfoPage->setDefaults();

	this->setWindowTitle(TR("New Project Wizard"));
}
//-----------------------------------------------------------------------------
void NewProjectWizard::accept()
{
	QDialog::accept();
}
//-----------------------------------------------------------------------------
QString NewProjectWizard::projectName() const
{
	return this->field( ED_PROJECT_NAME ).toString();
}
//-----------------------------------------------------------------------------
QString NewProjectWizard::projectDir() const
{
	return this->field( ED_PROJECT_DIR ).toString();
}
//-----------------------------------------------------------------------------
void NewProjectWizard::getSettings( EdProjectCreationSettings & settings )
{
	settings.name = this->projectName();
	settings.absolutePath = this->projectDir();
}

/*
-----------------------------------------------------------------------------
	ProjectInfoPage
-----------------------------------------------------------------------------
*/
ProjectInfoPage::ProjectInfoPage(QWidget *parent)
	: QWizardPage( parent )
{
	this->setTitle(TR("Project Information"));
	this->setSubTitle(TR("Specify basic information about the project "
		"you want to create."));

	QLabel* projectNameLabel = new QLabel( TR("&Project name:"), this );
	this->projectNameLineEdit = new QLineEdit( this );
	projectNameLabel->setBuddy( this->projectNameLineEdit );

	QLabel* projectDirLabel = new QLabel( TR("&Project directory:"), this );
	this->projectDirLineEdit = new QLineEdit( this );
	projectDirLabel->setBuddy( this->projectDirLineEdit );

	QPushButton* browseBtn = new QPushButton("Browse",this);
	connect(browseBtn, SIGNAL(pressed()), this, SLOT(selectProjectDir()));


	QGridLayout* layout = new QGridLayout(this);

	layout->addWidget( projectNameLabel );
	layout->addWidget( this->projectNameLineEdit );

	layout->addWidget( projectDirLabel );
	layout->addWidget( this->projectDirLineEdit );

	layout->addWidget( browseBtn, layout->rowCount()-1, 1 );

	
	this->registerField( ED_PROJECT_NAME"*", this->projectNameLineEdit );
	this->registerField( ED_PROJECT_DIR"*", this->projectDirLineEdit );

}
//-----------------------------------------------------------------------------
void ProjectInfoPage::setDefaults()
{
	this->projectNameLineEdit->setText(ED_DEFAULT_PROJECT_NAME);//<Enter_name>");

	//this->projectDirLineEdit->setText( QCoreApplication::applicationDirPath() + ED_DEFAULT_PROJECT_DIR );

	if( EdSystem::Get().settings.pathToProjects.IsEmpty() )
	{
		this->projectDirLineEdit->setText( QCoreApplication::applicationDirPath() + ED_DEFAULT_PROJECT_DIR );
	}
	else
	{
		String	tmp = EdSystem::Get().settings.pathToProjects;
		tmp.StripTrailingOnce("/");

		this->projectDirLineEdit->setText( QString(tmp.ToChars()) + ED_DEFAULT_PROJECT_DIR );
	}

	emit completeChanged();
}
//-----------------------------------------------------------------------------
void ProjectInfoPage::selectProjectDir()
{
	QFileDialog dialog(this);
	dialog.setFileMode(QFileDialog::Directory);

	dialog.setDirectory(this->projectDirLineEdit->text());

	if (dialog.exec())
	{
		this->projectDirLineEdit->setText( dialog.directory().path( ));

		EdSystem::Get().settings.pathToProjects = dialog.directory().path().toAscii().data();
		FS_FixPathSlashes( EdSystem::Get().settings.pathToProjects );
	}
}
//-----------------------------------------------------------------------------

namespace EditorApp
{

EdProject* GetCurrentProject()
{
	return EdProjectManager::HasInstance() ?
		EdProjectManager::Get().GetCurrentProject() : nil;
}

bool IsProjectLoaded()
{
	return GetCurrentProject() != nil;
}

}//namespace EditorUtil

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//

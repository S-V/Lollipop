/*
=============================================================================
	File:	project_manager.h
	Desc:	Editor project management.
=============================================================================
*/
#pragma once

#include <Engine/Engine.h>

#include <EditorSupport/AssetPipeline/DevAssetManager.h>

#include <QtSupport/RecentFilesUtil.h>

#include "editor_system.h"

/*
-----------------------------------------------------------------------------
	EdProjectCreationSettings
	project settings
-----------------------------------------------------------------------------
*/
struct EdProjectCreationSettings
{
	// name of the project
	QString	name;

	// folder where the project is located (not including file name)
	QString	absolutePath;
};

/*
-----------------------------------------------------------------------------
	EdProject
-----------------------------------------------------------------------------
*/
class EdProject
	: public ReferenceCounted
	, public AEngineClient
	, SingleInstance< EdProject >	// only project can be loaded at a time
{
public:
	typedef TRefPtr< EdProject >	Ref;

	EdProject();
	EdProject( const EdProjectCreationSettings& settings );
	~EdProject();

	bool Save();
	bool Load();

	// returns absolute path to folder where the project is located (and opened from)
	// NOT including file name
	const char* GetPathName() const	{ return m_absolutePath.ToChars(); }

	// returns absolute file path to the project file
	// INCLUDING file name
	const char* GetFileName() const	{ return m_fullFilePath.ToChars(); }

	// returns the name of this project
	const char* GetName() const	{ return m_name.ToChars(); }


	void LoadAssetDatabase( const char* assetDbFilePath );

public:
	static QString ComposeFullFilePath(const QString& absolutePath, const QString& projectName);

public:	//-- AEngineClient
	//virtual void MoundResourceArchives() override;

private:
	// persistent data

	String	m_name;	// name of the project

	// asset database
	DevAssetManager	m_assetMgr;
	String		m_assetDbFilePath;

	// in memory only

	// folder where the project is located (and opened from)
	String	m_absolutePath;	// absolute path NOT including file name
	String	m_fullFilePath;	// absolute file path including file name

private:
	PREVENT_COPY(EdProject);
};

/*
-----------------------------------------------------------------------------
	EdProjectManager
-----------------------------------------------------------------------------
*/
class EdProjectManager
	: public QObject
	, public EdModule
	, public EdWidgetChild
	, public TGlobal< EdProjectManager >
	, public RecentFilesUtil::IClient
{
	Q_OBJECT

public:
	EdProjectManager();
	~EdProjectManager();

	virtual void PreInit() override;
	virtual void SetDefaultValues() override;
	virtual void PostInit() override;

	virtual void Shutdown() override;

	// serialize app-specific settings
	virtual void SerializeAddData( ATextSerializer & serializer ) override;

	virtual void SerializeWidgetLayout( QDataStream & stream ) override;

	//== RecentFilesUtil::IClient
	virtual bool OpenRecentFile( const char* fileName );

	void OpenMostRecentlyUsedFile();

	EdProject* GetCurrentProject();
	bool IsProjectLoaded() const;

public slots:
	void CreateNewProject();
	void OpenExistingProject();
	void SaveCurrentProject();
	void CloseCurrentProject();

	// generate an optimized world file which still references resources in original packages
	//
	void slot_BuildProject();

	// pack all referenced resources into one optimized archive file
	// and generate an optimized world file
	//
	void slot_PublishProject();

	void CreateNewProject( const EdProjectCreationSettings& settings );
	bool OpenProjectFile( const String& filePath );

private slots:
	// prompt the user to save the project
	void MaybeSaveCurrentProject();

private:
	void CreateActions();
	void CreateMenus();
	void ConnectSigSlots();

private:
	EdAction	m_newProjectAct;
	EdAction	m_openProjectAct;
	EdAction	m_saveProjectAct;
	EdAction	m_closeProjectAct;

	EdAction	m_buildProjectAct;
	EdAction	m_publishProjectAct;

	EdProject::Ref	m_currentProject;
	RecentFilesUtil	m_recentProjects;
};

/*
-----------------------------------------------------------------------------
	NewProjectWizard
-----------------------------------------------------------------------------
*/
class NewProjectWizard : public QWizard
{
	Q_OBJECT

public:
	NewProjectWizard(QWidget *parent = 0);

	void getSettings( EdProjectCreationSettings & settings );

	QString projectName() const;
	QString projectDir() const;

public slots:
	virtual void accept();

private:
};

/*
-----------------------------------------------------------------------------
	ProjectInfoPage
-----------------------------------------------------------------------------
*/
class ProjectInfoPage : public QWizardPage
{
	Q_OBJECT

public:
	ProjectInfoPage(QWidget *parent = 0);

	void setDefaults();

private slots:
	void selectProjectDir();

private:
	QLineEdit* projectNameLineEdit;
	QLineEdit* projectDirLineEdit;
};

#ifndef CONTENTMANAGER_H
#define CONTENTMANAGER_H

#include <EditorSupport/Serialization/TextSerializer.h>
#include <EditorSupport/AssetPipeline/DevAssetManager.h>

#include <QtSupport/log_widget.h>
#include <QtSupport/RecentFilesUtil.h>

#include <QtGui/QMainWindow>
#include "ui_contentmanager.h"

/*
-----------------------------------------------------------------------------
	AppSettings
-----------------------------------------------------------------------------
*/
struct AppSettings : public TGlobal< AppSettings >
{
	String		pathToContentDb;	// folder where resource catalog will be stored

	String		pathToSrcAssets;	// path to folder with source assets
	String		pathToIntAssets;	// path to folder with intermediate assets
	String		pathToBinAssets;	// path to folder with compiled (binary) assets

public:
	AppSettings();
	~AppSettings();

	void Serialize( ATextSerializer & serializer );
	void NormalizePaths();
};

/*
-----------------------------------------------------------------------------
	ContentManager
-----------------------------------------------------------------------------
*/
class ContentManager : public QMainWindow
	, public RecentFilesUtil::IClient
	, DependsOnGlobal< AppSettings >
{
	Q_OBJECT

public:
	ContentManager(QWidget *parent = 0, Qt::WFlags flags = 0);
	~ContentManager();

	bool OpenAssetDatabase( const char* filePath );

	bool IsOpen() const;

	//=-- RecentFilesUtil::IClient
	virtual bool OpenRecentFile( const char* fileName ) override;

private slots:

	void slot_CreateNewAssetDatabase();
	void slot_OpenExistingAssetDatabase();
	void slot_RebuildCurrentAssetDatabase();

private:
	Ui::ContentManagerClass ui;

	LogWidget		m_logWidget;
	RecentFilesUtil	m_recentFiles;

	DevAssetManager	m_assetDb;

	String		m_assetDbFilePath;
};

#endif // CONTENTMANAGER_H

#pragma once

#if !MX_EDITOR
	#error Invalid option: must be built with editor mode enabled!
#endif //!MX_EDITOR


// compile-time options



class ATextSerializer;
class InputClient;
class rxSceneContext;
class BatchRenderer;
class rxSkyModel;
class AEntity;
class World;

class EdApp;
class EdModule;
class EdMainWindowFrame;
class EdSystem;
class EdProject;
class EdViewport;


class StaticModelEntity;
class Lights_Editor;
class rxRenderWorld;
class Graphics_Scene_Editor;
class World_Entities_List;
class World_Editor;
class Global_Level_Editor;


//-----------------------------------------------------------------------------

namespace EditorApp
{

	QMainWindow* GetMainFrame();
	QStatusBar* GetStatusBar();

	// may return nil
	EdProject* GetCurrentProject();
	bool IsProjectLoaded();

}//namespace EditorUtil

//-----------------------------------------------------------------------------

/*
-----------------------------------------------------------------------------
	EdGlobalSettings
-----------------------------------------------------------------------------
*/
struct EdGlobalSettings
{
	String		pathToQtStyleSheet;

	String		pathToProjects;
	String		pathToAssetDb;

	String		pathToSrcAssets;
	String		pathToIntAssets;
	String		pathToBinAssets;

public:
	EdGlobalSettings();
	void Serialize( ATextSerializer & s );
};

//-----------------------------------------------------------------------------

#define ED_ORGANIZATION_NAME	"rats"
#define ED_ORGANIZATION_DOMAIN	"ratsfaction.com"
#define ED_APPLICATION_NAME		"World Editor"

#define ED_PROJECT_NAME	"projectName"
#define ED_PROJECT_DIR	"projectDir"
#define ED_PACKAGE_DIR	"packageDir"

#define ED_PATH_TO_APP_SETTINGS	"WorldEditor.cfg"
#define ED_PATH_TO_APP_LAYOUT	"WorldEditor.layout"

#define ED_PROJECT_FILE_EXTENSION	".proj"
//#define ED_RELEASE_FILE_EXTENSION	".game"

#define ED_DEFAULT_PROJECT_NAME		"Untitled"
#define ED_DEFAULT_PROJECT_DIR		"/Untitled/"

#define RESOURCE_MIME_FORMAT	"app/resources"

// 10 seconds
#define ED_STATUS_BAR_MSG_TIMEOUT	10000

// temporary workaround
#define TR(x)	x

//-----------------------------------------------------------------------------

namespace EditorApp
{

	inline void ShowMsgInStatusBar( const char* fmt, ... )
	{
		char	buf[MAX_STRING_CHARS];
		MX_GET_VARARGS_ANSI(buf,fmt);
		GetStatusBar()->showMessage(buf,ED_STATUS_BAR_MSG_TIMEOUT);
	}

	inline void ShowMsg_OnWrongUserAction( const char* fmt, ... )
	{
		char	buf[MAX_STRING_CHARS];
		MX_GET_VARARGS_ANSI(buf,fmt);

		ShowMsgInStatusBar( buf );

		mxPut( buf );
	}

}//namespace EditorUtil


//-----------------------------------------------------------------------------


/*
-----------------------------------------------------------------------------
	EdAppChild
-----------------------------------------------------------------------------
*/
struct EdAppChild
{
protected:
	// serialize application-specific settings (e.g. user preferences)
	virtual void SerializeAddData( ATextSerializer & serializer ) {};

protected:
	EdAppChild();
	virtual ~EdAppChild();
};

/*
-----------------------------------------------------------------------------
	EdWidgetChild
-----------------------------------------------------------------------------
*/
struct EdWidgetChild : public virtual EdAppChild
{
protected:

	// this function called if the file with layouts couldn't be loaded.
	//virtual void SetDefaultWidgetLayout() {};

	// serialize widget-specific data (e.g. window layouts)
	virtual void SerializeWidgetLayout( QDataStream & stream ) {};

protected:
	EdWidgetChild();
	virtual ~EdWidgetChild();
};

/*
-----------------------------------------------------------------------------
	EdProjectChild
-----------------------------------------------------------------------------
*/
// valid only if some project is loaded
struct EdProjectChild : public virtual EdAppChild
{
protected:
	virtual void OnProjectLoaded() {};
	virtual void OnProjectUnloaded() {};

	// MUST BE EMPTY! Subclasses do not call parent class's method!
	virtual void SerializeProjectData( ATextSerializer & archive ) {};

protected:
	EdProjectChild();
	virtual ~EdProjectChild();
};

/*
-----------------------------------------------------------------------------
	EdSystemChild
-----------------------------------------------------------------------------
*/
struct EdSystemChild : public virtual EdProjectChild
{
protected:
	virtual void OnSelectionChanged( AEditable* theObject ) {}
	virtual void OnObjectDestroyed( AEditable* theObject ) {}

protected:
	// automatic connection/disconnection
	EdSystemChild();
	virtual ~EdSystemChild();
};

/*
-----------------------------------------------------------------------------
	EdAction
-----------------------------------------------------------------------------
*/
class EdAction : public QAction
{
	Q_OBJECT

public:
	EdAction( const QString& text = "?", QObject* parent = nil );

private slots:
	void slot_OnTriggered();

public:
	void *	userData;
	TCallback< void (void*) > OnTriggered;
};


namespace EditorApp
{
	// make the widget enabled only when some project is loaded;
	// most of the editor widgets and actions only make sense when a valid project is open for editing;
	//
	void ValidOnlyWhenProjectIsLoaded( QWidget* w );
	void ValidOnlyWhenProjectIsLoaded( QAction* a );

	void ShowOnActionTriggered( QAction* a, QWidget* w );
	void VisibleWhenActionIsChecked( QAction* a, QWidget* w );
	void SetActionCheckedIfVisible( QAction* a, QWidget* w );

	//void ExpandTreeViewOnDataChanged( QTreeView* v, QAbstractItemModel* m );

	/*
		returns the path to the selected folder
	*/
	QString SelectDirectoryOnly(
		QString &outSelectedPath,
		QWidget* parentWidget,
		const QString& dialogTitle = "Select a directory"
		);


	QString Get_AssetPath_SaveFileAs( const char* title, const char* fileName, const char* filter );

}//namespace EditorUtil




/*
-----------------------------------------------------------------------------
	AObjectEditor

	base class for object editors
-----------------------------------------------------------------------------
*/
class AObjectEditor : public AEditableRefCounted
{
	//mxDECLARE_ABSTRACT_CLASS(AObjectEditor,AEditableRefCounted);



public:	// Drag and Drop

	virtual bool edAcceptsDrop( const QMimeData* mimeData )
	{
		//return true;
		return mimeData->hasUrls();
	}

public:
	virtual ~AObjectEditor() {}
};



//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//

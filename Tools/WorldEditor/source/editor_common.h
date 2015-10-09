#pragma once

#if !MX_EDITOR
	#error Invalid option: must be built with editor mode enabled!
#endif //!MX_EDITOR


class ATextSerializer;
class SAssetInfo;
class AInputClient;
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
class EdSceneViewport;
class EdDrawContext;

class APlaceable;

class Graphics_Model_Editor;
class Graphics_Models_List;
class StaticModelEntity;
class Scene_Editor_Local_Lights;
class Scene_Editor_Global_Lights;
class Local_Light_Editor;
class Dir_Light_Editor;
class rxRenderWorld;
class Graphics_Scene_Editor;
class World_Entities_List;
class World_Editor;
class Universe_Editor;


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

#define ED_APP_NAME				"WorldEditor"
#define ED_PATH_TO_APP_SETTINGS	"WorldEditor.cfg"
#define ED_PATH_TO_APP_LAYOUT	"WorldEditor.layout"

#define ED_PROJECT_FILE_EXTENSION	".proj"
//#define ED_RELEASE_FILE_EXTENSION	".game"

#define ED_DEFAULT_PROJECT_NAME		"Untitled"
#define ED_DEFAULT_PROJECT_DIR		"/Untitled/"

#define RESOURCE_MIME_FORMAT	"editor_app/resources"

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
	APlaceable

	abstract base interface for manipulating placeable objects
-----------------------------------------------------------------------------
*/
class APlaceable
{
public:	// Spatial location
	void SetOrigin( const Vec3D& newPos );

	void SetOrientation( const Quat& newRot );

	// (Non-uniform scale is not supported).
	void SetScale( const FLOAT newScale );

public:
	virtual const Vec3D& GetOrigin() const;
	virtual const Quat& GetOrientation() const;
	virtual const FLOAT GetScale() const;

	virtual Matrix4 GetWorldTransform() const;

	// Retrieves the entity's axis aligned bounding box in the world space.
	virtual void GetWorldAABB( AABB & bbox ) const;

	// Retrieves the entity's axis aligned bounding box in the local entity space.
	//virtual void GetLocalAABB( AABB & bbox ) const;

	virtual Sphere GetBoundingSphere() const;

protected:
	virtual void SetOrigin_Impl( const Vec3D& newPos );

	virtual void SetOrientation_Impl( const Quat& newRot );

	// (Non-uniform scale is not supported).
	virtual void SetScale_Impl( const FLOAT newScale );

public:
	// returns the editor corresponding to this placeable
	//virtual AEditable* GetEditorObject() { return nil; }


	enum EDrawFlags
	{
		Draw_AABB = BIT(0),

		Draw_All = BITS_ALL
	};

	virtual UINT Placeable_GetDrawFlags() const
	{
		return EDrawFlags::Draw_All;
	}

	// draw debug lines, etc.
	virtual void Placeable_Draw_Editor_Stuff( const EdDrawContext& drawContext )
	{
		mxUNUSED(drawContext);
	}


	void edCreateCommonProperties( EdPropertyList * properties );

protected:
	APlaceable();
	virtual ~APlaceable();
};

/*
-----------------------------------------------------------------------------
	AObjectEditor

	base class for object editors
-----------------------------------------------------------------------------
*/
class AObjectEditor : public AEditableRefCounted
{
	//mxDECLARE_ABSTRACT_CLASS(AObjectEditor,AEditableRefCounted);

public:	// Gizmo

	virtual APlaceable* IsPlaceable()
	{return nil;}

public:	// Drag and Drop

	virtual bool edAcceptsDrop( const QMimeData* mimeData )
	{
		//return true;
		return mimeData->hasUrls();
	}

public:
	virtual ~AObjectEditor() {}
};

enum EEditorMode
{
	EdMode_Brush,
	EdMode_Face,	// Surface
	EdMode_Vertex,
	EdMode_Material,
	EdMode_Object,
	EdMode_Group,
};



template< class EDITOR >
EDITOR* F_Get_Editor_By_Object_Index_Template(
	EDITOR** editors
	, UINT numEditors
	, UINT objectIndex
	)
{
	for( UINT i=0; i < numEditors; i++ )
	{
		EDITOR* pItem = editors[i];
		if( pItem->m_editedObjectIndex == objectIndex )
		{
			return pItem;
		}
	}
	mxDBG_UNREACHABLE;
	return nil;
}

template< class EDITOR, class PARENT, class OBJECT >
void F_Remove_Editor_Template( EDITOR* pEditor, PARENT& pParent, TList<OBJECT>& rObjectsArray )
{
	// swap-with-last trick

	const UINT thisObjectIndex = pEditor->m_editedObjectIndex;

	EDITOR* pLastEditor = F_Get_Editor_By_Object_Index_Template(
		pParent.ToPtr(), pParent.Num(), rObjectsArray.Num()-1 );

	rObjectsArray.RemoveAt_Fast( thisObjectIndex );
	pLastEditor->m_editedObjectIndex = thisObjectIndex;

	pEditor->m_editedObjectIndex = INDEX_NONE;

	pParent.Remove( pEditor );
}

AEditable* F_Get_Editor_For_Resource( SResPtrBase& resRef, EAssetType resType );

//const SAssetInfo* F_URL_To_Asset_Info( const QMimeData* mimeData );

const ObjectGUID F_URL_To_Asset_Guid( const QMimeData* mimeData );

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//

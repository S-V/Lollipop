/*
=============================================================================
	File:	EditorInterface.h
	Desc:	Support for communication with the editor.
	Note:	functions prefixed with ed* are meant to be called
			from the editor only!
=============================================================================
*/

#pragma once

#include <Base/Object/AObject.h>
#include <Base/Object/ClassDescriptor.h>
#include <Base/Templates/Delegate/Delegate.h>

//== MACRO =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//
#if MX_EDITOR
	#define EDITOR_CODE(x)	x
#else
	#define EDITOR_CODE(x)
#endif

//== MACRO =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//
#if MX_EDITOR
	#define EDITOR_CHECK_VRET( expr )	if( !(expr) ) { mxWarnf("check failed: '%s' in '%s'\n", #expr, __FUNCLINE__ ); return; }
	#define EDITOR_CHECK_VRETB( expr )	if( !(expr) ) { mxWarnf("check failed: '%s' in '%s'\n", #expr, __FUNCLINE__ ); return false; }
#else
	#define EDITOR_CHECK_VRET( expr )	Assert( expr )
	#define EDITOR_CHECK_VRETB( expr )	Assert( expr )
#endif

//-----------------------------------------------------------------------------

mxNAMESPACE_BEGIN

class AEditableProperty;
class EdPropertyList;
class AEditable;
class AObjectEditor;
class SResourceObject;
class EdEventHandler;
class SFileMetadata;
class SResourceInfo;


struct EdShowContextMenuArgs
{
	//TPtr<AEditable>	pObject;
	//int	x, y;
	TPtr< class QMenu >	parent;
};

struct SDropArgs
{
	AEditable *	pDroppedItem;

public:
	inline SDropArgs()
	{
		pDroppedItem = nil;
	}	
};
struct ADropTarget
{
	virtual bool AcceptDrop( const SDropArgs& args )
	{
		mxUNUSED(args);
		return false;
	}
};

// this structure contains info about object memory layout
// (used for inspecting object fields)
//
struct SObjectMemoryInfo
{
	void *			objPtr;
	const mxType *	typeInfo;

public:
	SObjectMemoryInfo() : objPtr(nil), typeInfo(nil)
	{}
};

/*
-----------------------------------------------------------------------------
	AEditable

	This class describes an editable node in a tree structure.
	Mainly used to display items in tree views.
	NOTE: tree structures cannot have cyclic dependencies!
	NOTE: if this class is used as a base class when using multiple inheritance,
	this class should go first.

	NOTE: this class must be abstract!
	NOTE: this class must not define any member fields!
-----------------------------------------------------------------------------
*/
struct AEditable : public AObject
{
	mxDECLARE_ABSTRACT_CLASS(AEditable,AObject);

public:	// User interface

	// returns the number of children
	virtual UINT edNumRows() const
	{
		return 0;
	}

	// returns the number of displayed columns
	virtual UINT uiNumColumns() const
	{
		return 1;
	}

	// returns the child of this node at the specified row
	virtual AEditable* edItemAt( UINT index )
	{
		return nil;
	}

	// returns -1 if the specified node was not found
	virtual UINT edIndexOf( const AEditable* child ) const
	{
		return INDEX_NONE;
	}

	// returns the parent of this node
	virtual AEditable* edGetParent()
	{
		return nil;
	}

public:	// Name

	// used for displaying the object's name
	virtual const char* edToChars( UINT column ) const
	{
		return this->edGetName();
	}
	virtual const char* edGetName() const
	{
		//return "UNKNOWN";
		return this->rttiGetClass().GetTypeName();
	}
	virtual bool edCanBeRenamed() const
	{
		return false;
	}
	virtual bool edSetName( const char* newName )
	{
		return false;
	}

public:	// Properties

	// Creates dynamic properties of this object and collects them in the supplied list.
	// Default implementation clears the property list.
	// Don't forget to call the base class's function first!
	//
	virtual void edCreateProperties( EdPropertyList *properties, bool bClear = true );

	virtual void edGetMemoryLayout( SObjectMemoryInfo & outLayout ) {}

public:	// Serialization

	// save any made changes to persistent storage;
	// this function is called when this object is deselected in the property grid
	virtual void edSaveChanges() {}

	//==- AObject
	virtual void PostLoad() override;

public:	// Selection
	//virtual AEditable* edGetTreeViewSelectionProxy()
	//{return this;}

public:	// Menus

	virtual void edShowContextMenu( const EdShowContextMenuArgs& args )
	{
		mxUNUSED(args);
	}

public:	// Insertion/Removal operations on trees.

	//edDetachFromParent
	virtual void edRemoveSelf()
	{
		mxDBG_UNIMPLEMENTED;
	}

public:	// Drag and Drop

	virtual bool edOnItemDropped( AEditable* pDroppedItem )
	{
		mxUNUSED(pDroppedItem);
		return false;
	}

	// proxy for drag-and-drop
	//virtual AEditable* edGetDropProxy()
	//{return this;}

public:	// Cloning
	//virtual AEditable* edCreateClone()
	//{return nil;}

public:	// Thumbnail previews



public:	// Debugging

	// Debugging hook.
	//virtual void edDebug() {}

	//virtual bool edIsValid() const { return true; }

	//virtual void edDbgCheckValid()
	//{
	//	Assert( this->edIsValid() );
	//}

public:
	// returns row index relative to parent node
	// or INDEX_NONE if this node has no parent
	UINT uiRow()
	{
		AEditable* myParent = this->edGetParent();
		if( myParent != nil )
		{
			return myParent->edIndexOf(this);
		}
		return 0;
	}

	// returns the index of column in which this node displayed
	inline UINT uiColumn()
	{
		return 0;
	}
	inline bool uiHasChildren() const
	{
		return this->edNumRows() > 0;
	}
	inline bool uiIsLeafNode() const
	{
		return !this->uiHasChildren();
	}

	// searches parent node of the given class
	template< class KLASS >
	KLASS* edFindParentNode()
	{
		AEditable* node = this;
		while( node != nil )
		{
			KLASS* ptr = SafeCast<KLASS>( node );
			if( ptr != nil ) {
				return ptr;
			}
			node = node->edGetParent();
		}
		return nil;
	}

	bool edBelongsToTree( AEditable* root )
	{
		AssertPtr(root);

		AEditable* node = this;
		while( node != nil )
		{
			if( node == root ) {
				return true;
			}
			AEditable* parent = node->edGetParent();
			node = parent;
		}
		return false;
	}

	bool edIsImmediateChildOf( AEditable* root )
	{
		AssertPtr(root);

		AEditable* parent = this->edGetParent();
		return parent == root;
	}

public:	// Kludges


protected:	// Reference tracking

	friend class EdSystem;

	// invalidate pointers to objects that are being destroyed
	virtual void RemoveInvalidReference( AEditable* badPtr ) {};

protected:

	// Refreshes all views attached to this node. Slow!
	void uiRefreshView();

	void uiBeginRefresh();
	void uiEndRefresh();

protected:
	AEditable(ENoInit);
	AEditable();
	virtual ~AEditable();
};




/*
-----------------------------------------------------------------------------
	AEditableRefCounted

	The common base class for most complex (heavyweight) editable objects.
	Implement a strong reference counting
	mechanism and runtime type information.
-----------------------------------------------------------------------------
*/
class AEditableRefCounted
	: public AEditable
{
public:
	mxDECLARE_ABSTRACT_CLASS( AEditableRefCounted, AEditable );

	typedef TRefPtr<AEditableRefCounted> Ref;

public:	// Lifetime management, reference counting
	void	Grab() const;
	void	Drop() const;
	UINT	GetNumRefs() const { return m__numReferences; }

public:	// Editor
	virtual void edCreateProperties( EdPropertyList *properties, bool bClear = true ) override;

protected:
	virtual ~AEditableRefCounted();

	virtual void DeleteThis() const;

protected:
	AEditableRefCounted();

private:
	mutable UINT	m__numReferences;
};

/*
-----------------------------------------------------------------------------
	TRefCountedObjectList
-----------------------------------------------------------------------------
*/
template< class KLASS >	// where KLASS : AEditableRefCounted
class TRefCountedObjectList
	: public AEditable
	, public TArrayBase< KLASS*, TRefCountedObjectList< KLASS > >
{
	TList< KLASS* >		m_objects;

public:
	explicit TRefCountedObjectList( HMemory hMemHeap = EMemHeap::DefaultHeap )
		: m_objects( hMemHeap )
	{
	}
	virtual ~TRefCountedObjectList()
	{
		this->Clear();
	}
	virtual void Add( KLASS* newItem )
	{
		CHK_VRET_IF_NIL( newItem );

		AEditable* parent = this;

		EdBeginInsertRows( parent, m_objects.Num(), 1 );

		m_objects.Add( newItem );

		newItem->Grab();

		EdEndInsertRows( parent );

		Assert( newItem->edIsImmediateChildOf( parent ) );
	}

	virtual void Remove( KLASS* theItem )
	{
		CHK_VRET_IF_NIL( theItem );
		Assert( theItem->edIsImmediateChildOf( this ) );

		const UINT index = m_objects.FindIndexOf( theItem );
		Assert(index != INDEX_NONE);
		CHK_VRET_IF_NOT(index != INDEX_NONE);

		this->Internal_RemoveAt<true>( index );
	}

	virtual void Clear()
	{
		//gCore.editor->BeginResetModel( this->edGetParent() );
		const UINT numItems = m_objects.Num();
		if( numItems == 0 ) {
			return;
		}

		AEditable* parent = this;

		EdBeginRemoveRows( parent, 0, numItems );

		Array_Util::DropReferences( m_objects );

		m_objects.Clear();

		//gCore.editor->EndResetModel( this->edGetParent() );
		EdEndRemoveRows( parent );
	}

	void Reserve( UINT reservedSize )
	{
		m_objects.Reserve( reservedSize );
	}

public:	//=== TArrayBase

	inline UINT Num() const
	{
		return m_objects.Num();
	}
	inline UINT GetCapacity() const
	{
		return m_objects.GetCapacity();
	}

	inline KLASS** ToPtr()
	{
		return m_objects.ToPtr();
	}
	inline const KLASS* const* ToPtr() const
	{
		return m_objects.ToPtr();
	}

public:	//==- AEditable

	virtual UINT edNumRows() const override
	{
		return m_objects.Num();
	}
	virtual AEditable* edItemAt( UINT index ) override
	{
		AEditable* item = m_objects[ index ];
		Assert( item );
		return item;
	}
	virtual UINT edIndexOf( const AEditable* child ) const override
	{
		//Assert( child );
		return m_objects.FindPtrIndex( child );
	}

public:	//==- AEditable

	mxREMOVE_THIS	// remove pointer tracking system
	//virtual void RemoveInvalidReference( AEditable* badPtr ) override
	//{
	//	const UINT index = m_objects.FindPtrIndex( badPtr );
	//	if( index != INDEX_NONE )
	//	{
	//		this->Internal_RemoveAt<false>( index );
	//	}
	//}

public:
	virtual void Serialize( mxArchive& archive ) override
	{
		Unimplemented;
	}

private:
	template< bool bDropRef >
	void Internal_RemoveAt( UINT index )
	{
		Assert(index != INDEX_NONE);

		AEditable* parent = this;

		EdBeginRemoveRows( parent, index, 1 );

		KLASS* pObject = m_objects[ index ];

		m_objects.RemoveAt( index );

		EdEndRemoveRows( parent );

		if( bDropRef ) {
			pObject->Drop();
		}
	}
};

//---------------------------------------------------------------------------

/*
-----------------------------------------------------------------------------
	AEditorInterface

	interface for doing callbacks into the editor

	used for game engine -> editor communication.
-----------------------------------------------------------------------------
*/
// this interface will be implemented by the editor
//
class AEditorInterface
{
public:	// Callbacks that you can use

	// should be called immediately after a new object has been created
	virtual void Notify_ObjectCreated( AEditable* newObject ) = 0;


	// NOTE: the passed argument can be null if deselected
	virtual void Notify_SelectionChanged( AEditable* theObject ) = 0;

	// returns the currently selected object;
	// may return null
	virtual AEditable* GetSelectedObject() = 0;


	virtual void Notify_ObjectModified( AEditable* theObject ) = 0;

	virtual void Notify_ObjectModified( AEditable* node, bool bPropagateToParentNodes );

	// should be called before the object is destroyed
	virtual void Notify_ObjectBeingDestroyed( AEditable* theObject ) = 0;


public:	// GUI

	virtual bool Show_YesNo_Warning( const char* fmt, ... ) = 0;



public_internal:	// Insertion/Removal operations on trees.

	// Begins a row insertion operation.
	// You must call this function before inserting data into the model's underlying data store.
	//
	virtual void Notify_BeginInsertRows( AEditable* parent, UINT iFirstRow, UINT iNumRows ) = 0;


	// Ends a row insertion operation.
	// You must call this function after inserting data from the model's underlying data store.
	//
	virtual void Notify_EndInsertRows( AEditable* parent ) = 0;



	// Begins a row removal operation.
	// You must call this function before removing data from the model's underlying data store.
	//
	virtual void Notify_BeginRemoveRows( AEditable* parent, UINT iFirstRow, UINT iNumRows ) = 0;

	// Ends a row removal operation.
	// You must call this function after removing data from the model's underlying data store.
	//
	virtual void Notify_EndRemoveRows( AEditable* parent ) = 0;



	// Begins a model reset operation.
	// When a model is reset it means that any previous data reported from the model is now invalid and has to be queried for again.
	// This also means that the current item and any selected items will become invalid.
	// You must call this function before resetting any internal data structures in your model or proxy model.
	//
	virtual void Notify_BeginResetModel( AEditable* parent ) = 0;


	// Completes a model reset operation.
	// You must call this function after resetting any internal data structure in your model or proxy model.
	//
	virtual void Notify_EndResetModel( AEditable* parent ) = 0;


public:
	static AEditorInterface* GetDummyInstance();

protected:
	AEditorInterface() {}
	virtual ~AEditorInterface() {}
};

#if MX_EDITOR

void EdBeginInsertRows( AEditable* theParent, UINT startRow, UINT numRows );
void EdEndInsertRows( AEditable* theParent );
void EdBeginRemoveRows( AEditable* theParent, UINT startRow, UINT numRows );
void EdEndRemoveRows( AEditable* theParent );

#else

inline void EdBeginInsertRows( AEditable* theParent, UINT startRow, UINT numRows ) {}
inline void EdEndInsertRows( AEditable* theParent ) {}
inline void EdBeginRemoveRows( AEditable* theParent, UINT startRow, UINT numRows ) {}
inline void EdEndRemoveRows( AEditable* theParent ) {}

#endif // #if MX_EDITOR

//---------------------------------------------------------------------------

struct EdClassEnumInterace
{
	virtual AEditable* GetRoot() = 0;

protected:
	virtual ~EdClassEnumInterace() {}
};

struct EdAddNewItemDialogInterace
{
	virtual void SetClassEnumerator( EdClassEnumInterace* pEnum ) = 0;
	virtual void Exec() = 0;

protected:
	virtual ~EdAddNewItemDialogInterace() {}
};

class EdGroup;
typedef void EdNodeCallback( EdGroup* pSelectedGroup, void* pUserData );

/*
-----------------------------------------------------------------------------
	EdGroup
-----------------------------------------------------------------------------
*/
struct EdGroup : public AEditable
{
	const char *	name;
	EdGroup *		kids;
	UINT			numKids;
	EdGroup *		parent;
	EdNodeCallback*	callback;
	void *			userData;

public:
	EdGroup( const char* name, EdGroup* parent, EdNodeCallback* callback = nil, void* userData = nil )
		: AEditable(_NoInit)
	{
		this->name = name;
		this->kids = nil;
		this->numKids = 0;
		this->parent = parent;
		this->callback = callback;
		this->userData = userData;
	}
	EdGroup( EdGroup* alias, EdGroup* parent )
		: AEditable(_NoInit)
	{
		this->name = alias->name;
		this->kids = alias->kids;
		this->numKids = alias->numKids;
		for( UINT iKid = 0; iKid < numKids; iKid++ )
		{
			this->kids[ iKid ].parent = this;
		}
		this->parent = parent;
		this->callback = alias->callback;
		this->userData = alias->userData;
	}
	virtual UINT edNumRows() const override
	{
		return numKids;
	}
	virtual AEditable* edItemAt( UINT index )
	{
		return &kids[index];
	}
	virtual UINT edIndexOf( const AEditable* child ) const
	{
		for( UINT i=0; i<numKids; i++ )
		{
			if( &kids[i] == child ) {
				return i;
			}
		}
		return INDEX_NONE;
	}
	virtual AEditable* edGetParent()
	{
		return parent;
	}
	virtual const char* edToChars( UINT column ) const
	{
		return name;
	}
};



//---------------------------------------------------------------------------

#define BEGIN_GROUP( funcName, groupNameString )\
	EdGroup* funcName()\
	{\
		static EdGroup	parentGroup( groupNameString, nil );\
		\
		static EdGroup kids[] = {



#define ADD_ITEM( itemNameString, ... )\
	EdGroup( itemNameString, &parentGroup, ## __VA_ARGS__ ),


#define ADD_GROUP( funcName )\
	EdGroup( funcName(), &parentGroup ),


#define END_GROUP\
		};\
		parentGroup.kids = kids;\
		parentGroup.numKids = ARRAY_SIZE(kids);\
		return &parentGroup;\
	}

//---------------------------------------------------------------------------


#define mxIMPLEMENT_ED_GROUP_1( field0 )\
	virtual UINT edNumRows() const override\
	{\
		return 1;\
	}\
	virtual AEditable* edItemAt( UINT index ) override\
	{\
		return index == 0 ? &field0 : nil;\
	}\
	virtual UINT edIndexOf( const AEditable* child ) const override\
	{\
		return child == &field0 ? 0 : INDEX_NONE;\
	}

//---------------------------------------------------------------------------

#define mxIMPLEMENT_ED_GROUP_2( field0, field1 )\
	virtual UINT edNumRows() const override\
	{\
		return 2;\
	}\
	virtual AEditable* edItemAt( UINT index ) override\
	{\
		if( 0 == index ) {\
			return &field0;\
		} else if( 1 == index ) {\
			return &field1;\
		} else {\
			return nil;\
		}\
	}\
	virtual UINT edIndexOf( const AEditable* child ) const override\
	{\
		if( child == &field0 ) {\
			return 0;\
		} else if( child == &field1 ) {\
			return 1;\
		} else {\
			return INDEX_NONE;\
		}\
	}

//---------------------------------------------------------------------------

#define mxIMPLEMENT_ED_GROUP_3( field0, field1, field2 )\
	virtual UINT edNumRows() const override\
	{\
		return 3;\
	}\
	virtual AEditable* edItemAt( UINT index ) override\
	{\
		if( 0 == index ) {\
			return &field0;\
		} else if( 1 == index ) {\
			return &field1;\
		} else if( 2 == index ) {\
			return &field2;\
		} else {\
			return nil;\
		}\
	}\
	virtual UINT edIndexOf( const AEditable* child ) const override\
	{\
		if( child == &field0 ) {\
			return 0;\
		} else if( child == &field1 ) {\
			return 1;\
		} else if( child == &field2 ) {\
			return 2;\
		} else {\
			return INDEX_NONE;\
		}\
	}

//---------------------------------------------------------------------------

#define mxIMPLEMENT_ED_GROUP_4( field0, field1, field2, field3 )\
	virtual UINT edNumRows() const override\
	{\
		return 4;\
	}\
	virtual AEditable* edItemAt( UINT index ) override\
	{\
		if( 0 == index ) {\
			return &field0;\
		} else if( 1 == index ) {\
			return &field1;\
		} else if( 2 == index ) {\
			return &field2;\
		} else if( 3 == index ) {\
			return &field3;\
		} else {\
			return nil;\
		}\
	}\
	virtual UINT edIndexOf( const AEditable* child ) const override\
	{\
		if( child == &field0 ) {\
			return 0;\
		} else if( child == &field1 ) {\
			return 1;\
		} else if( child == &field2 ) {\
			return 2;\
		} else if( child == &field3 ) {\
			return 3;\
		} else {\
			return INDEX_NONE;\
		}\
	}

//---------------------------------------------------------------------------


/*
-Make an abstract “EditAction” class, that represents a single action taken by the user to edit the level.  Give it virtual methods for “Undo” and “Redo”
-Make a few classes that inherit from EditAction and implement Undo and Redo for a specific action (moving an object, rotating an object, adding/removing an object, changing a Property, etc.)
-MapEditorLevel class has two stacks of EditAction’s: an Undo stack and a Redo stack.  When the user performs an action, it’s pushed onto the Undo stack and the Redo stack is cleared.
 When the user does an Undo, the Undo stack is popped, Undo is called on that EditAction, and the action is pushed onto the Redo stack.
 When the user does a Redo, the Redo stack is popped, Redo is called, and the action is pushed onto the Undo stack.
*/
class EditorAction
{
public:
	virtual void Undo() = 0;
	virtual void Redo() = 0;
};


mxNAMESPACE_END

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//

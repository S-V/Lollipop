/*
=============================================================================
	File:	EditorInterface.cpp
	Desc:	Editor support.
=============================================================================
*/

#include <Core_PCH.h>
#pragma hdrstop
#include <Core.h>

#include <Core/Editor.h>
#include <Core/Editor/EditableProperties.h>

NO_EMPTY_FILE

mxNAMESPACE_BEGIN

/*
-----------------------------------------------------------------------------
	Editable
-----------------------------------------------------------------------------
*/
mxDEFINE_ABSTRACT_CLASS(AEditable);

AEditable::AEditable(ENoInit)
{
	// empty.
}

AEditable::AEditable()
{
#if MX_EDITOR
	{
		gCore.editor->Notify_ObjectCreated(this);
	}
#endif // MX_EDITOR
}

AEditable::~AEditable()
{
	//DBGOUT("=== ~AEditable at address 0x%x\n", (void*)this );

#if MX_EDITOR
	//@fixme: check needed for statically allocated objects (with global lifetime)
	if( gCore.editor != nil )
	{
		gCore.editor->Notify_ObjectBeingDestroyed(this);
	}	
#endif // MX_EDITOR

}

void AEditable::edCreateProperties( EdPropertyList *properties, bool bClear )
{
	Assert( properties->edIndexOf(this) == INDEX_NONE );

	if( bClear ) {
		properties->Clear();
	}

	mxDBG_CHECK_VTBL( this );

#if MX_EDITOR
	(new EdProperty_ClassGuid( properties, "[Object:Type]", this->rttiGetTypeGuid() ));

	(new EdProperty_UInt32( properties, "[Object:Size]" ))
		->Bind< AObject, &AObject::GetInstanceSize >( *this );

	(new EdProperty_VoidPointer( properties, "[Object:Addr]" ))
		->Bind< typename AObject, &AObject::GetAddress >( *this );

	(new EdProperty_UInt32( properties, "[Editable:NumKids]" ))
		->Bind< AEditable, &AEditable::edNumRows >( *this );
#endif //MX_DEVELOPER
}

void AEditable::PostLoad()
{
	//Super::PostLoad( loadArgs );
	Super::PostLoad();

//#if MX_EDITOR
//	this->uiRefreshView();
//#endif // MX_EDITOR
}

void AEditable::uiRefreshView()
{
#if MX_EDITOR
	bool bPropagateToParentNodes = true;
	gCore.editor->Notify_ObjectModified( this, bPropagateToParentNodes );
#endif // MX_EDITOR
}

void AEditable::uiBeginRefresh()
{
#if MX_EDITOR
	gCore.editor->Notify_BeginResetModel( this );
#endif // MX_EDITOR
}

void AEditable::uiEndRefresh()
{
#if MX_EDITOR
	gCore.editor->Notify_EndResetModel( this );
#endif // MX_EDITOR
}

#if 0
void DbgDumpTree( Editable* node, UINT depth )
{
	AssertPtr(node);

	for(UINT i=0; i<depth; i++)
	{
		mxPutf("\t");
	}

	const UINT numKids = node->edNumRows();
	mxPutf("[%p] %s (%u kids)",node,node->edToChars(0),numKids);

	Editable* parent = node->edGetParent();
	if( parent != nil )
	{
		mxPutf(", parent: [%p] '%s'",parent,parent->edToChars(0));
	}
	mxPutf("\n");

	for(UINT i=0; i<numKids; i++)
	{
		Editable* kid = node->edItemAt(i);
		DbgDumpTree(kid,depth+1);
	}
}
#endif



/*
--------------------------------------------------------------
	AEditableRefCounted
--------------------------------------------------------------
*/
mxDEFINE_ABSTRACT_CLASS( AEditableRefCounted );

AEditableRefCounted::AEditableRefCounted()
{
	m__numReferences = 0;
}

AEditableRefCounted::~AEditableRefCounted()
{
	//Assert( m__numReferences == 0 );
#if MX_DEVELOPER
	if( m__numReferences != 0 )
	{
		mxWarnf("~Object '%s' with %u ref(s).\n",
			this->rttiGetTypeName(), (UINT)this->GetNumRefs()
			);
	}
#endif // MX_DEVELOPER

	//// check if this object hasn't yet been unregistered
	//if( m__internalArrayIndex != INDEX_NONE )
	//{
	//	// this shouldn't happen,
	//	// the object is likely to be allocated on stack
	//	this->Destruct( false );
	//}

	//AssertX( m__internalArrayIndex == INDEX_NONE,
	//	"The object must be removed from the global objects database at this point!" );
}

void AEditableRefCounted::DeleteThis() const
{
	delete this;
}

void AEditableRefCounted::Grab() const
{
	++m__numReferences;
}

void AEditableRefCounted::Drop() const
{
	Assert( m__numReferences > 0 );

	--m__numReferences;

	if ( m__numReferences == 0 )
	{
		// this object is no longer referenced,
		// it must be allocated on heap,
		// because someone who has kept a reference to it
		// (with 'strong', ref. counting pointers)
		// and is now dropping its ownership
		//
		this->DeleteThis();
	}
}

void AEditableRefCounted::edCreateProperties( EdPropertyList *properties, bool bClear )
{
	Super::edCreateProperties( properties, bClear );

#if MX_DEVELOPER
	(new EdProperty_UInt32( properties, "[Object:NumRefs]" ))
		->Bind< AEditableRefCounted, &AEditableRefCounted::GetNumRefs >( *this );
#endif //MX_DEVELOPER
}

/*
-----------------------------------------------------------------------------
	AEditorInterface
-----------------------------------------------------------------------------
*/
void AEditorInterface::Notify_ObjectModified( AEditable* node, bool bPropagateToParentNodes )
{
	this->Notify_ObjectModified( node );

	if( bPropagateToParentNodes )
	{
		AEditable* theParent = node->edGetParent();
		if( theParent != nil )
		{
			this->Notify_ObjectModified( theParent, bPropagateToParentNodes );
		}
	}
}

#if MX_EDITOR
void EdBeginInsertRows( AEditable* theParent, UINT startRow, UINT numRows )
{
	gCore.editor->Notify_BeginInsertRows( theParent, startRow, numRows );
}
void EdEndInsertRows( AEditable* theParent )
{
	gCore.editor->Notify_EndInsertRows( theParent );
}
void EdBeginRemoveRows( AEditable* theParent, UINT startRow, UINT numRows )
{
	gCore.editor->Notify_BeginRemoveRows( theParent, startRow, numRows );
}
void EdEndRemoveRows( AEditable* theParent )
{
	gCore.editor->Notify_EndRemoveRows( theParent );
}

#endif // #if MX_EDITOR


//---------------------------------------------------------------------------

namespace
{

	class NullEditorSystem : public AEditorInterface
	{
	public:
		NullEditorSystem()
		{}

		virtual ~NullEditorSystem()
		{}

		//=-- AEditorInterface
		// should be called immediately after a new object has been created
		virtual void Notify_ObjectCreated( AEditable* newObject ) override
		{}


		// NOTE: the passed argument can be null if deselected
		virtual void Notify_SelectionChanged( AEditable* theObject ) override
		{}

		// returns the currently selected object;
		// may return null
		virtual AEditable* GetSelectedObject() override
		{return nil;}

		virtual void Notify_ObjectModified( AEditable* theObject ) override
		{}

		// should be called before the object is destroyed
		virtual void Notify_ObjectBeingDestroyed( AEditable* theObject ) override
		{}

		// Begins a row insertion operation.
		// You must call this function before inserting data into the model's underlying data store.
		//
		virtual void Notify_BeginInsertRows( AEditable* parent, UINT iFirstRow, UINT iNumRows ) override
		{}


		// Ends a row insertion operation.
		// You must call this function after inserting data from the model's underlying data store.
		//
		virtual void Notify_EndInsertRows( AEditable* parent ) override
		{}


		// Begins a row removal operation.
		// You must call this function before removing data from the model's underlying data store.
		//
		virtual void Notify_BeginRemoveRows( AEditable* parent, UINT iFirstRow, UINT iNumRows ) override
		{}

		// Ends a row removal operation.
		// You must call this function after removing data from the model's underlying data store.
		//
		virtual void Notify_EndRemoveRows( AEditable* parent ) override
		{}

		// Begins a model reset operation.
		// When a model is reset it means that any previous data reported from the model is now invalid and has to be queried for again.
		// This also means that the current item and any selected items will become invalid.
		// You must call this function before resetting any internal data structures in your model or proxy model.
		//
		virtual void Notify_BeginResetModel( AEditable* parent ) override
		{}

		// Completes a model reset operation.
		// You must call this function after resetting any internal data structure in your model or proxy model.
		//
		virtual void Notify_EndResetModel( AEditable* parent ) override
		{}

		virtual bool Show_YesNo_Warning( const char* fmt, ... ) override
		{Unimplemented;return false;}


		static NullEditorSystem		gs_instance;
	};

	// static
	NullEditorSystem NullEditorSystem::gs_instance;

}//namespace

//---------------------------------------------------------------------------

//static
AEditorInterface* AEditorInterface::GetDummyInstance()
{
	return &NullEditorSystem::gs_instance;
}

//---------------------------------------------------------------------------

#if 0

/*
-----------------------------------------------------------------------------
	EdEventHandler

	used for game engine -> editor communication.
-----------------------------------------------------------------------------
*/
struct EdEventHandler
{
public:

	virtual void OnPropertyAdded( Editable* theSender, Editable* newProperty )
	{
		OnObjectModified(theSender);
	}
	virtual void OnPropertyRemoved( Editable* theSender, Editable* theProperty )
	{
		OnObjectModified(theSender);
	}
	virtual void OnPropertyChanged( Editable* theSender, Editable* theProperty )
	{
		OnObjectModified(theSender);
	}

	virtual void OnObjectModified( Editable* theSender )
	{}

	virtual void OnObjectDestroyed( Editable* theSender )
	{}

	virtual ~EdEventHandler() {}
};

/*
--------------------------------------------------------------
	EditableChild
--------------------------------------------------------------
*/
class EditableChild : Editable
{
public:
	// returns the parent of this node
	virtual Editable* edGetParent()
	{
		return m_parentEditable.Ptr;
	}

protected:
	EditableChild( Editable* parent = nil );
	~EditableChild();

protected:
	TPtr< Editable >	m_parentEditable;
};

/*
--------------------------------------------------------------
	DbgEdNode
	used mainly for testing and debugging
--------------------------------------------------------------
*/
struct DbgEdNode : public Editable, public ReferenceCounted
{
public:
	mxDECLARE_CLASS_ALLOCATOR(EMemHeap::DefaultHeap,DbgEdNode);
	mxDECLARE_CLASS(DbgEdNode,Editable);

	typedef TRefPtr< DbgEdNode >	Ref;

	DbgEdNode( const char* text )
		: mText( text )
	{
	}

	// returns the number of children
	virtual UINT edNumRows() const
	{
		return mKids.Num();
	}

	// returns the child of this node at the specified index
	virtual Editable* edItemAt( UINT index )
	{
		return mKids[ index ];
	}

	// returns -1 if the specified node was not found
	virtual UINT edIndexOf( const Editable* child ) const
	{
		return mKids.FindPtrIndex( child );
	}

	// returns the number of displayed columns
	virtual UINT uiNumColumns() const
	{
		return 1;
	}

	// returns the parent of this node
	virtual Editable* edGetParent()
	{
		return mParent.Ptr;
	}

	// used for displaying the object's info
	virtual const char* edToChars( UINT column ) const
	{
		return mText.ToChars();
	}

	void ReserveKids( UINT num )
	{
		mKids.Reserve( num );
	}

	void AddKid( DbgEdNode* node )
	{
		mKids.AddUnique( node );
		node->mParent = this;
	}

	void SetParent( Editable* newParent )
	{
		mParent = newParent;
	}

protected:
	DbgEdNode()
	{}
	~DbgEdNode()
	{}
protected:
	String					mText;
	TPtr< Editable >		mParent;
	TList< DbgEdNode::Ref >	mKids;
};

/*
-----------------------------------------------------------------------------
	EditableChild
-----------------------------------------------------------------------------
*/
EditableChild::EditableChild( Editable* parent )
{
	m_parentEditable = parent;
}

EditableChild::~EditableChild()
{}


/*
-----------------------------------------------------------------------------
	DbgEdNode
-----------------------------------------------------------------------------
*/
mxDEFINE_CLASS(DbgEdNode,Editable);

//---------------------------------------------------------------------------


#endif

mxNAMESPACE_END

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//

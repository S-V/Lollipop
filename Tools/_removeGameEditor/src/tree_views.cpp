#include <stdafx.h>
#pragma hdrstop
#include <QtSupport/common.h>
#include "editor_common.h"
#include "app.h"
#include "editor_system.h"
#include "tree_views.h"

#define ED_DEBUG_TREE_VIEW	(0)

/*
-----------------------------------------------------------------------------
	EdTreeViewModel
-----------------------------------------------------------------------------
*/
EdTreeViewModel::EdTreeViewModel( AEditable* root )
{
	this->setSupportedDragActions(Qt::ActionMask);

	//***********

	this->SetRootObject( root );

	MX_CONNECT_THIS( EdSystem::Get().Event_ObjectModified, EdTreeViewModel, OnDataModified );
	MX_CONNECT_THIS( EdSystem::Get().Event_ObjectBeingDestroyed, EdTreeViewModel, OnDataBeingDestroyed );

	MX_CONNECT_THIS( EdSystem::Get().Event_BeginInsertRows, EdTreeViewModel, OnBeginInsertRows );
	MX_CONNECT_THIS( EdSystem::Get().Event_EndInsertRows, EdTreeViewModel, OnEndInsertRows );

	MX_CONNECT_THIS( EdSystem::Get().Event_BeginRemoveRows, EdTreeViewModel, OnBeginRemoveRows );
	MX_CONNECT_THIS( EdSystem::Get().Event_EndRemoveRows, EdTreeViewModel, OnEndRemoveRows );

	MX_CONNECT_THIS( EdSystem::Get().Event_BeginResetModel, EdTreeViewModel, OnBeginResetModel );
	MX_CONNECT_THIS( EdSystem::Get().Event_EndResetModel, EdTreeViewModel, OnEndResetModel );
}

EdTreeViewModel::~EdTreeViewModel()
{
	this->CleanUp();

	MX_DISCONNECT_THIS( EdSystem::Get().Event_ObjectModified );
	MX_DISCONNECT_THIS( EdSystem::Get().Event_ObjectBeingDestroyed );

	MX_DISCONNECT_THIS( EdSystem::Get().Event_BeginInsertRows );
	MX_DISCONNECT_THIS( EdSystem::Get().Event_EndInsertRows );

	MX_DISCONNECT_THIS( EdSystem::Get().Event_BeginRemoveRows );
	MX_DISCONNECT_THIS( EdSystem::Get().Event_EndRemoveRows );

	MX_DISCONNECT_THIS( EdSystem::Get().Event_BeginResetModel );
	MX_DISCONNECT_THIS( EdSystem::Get().Event_EndResetModel );
}

void EdTreeViewModel::CleanUp()
{
	// avoids crashes when user clicks on non-existent (deleted) nodes
	this->reset();
}

void EdTreeViewModel::SetRootObject( AEditable* node )
{
	MX_DBG_CHECK_VTBL( node );

	this->CleanUp();

	m_rootItem = node;
}

void EdTreeViewModel::Refresh()
{
	if(ED_DEBUG_TREE_VIEW)
	{
		DBGOUT("Refreshing %s\n",
			this->metaObject()->className());
	}
	MX_UNDONE;
//MX_DEBUG_BREAK;
	MX_PERM(:);MX_HACK(:);MX_OPTIMIZE(:);
	emit layoutChanged();
}

QModelIndex EdTreeViewModel::IndexFromNode( AEditable* node )
{
	if ( node != nil )
	{
		AEditable* parentNode = node->edGetParent();
		if ( parentNode != nil )
		{
			const int row = parentNode->edIndexOf(node);
			const int column = node->uiColumn();
			return this->createIndex( row, column, node );
		}
	}
	return QModelIndex();
}

void EdTreeViewModel::OnDataModified( AEditable* pObject )
{
	VRET_IF_NOT( m_rootItem != nil );
	VRET_IF_NOT( pObject->edBelongsToTree( m_rootItem ) );

	if(ED_DEBUG_TREE_VIEW)
	{
		DBGOUT("Updating %s, because %s changed\n",
			this->metaObject()->className(),pObject->rttiGetTypeName());
	}
#if 0
	const QModelIndex	topLeft = this->IndexFromNode(pTopLeftNode);
	const QModelIndex	bottomRight = this->IndexFromNode(pBottomRightNode);
	emit dataChanged( topLeft, bottomRight );
#else
	const QModelIndex	objectIndex = this->IndexFromNode( pObject );
	emit dataChanged( objectIndex, objectIndex );
#endif

}

void EdTreeViewModel::OnDataBeingDestroyed( AEditable* pObject )
{
	VRET_IF_NOT( m_rootItem != nil );

	if( m_rootItem == pObject )
	{
		this->SetRootObject(nil);
	}
}

void EdTreeViewModel::OnBeginInsertRows( AEditable* parent, UINT iFirstRow, UINT iNumRows )
{
	VRET_IF_NOT( m_rootItem != nil );
	Assert( iNumRows > 0 );
	if( !parent->edBelongsToTree( m_rootItem ) )
	{
		return;
	}
	const QModelIndex	parentIndex = this->IndexFromNode( parent );
	this->beginInsertRows( parentIndex, iFirstRow, iFirstRow + iNumRows - 1 );

	if(ED_DEBUG_TREE_VIEW)
	{
		DBGOUT("Inserting %u rows into %s starting at %u\n",
		iNumRows,this->metaObject()->className(),iFirstRow);
	}
}

void EdTreeViewModel::OnEndInsertRows( AEditable* parent )
{
	VRET_IF_NOT( m_rootItem != nil );
	if( !parent->edBelongsToTree( m_rootItem ) )
	{
		return;
	}
	this->endInsertRows();
}

void EdTreeViewModel::OnBeginRemoveRows( AEditable* parent, UINT iFirstRow, UINT iNumRows )
{
	VRET_IF_NOT( m_rootItem != nil );
	Assert( iNumRows > 0 );

	if( !parent->edBelongsToTree( m_rootItem ) )
	{
		return;
	}
	const QModelIndex	parentIndex = this->IndexFromNode( parent );
	this->beginRemoveRows( parentIndex, iFirstRow, iFirstRow + iNumRows - 1 );

	if(ED_DEBUG_TREE_VIEW)
	{
		DBGOUT("Removing %u rows from %s starting at %u\n",
			iNumRows,this->metaObject()->className(),iFirstRow);
	}
}

void EdTreeViewModel::OnEndRemoveRows( AEditable* parent )
{
	VRET_IF_NOT( m_rootItem != nil );
	if( !parent->edBelongsToTree( m_rootItem ) )
	{
		return;
	}
	this->endRemoveRows();
}

void EdTreeViewModel::OnBeginResetModel( AEditable* parent )
{
	VRET_IF_NOT( m_rootItem != nil );
	if( !parent->edBelongsToTree( m_rootItem ) )
	{
		return;
	}
	this->beginResetModel();
}

void EdTreeViewModel::OnEndResetModel( AEditable* parent )
{
	VRET_IF_NOT( m_rootItem != nil );
	if( !parent->edBelongsToTree( m_rootItem ) )
	{
		return;
	}
	this->endResetModel();
}

void EdTreeViewModel::OnProjectLoaded()
{

}

void EdTreeViewModel::OnProjectUnloaded()
{
	// invalidate the model's internal state (e.g. persistent model indexes)
	// to avoid crashes
	this->reset();
}

AEditable* EdTreeViewModel::GetRootObject()
{
	return m_rootItem.Ptr;
}

QModelIndex EdTreeViewModel::index(int row, int column, const QModelIndex &parent) const
{
	if( !this->hasIndex(row, column, parent) ) {
		return QModelIndex();
	}

	AEditable* parentNode = parent.isValid() ?
		static_cast< AEditable* >( parent.internalPointer() )
		:
		m_rootItem
		;

	AEditable* childNode = parentNode->edItemAt(row);

	return (childNode != nil) ?
		this->createIndex(row, column, childNode) : QModelIndex();
}

QModelIndex EdTreeViewModel::parent(const QModelIndex &child) const
{
	if( nil == m_rootItem ) {
		return QModelIndex();
	}
	if( !child.isValid() ) {
		return QModelIndex();
	}

	AEditable* childItem = static_cast<AEditable*>(child.internalPointer());
	if( nil == childItem ) {
		return QModelIndex();
	}

	MX_DBG_CHECK_VTBL(childItem);
	//qDebug( "TreeModel: %s\n", this->metaObject()->className() );
	//DBGOUT("\n**** childItem addr = 0x%x\n",childItem);

	AEditable* parentItem = childItem->edGetParent();

	MX_DBG_CHECK_VTBL(parentItem);

	if( parentItem == m_rootItem ) {
		return QModelIndex();
	}

	// shouldn't happen, but just in case
	if( nil == parentItem ) {
		return QModelIndex();
	}

	return this->createIndex(parentItem->uiRow(), child.column(), parentItem);
}

int EdTreeViewModel::rowCount(const QModelIndex &parent) const
{
	return parent.isValid() ?
		this->NodeFromIndex( parent )->edNumRows()
		:
		( m_rootItem != nil ) ? m_rootItem->edNumRows() : 0
		;
}

int EdTreeViewModel::columnCount(const QModelIndex &parent) const
{
	return parent.isValid() ?
		this->NodeFromIndex( parent )->uiNumColumns()
		:
		( m_rootItem != nil ) ? m_rootItem->uiNumColumns() : 0
		;
}

bool EdTreeViewModel::hasChildren(const QModelIndex &index) const
{
	return Super::hasChildren(index);
}

QVariant EdTreeViewModel::data(const QModelIndex &index, int role ) const
{
	if( !index.isValid() ) {
		return QVariant();
	}
	if( Qt::DecorationRole == role )
	{
		AEditable* node = this->NodeFromIndex(index);
		if( node != nil ) {
			return this->GetIcon(node);
		}
	}
	if (role != Qt::DisplayRole) {
		return QVariant();
	}

	//const int	rowIndex	= index.row();
	const int	columnIndex = index.column();

	AEditable* node = this->NodeFromIndex( index );
	AssertPtr(node);

	return node->edToChars( columnIndex );
}

//@todo: attempts at renaming destroy previous value
bool EdTreeViewModel::setData( const QModelIndex &index, const QVariant &value, int role )
{
	const bool bResult = Super::setData( index, value, role );
	if( Qt::EditRole == role )
	{
		const QString newName = value.toString();
		if( !newName.isEmpty() )
		{
			AEditable* node = this->NodeFromIndex( index );
			if( node != nil ) {
				if( node->edCanBeRenamed() ) {
					node->edSetName( newName.toAscii().data() );
				}
				return true;
			}
		}
	}
	return bResult;
}

QVariant EdTreeViewModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
	{
		if ( m_rootItem != nil )
		{
			return m_rootItem->edToChars(section);
		}
	}
	return QVariant();
}

bool EdTreeViewModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value,
									int role )
{
	return Super::setHeaderData(section, orientation, value, role);
}

QMap<int, QVariant> EdTreeViewModel::itemData(const QModelIndex &index) const
{
	return Super::itemData(index);
}

bool EdTreeViewModel::setItemData(const QModelIndex &index, const QMap<int, QVariant> &roles)
{
	return Super::setItemData( index, roles );
}

QStringList EdTreeViewModel::mimeTypes() const
{
	return Super::mimeTypes();
}

QMimeData* EdTreeViewModel::mimeData(const QModelIndexList &indexes) const
{
	return Super::mimeData( indexes );
}

bool EdTreeViewModel::dropMimeData(const QMimeData *data, Qt::DropAction action,
								   int row, int column, const QModelIndex &parent)
{
	return Super::dropMimeData(data, action, row, column, parent);
}

Qt::DropActions EdTreeViewModel::supportedDropActions() const
{
	return Super::supportedDropActions();
}

Qt::ItemFlags EdTreeViewModel::flags(const QModelIndex &index) const
{
	if (!index.isValid()) {
		return 0;
	}
	return Qt::ItemIsEnabled
		| Qt::ItemIsSelectable
		| Qt::ItemIsEditable
		| Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled
		;
}

void EdTreeViewModel::sort(int column, Qt::SortOrder order )
{
	Super::sort(column, order);
}

QIcon EdTreeViewModel::GetIcon( AEditable* node ) const
{
	return QIcon();
}
QIcon EdTreeViewModel::GetIcon( const QModelIndex& index ) const
{
	return this->GetIcon( this->NodeFromIndex(index) );
}

AEditable* EdTreeViewModel::NodeFromIndex(const QModelIndex &index) const
{
	return index.isValid() ?
		static_cast< AEditable* >( index.internalPointer() ) : m_rootItem;
}

QModelIndex EdTreeViewModel::GetRootIndex() const
{
	if ( nil == m_rootItem ) {
		return QModelIndex();
	}
	return this->createIndex( 0, 0, m_rootItem );
}

static AEditable* ObjectFromSelection( const QItemSelection& selection )
{
	QModelIndexList	selectedIndices( selection.indexes() );
	if( selectedIndices.count() > 0 )
	{
		//Assert( selectedIndices.count() == 1 );
		if( selectedIndices.count() > 1 ) {
			DBGOUT("[%s]: Selected %u indices, but impl support for only one\n",
				__FUNCTION__,selectedIndices.count());
		}

		QModelIndex& selectedIndex = selectedIndices.first();

		AEditable* pObject = static_cast< AEditable* >( selectedIndex.internalPointer() );

		return pObject;
	}
	return nil;
}


/*================================
	EdTreeView
================================*/

EdTreeView::EdTreeView( QWidget *parent )
	: Super( parent )
{
	this->header()->setDefaultAlignment(Qt::AlignCenter);

	EditorApp::ValidOnlyWhenProjectIsLoaded( this );

	this->setAcceptDrops(true);
	this->setAutoExpandDelay(1000);	// 1 sec

	this->setDragDropMode(DragDropMode::DragDrop);
	this->setDropIndicatorShown(true);

	this->setAutoScroll(true);
	this->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	this->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

	//this->setContextMenuPolicy(Qt::CustomContextMenu);

	//autoExpandDepth = 0;
	bAutoExpandNodes = true;

	connect( this, SIGNAL(pressed(QModelIndex)), this, SLOT(slot_OnItemPressed(QModelIndex)) );
	connect( this, SIGNAL(clicked(QModelIndex)), this, SLOT(slot_OnItemClicked(QModelIndex)) );
	connect( this, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(slot_OnItemDoubleClicked(QModelIndex)) );
}

EdTreeView::~EdTreeView()
{

}

void EdTreeView::AutoExpandSelf( const QModelIndex& index )
{
	//if( autoExpandDepth == 0 ) {
	//	return;
	//}
	//if( autoExpandDepth == -1 ) {
	//	this->expandAll();
	//	return;
	//}
	////this->expandToDepth( autoExpandDepth );
	//this->expand( index );
	////this->expandAll();

	if( bAutoExpandNodes )
	{
		this->expand( index );
	}
}

void EdTreeView::setModel( QAbstractItemModel* theModel )
{
	QAbstractItemModel* currentModel = Super::model();
	(void)currentModel;

	Super::setModel( theModel );

	this->AutoExpandSelf(QModelIndex());
}

//void EdTreeView::OnSelectionChanged( Editable* pSelected, Editable* pDeselected )
//{
//	if( pSelected != nil )
//	{
//		this->OnItemSelected(pSelected);
//	}
//
//	MX_UNUSED(pDeselected);
//}

void EdTreeView::OnItemPressed( AEditable* pObject )
{
	this->OnItemSelected( pObject );
}

void EdTreeView::OnItemClicked( AEditable* pObject )
{
	this->OnItemSelected( pObject );
}

void EdTreeView::OnItemDoubleClicked( AEditable* pObject )
{
	this->OnItemSelected( pObject );
}

void EdTreeView::OnItemSelected( AEditable* pObject )
{
	if( pObject != nil ) {
		//EdSystem::Get().Notify_SelectionChanged( pObject->edGetTreeViewSelectionProxy() );
		EdSystem::Get().Notify_SelectionChanged( pObject );
	}
}

void EdTreeView::mouseDoubleClickEvent( QMouseEvent * theEvent )
{
	Super::mouseDoubleClickEvent( theEvent );
}

void EdTreeView::keyPressEvent( QKeyEvent * theEvent )
{
	Super::keyPressEvent( theEvent );

	//const int key = theEvent->key();
	//if( key == Qt::Key_Return || key == Qt::Key_Enter )
	//{
	//	this->TryToSelectItem();
	//}
}

void EdTreeView::contextMenuEvent( QContextMenuEvent * theEvent )
{
	//Super::contextMenuEvent( theEvent );

	const QModelIndex& modelIndex = this->indexAt( theEvent->pos() );
	AEditable* pObject = this->NodeFromIndex( modelIndex );

	if( pObject != nil )
	{
		EdShowContextMenuArgs	args;

		//args.pObject = pObject;

		args.posX = theEvent->globalPos().x();
		args.posY = theEvent->globalPos().y();

		pObject->edShowContextMenu( args );
	}
}

void EdTreeView::selectionChanged( const QItemSelection& selected, const QItemSelection& deselected )
{
	Super::selectionChanged( selected, deselected );

	AEditable* pSelected = ObjectFromSelection( selected );
	AEditable* pDeselected = ObjectFromSelection( deselected );

	//this->OnSelectionChanged( pSelected, pDeselected );
	this->OnItemSelected( pSelected );
}

void EdTreeView::dataChanged( const QModelIndex& topLeft, const QModelIndex& bottomRight )
{
	Super::dataChanged( topLeft, bottomRight );

	if( bAutoExpandNodes ) {
		this->ExpandToIndex( bottomRight );
	}
}

void EdTreeView::rowsInserted( const QModelIndex & parent, int start, int end )
{
	Super::rowsInserted( parent, start, end );

	if( bAutoExpandNodes ) {
		this->ExpandToIndex( parent );
	}
}

void EdTreeView::slot_OnItemPressed( const QModelIndex& index )
{
	AEditable* pObject = this->NodeFromIndex( index );
	if( pObject != nil )
	{
		this->OnItemPressed( pObject );
	}
}

void EdTreeView::slot_OnItemClicked( const QModelIndex& index )
{
	AEditable* pObject = this->NodeFromIndex( index );
	if( pObject != nil )
	{
		this->OnItemClicked( pObject );
	}
}

void EdTreeView::slot_OnItemDoubleClicked( const QModelIndex& index )
{
	AEditable* pObject = this->NodeFromIndex( index );
	if( pObject != nil )
	{
		this->OnItemDoubleClicked( pObject );
	}
}

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//

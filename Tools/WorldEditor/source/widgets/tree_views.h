#pragma once

#include <Core/Editor.h>

#include <QtSupport/qt_common.h>

#include "editor_common.h"

inline
void ExpandTreeToIndex( QTreeView *tv, const QModelIndex& index )
{
	if( index.isValid() )
	{
		tv->expand( index );
		ExpandTreeToIndex( tv, index.parent() );
	}
}

/*
-----------------------------------------------------------------------------
	EdTreeViewModel
-----------------------------------------------------------------------------
*/
class EdTreeViewModel : public QAbstractItemModel, public EdProjectChild
{
	Q_OBJECT

public:
	typedef QAbstractItemModel Super;

	EdTreeViewModel( AEditable* root = nil );
	~EdTreeViewModel();

	void SetRootObject( AEditable* node );
	AEditable* GetRootObject();

	// can be slow
	void Refresh();

public:
	virtual void OnProjectLoaded() override;
	virtual void OnProjectUnloaded() override;

	inline QModelIndexList getPersistentIndexList() const
	{
		return this->persistentIndexList();
	}

	QIcon GetIcon( const QModelIndex& index ) const;

protected:
	virtual QIcon GetIcon( AEditable* node ) const;

public: // Overrides QAbstractItemModel

	virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
	virtual QModelIndex parent(const QModelIndex &child) const override;

	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override;
	virtual int columnCount(const QModelIndex &parent = QModelIndex()) const override;
	virtual bool hasChildren(const QModelIndex &index = QModelIndex()) const override;

	virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
	virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

	virtual bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value,
		int role = Qt::EditRole) override;

	virtual QMap<int, QVariant> itemData(const QModelIndex &index) const override;
	virtual bool setItemData(const QModelIndex &index, const QMap<int, QVariant> &roles) override;

	virtual QStringList mimeTypes() const override;
	virtual QMimeData* mimeData(const QModelIndexList &indexes) const override;
	virtual bool dropMimeData(const QMimeData *data, Qt::DropAction action,
		int row, int column, const QModelIndex &parent) override;
	virtual Qt::DropActions supportedDropActions() const override;

	virtual Qt::ItemFlags flags(const QModelIndex &index) const override;

	virtual void sort(int column, Qt::SortOrder order = Qt::AscendingOrder) override;

public:
	QModelIndex IndexFromNode( AEditable* node );
	AEditable* NodeFromIndex(const QModelIndex &index) const;

	// needed for expanding the tree
	QModelIndex GetRootIndex() const;

protected:
	//virtual void OnDataModified( Editable* topLeft, Editable* bottomRight );
	virtual void OnDataModified( AEditable* pObject );
	virtual void OnDataBeingDestroyed( AEditable* pObject );

private:
	void OnBeginInsertRows( AEditable* parent, UINT iFirstRow, UINT iNumRows );
	void OnEndInsertRows( AEditable* parent );

	void OnBeginRemoveRows( AEditable* parent, UINT iFirstRow, UINT iNumRows );
	void OnEndRemoveRows( AEditable* parent );

	void OnBeginResetModel( AEditable* parent );
	void OnEndResetModel( AEditable* parent );

private:
	void CleanUp();

private:
	mutable TPtr< AEditable >	m_rootItem;
};


/*
-----------------------------------------------------------------------------
	tree_views
-----------------------------------------------------------------------------
*/
class EdTreeView : public QTreeView
{
	Q_OBJECT

public:
	typedef QTreeView Super;

	EdTreeView( QWidget *parent = nil );
	~EdTreeView();

public:
	// 0 - don't expand, -1 - expand all nodes
	//UINT	autoExpandDepth;

	// expand nodes if something changed
	bool	bAutoExpandNodes;// by default is true

public:
	inline AEditable* NodeFromIndex( const QModelIndex& index )
	{
		AEditable* pObject = static_cast< AEditable* >( index.internalPointer() );
		return pObject;
	}
	inline void ExpandToIndex( const QModelIndex& modelIndex )
	{
		ExpandTreeToIndex( this, modelIndex );
	}

public:
	virtual void setModel( QAbstractItemModel* theModel );

protected:	//-- tree_views
	//virtual void OnSelectionChanged( Editable* pSelected, Editable* pDeselected );
	virtual void OnItemPressed( AEditable* pObject );
	virtual void OnItemClicked( AEditable* pObject );
	virtual void OnItemDoubleClicked( AEditable* pObject );
	virtual void OnItemSelected( AEditable* pObject );

protected:	//-- QTreeView
	virtual void mouseDoubleClickEvent( QMouseEvent * event ) override;

	virtual void keyPressEvent( QKeyEvent * event ) override;

	virtual void contextMenuEvent( QContextMenuEvent * event ) override;

	virtual void selectionChanged( const QItemSelection& selected, const QItemSelection& deselected ) override;

	virtual void dataChanged( const QModelIndex& topLeft, const QModelIndex& bottomRight ) override;

	virtual void rowsInserted( const QModelIndex & parent, int start, int end ) override;

private slots:
	void slot_OnItemPressed( const QModelIndex& index );
	void slot_OnItemClicked( const QModelIndex& index );
	void slot_OnItemDoubleClicked( const QModelIndex& index );

private:
	void AutoExpandSelf(const QModelIndex& index);
};













/*
-----------------------------------------------------------------------------
	TEditableTreeView
-----------------------------------------------------------------------------
*/
template< class ITEM_MODEL = EdTreeViewModel >
class TEditableTreeView : public EdTreeView
{
public:
	typedef EdTreeView Super;
	typedef ITEM_MODEL ModelType;

	TEditableTreeView( QWidget *parent = nil )
		: Super( parent )
	{
	}
	~TEditableTreeView()
	{
	}

	ITEM_MODEL* GetModel()
	{
		QAbstractItemModel* pAbstractModel = this->model();
		if( nil == pAbstractModel ) {
			return nil;
		}
		ITEM_MODEL* pItemModel = checked_cast< ITEM_MODEL* >( pAbstractModel );
		return pItemModel;
	}

	AEditable* NodeAtPos( const QPoint& pos )
	{
		const QModelIndex modelIndex = this->indexAt( pos );
		if( !modelIndex.isValid() ) {
			return nil;
		}
		ITEM_MODEL* pItemModel = this->GetModel();
		if( nil == GetModel() ) {
			return nil;
		}
		AEditable* pObject = pItemModel->NodeFromIndex( modelIndex );
		return pObject;
	}

	template< class KLASS >	// where KLASS : Editable
	KLASS* GetSelectedObject()
	{
		ITEM_MODEL* pItemModel = this->GetModel();
		if( nil == GetModel() ) {
			return nil;
		}
		const QModelIndexList selectedOnes = this->selectedIndexes();
		//Assert(selectedOnes.count() == 1);
		if( selectedOnes.count() > 0 )
		{
			if( selectedOnes.count() > 1 )
			{
				ShowQMessageBox("This operation can not be applied to more than one object.\nPlease select only one item.");
				return nil;
			}

			const QModelIndex& modelIndex = selectedOnes.first();

			AEditable * pObject = pItemModel->NodeFromIndex( modelIndex );
			return SafeCast<KLASS>( pObject );
		}
		return nil;
	}


protected:
	virtual void dragEnterEvent( QDragEnterEvent* theEvent ) override
	{
		Super::dragEnterEvent( theEvent );
	}
	virtual void dragLeaveEvent( QDragLeaveEvent* theEvent ) override
	{
		Super::dragLeaveEvent( theEvent );
	}
	virtual void dragMoveEvent( QDragMoveEvent* theEvent ) override
	{
		Super::dragMoveEvent( theEvent );
	}
	virtual void dropEvent( QDropEvent* theEvent ) override
	{
		Super::dropEvent( theEvent );
	}
	virtual void keyPressEvent( QKeyEvent * theEvent ) override
	{
		QTreeView::keyPressEvent( theEvent );

		const int key = theEvent->key();
		if( key == Qt::Key_Return || key == Qt::Key_Enter
			|| KeyCodeIsDirection( key ) )
		{
			this->TryToSelectObject();
		}

		if( key == Qt::Key_Delete )
		{
			AEditable* pObject = this->GetSelectedObject<AEditable>();
			if( pObject != nil )
			{
				pObject->edRemoveSelf();
			}
		}
	}
	virtual void mouseDoubleClickEvent( QMouseEvent* theEvent ) override
	{
		Super::mouseDoubleClickEvent( theEvent );

		this->SelectObjectAt( theEvent->pos() );
	}
	virtual void mousePressEvent( QMouseEvent* theEvent ) override
	{
		Super::mousePressEvent( theEvent );

		this->SelectObjectAt( theEvent->pos() );
	}
	virtual void mouseMoveEvent( QMouseEvent* theEvent ) override
	{
		Super::mouseMoveEvent( theEvent );
	}

private:
	void SelectObjectAt( const QPoint& pos )
	{
		AEditable* pObject = this->NodeAtPos( pos );
		if( pObject != nil )
		{
			this->OnItemSelected( pObject );
		}
	}
	void TryToSelectObject()
	{
		AEditable * pObject = this->GetSelectedObject<AEditable>();
		if( pObject != nil )
		{
			this->OnItemSelected( pObject );
		}
	}
};

/*
-----------------------------------------------------------------------------
	TEditableTreeWidget

	derives from tree view,
	contains the item model
-----------------------------------------------------------------------------
*/
template
<
	class ITEM = AEditable,	// where ITEM : Editable
	class ITEM_MODEL = EdTreeViewModel
>
class TEditableTreeWidget : public TEditableTreeView< ITEM_MODEL >
{
	ITEM_MODEL		m_model;

public:
	typedef TEditableTreeView Super;

	TEditableTreeWidget( AEditable* rootObject, QWidget *parent = nil )
		: Super( parent )
	{
		AssertPtr(rootObject);
		m_model.SetRootObject( rootObject );
		this->setModel( &m_model );
	}
	~TEditableTreeWidget()
	{
	}

protected:


#if 0
	virtual void keyPressEvent( QKeyEvent* theEvent ) override
	{
		Super::keyPressEvent( theEvent );

		const int key = theEvent->key();
		if( key == Qt::Key_Delete )
		{
			Editable * pObject = this->GetSelectedObject();
			if( pObject != nil )
			{
				GetItemManagerPtr()->edRemoveChild( pObject );
			}
		}
	}
#endif
};

#ifndef SHADERLISTVIEW_H
#define SHADERLISTVIEW_H

#include <Base/Memory/Pool/TMemoryPool.h>
//#include <Base/Memory/BlockAlloc/Pool.h>

/*
--------------------------------------------------------------
	GpuResourcesTreeView
--------------------------------------------------------------
*/
class GpuResourcesTreeView : public QTreeView
{
public:
	GpuResourcesTreeView(QWidget *parent = 0);
	~GpuResourcesTreeView();

protected:
	virtual void mouseDoubleClickEvent( QMouseEvent * event );

	virtual void keyPressEvent( QKeyEvent * event );

	virtual void contextMenuEvent( QContextMenuEvent * event );

private:
	void loadSelectedShader();
};

/*
--------------------------------------------------------------
	GpuResourcesModel
--------------------------------------------------------------
*/

struct GpuResourceNode
{
	MX_DECLARE_CLASS_ALLOCATOR(EMemHeap::HeapGeneric,GpuResourceNode);

	enum Columns
	{
		Column_Default = 0,

		Column_Count
	};

	// NOTE: the order is important (optimized for range checks)
	enum Type
	{
		// default item type
		Node_Default = 0,

		// gpu program
		Node_Shader,

		// folder
		Node_ShadersGroup,
		Node_SamplerStatesGroup,
		Node_RasterizerStatesGroup,
		Node_DepthStencilStatesGroup,

		// marker. don't use.
		Node_Count
	};
	static bool IsFolder( Type nodeType )
	{
		return nodeType >= Node_ShadersGroup && nodeType <= Node_DepthStencilStatesGroup;
	}

	GpuResourceNode( Type type, GpuResourceNode* parentNode = nil )
		: nodeType( type )
	{
		parent = parentNode;
	}
	~GpuResourceNode()
	{
		this->kids.DeleteContents();
	}

	UINT Row() const
	{
		if( this->parent != nil )
		{
			GpuResourceNode* thisPtr = const_cast<GpuResourceNode*>( this );
			const UINT index = this->parent->kids.FindIndexOf( thisPtr );
			Assert( index != INDEX_NONE );
			return index;
		}
		return 0;
	}
	UINT NumKids() const
	{
		return this->kids.Num();
	}
	UINT NumColumns() const
	{
		return Column_Count;
	}
	GpuResourceNode* ChildAt( UINT row )
	{
		if( this->kids.IsValidIndex( row ) ) {
			return this->kids.ToPtr()[ row ];
		}
		return nil;
	}
	void AddKid( GpuResourceNode* newChild )
	{
		newChild->parent = this;
		this->kids.Add( newChild );
	}
	GpuResourceNode* FindKidByType( Type type )
	{
		for( UINT iKid = 0; iKid < this->kids.Num(); iKid++ )
		{
			GpuResourceNode* kid = this->kids[ iKid ];
			if( kid->nodeType == type )
			{
				return kid;
			}
		}
		return nil;
	}
	QVariant GetData( UINT column ) const
	{
		return this->text;
	}

	bool isFolder() const
	{
		return IsFolder( this->nodeType );
	}
	bool isShader() const
	{
		return this->nodeType == Node_Shader;
	}

	void DeleteKids()
	{
		this->kids.DeleteContents();
		this->kids.Empty();
	}

	PREVENT_COPY(GpuResourceNode);

public_internal:
	QString		text;
	TPtr<GpuResourceNode>	parent;
	TList<GpuResourceNode*>	kids;
	const Type	nodeType;
};


class GpuResourcesModel : public QAbstractItemModel
{
public:
	GpuResourcesModel();
	~GpuResourcesModel();

public: // Overrides QAbstractItemModel

	virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
	virtual QModelIndex parent(const QModelIndex &child) const;

	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
	virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
	virtual bool hasChildren(const QModelIndex &index = QModelIndex()) const;

	virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
	virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

	virtual bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value,
		int role = Qt::EditRole);

	virtual QMap<int, QVariant> itemData(const QModelIndex &index) const;
	virtual bool setItemData(const QModelIndex &index, const QMap<int, QVariant> &roles);

	virtual QStringList mimeTypes() const;
	virtual QMimeData* mimeData(const QModelIndexList &indexes) const;
	virtual bool dropMimeData(const QMimeData *data, Qt::DropAction action,
		int row, int column, const QModelIndex &parent);
	virtual Qt::DropActions supportedDropActions() const;

	virtual Qt::ItemFlags flags(const QModelIndex &index) const;

	virtual void sort(int column, Qt::SortOrder order = Qt::AscendingOrder);

public:
	QIcon GetIcon(const QModelIndex &index) const;

public:
	

private:
	GpuResourceNode* NodeFromIndex(const QModelIndex &index) const
	{
		Assert(index.isValid());
		GpuResourceNode* node = cast(GpuResourceNode*) index.internalPointer();
		return node;
	}
	GpuResourceNode* ParentNodeFromIndex(const QModelIndex &parent) const
	{
		GpuResourceNode* parentNode = parent.isValid()
										? NodeFromIndex( parent ) : &root;
		return parentNode;
	}

private:
	mutable GpuResourceNode	root;

	GpuResourceNode* node_ShadersFolder;
	GpuResourceNode* node_SamplerStatesFolder;

	//QIcon	icons[ GpuResourceNode::Node_Count ];
};


#include <QDockWidget>
#include "ui_gpuresourcesview.h"

/*
--------------------------------------------------------------
	GpuResourcesView
--------------------------------------------------------------
*/
class GpuResourcesView : public QDockWidget
	, public TSingleton< GpuResourcesView >
{
	Q_OBJECT

public:
	GpuResourcesView(QWidget *parent = 0);
	~GpuResourcesView();

	void OnConnectedTo( Socket socket );

private:
	Ui::GpuResourcesView ui;

	GpuResourcesModel	gpuResourcesModel;

	QTreeView *		treeView;

public_internal:

};

#endif // SHADERLISTVIEW_H

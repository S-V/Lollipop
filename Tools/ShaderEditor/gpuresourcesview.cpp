#include "StdAfx.h"
#include "gpuresourcesview.h"

#include "shadereditor.h"

/*================================
	GpuResourcesTreeView
================================*/

GpuResourcesTreeView::GpuResourcesTreeView(QWidget *parent )
	: QTreeView( parent )
{

}

GpuResourcesTreeView::~GpuResourcesTreeView()
{

}

void GpuResourcesTreeView::mouseDoubleClickEvent( QMouseEvent * event )
{
	QTreeView::mouseDoubleClickEvent(event);

	loadSelectedShader();
}

void GpuResourcesTreeView::keyPressEvent( QKeyEvent * event )
{
	QTreeView::keyPressEvent(event);

	const int key = event->key();
	if( key == Qt::Key_Return
		|| key == Qt::Key_Enter )
	{
		loadSelectedShader();
	}
}

void GpuResourcesTreeView::contextMenuEvent( QContextMenuEvent * event )
{
	QTreeView::contextMenuEvent(event);
}

void GpuResourcesTreeView::loadSelectedShader()
{
	GpuResourcesModel* myModel = checked_cast< GpuResourcesModel* >( this->model() );

	QModelIndexList selectedIndices = this->selectedIndexes();
	foreach( QModelIndex index, selectedIndices )
	{
		GpuResourceNode* node = myModel->NodeFromIndex(index);
		if(node->isFolder()) {
			continue;
		}

		if(node->isShader()) {
			ShaderEditor::Get().LoadShader(node->text);
		}
		//GpuResourceNode* parentNode = node->parent;
	}
}

/*================================
	GpuResourcesModel
================================*/

GpuResourcesModel::GpuResourcesModel()
	: root(GpuResourceNode::Node_Default)
{
	//icons[ GpuResourceNode::Node_Count ];
	{
		node_ShadersFolder = new GpuResourceNode(GpuResourceNode::Node_ShadersGroup);
		node_ShadersFolder->text = "Shaders";

		this->root.AddKid( node_ShadersFolder );
	}
	{
		node_SamplerStatesFolder = new GpuResourceNode(GpuResourceNode::Node_SamplerStatesGroup);
		node_SamplerStatesFolder->text = "Sampler states";

		this->root.AddKid( node_SamplerStatesFolder );
	}
}

GpuResourcesModel::~GpuResourcesModel()
{

}

QModelIndex GpuResourcesModel::index(int row, int column, const QModelIndex &parent) const
{
	if (!hasIndex(row, column, parent)) {
		return QModelIndex();
	}

	GpuResourceNode* parentNode = ParentNodeFromIndex( parent );

	GpuResourceNode* childNode = parentNode->ChildAt(row);

	return childNode ? createIndex(row, column, childNode) : QModelIndex();
}

QModelIndex GpuResourcesModel::parent(const QModelIndex &child) const
{
	if (!child.isValid()) {
		return QModelIndex();
	}

	GpuResourceNode* childNode = cast(GpuResourceNode*) child.internalPointer();
	GpuResourceNode* parentNode = childNode->parent;

	if( &root == parentNode ) {
		return QModelIndex();
	}

	return createIndex(parentNode->Row(), 0, parentNode);
}

int GpuResourcesModel::rowCount(const QModelIndex &parent) const
{
	if (parent.column() > 0)
		return 0;

	GpuResourceNode* parentNode = ParentNodeFromIndex( parent );

	return parentNode->NumKids();
}

int GpuResourcesModel::columnCount(const QModelIndex &parent) const
{
	GpuResourceNode* parentNode = ParentNodeFromIndex( parent );

	return parentNode->NumColumns();
}

bool GpuResourcesModel::hasChildren(const QModelIndex &index) const
{
	return __super::hasChildren(index);
}

QVariant GpuResourcesModel::data(const QModelIndex &index, int role ) const
{
	if (!index.isValid()) {
		return QVariant();
	}
	if( Qt::DecorationRole == role )
	{
		return GetIcon(index);
	}
	if (role != Qt::DisplayRole) {
		return QVariant();
	}
	GpuResourceNode* node = NodeFromIndex(index);

	return node->GetData(index.column());
}

bool GpuResourcesModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
	return __super::setData(index, value, role);
}

QVariant GpuResourcesModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
	{
		return root.GetData(section);
	}
	return QVariant();

	//return __super::headerData(section, orientation, role);
}

bool GpuResourcesModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value,
									  int role )

{
	return __super::setHeaderData(section, orientation, value, role);
}

QMap<int, QVariant> GpuResourcesModel::itemData(const QModelIndex &index) const
{
	return __super::itemData(index);
}

bool GpuResourcesModel::setItemData(const QModelIndex &index, const QMap<int, QVariant> &roles)
{
	return __super::setItemData(index, roles);
}

QStringList GpuResourcesModel::mimeTypes() const
{
	return __super::mimeTypes();
}

QMimeData* GpuResourcesModel::mimeData(const QModelIndexList &indexes) const
{
	return __super::mimeData(indexes);
}

bool GpuResourcesModel::dropMimeData(const QMimeData *data, Qt::DropAction action,
									 int row, int column, const QModelIndex &parent)
{
	return __super::dropMimeData(data, action, row, column, parent);
}

Qt::DropActions GpuResourcesModel::supportedDropActions() const
{
	return __super::supportedDropActions();
}

Qt::ItemFlags GpuResourcesModel::flags(const QModelIndex &index) const
{
	if (!index.isValid()) {
		return 0;
	}
	return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
	//return __super::flags( index );
}

void GpuResourcesModel::sort(int column, Qt::SortOrder order )
{
	__super::sort(column, order);
}

QIcon GpuResourcesModel::GetIcon(const QModelIndex &index) const
{
	GpuResourceNode* node = NodeFromIndex(index);
	if( node->isShader() )
	{
		//return QIcon(":/images/Resources/directx.ico");
	}

	return QIcon();
}

/*================================
	GpuResourcesView
================================*/

GpuResourcesView::GpuResourcesView(QWidget *parent)
	: QDockWidget(parent)
{
	ui.setupUi(this);

	this->setAttribute( Qt::WidgetAttribute::WA_DeleteOnClose, false );

	this->treeView = new GpuResourcesTreeView(this);
	ui.gridLayout->addWidget(this->treeView, 0, 0, 1, 1);

	this->treeView->setModel( &this->gpuResourcesModel );
	this->treeView->setHeaderHidden(true);
}

GpuResourcesView::~GpuResourcesView()
{

}

void GpuResourcesView::OnConnectedTo( Socket socket )
{
	Cmd_GetLoadedShadersList	getShaders;
	ShaderEditor::Get().cmdExec.ExecuteCommand( socket, getShaders );

	Cmd_GetLoadedShadersList::Results & results = getShaders.results;


	this->gpuResourcesModel.node_ShadersFolder->DeleteKids();
	this->gpuResourcesModel.node_SamplerStatesFolder->DeleteKids();


	const UINT numLoadedShaders = results.loadedShaders.Num();
	if( numLoadedShaders )
	{
		GpuResourceNode* node_ShadersFolder = this->gpuResourcesModel.node_ShadersFolder;

		for( UINT iShader = 0; iShader < numLoadedShaders; iShader++ )
		{
			const FixedString& shaderName = results.loadedShaders[ iShader ];

			GpuResourceNode* newNode = new GpuResourceNode(GpuResourceNode::Node_Shader);
			newNode->text = shaderName.ToChars();
			
			node_ShadersFolder->AddKid( newNode );
		}
	}

	this->treeView->expandAll();
}


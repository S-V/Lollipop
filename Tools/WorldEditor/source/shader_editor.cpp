#include "stdafx.h"

#include <Graphics/DX11/DX11Private.h>
#include <Graphics/Graphics_DX11.h>

#include <Renderer/Renderer.h>
#include <Renderer/GPU/Main.hxx>

#include "main_window_frame.h"
#include "editor_app.h"

#include "shader_editor.h"


/*
-----------------------------------------------------------------------------
	EdShaderOptionsListView
-----------------------------------------------------------------------------
*/
EdShaderOptionsListView::EdShaderOptionsListView()
{
	connect( this, SIGNAL(activated(QModelIndex)), this, SLOT(OnShaderOptionSelected(QModelIndex)) );
	connect( this, SIGNAL(itemPressed(QListWidgetItem*)), this, SLOT(OnShaderOptionSelected(QListWidgetItem*)) );
	connect( this, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(OnShaderOptionSelected(QListWidgetItem*)) );
	connect( this, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(OnShaderOptionSelected(QListWidgetItem*)) );
	//connect( this, SIGNAL(itemChanged(QListWidgetItem*)), this, SLOT(OnItemActivated(QListWidgetItem*)) );

	//m_currShaderInstance = INDEX_NONE;
}

void EdShaderOptionsListView::SetShaderObject( EdShaderDataProxy* shader )
{
	this->clear();

	m_currShader = shader;
	//m_currShaderInstance = INDEX_NONE;

	this->OnShaderOptionSelected( nil );

	if( !m_currShader )
	{
		//m_currShaderInstance = INDEX_NONE;
		return;
	}

	const GrShaderData& d = shader->d;

	for( UINT iShaderInstance=0; iShaderInstance < d.numInstances; iShaderInstance++ )
	{
		QListWidgetItem* newItem = new QListWidgetItem(this);

		newItem->setData( Qt::ItemDataRole::UserRole, QVariant(iShaderInstance) );

		dxShaderMacros	macros;
		(*d.getDefines)( iShaderInstance, macros );

		QString	itemText;
		for( UINT k=0; k < macros.Num(); k++ )
		{
			const dxShaderMacro& macro = macros[ k ];
			if( macro.Name == nil ) {
				break;	// null terminator
			}

			if( !mxStrEquAnsi( macro.Definition, "0" ) )
			{
				itemText.append( macro.Name );
				itemText.append( "     " );
			}
		}

		newItem->setText( itemText );

		this->addItem( newItem );
	}

	if( d.numInstances == 1 )
	{
		this->item(0)->setSelected(true);
		this->OnShaderOptionSelected( this->item(0) );
	}
}

void EdShaderOptionsListView::OnShaderOptionSelected( const QModelIndex& modelIndex )
{
	QListWidgetItem* currItem = (QListWidgetItem*) modelIndex.internalPointer();
	this->OnShaderOptionSelected( currItem );
}

void EdShaderOptionsListView::OnShaderOptionSelected( QListWidgetItem* currentItem )
{
	QPlainTextEdit& disasmView = EdModule_ShaderEditor::Get().GetDisassembledCodeView();
	disasmView.clear();

	if( currentItem == nil )
	{
		//m_currShaderInstance = INDEX_NONE;
		return;
	}

	disasmView.setStyleSheet(QString(
		"QPlainTextEdit {"
		//"selection-color: yellow;"
		//"selection-background-color: blue;"
		"background-color:rgb(0,0,0);"
		"alternate-background-color: rgb(0,0,0);"
		"}"
	));

	D3D11ShaderSystem* shaderSystem = checked_cast< D3D11ShaderSystem* >( graphics.shaders.Ptr );
	D3D11ShaderCache& shaderCache = shaderSystem->GetShaderCache();

	const GrShaderData& d = m_currShader->d;

	QVariant data = currentItem->data( Qt::ItemDataRole::UserRole );

	const UINT shaderInstanceIndex = data.toUInt();

	const ShaderInstanceData& shaderInstanceData = d.instancesData[ shaderInstanceIndex ];


	if( shaderInstanceData.VS_CodeIndex != INDEX_NONE )
	{
		const D3D11CompiledShader& VSByteCode = shaderCache.GetShaderByteCodeByIndex( shaderInstanceData.VS_CodeIndex );

		dxPtr<ID3DBlob> disassembledCodeBlob = D3D_DisassembleShader(
			VSByteCode.data.ToPtr(),
			VSByteCode.data.GetDataSize(),
			true,	// HTML format?
			"\n\n//========= Vertex Shader ======================================================\n"	// comments
			);

		disasmView.appendHtml( (const char*) disassembledCodeBlob->GetBufferPointer() );
	}
	if( shaderInstanceData.PS_CodeIndex != INDEX_NONE )
	{
		const D3D11CompiledShader& VSByteCode = shaderCache.GetShaderByteCodeByIndex( shaderInstanceData.PS_CodeIndex );

		dxPtr<ID3DBlob> disassembledCodeBlob = D3D_DisassembleShader(
			VSByteCode.data.ToPtr(),
			VSByteCode.data.GetDataSize(),
			true,	// HTML format?
			"\n\n//========= Pixel Shader ======================================================\n"	// comments
			);

		disasmView.appendHtml( (const char*) disassembledCodeBlob->GetBufferPointer() );
	}
}

/*
-----------------------------------------------------------------------------
	EdShadersTreeView
-----------------------------------------------------------------------------
*/
EdShadersTreeView::EdShadersTreeView( CodeEditor& editor, EdShaderOptionsListView& optionsList, QWidget *parent )
	: TEditableTreeWidget( graphics.shaders, parent )
	, m_codeEditor( editor )
	, m_shaderOptionsList( optionsList )
{
	this->header()->hide();
}

EdShadersTreeView::~EdShadersTreeView()
{

}

void EdShadersTreeView::OnItemSelected( AEditable* pObject )
{
	EdShaderDataProxy* selectedShader = this->GetSelectedObject<EdShaderDataProxy>();
	if( selectedShader != nil ) {
		m_codeEditor.SetCurrentShader( &selectedShader->d );
		m_shaderOptionsList.SetShaderObject( selectedShader );

		EdModule_ShaderEditor::Get().ChangeWindowTitle( selectedShader->d.name );
	}
	else
	{
		EdModule_ShaderEditor::Get().ChangeWindowTitle( nil );
	}
}

/*
-----------------------------------------------------------------------------
	EdDockWidgetShadersList
-----------------------------------------------------------------------------
*/
EdDockWidgetShadersList::EdDockWidgetShadersList( CodeEditor& editor, QWidget* parent )
	: Super( parent )
	, m_shadersTreeView( editor, m_shaderOptionsView )
{
	this->setWindowTitle("Shader programs");
	this->setObjectName("EdDockWidgetShadersList");
	this->setAttribute( Qt::WidgetAttribute::WA_DeleteOnClose, false );

	 QTabWidget *tabWidget = new QTabWidget();
     tabWidget->addTab( &m_shadersTreeView, "Shaders" );
     tabWidget->addTab( &m_shaderOptionsView, "Options" );
 
	 this->setWidget( tabWidget );
}

EdDockWidgetShadersList::~EdDockWidgetShadersList()
{
}

/*
-----------------------------------------------------------------------------
	LineNumberArea
-----------------------------------------------------------------------------
*/
LineNumberArea::LineNumberArea(CodeEditor *editor)
	: QWidget(editor)
{
	codeEditor = editor;
}
QSize LineNumberArea::sizeHint() const
{
	return QSize(codeEditor->lineNumberAreaWidth(), 0);
}
void LineNumberArea::paintEvent(QPaintEvent *event)
{
	codeEditor->lineNumberAreaPaintEvent(event);
}

/*
-----------------------------------------------------------------------------
	CodeEditor
-----------------------------------------------------------------------------
*/
CodeEditor::CodeEditor(QWidget *parent)
	: QPlainTextEdit( parent )
	, m_lineNumberArea(this)
{
	this->setAttribute( Qt::WidgetAttribute::WA_DeleteOnClose, false );

	this->setWordWrapMode(QTextOption::WrapMode::WordWrap);
	this->setTabStopWidth(40);

	new HLSLSyntaxHighlighter(this->document());

	connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(updateLineNumberAreaWidth(int)));
	connect(this, SIGNAL(updateRequest(QRect,int)), this, SLOT(updateLineNumberArea(QRect,int)));
	connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(highlightCurrentLine()));

	this->updateLineNumberAreaWidth(0);
	this->highlightCurrentLine();
}

CodeEditor::~CodeEditor()
{

}

void CodeEditor::SetCurrentShader( GrShaderData* shaderInfo )
{
	m_currentShader = shaderInfo;

	if (m_currentShader != nil)
	{
		const ANSICHAR* fileData;
		UINT fileSize;

		graphics.shaders->GetShaderSource( m_currentShader->file, fileData, fileSize );

		if( fileData != nil )
		{
			this->setPlainText( QString( fileData ) );
		}
	}
}

int CodeEditor::lineNumberAreaWidth()
{
	int digits = 1;
	int max = qMax(1, blockCount());
	while (max >= 10) {
		max /= 10;
		++digits;
	}

	int space = 3 + fontMetrics().width(QLatin1Char('9')) * digits;
	return space;
}

void CodeEditor::updateLineNumberAreaWidth(int /* newBlockCount */)
{
	setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void CodeEditor::updateLineNumberArea(const QRect &rect, int dy)
{
	if (dy)
		m_lineNumberArea.scroll(0, dy);
	else
		m_lineNumberArea.update(0, rect.y(), m_lineNumberArea.width(), rect.height());

	if (rect.contains(viewport()->rect()))
		updateLineNumberAreaWidth(0);
}

void CodeEditor::resizeEvent(QResizeEvent *e)
{
	QPlainTextEdit::resizeEvent(e);

	QRect cr = contentsRect();
	m_lineNumberArea.setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}

void CodeEditor::highlightCurrentLine()
{
	QList<QTextEdit::ExtraSelection> extraSelections;

	if (!isReadOnly())
	{
		QTextEdit::ExtraSelection selection;

		QColor lineColor = QColor(Qt::yellow).lighter(160);

		selection.format.setBackground(lineColor);
		selection.format.setProperty(QTextFormat::FullWidthSelection, true);
		selection.cursor = textCursor();
		selection.cursor.clearSelection();
		extraSelections.append(selection);
	}

	setExtraSelections(extraSelections);
}

void CodeEditor::lineNumberAreaPaintEvent(QPaintEvent *theEvent)
{
	QPainter painter(&m_lineNumberArea);
	painter.fillRect(theEvent->rect(), Qt::lightGray);

	QTextBlock block = firstVisibleBlock();
	int blockNumber = block.blockNumber();
	int top = (int) blockBoundingGeometry(block).translated(contentOffset()).top();
	int bottom = top + (int) blockBoundingRect(block).height();

	while (block.isValid() && top <= theEvent->rect().bottom()) {
		if (block.isVisible() && bottom >= theEvent->rect().top()) {
			QString number = QString::number(blockNumber + 1);
			painter.setPen(Qt::black);
			painter.drawText(0, top, m_lineNumberArea.width(), fontMetrics().height(),
				Qt::AlignRight, number);
		}

		block = block.next();
		top = bottom;
		bottom = top + (int) blockBoundingRect(block).height();
		++blockNumber;
	}
}

void CodeEditor::keyPressEvent( QKeyEvent* theEvent )
{
	// Ctrl + S
	if( theEvent->modifiers() == Qt::ControlModifier
		&& theEvent->key() == Qt::Key_S )
	{
		if (m_currentShader != nil)
		{
			const QString& text = this->toPlainText();

			graphics.shaders->ReloadShader( m_currentShader, text.toAscii().data(), text.length() );

			EdShaderOptionsListView& shaderOptionsList = EdModule_ShaderEditor::Get().m_shadersView.m_shaderOptionsView;

			if( shaderOptionsList.GetCurrShader() )
			{
				QList<QListWidgetItem*> selectedItems = shaderOptionsList.selectedItems();
				if( selectedItems.count() > 0 )
				{
					Assert( selectedItems.count() == 1 );

					shaderOptionsList.OnShaderOptionSelected( selectedItems[0] );
				}
			}
		}
	}

	Super::keyPressEvent( theEvent );
}

void CodeEditor::closeEvent( QCloseEvent * theEvent )
{
	QPlainTextEdit::clear();

	Super::closeEvent( theEvent );
}

/*
-----------------------------------------------------------------------------
	EdModule_ShaderEditor
-----------------------------------------------------------------------------
*/
EdModule_ShaderEditor::EdModule_ShaderEditor()
	: QMainWindow(EditorApp::GetMainFrame())
	, m_shadersView(m_codeEditor)
{
	this->setWindowTitle("Shader Editor");
	this->setObjectName("GPU");
	this->setAttribute( Qt::WidgetAttribute::WA_DeleteOnClose, false );
	this->hide();

	m_disassembledHLSL.setReadOnly(true);
}

EdModule_ShaderEditor::~EdModule_ShaderEditor()
{
}

void EdModule_ShaderEditor::PreInit()
{
	this->CreateWidgets();
	this->CreateActions();
	this->CreateMenus();
	this->ConnectSigSlots();
}

void EdModule_ShaderEditor::SetDefaultValues()
{
	//QMainWindow* mainWindow = GetMainFrame();

	this->resize(800,600);
}

void EdModule_ShaderEditor::PostInit()
{
}

void EdModule_ShaderEditor::CreateWidgets()
{
	this->addDockWidget( Qt::LeftDockWidgetArea, &m_shadersView );

	QSplitter *splitter = new QSplitter(this);
	splitter->addWidget(&m_codeEditor);
	splitter->addWidget(&m_disassembledHLSL);
	this->setCentralWidget( splitter );

	//@todo: hide disasm view
	//const QSize oldSize = m_disassembledHLSL.size();
	//m_disassembledHLSL.resize( 0, oldSize.height() );

	//m_disassembledHLSL.hide();
}

void EdModule_ShaderEditor::CreateActions()
{
	m_actViewGPU.setText( tr("Shader Editor") );
}

void EdModule_ShaderEditor::CreateMenus()
{
	EdMenus& menus = EdApp::Get().menus;
	menus.viewMenu->addAction( &m_actViewGPU );
}

void EdModule_ShaderEditor::ConnectSigSlots()
{
	EditorApp::ShowOnActionTriggered( &m_actViewGPU, this );
}

void EdModule_ShaderEditor::SerializeWidgetLayout( QDataStream & stream )
{
	//DBG_TRACE_CALL;
	SerializeMainWindow_GeometryAndState(stream,this);

	int bIsVisible = this->isVisible();
	if( IsLoading(stream) ) {
		stream >> bIsVisible;
		this->setVisible(bIsVisible);
	} else {
		stream << bIsVisible;
	}
}

void EdModule_ShaderEditor::ChangeWindowTitle( OptPtr(const char) shaderName )
{
	if( shaderName != nil )
	{
		this->setWindowTitle( QString("Shader Editor - ") + QString(shaderName) );
	}
	else
	{
		this->setWindowTitle("Shader Editor");
	}
}

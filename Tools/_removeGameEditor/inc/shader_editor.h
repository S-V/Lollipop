#pragma once

#include <Graphics/DX11/DX11Private.h>

#include "tree_views.h"
#include "editor_system.h"
#include "hlslsyntaxhighlighter.h"

class CodeEditor;



/*
-----------------------------------------------------------------------------
	EdShaderOptionsListView

	Shows shader combinations
-----------------------------------------------------------------------------
*/
class EdShaderOptionsListView : public QListWidget
{
	Q_OBJECT

public:
	EdShaderOptionsListView();

	void SetShaderObject( EdShaderDataProxy* shader );

	EdShaderDataProxy* GetCurrShader()
	{
		return m_currShader;
	}

public slots:
	void OnShaderOptionSelected( const QModelIndex& modelIndex );
	void OnShaderOptionSelected( QListWidgetItem* currentItem );

private:
	TPtr< EdShaderDataProxy >	m_currShader;
	//UINT	m_currShaderInstance;
};

/*
-----------------------------------------------------------------------------
	EdShadersTreeView
-----------------------------------------------------------------------------
*/
class EdShadersTreeView
	: public TEditableTreeWidget< EdShaderDataProxy >
{
public:
	EdShadersTreeView( CodeEditor& editor, EdShaderOptionsListView& optionsList, QWidget *parent = nil );
	~EdShadersTreeView();

protected:
	virtual void OnItemSelected( AEditable* pObject ) override;

private:
	CodeEditor &	m_codeEditor;
	EdShaderOptionsListView & m_shaderOptionsList;
};


/*
-----------------------------------------------------------------------------
	EdDockWidgetShadersList
-----------------------------------------------------------------------------
*/
class EdDockWidgetShadersList : public QDockWidget
{
	Q_OBJECT

public:
	typedef QDockWidget Super;

	EdDockWidgetShadersList( CodeEditor& editor, QWidget* parent = nil );
	~EdDockWidgetShadersList();

//private:
	EdShaderOptionsListView	m_shaderOptionsView;
	EdShadersTreeView	m_shadersTreeView;
};


/*
-----------------------------------------------------------------------------
	CodeEditor
-----------------------------------------------------------------------------
*/
class CodeEditor;

class LineNumberArea : public QWidget
{
public:
	LineNumberArea(CodeEditor *editor);

	QSize sizeHint() const;

protected:
	void paintEvent(QPaintEvent *event);

private:
	CodeEditor *codeEditor;
};

class CodeEditor : public QPlainTextEdit
{
	Q_OBJECT

public:
	typedef QPlainTextEdit Super;

	CodeEditor(QWidget *parent = nil);
	~CodeEditor();

	void lineNumberAreaPaintEvent(QPaintEvent *theEvent);
	int lineNumberAreaWidth();

	void SetCurrentShader( GrShaderData* shaderInfo );

protected:
	virtual void keyPressEvent( QKeyEvent* theEvent );

	virtual void closeEvent( QCloseEvent* theEvent );
	virtual void resizeEvent(QResizeEvent *theEvent);

private slots:
	void updateLineNumberAreaWidth(int newBlockCount);
	void highlightCurrentLine();
	void updateLineNumberArea(const QRect &, int);

private:
	LineNumberArea	m_lineNumberArea;

	TPtr<GrShaderData>	m_currentShader;
};

/*
-----------------------------------------------------------------------------
	EdModule_ShaderEditor
-----------------------------------------------------------------------------
*/
class EdModule_ShaderEditor
	: public QMainWindow
	, public EdModule
	, public TGlobal< EdModule_ShaderEditor >
	, public EdWidgetChild
{
	Q_OBJECT

public:
	EdModule_ShaderEditor();
	~EdModule_ShaderEditor();

	virtual void PreInit() override;
	virtual void SetDefaultValues() override;
	virtual void PostInit() override;
	virtual void SerializeWidgetLayout( QDataStream & stream ) override;

	QPlainTextEdit& GetDisassembledCodeView() { return m_disassembledHLSL; }

	void ChangeWindowTitle( OptPtr(const char) shaderName );

private:
	void CreateWidgets();
	void CreateActions();
	void CreateMenus();
	void ConnectSigSlots();

public_internal://private:
	HLSLUtil	m_HLSL_Util;

	EdDockWidgetShadersList	m_shadersView;
	CodeEditor				m_codeEditor;
	QPlainTextEdit			m_disassembledHLSL;

	EdAction		m_actViewGPU;
};

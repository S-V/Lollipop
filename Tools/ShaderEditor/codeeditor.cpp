#include "StdAfx.h"
#include "shadereditor.h"
#include "codeeditor.h"
#include "hlslsyntaxhighlighter.h"
#include "utils.h"

#include "outputwindow.h"

#include <Graphics/Graphics_DX11.h>

CodeEditor::CodeEditor(QWidget *parent)
	: QPlainTextEdit(parent)
{
	this->setAttribute( Qt::WidgetAttribute::WA_DeleteOnClose, false );

	this->setWordWrapMode(QTextOption::WrapMode::WordWrap);
	this->setTabStopWidth(40);

	new HLSLSyntaxHighlighter(this->document());

	lineNumberArea = new LineNumberArea(this);

	connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(updateLineNumberAreaWidth(int)));
	connect(this, SIGNAL(updateRequest(QRect,int)), this, SLOT(updateLineNumberArea(QRect,int)));
	connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(highlightCurrentLine()));

	updateLineNumberAreaWidth(0);
	highlightCurrentLine();
}

CodeEditor::~CodeEditor()
{

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
		lineNumberArea->scroll(0, dy);
	else
		lineNumberArea->update(0, rect.y(), lineNumberArea->width(), rect.height());

	if (rect.contains(viewport()->rect()))
		updateLineNumberAreaWidth(0);
}



void CodeEditor::resizeEvent(QResizeEvent *e)
{
	QPlainTextEdit::resizeEvent(e);

	QRect cr = contentsRect();
	lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
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



void CodeEditor::lineNumberAreaPaintEvent(QPaintEvent *event)
{
	QPainter painter(lineNumberArea);
	painter.fillRect(event->rect(), Qt::lightGray);


	QTextBlock block = firstVisibleBlock();
	int blockNumber = block.blockNumber();
	int top = (int) blockBoundingGeometry(block).translated(contentOffset()).top();
	int bottom = top + (int) blockBoundingRect(block).height();

	while (block.isValid() && top <= event->rect().bottom()) {
		if (block.isVisible() && bottom >= event->rect().top()) {
			QString number = QString::number(blockNumber + 1);
			painter.setPen(Qt::black);
			painter.drawText(0, top, lineNumberArea->width(), fontMetrics().height(),
				Qt::AlignRight, number);
		}

		block = block.next();
		top = bottom;
		bottom = top + (int) blockBoundingRect(block).height();
		++blockNumber;
	}
}

void CodeEditor::keyPressEvent( QKeyEvent* event )
{
	//if( event->modifiers() == Qt::NoModifier || event->modifiers() == Qt::ShiftModifier))
	//	QPlainTextEdit::keyPressEvent(event);

	if( event->key() == Qt::Key_Save )
	{
		DEBUG_BREAK;
	}

	if( event->modifiers() == Qt::ControlModifier
		&& event->key() == Qt::Key_S )
	{
		saveDocument();
	}

	__super::keyPressEvent( event );
}

void CodeEditor::closeEvent( QCloseEvent * event )
{
	QPlainTextEdit::clear();

	__super::closeEvent(event);
}

bool CodeEditor::loadDocument(const QString& name)
{
	//if( name == this->getShaderName() ) {
	//	return true;
	//}

	Cmd_GetShaderSourceCode	getShaderCode;
	getShaderCode.inputs.shaderName.SetString( name.toAscii().data() );

	const bool bOk = ExecuteCommand( getShaderCode );

	if( bOk )
	{
		this->setShaderName(name);

		this->setPlainText(
			QString((const char*)getShaderCode.results.shaderCode.ToPtr())
		);
	}

	return bOk;
}

void CodeEditor::saveDocument()
{
	Cmd_ReloadShader	reloadShaderCmd;
	reloadShaderCmd.inputs.shaderName.SetString( this->getShaderName() );
	reloadShaderCmd.inputs.shaderCode.SetString( this->toPlainText().toAscii().data() );

	bool bOk = ExecuteCommand( reloadShaderCmd );
	if(bOk)
	{
		OutputWindow::Get().Clear();

		if(reloadShaderCmd.results.messages.Num())
		{
			for( UINT iMessage = 0;
				iMessage < reloadShaderCmd.results.messages.Num();
				iMessage++ )
			{
				const CompileMessage& msg = reloadShaderCmd.results.messages[iMessage];
				OutputWindow::Get().Log(msg);
			}
		}
		else
		{
			mxPutf("Reloaded shader '%s'.\n",this->getShaderName());
		}
	}
}

void CodeEditor::setShaderName(const QString& name)
{
	this->shaderName = name.toAscii().data();
	this->setDocumentTitle(name);
}

const char* CodeEditor::getShaderName() const
{
	return this->shaderName;
}

#include "StdAfx.h"
#include "consolepanel.h"

ConsolePanel::ConsolePanel(QWidget *parent)
: QPlainTextEdit(parent)
{
	this->setWindowTitle("Console");

	prompt = "ready > ";

	insertPrompt(false);

	isLocked = false;

	QPalette p = palette();
	p.setColor(QPalette::Base, Qt::black);
	p.setColor(QPalette::Text, Qt::green);
	setPalette(p);

	history = new QStringList;
	historyPos = 0;
}

ConsolePanel::~ConsolePanel()
{
	delete history;

}
void ConsolePanel::keyPressEvent(QKeyEvent *event)
{
	if(event->key() >= 0x20 && event->key() <= 0x7e
		&& (event->modifiers() == Qt::NoModifier || event->modifiers() == Qt::ShiftModifier))
		QPlainTextEdit::keyPressEvent(event);

	if(event->key() == Qt::Key_Backspace
		&& event->modifiers() == Qt::NoModifier
		&& textCursor().positionInBlock() > prompt.length())
		QPlainTextEdit::keyPressEvent(event);

	if(event->key() == Qt::Key_Return && event->modifiers() == Qt::NoModifier)
		onEnter();

	if(event->key() == Qt::Key_Up && event->modifiers() == Qt::NoModifier)
		historyBack();
	if(event->key() == Qt::Key_Down && event->modifiers() == Qt::NoModifier)
		historyForward();

	// Ctrl + C
	if(event->key() == Qt::Key_C && event->modifiers() == Qt::ControlModifier)
		__super::keyPressEvent(event);
	// Ctrl + V
	if(event->key() == Qt::Key_V && event->modifiers() == Qt::ControlModifier)
		__super::keyPressEvent(event);
}
void ConsolePanel::mousePressEvent(QMouseEvent *event)
{
	__super::mousePressEvent(event);
}
void ConsolePanel::mouseDoubleClickEvent(QMouseEvent *event)
{
	__super::mouseDoubleClickEvent(event);
}
void ConsolePanel::contextMenuEvent(QContextMenuEvent *event)
{
	__super::contextMenuEvent(event);
}
void ConsolePanel::scrollDown()
{
	QScrollBar *vbar = verticalScrollBar();
	vbar->setValue(vbar->maximum());
}
void ConsolePanel::onEnter()
{
	if(textCursor().positionInBlock() == prompt.length())
	{
		insertPrompt(true);
		return;
	}
	QString cmd = textCursor().block().text().mid(prompt.length());
	emit onCommand(cmd);

	historyAdd(cmd);

	isLocked = true;

}
void ConsolePanel::insertPrompt(bool insertNewBlock)
{
	if(insertNewBlock)
		textCursor().insertBlock();
	textCursor().insertText(prompt);

	QTextCharFormat format;
	format.setForeground(Qt::green);
	textCursor().setBlockCharFormat(format);

	scrollDown();
}
void ConsolePanel::output(const QString& s)
{
	textCursor().insertBlock();
	textCursor().insertText(s);
	insertPrompt(true);
	isLocked = false;

	QTextCharFormat format;
	format.setForeground(Qt::white);
	textCursor().setBlockCharFormat(format);
}

void ConsolePanel::historyAdd(const QString& cmd)
{
	history->append(cmd);
	historyPos = history->length();
}

void ConsolePanel::historyBack()
{
	if(!historyPos)
		return;
	QTextCursor cursor = textCursor();
	cursor.movePosition(QTextCursor::StartOfBlock);
	cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
	cursor.removeSelectedText();
	cursor.insertText(prompt + history->at(historyPos-1));
	setTextCursor(cursor);
	historyPos--;
}

void ConsolePanel::historyForward()
{
	if(historyPos == history->length())
		return;
	QTextCursor cursor = textCursor();
	cursor.movePosition(QTextCursor::StartOfBlock);
	cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
	cursor.removeSelectedText();
	if(historyPos == history->length() - 1)
		cursor.insertText(prompt);
	else
		cursor.insertText(prompt + history->at(historyPos + 1));
	setTextCursor(cursor);
	historyPos++;
}
void ConsolePanel::onCommand(const QString& s)
{
	output(s);
}


ConsolePanel_DockWidget::ConsolePanel_DockWidget(QWidget *parent)
: QDockWidget(parent)
{
	p = new ConsolePanel(this);
	QDockWidget::setWidget(this->p);

}

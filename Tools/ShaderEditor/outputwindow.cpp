#include "StdAfx.h"
#include "outputwindow.h"

OutputWindow::OutputWindow(QWidget *parent)
	: QDockWidget(parent)
{
	this->setWindowTitle("Output Window");
	this->setObjectName("Output_Window");
	this->setAttribute( Qt::WidgetAttribute::WA_DeleteOnClose, false );


	this->plainTextEdit = new QPlainTextEdit(this);
	this->setWidget(this->plainTextEdit);

	this->plainTextEdit->setUndoRedoEnabled(false);
	this->plainTextEdit->setReadOnly(true);
	//this->plainTextEdit->zoomOut();
	this->plainTextEdit->viewport()->installEventFilter(this);
	this->plainTextEdit->setFocusPolicy(Qt::NoFocus);
	this->plainTextEdit->setMaximumBlockCount(512);

	GetGlobalLogger().Attach(this);


	QFont	font(this->plainTextEdit->font());
	//font.setOverline(false);
	font.setStyleHint(QFont::StyleHint::Courier);
	font.setStyleStrategy(QFont::StyleStrategy::PreferAntialias);
	this->plainTextEdit->setFont(font);

	QTextCharFormat fontCharFormat(this->plainTextEdit->currentCharFormat());
	fontCharFormat.setVerticalAlignment(QTextCharFormat::VerticalAlignment::AlignMiddle);
	fontCharFormat.setFontFixedPitch(true);
	this->plainTextEdit->setCurrentCharFormat(fontCharFormat);

	this->plainTextEdit->setLineWrapMode(QPlainTextEdit::LineWrapMode::NoWrap);
}

OutputWindow::~OutputWindow()
{
	GetGlobalLogger().Detach(this);
}

void OutputWindow::closeEvent( QCloseEvent* event )
{
	//Editor::Get().ui.aShowOutputWindow->setChecked(false);
	event->accept();
}

bool OutputWindow::eventFilter(QObject* object, QEvent* event)
{
	if (object == this->plainTextEdit->viewport())
	{
		if( event->type() == QEvent::MouseButtonDblClick)
		{
			QMouseEvent* mouseEvent = (QMouseEvent*)event;
			QTextCursor cursor = this->plainTextEdit->cursorForPosition(mouseEvent->pos());
			
			if (!cursor.isNull())
			{
				QTextBlockUserData* userData = cursor.block().userData();

				if( userData )
				{
					LogEntry* data = checked_cast<LogEntry*>(userData);

					emit(messageClicked(data->line, data->column));

					return true;
				}
			}
		}
	}

	return QDockWidget::eventFilter(object, event);
}

void OutputWindow::Log( ELogLevel level, const char* message, UINT numChars )
{
	QString	plainText;

	const UINT fontSize = 8;
	QFont font("Courier New", fontSize);

	Qt::GlobalColor foregroundColor = Qt::GlobalColor::black;

	switch( level )
	{
	case ELogLevel::LL_Info :
		{
//			plainText.append("INFO: ");
		}
		break;

	case ELogLevel::LL_Warning :
		{
			plainText.append("WARNING: ");
			font = QFont("Courier New", fontSize, QFont::Bold);
			foregroundColor = Qt::GlobalColor::darkYellow;
		}
		break;

	case ELogLevel::LL_Error :
		{
			plainText.append("ERROR: ");
			font = QFont("Courier New", fontSize, QFont::Bold);
			foregroundColor = Qt::GlobalColor::darkRed;
		}
		break;

	default:
		Unreachable;
	}

	plainText.append( message );

#if 0
	QTextCursor cursor(this->plainTextEdit->textCursor());
	cursor.insertText(plainText);
	//cursor.block().setUserData(new LogData(line, column));
	cursor.insertBlock();
	this->plainTextEdit->setTextCursor(cursor);
#else
	this->plainTextEdit->appendPlainText( plainText );
#endif
}

void OutputWindow::Logf( ELogLevel level, const char* fmt, ... )
{
	char	buffer[ MAX_STRING_CHARS ];
	UINT	length;
	MX_GET_VARARGS_ANSI_X( buffer, fmt, length );

	Log( level, buffer, length );
}

void OutputWindow::Log( const CompileMessage& msg )
{
	LogEntry * newLogEntry = new LogEntry();
	newLogEntry->line = msg.line;
	newLogEntry->column = msg.column;

	QTextCursor cursor(this->plainTextEdit->textCursor());

	QTextCharFormat format;
	format.setBackground(QColor::fromRgb(250, 220, 230));
	format.setForeground(QColor::fromRgb(0, 0, 0));

	cursor.setCharFormat(format);
	cursor.movePosition(QTextCursor::MoveOperation::NextRow);
	cursor.insertText(msg.text.ToChars());
	cursor.block().setUserData(newLogEntry);
	cursor.insertBlock();
	this->plainTextEdit->setTextCursor(cursor);

#if MX_COMPILER == Compiler_MSVCpp
	char	tmp[ MAX_STRING_CHARS ];
	MX_SPRINTF_ANSI(tmp,"%s\n",msg.text.ToChars());
	::OutputDebugStringA(tmp);
#endif
}

void OutputWindow::Clear()
{
	this->plainTextEdit->clear();
}

void EdMsgBoxf( ELogLevel level, const char* fmt, ... )
{
	char	buffer[ MAX_STRING_CHARS ];
	MX_GET_VARARGS_ANSI( buffer, fmt );
	switch(level)
	{
	case ELogLevel::LL_Info :
		QMessageBox::information(nil,"Information",buffer);
		return;

	case ELogLevel::LL_Warning :
		QMessageBox::warning(nil,"Warning",buffer);
		return;

	case ELogLevel::LL_Error :
		QMessageBox::critical(nil,"Error",buffer);
		return;
	}
	Unreachable;
}


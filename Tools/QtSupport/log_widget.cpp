#include "stdafx.h"

#include "log_widget.h"

/*
-----------------------------------------------------------------------------
	LogWidget
-----------------------------------------------------------------------------
*/
LogWidget::LogWidget( QWidget* parent )
	: QDockWidget( parent )
{
	this->setWindowTitle("Output");
	this->setObjectName("Output");
	//this->setAttribute( Qt::WidgetAttribute::WA_DeleteOnClose, false );

	m_plainTextEdit.setReadOnly(true);
	QDockWidget::setWidget( &m_plainTextEdit );
	m_plainTextEdit.setBackgroundVisible(true);

	GetGlobalLogger().Attach(this);
}

LogWidget::~LogWidget()
{
	GetGlobalLogger().Detach(this);
}

void LogWidget::ClearLog()
{
	m_plainTextEdit.clear();
}

static
Qt::GlobalColor GetColorForLogLevel( ELogLevel level )
{
	switch( level )
	{
	case ELogLevel::LL_Info :		return Qt::black;
	case ELogLevel::LL_Warning :	return Qt::darkBlue;
	case ELogLevel::LL_Error :		return Qt::darkRed;
	default:						return Qt::black;
	}
}

void LogWidget::Log( ELogLevel level, const char* message, UINT numChars )
{
	//m_plainTextEdit.insertPlainText( message );

	// because appendPlainText() inserts a new line
	if( message[numChars-1] == '\n' )
	{
		ANSICHAR	buffer[MAX_STRING_CHARS];
		MemCopy(buffer,message,numChars*sizeof buffer[0]);
		buffer[numChars-1] = 0;

		m_plainTextEdit.appendPlainText( buffer );
	}
	else
	{
		m_plainTextEdit.appendPlainText( message );
	}
	
	// scroll to bottom
	m_plainTextEdit.ensureCursorVisible();
}

NO_EMPTY_FILE

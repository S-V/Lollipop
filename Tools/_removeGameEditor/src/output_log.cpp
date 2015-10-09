#include "stdafx.h"

#include "main_window_frame.h"
#include "app.h"

#include "output_log.h"

/*
-----------------------------------------------------------------------------
	EdOutputLog
-----------------------------------------------------------------------------
*/
EdOutputLog::EdOutputLog()
	: QDockWidget(EditorApp::GetMainFrame())
{
	this->setWindowTitle("Output");
	this->setObjectName("Output");
	this->setAttribute( Qt::WidgetAttribute::WA_DeleteOnClose, false );

#if ED_CONSOLE_USE_LIST_WIDGET
	QDockWidget::setWidget( &m_listWidget );
	QSize gridSize = m_listWidget.gridSize();
	gridSize.setHeight( gridSize.height() / 4 );
	m_listWidget.setGridSize( gridSize );
#else
	m_plainTextEdit.setReadOnly(true);
	QDockWidget::setWidget( &m_plainTextEdit );
	m_plainTextEdit.setBackgroundVisible(true);
#endif


	{
		//QObject* parent = GetMainFrame();

		m_viewConsoleAct.setText(tr("Output"));
		m_viewConsoleAct.setShortcut( QKeySequence( Qt::ALT + Qt::Key_2 ) );
	}

	{
		EdMenus& menus = EdApp::Get().menus;
		menus.viewMenu->addAction( &m_viewConsoleAct );
	}

	connect(&m_viewConsoleAct, SIGNAL(triggered()), this, SLOT(show()));

	GetGlobalLogger().Attach(this);
}

EdOutputLog::~EdOutputLog()
{
	GetGlobalLogger().Detach(this);
}

void EdOutputLog::SetDefaultValues()
{
	QMainWindow* mainWindow = EditorApp::GetMainFrame();

	mainWindow->addDockWidget( Qt::BottomDockWidgetArea, this );
}

void EdOutputLog::SerializeWidgetLayout( QDataStream & stream )
{
	//DBG_TRACE_CALL;
	QMainWindow* mainWindow = EditorApp::GetMainFrame();

	SerializeDockWidgetState( stream, mainWindow, Qt::BottomDockWidgetArea, this );
}

static Qt::GlobalColor GetColorForLogLevel( ELogLevel level )
{
	switch(level)
	{
	case ELogLevel::LL_Info :	return Qt::black;
	case ELogLevel::LL_Warning :	return Qt::darkBlue;
	case ELogLevel::LL_Error :		return Qt::darkRed;
		//darkGreen
	default:	return Qt::black;
	}
}

void EdOutputLog::Log( ELogLevel level, const char* message, UINT numChars )
{
#if ED_CONSOLE_USE_LIST_WIDGET

	QListWidgetItem* newItem = new QListWidgetItem( &m_listWidget );

	newItem->setText( message );

	QBrush brush;
	brush.setColor( GetColorForLogLevel(level) );
	newItem->setForeground( brush );

	QFont newFont = newItem->font();
	newFont.setBold(true);
	newItem->setFont( newFont );

	//QFontMetrics fontMetrics( newFont );
	//newItem->setSizeHint( fontMetrics.size() );

	m_listWidget.addItem(newItem);

	//qApp->processEvents();

	//emit signal_ConsoleChanged();

#else

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
	

#endif

	this->slot_ScrollToBottom();
}

void EdOutputLog::slot_ScrollToBottom()
{
#if ED_CONSOLE_USE_LIST_WIDGET
	m_listWidget.setCurrentRow( m_listWidget.count()-1 );
#else
	// scroll to bottom
	m_plainTextEdit.ensureCursorVisible();
#endif

}

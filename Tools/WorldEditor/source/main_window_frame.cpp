#include "stdafx.h"

#include "main_window_frame.h"
#include "editor_app.h"

/*
-----------------------------------------------------------------------------
	EdMainWindowFrame
-----------------------------------------------------------------------------
*/
EdMainWindowFrame::EdMainWindowFrame(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags)
{
	this->setAttribute( Qt::WidgetAttribute::WA_DeleteOnClose, false );

	this->setAcceptDrops(true);
}
//-----------------------------------------------------------------------------
EdMainWindowFrame::~EdMainWindowFrame()
{

}
void EdMainWindowFrame::keyPressEvent( QKeyEvent* theEvent )
{
	theEvent->accept();
}
//-----------------------------------------------------------------------------
void EdMainWindowFrame::keyReleaseEvent( QKeyEvent* theEvent )
{
	theEvent->accept();
}
//-----------------------------------------------------------------------------
void EdMainWindowFrame::mousePressEvent( QMouseEvent* theEvent )
{
	theEvent->accept();
}
//-----------------------------------------------------------------------------
void EdMainWindowFrame::mouseReleaseEvent( QMouseEvent* theEvent )
{
	theEvent->accept();
}
//-----------------------------------------------------------------------------
void EdMainWindowFrame::mouseMoveEvent( QMouseEvent* theEvent )
{
	theEvent->accept();
}
//-----------------------------------------------------------------------------
void EdMainWindowFrame::wheelEvent( QWheelEvent* theEvent )
{
	theEvent->accept();
}

void EdMainWindowFrame::dragEnterEvent(QDragEnterEvent *theEvent)
{
	//Super::dragEnterEvent( theEvent );
	theEvent->accept();
}
//-----------------------------------------------------------------------------
void EdMainWindowFrame::dropEvent(QDropEvent *theEvent)
{
	//Super::dropEvent( theEvent );
	theEvent->accept();
}
//-----------------------------------------------------------------------------
void EdMainWindowFrame::closeEvent(QCloseEvent* theEvent)
{
	const bool bExit = EdApp::Get().RequestExit();

	if( bExit )
	{
		theEvent->accept();
	}
	else
	{
		theEvent->ignore();
	}
}

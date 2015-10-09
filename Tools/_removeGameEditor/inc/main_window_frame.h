#pragma once

#include <QtGui/QMainWindow>

#include "editor_common.h"

/*
-----------------------------------------------------------------------------
	EdMainWindowFrame
-----------------------------------------------------------------------------
*/
class EdMainWindowFrame : public QMainWindow
{
	Q_OBJECT

public:
	typedef QMainWindow Super;

	EdMainWindowFrame(QWidget *parent = nil, Qt::WFlags flags = 0);
	~EdMainWindowFrame();

protected:
	virtual void keyPressEvent( QKeyEvent* theEvent ); 
	virtual void keyReleaseEvent( QKeyEvent* theEvent ); 
	virtual void mousePressEvent( QMouseEvent* theEvent ); 
	virtual void mouseReleaseEvent( QMouseEvent* theEvent ); 
	virtual void mouseMoveEvent( QMouseEvent* theEvent );
	virtual void wheelEvent( QWheelEvent* theEvent );

	virtual void dragEnterEvent(QDragEnterEvent* theEvent);
	virtual void dropEvent(QDropEvent* theEvent);

	virtual void closeEvent(QCloseEvent* theEvent);
};


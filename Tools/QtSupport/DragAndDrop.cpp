#include <stdafx.h>
#pragma hdrstop

#include "qt_common.h"
#include "DragAndDrop.h"

/*
-----------------------------------------------------------------------------
	EdMimeData
-----------------------------------------------------------------------------
*/
EdMimeData::EdMimeData()
	: objects( EMemHeap::HeapTemp )
{

}

EdMimeData::~EdMimeData()
{

}


#if 0
DropArea::DropArea( QWidget *parent, MimeDataEncoder* dataEncoder )
: QLabel( parent )
, m_dataEncoder( dataEncoder )
{
	setMinimumSize( 20, 20 );
	setFrameStyle( QFrame::Sunken | QFrame::StyledPanel );
	setAlignment( Qt::AlignLeft );
	setAcceptDrops( true );
	setAutoFillBackground( true );
	clear();
}

///////////////////////////////////////////////////////////////////////////////

DropArea::~DropArea()
{
	delete m_dataEncoder;
	m_dataEncoder = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void DropArea::dragEnterEvent( QDragEnterEvent *event )
{
	setBackgroundRole( QPalette::Highlight );
	event->acceptProposedAction();
}

///////////////////////////////////////////////////////////////////////////////

void DropArea::dragMoveEvent( QDragMoveEvent *event )
{
	event->acceptProposedAction();
}

///////////////////////////////////////////////////////////////////////////////

void DropArea::dropEvent( QDropEvent *event )
{
	const QMimeData* mimeData = event->mimeData();

	m_dataEncoder->load( *mimeData );

	setBackgroundRole( QPalette::Light );
	event->acceptProposedAction();

	emit changed();
}

///////////////////////////////////////////////////////////////////////////////

void DropArea::dragLeaveEvent( QDragLeaveEvent *event )
{
	clear();
	event->accept();
}

///////////////////////////////////////////////////////////////////////////////

void DropArea::clear()
{
	setBackgroundRole( QPalette::Light );

	emit changed();
}
#endif


#include "stdafx.h"
#pragma hdrstop
#include "qt_common.h"
#include "DragAndDrop.h"


QDataStream& SerializeWidgetSize( QDataStream & serializer, QWidget * o )
{
	//DBG_TRACE_CALL;
	QSize size = o->size();

	if( IsStoring( serializer ) )
	{
		serializer << size;
	}
	else
	{
		serializer >> size;
		o->resize( size );
	}

	return serializer;
}
//---------------------------------------------------------------------------
QDataStream& SerializeWidgetGeometry( QDataStream & serializer, QWidget * o )
{
	//DBG_TRACE_CALL;
	AssertPtr(o);

	QByteArray geometryData;

	if( IsLoading(serializer) )
	{
		serializer >> geometryData;
		o->restoreGeometry( geometryData );

		QPoint pos;
		serializer >> pos;

		QSize size;
		serializer >> size;

		o->resize( size );
		o->move( pos );
	}
	else
	{
		geometryData = o->saveGeometry();
		serializer << geometryData;

		serializer << o->pos();
		serializer << o->size();
	}

	bool bIsVisible = o->isVisible();

	if( IsLoading(serializer) )
	{
		serializer >> bIsVisible;
		o->setVisible( bIsVisible );
	}
	else
	{
		serializer << bIsVisible;
	}

	return serializer;
}
//---------------------------------------------------------------------------
QDataStream& SerializeDockWidgetState(
	QDataStream & serializer,
	QMainWindow* mainWindow, Qt::DockWidgetArea defaultValue,
	QDockWidget * o )
{
	SerializeWidgetGeometry( serializer, o );

	Qt::DockWidgetArea dockArea;

	bool isVisible = o->isVisible();

	if( IsLoading(serializer) )
	{
		serializer >> isVisible;
	}
	else
	{
		serializer << isVisible;
	}

	if( IsLoading(serializer) )
	{
		serializer >> (U4&)dockArea;

		mainWindow->addDockWidget( dockArea, o );
		if( isVisible ) {
			o->showMaximized();
		} else {
			o->hide();
		}
	}
	else
	{
		Qt::DockWidgetArea dockArea = mainWindow->dockWidgetArea( o );

		if( Qt::NoDockWidgetArea == dockArea )
		{
			dockArea = defaultValue;
		}

		serializer << (U4&)dockArea;
	}

	return serializer;
}
//---------------------------------------------------------------------------
QDataStream& SerializeMainWindowState( QDataStream & serializer, QMainWindow * o )
{
	//DBG_TRACE_CALL;
	QByteArray windowState;

	if( IsLoading(serializer) )
	{
		serializer >> windowState;
		o->restoreState( windowState );
	}
	else
	{
		windowState = o->saveState();
		serializer << windowState;
	}

	return serializer;
}
//---------------------------------------------------------------------------
//void SetWidgetBackgroundColor( QWidget* w )
//{
//	AssertPtr(w);
//
//#if 0
//	QPalette p = w->palette();
//
//	//const SColor c = SColor::LightSlateGray;
//
//	QColor backgroundColor =
//		Qt::lightGray
//		//QColor(qRgb(c.R,c.G,c.B))
//		;
//
//	p.setColor(QPalette::Active, QPalette::Base, backgroundColor);
//	p.setColor(QPalette::Inactive, QPalette::Base, backgroundColor);
//
//#if 0
//	p.setColor(QPalette::Active, QPalette::Text, Qt::white);
//	p.setColor(QPalette::Inactive, QPalette::ButtonText, Qt::white);
//#endif
//
//	w->setPalette(p);
//#endif
//
//	/*
//	info:
//	If style sheets are supported, you could do it like this:
//	myPlainTextEdit->setStyleSheet("background-color: yellow");
//	or
//	qApp->setStyleSheet("QPlainTextEdit {background-color: yellow}");
//	*/
//}


//---------------------------------------------------------------------------
void SetWidgetToScreenCenter(QWidget* w)
{
	QDesktopWidget *deskTop = QApplication::desktop();
	const int iScreenIndex = deskTop->screenNumber(w);
	const QRect rect = deskTop->screenGeometry(iScreenIndex);
	const int x = (rect.width() - w->width())/2;
	const int y = (rect.height() - w->height())/2;
	w->move(x, y);
}
//---------------------------------------------------------------------------
EKeyCode F_MapQtKey( int keyCode )
{
	const Qt::Key qtKeyCode = (Qt::Key)keyCode;
	switch( qtKeyCode )
	{
	case Qt::Key_Back			:	return EKeyCode::Key_Back						;
	case Qt::Key_Tab			:	return EKeyCode::Key_Tab							;
	case Qt::Key_Clear			:	return EKeyCode::Key_Clear						;
	case Qt::Key_Return			:	return EKeyCode::Key_Return						;
	case Qt::Key_Shift			:	return EKeyCode::Key_Shift						;
	case Qt::Key_Control		:	return EKeyCode::Key_Control						;
	case Qt::Key_Menu			:	return EKeyCode::Key_Menu						;
	case Qt::Key_Pause			:	return EKeyCode::Key_Pause						;
	case Qt::Key_Escape			:	return EKeyCode::Key_Escape						;
	case Qt::Key_Space			:	return EKeyCode::Key_Space						;
	case Qt::Key_End			:	return EKeyCode::Key_End							;
	case Qt::Key_Home			:	return EKeyCode::Key_Home						;
	case Qt::Key_Left			:	return EKeyCode::Key_Left						;
	case Qt::Key_Up				:	return EKeyCode::Key_Up							;
	case Qt::Key_Right			:	return EKeyCode::Key_Right						;
	case Qt::Key_Down			:	return EKeyCode::Key_Down						;
	case Qt::Key_Select			:	return EKeyCode::Key_Select						;
	case Qt::Key_Print			:	return EKeyCode::Key_Print						;
	case Qt::Key_Execute		:	return EKeyCode::Key_Execute						;
	case Qt::Key_Insert			:	return EKeyCode::Key_Insert						;
	case Qt::Key_Delete			:	return EKeyCode::Key_Delete						;
	case Qt::Key_Help			:	return EKeyCode::Key_Help						;
	case Qt::Key_0				:	return EKeyCode::Key_0							;
	case Qt::Key_1				:	return EKeyCode::Key_1							;
	case Qt::Key_2				:	return EKeyCode::Key_2							;
	case Qt::Key_3				:	return EKeyCode::Key_3							;
	case Qt::Key_4				:	return EKeyCode::Key_4							;
	case Qt::Key_5				:	return EKeyCode::Key_5							;
	case Qt::Key_6				:	return EKeyCode::Key_6							;
	case Qt::Key_7				:	return EKeyCode::Key_7							;
	case Qt::Key_8				:	return EKeyCode::Key_8							;
	case Qt::Key_9				:	return EKeyCode::Key_9							;
	case Qt::Key_A				:	return EKeyCode::Key_A							;
	case Qt::Key_B				:	return EKeyCode::Key_B							;
	case Qt::Key_C				:	return EKeyCode::Key_C							;
	case Qt::Key_D				:	return EKeyCode::Key_D							;
	case Qt::Key_E				:	return EKeyCode::Key_E							;
	case Qt::Key_F				:	return EKeyCode::Key_F							;
	case Qt::Key_G				:	return EKeyCode::Key_G							;
	case Qt::Key_H				:	return EKeyCode::Key_H							;
	case Qt::Key_I				:	return EKeyCode::Key_I							;
	case Qt::Key_J				:	return EKeyCode::Key_J							;
	case Qt::Key_K				:	return EKeyCode::Key_K							;
	case Qt::Key_L				:	return EKeyCode::Key_L							;
	case Qt::Key_M				:	return EKeyCode::Key_M							;
	case Qt::Key_N				:	return EKeyCode::Key_N							;
	case Qt::Key_O				:	return EKeyCode::Key_O							;
	case Qt::Key_P				:	return EKeyCode::Key_P							;
	case Qt::Key_Q				:	return EKeyCode::Key_Q							;
	case Qt::Key_R				:	return EKeyCode::Key_R							;
	case Qt::Key_S				:	return EKeyCode::Key_S							;
	case Qt::Key_T				:	return EKeyCode::Key_T							;
	case Qt::Key_U				:	return EKeyCode::Key_U							;
	case Qt::Key_V				:	return EKeyCode::Key_V							;
	case Qt::Key_W				:	return EKeyCode::Key_W							;
	case Qt::Key_X				:	return EKeyCode::Key_X							;
	case Qt::Key_Y				:	return EKeyCode::Key_Y							;
	case Qt::Key_Z				:	return EKeyCode::Key_Z							;
	case Qt::Key_Sleep			:	return EKeyCode::Key_Sleep						;
		//case Qt::Key_Asterisk		:	return EKeyCode::Key_Multiply					;
		//case Qt::Key_Separator		:	return EKeyCode::Key_Separator					;
		//case Qt::Key_Decimal		:	return EKeyCode::Key_Decimal						;
		//case Qt::Key_Slash			:	return EKeyCode::Key_Divide						;
	case Qt::Key_F1				:	return EKeyCode::Key_F1							;
	case Qt::Key_F2				:	return EKeyCode::Key_F2							;
	case Qt::Key_F3				:	return EKeyCode::Key_F3							;
	case Qt::Key_F4				:	return EKeyCode::Key_F4							;
	case Qt::Key_F5				:	return EKeyCode::Key_F5							;
	case Qt::Key_F6				:	return EKeyCode::Key_F6							;
	case Qt::Key_F7				:	return EKeyCode::Key_F7							;
	case Qt::Key_F8				:	return EKeyCode::Key_F8							;
	case Qt::Key_F9				:	return EKeyCode::Key_F9							;
	case Qt::Key_F10			:	return EKeyCode::Key_F10							;
	case Qt::Key_F11			:	return EKeyCode::Key_F11							;
	case Qt::Key_F12			:	return EKeyCode::Key_F12							;
	case Qt::Key_F13			:	return EKeyCode::Key_F13							;
	case Qt::Key_F14			:	return EKeyCode::Key_F14							;
	case Qt::Key_F15			:	return EKeyCode::Key_F15							;
	case Qt::Key_F16			:	return EKeyCode::Key_F16							;
	case Qt::Key_F17			:	return EKeyCode::Key_F17							;
	case Qt::Key_F18			:	return EKeyCode::Key_F18							;
	case Qt::Key_F19			:	return EKeyCode::Key_F19							;
	case Qt::Key_F20			:	return EKeyCode::Key_F20							;
	case Qt::Key_F21			:	return EKeyCode::Key_F21							;
	case Qt::Key_F22			:	return EKeyCode::Key_F22							;
	case Qt::Key_F23			:	return EKeyCode::Key_F23							;
	case Qt::Key_F24			:	return EKeyCode::Key_F24							;
		//case Qt::Key_Numlock		:	return EKeyCode::Key_Numlock						;
		//	case Qt::Key_Scroll			:	return EKeyCode::Key_Scroll						;
	case Qt::Key_Comma			:	return EKeyCode::Key_Comma						;
	case Qt::Key_Plus			:	return EKeyCode::Key_Plus						;
	case Qt::Key_Minus			:	return EKeyCode::Key_Minus						;
	case Qt::Key_Period			:	return EKeyCode::Key_Period						;
	case Qt::Key_Play			:	return EKeyCode::Key_Play						;
	case Qt::Key_Zoom			:	return EKeyCode::Key_Zoom						;
	case Qt::Key_Alt			:	return EKeyCode::Key_Menu						;
	case Qt::Key_Meta			:	return EKeyCode::Key_LWin;
	case Qt::Key_VolumeDown	:	return EKeyCode::Key_Unknown;
	case Qt::Key_VolumeUp	:	return EKeyCode::Key_Unknown;

	case Qt::Key_CapsLock	:	return EKeyCode::Key_Unknown;
	
	default:
		//Unreachable;
		return EKeyCode::Key_Unknown;
	}

	return (EKeyCode)qtKeyCode;
}
//---------------------------------------------------------------------------
EMouseButton F_MapQtMouseButton( Qt::MouseButton btn )
{
	switch(btn)
	{
	case Qt::MouseButton::LeftButton :
		return EMouseButton::LeftMouseButton;

	case Qt::MouseButton::MiddleButton :
		return EMouseButton::MiddleMouseButton;

	case Qt::MouseButton::RightButton :
		return EMouseButton::RightMouseButton;

	default: Unimplemented;
	}
	return EMouseButton::LeftMouseButton;
}
//---------------------------------------------------------------------------
QPoint GetMouseCursorPosition( QWidget* w )
{
	return w->mapFromGlobal( QCursor::pos() );
}










void EdSplitPath(
	const QString& inPath,
	QString &outAbsPath,
	QString &outRelPath,
	QString &outBaseName,
	QString &outFileExt
)
{
	// e.g.: "R:/_/Bin"
	// e.g.: "R:/_/Bin"
	// e.g.: "R:/_/Bin"
	const QString appPath = QCoreApplication::applicationDirPath();


	QFileInfo	fileInfo( inPath );

	// e.g.: "R:/"
	// e.g.: "R:/TEMP"
	// e.g.: "R:/_/Bin/textures"
	const QString absPath = fileInfo.absolutePath();	

	// e.g.: "../../"
	// e.g.: "../../TEMP"
	// e.g.: "textures"
	const QString relPath = QDir( appPath ).relativeFilePath( absPath );


	outAbsPath = absPath;
	outRelPath = relPath;
	outBaseName = fileInfo.baseName();
	outFileExt = fileInfo.suffix();
}

void EdGetRelativePath(
	const QString& inPath,
	QString &outRelFilePath
)
{
	const QString appPath = QCoreApplication::applicationDirPath();

	QFileInfo	fileInfo( inPath );

	const QString relPath = QDir( appPath ).relativeFilePath( inPath );

	outRelFilePath = relPath;
}


namespace QtSupport
{

	String F_Get_Selected_File_Name( const QFileDialog& dialog )
	{
		const QStringList fileNames = dialog.selectedFiles();
		Assert( fileNames.count() == 1 );
		VRET_X_IF_NOT( fileNames.count() > 0, String() );

		return String( fileNames.first().toAscii().data() );
	}

	String F_OpenDialog_Select_Asset_DB_File(
		QWidget* parent
		, const char* initialDir
		)
	{
		QFileDialog dialog( parent, "Open an existing Asset Database - Select File" );
		{
			dialog.setViewMode(QFileDialog::Detail);
			dialog.setFileMode(QFileDialog::ExistingFile);
			dialog.setAcceptMode(QFileDialog::AcceptOpen);
		}
		dialog.setFilter("Resource Database Files (*.rdb)");	// ASSET_DB_FILE_EXTENSION
		if( initialDir != nil ) {
			dialog.setDirectory( initialDir );
		}
		dialog.selectFile("file_index.rdb");

		if( dialog.exec() )
		{
			const String	assetDbFileName = QtSupport::F_Get_Selected_File_Name( dialog );
			return assetDbFileName;
		}
		return String();
	}

}//namespace QtSupport


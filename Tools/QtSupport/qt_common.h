#pragma once

#include <Core/Serialization.h>

template< class STRING >
inline STRING& ConvertToQString( STRING& dest, const QString& src )
{
	dest.Empty();
	dest.Append( src.toAscii().data(), src.length() );
	return dest;
}
inline void SerializeQString( mxArchive & archive, QString & s )
{
	StackString	tmp;

	if( archive.IsReading() )
	{
		archive && tmp;
		s = tmp.ToChars();
	}
	if( archive.IsWriting() )
	{
		tmp.SetString( s.toAscii().data() );
		archive && tmp;
	}
}


inline bool IsStoring( QDataStream & serializer )
{
	return serializer.device()->isWritable();
}
inline bool IsLoading( QDataStream & serializer )
{
	return serializer.device()->isReadable();
}

//---------------------------------------------------------------------------
mxDECLARE_POD_TYPE(QPoint);
mxDECLARE_POD_TYPE(QSize);
//---------------------------------------------------------------------------
template< class S >
inline S& operator & ( S & serializer, QByteArray & o )
{
	SizeT size = (SizeT)o.size();
	serializer & size;

	Assert( size < F_GetMaxAllowedAllocationSize() );

	if( size > 0 )
	{
		o.resize( size );
		serializer.SerializeMemory( o.data(), size );
	}

	return serializer;
}
//---------------------------------------------------------------------------
inline mxArchive& operator && ( mxArchive & serializer, QByteArray & o )
{
	return Serialize_ArcViaBin( serializer, o );
}
//---------------------------------------------------------------------------
template< class S >
inline S& operator & ( S & serializer, QString & o )
{
	S32 size = o.size();
	serializer & size;

	if( size > 0 )
	{
		o.resize( size );
		serializer.SerializeMemory( o.data(), size );
	}

	return serializer;
}
//---------------------------------------------------------------------------

QDataStream& SerializeWidgetSize( QDataStream & serializer, QWidget * o );
//---------------------------------------------------------------------------

QDataStream& SerializeWidgetGeometry( QDataStream & serializer, QWidget * o );

//---------------------------------------------------------------------------
QDataStream& SerializeDockWidgetState(
	QDataStream & serializer,
	QMainWindow* mainWindow, Qt::DockWidgetArea defaultValue,
	QDockWidget * o
);
//---------------------------------------------------------------------------
QDataStream& SerializeMainWindowState( QDataStream & serializer, QMainWindow * o );
//---------------------------------------------------------------------------
inline
QDataStream& SerializeMainWindow_GeometryAndState( QDataStream & serializer, QMainWindow * o )
{
	SerializeWidgetGeometry( serializer, o );
	SerializeMainWindowState( serializer, o );
	return serializer;
}
//---------------------------------------------------------------------------


template< class STRING >
inline
bool SelectFiles( QFileDialog& dialog, const char* filter,
				 STRING & in_out_dir, TList<STRING> & out_files )
{
	dialog.setFileMode(QFileDialog::ExistingFiles);
	dialog.setNameFilter(filter);
	dialog.setDirectory(in_out_dir.ToChars());

	if (dialog.exec())
	{
		out_files.Empty();
		foreach(QString fileName, dialog.selectedFiles())
		{
			STRING & newFile = out_files.Add();
			newFile.SetString(fileName.toAscii().data());
		}

		in_out_dir.SetString(dialog.directory().path().toAscii().data());

		return true;
	}

	return false;
}
//---------------------------------------------------------------------------
inline
bool SelectFolder( QFileDialog& dialog,
				 OSPathName & in_out_dir )
{
	dialog.setFileMode(QFileDialog::DirectoryOnly);
	dialog.setDirectory(in_out_dir.ToChars());

	if (dialog.exec())
	{
		in_out_dir.SetString(dialog.directory().path().toAscii().data());
		F_NormalizePath(in_out_dir);
 		return true;
	}

	return false;
}
//---------------------------------------------------------------------------

//void SetWidgetBackgroundColor( QWidget* w );

//---------------------------------------------------------------------------

FORCEINLINE bool KeyCodeIsDirection( const int key )
{
	//return key >= Qt::Key_Left && key <= Qt::Key_Down;
	return key == Qt::Key_Left
		|| key == Qt::Key_Up
		|| key == Qt::Key_Right
		|| key == Qt::Key_Down

		|| key == Qt::Key_Direction_L
		|| key == Qt::Key_Direction_R

		;
}

//---------------------------------------------------------------------------
void SetWidgetToScreenCenter(QWidget* pkWidget);


/*
-----------------------------------------------------------------------------
	EdMainWindow
-----------------------------------------------------------------------------
*/
class EdMainWindow : public QMainWindow
{
public:
	typedef QMainWindow Super;

	EdMainWindow( QWidget* parent = nil )
		: Super( parent )
	{}

	~EdMainWindow()
	{}
};






void EdSplitPath(
	const QString& inPath,
	QString &outAbsPath,
	QString &outRelPath,	// relative to app .exe
	QString &outBaseName,
	QString &outFileExt
);

void EdGetRelativePath(
	const QString& inPath,
	QString &outRelFilePath
);




// used mainly for testing and debugging
inline void ShowQMessageBox( PCSTR text )
{
	QMessageBox msgBox;
	msgBox.setText( text );
	msgBox.exec();
}


namespace QtSupport
{

	String F_Get_Selected_File_Name(
		const QFileDialog& dialog
		);

	String F_OpenDialog_Select_Asset_DB_File(
		QWidget* parent
		, const char* initialDir = nil
		);


}//namespace QtSupport




//== MACRO =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//
#define qtCONNECT( sender, signal, receiver, method )\
	mxENSURE( QObject::connect( sender, signal, receiver, method ) )



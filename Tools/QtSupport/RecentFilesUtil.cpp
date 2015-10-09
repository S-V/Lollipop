#include <stdafx.h>
#pragma hdrstop

#include <EditorSupport/Serialization/TextSerializer.h>

#include "qt_common.h"
#include "RecentFilesUtil.h"

/*
-----------------------------------------------------------------------------
	RecentFilesUtil
-----------------------------------------------------------------------------
*/
RecentFilesUtil::RecentFilesUtil( IClient& client )
	: m_client( client )
{
}

void RecentFilesUtil::Serialize( QDataStream & stream )
{
	UNDONE;
	//if(IsLoading(stream))
	//{
	//	stream >> m_fileList;
	//}
	//else
	//{
	//	stream << m_fileList;
	//}
}

void RecentFilesUtil::Serialize( ATextSerializer & serializer )
{
	serializer.Serialize_StringList( "Recent_Files", m_fileList );
	//serializer.Enter_Scope("Recent_Files");
	//{
	//	UINT	numFiles = m_fileList.Num();
	//	serializer.Serialize_Uint32("Num_Recent_Files", numFiles);


	//	if( serializer.IsStoring() )
	//	{
	//		foreach( const QString fileName, m_fileList )
	//		{
	//			//serializer.Serialize_String();
	//		}
	//	}
	//	if( serializer.IsLoading() )
	//	{
	//		//UNDONE;
	//	}
	//}
	//serializer.Leave_Scope();
}

//-----------------------------------------------------------------------------
void RecentFilesUtil::CreateMenus( QMenu* filesMenu )
{
	if( m_fileList.IsEmpty() )
	{
		return;
	}

	m_menuSeparator = filesMenu->addSeparator();

	m_recentFilesMenu = filesMenu->addMenu(tr("Recent projects"));

	for( int i = m_fileList.Num()-1; i >= 0; i-- )
	{
		const QString	path = m_fileList[i];

		Assert(!path.isEmpty());

		QAction* newAct = new QAction(path, filesMenu);

		newAct->setText(QString(path));
		newAct->setData(QString(path));

		m_recentFilesMenu->addAction(newAct);

		connect( newAct, SIGNAL(triggered()),
			this, SLOT(openRecentFile()));
	}

	filesMenu->addSeparator();
}
//-----------------------------------------------------------------------------
void RecentFilesUtil::Add( const char* fileName )
{
	const UINT index = m_fileList.FindIndexOf( fileName );

	if( index != INDEX_NONE )
	{
		if( m_fileList.Num() > 1 )
		{
			String & firstItem = m_fileList[0];
			String & thisItem = m_fileList[index];

			TSwap( firstItem, thisItem );
		}
		return;
	}

	String	newItem( fileName );

	m_fileList.Add( newItem );

	if( m_fileList.Num() > MaxRecentFiles )
	{
		m_fileList.RemoveAt( 0 );
	}
}
//-----------------------------------------------------------------------------
bool RecentFilesUtil::Find( const char* fileName )
{
	return m_fileList.FindIndexOf( fileName ) != INDEX_NONE;
}
//-----------------------------------------------------------------------------
void RecentFilesUtil::Remove( const char* fileName )
{
	m_fileList.RemoveAll( fileName );

	if( m_fileList.IsEmpty() )
	{
		if( m_recentFilesMenu != nil )
		{
			m_recentFilesMenu->hide();
		}
		if( m_menuSeparator != nil )
		{
			m_menuSeparator->setVisible(false);
		}
	}
}

void RecentFilesUtil::openRecentFile()
{
	QAction *action = qobject_cast< QAction* >( this->sender() );
	AssertPtr(action);
	if( action != nil )
	{
		const String fileName( action->text().toAscii().data() );

		const int selectedIndex = m_fileList.FindIndexOf( fileName );
		Assert( selectedIndex != INDEX_NONE );
		(void)selectedIndex;

		const bool bOk = m_client.OpenRecentFile( fileName );
		if( !bOk )
		{
			if( this->Find( fileName ) )
			{
				QMessageBox::StandardButton ret =
					QMessageBox::warning(
					m_recentFilesMenu, tr("Application"),
					tr("The project cannot be opened.\n"
					"Do you want to remove it from the recent files list?"),
					QMessageBox::Ok | QMessageBox::Cancel
				);
				if( QMessageBox::Ok == ret )
				{
					this->Remove( fileName );
				}
			}
		}
	}
}


#include <EditorSupport_PCH.h>
#pragma hdrstop

#include "DevFileMonitor.h"

// time delay in milliseconds
static const UINT TIME_DELAY_MSEC = 200;

/*
-----------------------------------------------------------------------------
	DevFileMonitor
-----------------------------------------------------------------------------
*/
DevFileMonitor::DevFileMonitor( FileWatcherListener& clientListener )
	: m_clientListener( clientListener )
{
}

DevFileMonitor::~DevFileMonitor()
{
}

void DevFileMonitor::Start_Listening( const char* folder )
{
	AssertPtr(folder);
	VRET_IF_NIL(folder);
	Assert( m_fileWatcher == nil );
	Assert( m_fileWatchID == INDEX_NONE );
	if( m_fileWatcher == nil )
	{
		DEVOUT("DevFileMonitor::Install_File_Watcher: folder = '%s'\n", folder);

		m_fileWatcher = FileWatcher::New();
		m_fileWatchID = m_fileWatcher->addWatch(mxTO_UNICODE(folder));
		m_fileWatcher->listener = this;
	}
}

void DevFileMonitor::End_Listening()
{
	if( m_fileWatcher != nil )
	{
		Assert( m_fileWatchID != INDEX_NONE );
		if( m_fileWatchID != INDEX_NONE )
		{
			const mxChar* dirName = m_fileWatcher->getWatchedFolder( m_fileWatchID );
			AssertPtr(dirName);

			DEVOUT("DevFileMonitor::End_Listening: folder = '%s'\n", mxTO_ANSI(dirName));

			m_fileWatcher->removeWatch( m_fileWatchID );
			m_fileWatcher = nil;
		}
	}
	m_fileWatchID = INDEX_NONE;
}

bool DevFileMonitor::IsOpened() const
{
	return m_fileWatcher != nil;
}

void DevFileMonitor::Monitor_File_Changes()
{
	AssertPtr(m_fileWatcher);
	if( m_fileWatcher != nil ) {
		m_fileWatcher->update();
	}
}

void DevFileMonitor::onFileCreated( const mxChar* filename, const mxChar* directory, const WatchID watchId )
{
	DBGOUT( "Created File '%s' in '%s'\n", mxTO_ANSI(filename), mxTO_ANSI(directory) );
	m_clientListener.onFileCreated( filename, directory, watchId );
}

void DevFileMonitor::onFileDeleted( const mxChar* filename, const mxChar* directory, const WatchID watchId )
{
	DBGOUT( "Deleted File '%s' in '%s'\n", mxTO_ANSI(filename), mxTO_ANSI(directory) );
	m_clientListener.onFileDeleted( filename, directory, watchId );
}

void DevFileMonitor::onFileChanged( const mxChar* filename, const mxChar* directory, const WatchID watchId )
{
	DBGOUT( "Changed File '%s' in '%s'\n", mxTO_ANSI(filename), mxTO_ANSI(directory) );
	m_clientListener.onFileChanged( filename, directory, watchId );
}

void DevFileMonitor::onFileRenamed( const mxChar* oldFilename, const mxChar* newFilename, const mxChar* directory, const WatchID watchId )
{
	FileWatcherListener::onFileRenamed( oldFilename, newFilename, directory, watchId );
	m_clientListener.onFileRenamed( oldFilename, newFilename, directory, watchId );
}


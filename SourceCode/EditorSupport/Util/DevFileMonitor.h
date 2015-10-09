#pragma once

#include <EditorSupport/Util/FileWatcher.h>

/*
-----------------------------------------------------------------------------
	DevFileMonitor

	watches a single directory
	and notifies the listener about any file changes
-----------------------------------------------------------------------------
*/
class DevFileMonitor : public FileWatcherListener
{
public:
	DevFileMonitor( FileWatcherListener& clientListener );
	virtual ~DevFileMonitor();

	void Start_Listening( const char* folder );
	void End_Listening();

	bool IsOpened() const;

	// should be called periodically to track directory changes
	void Monitor_File_Changes();

public:	//=-- FileWatcherListener
	virtual void onFileCreated( const mxChar* filename, const mxChar* directory, const WatchID watchId ) override;
	virtual void onFileDeleted( const mxChar* filename, const mxChar* directory, const WatchID watchId ) override;
	virtual void onFileChanged( const mxChar* filename, const mxChar* directory, const WatchID watchId ) override;
	virtual void onFileRenamed( const mxChar* oldFilename, const mxChar* newFilename, const mxChar* directory, const WatchID watchId ) override;

private:
	FileWatcherListener &	m_clientListener;

	TAutoPtr< FileWatcher >		m_fileWatcher;
	WatchID						m_fileWatchID;	// ID of watched directory
};

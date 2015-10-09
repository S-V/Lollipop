#ifndef qtUtils_RecentFilesUtil_H
#define qtUtils_RecentFilesUtil_H

class ATextSerializer;

/*
-----------------------------------------------------------------------------
	RecentFilesUtil
	todo: access-based reordering (MRU)
-----------------------------------------------------------------------------
*/
class RecentFilesUtil : public QObject
{
	Q_OBJECT

public:
	struct IClient
	{
		// returns false if the file couldn't be opened
		// and should be removed from the recent files list
		//
		virtual bool OpenRecentFile( const char* fileName ) = 0;
	};

	RecentFilesUtil( IClient& client );

	void CreateMenus( QMenu* filesMenu );

	void Add( const char* fileName );	// add unique file path
	bool Find( const char* fileName );
	void Remove( const char* fileName );

	template< class S >
	friend S& operator & ( S & serializer, RecentFilesUtil & o )
	{
		serializer & o.m_fileList;
		return serializer;
	}

	void Serialize( QDataStream & stream );
	void Serialize( ATextSerializer & serializer );

	UINT Num() const
	{
		return m_fileList.Num();
	}
	const String& GetFirst() const
	{
		return m_fileList[0];
	}

private slots:
	void openRecentFile();

private:
	enum { MaxRecentFiles = 10 };

	StringListType	m_fileList;

	TPtr< QAction >	m_menuSeparator;
	TPtr< QMenu >	m_recentFilesMenu;

	IClient & m_client;
};


#endif // qtUtils_RecentFilesUtil_H

#pragma once

#include <Core/Editor.h>

/*
template< typename TYPE, UINT SIZE >
class TFixedGrowableAllocator
{
	TYPE 		mStaticData[ SIZE ];
	TYPE * 		mDynamicData;
	HMemory		mMemory;	// Handle to the memory manager

public:
	inline TFixedGrowableAllocator( HMemory hMemoryMgr = EMemHeap::DefaultHeap )
		: mMemory( EMemHeap::DefaultHeap )
	{
		mDynamicData = nil;
	}
	inline ~TDefaultAllocator()
	{

	}
	inline void* AllocateMemory( SizeT size )
	{
		return F_GetMemoryManager( mMemory )->Allocate( size);
	}
	inline void ReleaseMemory( void* ptr )
	{
		F_GetMemoryManager( mMemory )->Free( ptr );
	}
};

template< typename TYPE >
class TList : public TLinearBuffer< TYPE, U4 >
{
public:
	TList()
		: TLinearBuffer()
	{}

	explicit TList( HMemory hMemoryMgr )
		: TLinearBuffer( hMemoryMgr )
	{}

	explicit TList( EInPlaceSerializationHelper )
		: TLinearBuffer( _FinishedLoadingFlag )
	{}
};
*/


/*
-----------------------------------------------------------------------------
	EdMimeData
-----------------------------------------------------------------------------
*/
class EdMimeData : public QMimeData
{
	Q_OBJECT

public:
	EdMimeData();
	~EdMimeData();

public:
	mxOPTIMIZE("Use TLocalArray - fixed-size buffer + uses dynamic memory if needed");
	mutable TList<AEditable*>	objects;	// never empty
};

template< class MODEL >
EdMimeData* ZZ_Template_CreateMimeData( MODEL* model, const QModelIndexList& indexes )
{
	EdMimeData* pNewMimeData = new EdMimeData();

	const UINT num = indexes.count();

	pNewMimeData->objects.Reserve(num);

	foreach( const QModelIndex modelIndex, indexes )
	{
		AEditable* pObject = model->NodeFromIndex(modelIndex);

		pNewMimeData->objects.Add( pObject->edGetDropProxy() );
	}

	return pNewMimeData;
}

mxSWIPED("http://www.google.com/codesearch#po2oiEQdsu8/trunk/Code/Editor/TamyEditor/DropArea.h");
#if 0
/**
 * A widget that accepts drag&drops.
 */
class DropArea : public QLabel
{
   Q_OBJECT

private:
   QLabel*           m_label;
   MimeDataEncoder*  m_dataEncoder;

public:
   /**
    * Constructor.
    */
   DropArea( QWidget* parent, MimeDataEncoder* dataEncoder );
   ~DropArea();

public slots:
   void clear();

signals:
   void changed( const QMimeData *mimeData = 0 );

protected:
   void dragEnterEvent( QDragEnterEvent *event );
   void dragMoveEvent( QDragMoveEvent *event );
   void dragLeaveEvent( QDragLeaveEvent *event );
   void dropEvent( QDropEvent *event );
};
#endif

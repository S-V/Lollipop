/*
=============================================================================
	File:	TLinkedList.h
	Desc:	A simple linked list template class.
=============================================================================
*/

#ifndef __MX_DOUBLY_LINKED_LIST_H__
#define __MX_DOUBLY_LINKED_LIST_H__

mxNAMESPACE_BEGIN

//------------------------------------------------------
// Doubly linked list template.
//------------------------------------------------------
//
template< class T >
class TLinkedList {
public:
					TLinkedList();
					TLinkedList( const TLinkedList< T > & other );  // the list doesn't delete data in here
					~TLinkedList();  // the list doesn't delete data in here

	void		operator = ( const TLinkedList< T > & other );  // the list doesn't delete data in here

	bool		IsEmpty() const;
	INT			Count() const;
	INT			Num() const;

	bool		Contains( const T & value ) const;

	void		Append( const T & value );
	void		Prepend( const T & value );

	void		RemoveFirst();
	void		RemoveLast();

				// Deletes and removes all elements ( assumed T is a pointer ).
	void		Clear();

				// Removes all elements without deleting them.
	void		RemoveAll();

	T &			Head() { return m_head->data; }
	T &			Tail() { return m_tail->data; }

protected:
	// list element node with pointers to previous and next element in the list.
	struct listnode_s {
		listnode_s( const T & value ) : data( value ), next( 0 ), prev( 0 )  { }

		listnode_s *	next;
		listnode_s *	prev;
		T				data;
	};

protected:
	listnode_s *		m_head;
	listnode_s *		m_tail;
	INT					m_size;		// number of elements in this list

public:
	// TLinkedList iterator
	class Iterator {
	public:
					Iterator() : current( 0 )  { }

		Iterator &	operator ++ () {
			current = current->next;	
			return *this;
		}

		Iterator &	operator -- () {	// prefix form
			current = current->prev;	
			return *this;
		}

		Iterator &	operator ++ ( INT ) {	// postfix
			Iterator p = *this;
			current = current->next;	
			return p;
		}

		Iterator &	operator -- ( INT ) {
			Iterator p = *this;
			current = current->prev;	
			return p;
		}
		
		Iterator	operator + ( INT num ) const {
			Iterator p = *this;

			if ( num >= 0 ) {
				while ( num-- && p.current ) {
					++p;
				}
			} else {
				while ( num++ && p.current ) {
					--p;
				}
			}

			return p;
		}

		Iterator &	operator += ( INT num ) {
			if ( num >= 0 ) {
				while ( num-- && this->current ) {
					++( *this );
				}
			} else {
				while ( num++ && this->current ) {
					--( *this );
				}
			}
			return *this;
		}

		Iterator	operator - ( INT num ) const {
			return ( *this ) + ( -num );
		}

		Iterator	operator -= ( INT num ) const {
			( *this ) += ( -num );
			return *this;
		}

		bool	operator == ( const Iterator & other ) const {
			return current == other.current;
		}

		bool	operator != ( const Iterator & other ) const {
			return current != other.current;
		}

		T &		operator * () {
			return current->data;
		}

		//----------------------------
		bool	HasCurrent() {
			return current != NULL;
		}

		bool	HasNext() {
			return current->next != NULL;
		}

		bool	HasPrev() {
			return current->prev != NULL;
		}

	private:
		friend class TLinkedList< T >;

		Iterator( listnode_s * begin ) : current( begin )  { }

		listnode_s *	current;
	};


	// Gets begin node. Returns a list iterator pointing to the beginning of the list.
	Iterator	Begin() const { return Iterator( m_head ); }

	// Gets end node. Returns a list iterator pointing to null.
	Iterator	End() const { return Iterator( 0 ); }

	// Gets last element. Returns a list iterator pointing to the end of the list.
	Iterator    GetLast() const { return Iterator( m_tail ); }

	//-----------------------------------------------------------------------
	//	Inserts an element after an element. 
	//  Accepts iterator pointing to element after the new element 
	//  and the new element to be inserted into the list.
	//-----------------------------------------------------------------------
	void		InsertAfter( Iterator & i, const T & value ) {
		listnode_s * node = new listnode_s( value );

		node->next = i.current->next;

		if ( i.current->next ) {
			i.current->next->prev = node;
		}

		node->prev = i.current;
		i.current->next = node;
		++m_size;

		if ( i.current == m_tail ) {
			m_tail = node;
		}
	}

	//-----------------------------------------------------------------------
	//	Inserts an element before an element. 
	//  Accepts iterator pointing to element before the new element 
	//  and the new element to be inserted into the list.
	//-----------------------------------------------------------------------
	void		InsertBefore( Iterator & i, const T & value ) {
		listnode_s * node = new listnode_s( value );

		node->prev = i.current->prev;

		if ( i.current->prev ) {
			i.current->prev->next = node;
		}

		node->next = i.current;
	}

	//----------------------------------------------------------------------
	//	Erases an element. 
	//  Accepts an iterator, pointing to the element which shall be erased.
	//  Returns iterator pointing to next element.
	//----------------------------------------------------------------------
	Iterator	Erase( Iterator & i ) {
		Iterator	returnIterator( i );
		++returnIterator;

		if ( i.current == m_head ) {
			m_head = i.current->next;
		}
		if ( i.current == m_tail ) {
			m_tail = i.current->prev;
		}
		if ( i.current->next ) {
			i.current->next->prev = i.current->prev;
		}
		if ( i.current->prev ) {
			i.current->prev->next = i.current->next;
		}

		delete i.current;
		i.current = 0;
		--m_size;

		return returnIterator;
	}

	Iterator	Remove( Iterator & i ) {
		Iterator	returnIterator( i );
		++returnIterator;

		if ( i.current == m_head ) {
			m_head = i.current->next;
		}
		if ( i.current == m_tail ) {
			m_tail = i.current->prev;
		}
		if ( i.current->next ) {
			i.current->next->prev = i.current->prev;
		}
		if ( i.current->prev ) {
			i.current->prev->next = i.current->next;
		}

	//	delete i.current;
		i.current = 0;
		--m_size;

		return returnIterator;
	}
};

template< class T >
FORCEINLINE TLinkedList< T >::TLinkedList() : m_head( 0 ), m_tail( 0 ), m_size( 0 ) {
}

template< class T >
FORCEINLINE TLinkedList< T >::TLinkedList( const TLinkedList< T > & other ) 
: m_head( 0 ), m_tail( 0 ), m_size( 0 ) {
	*this = other;
}

template< class T >
FORCEINLINE TLinkedList< T >::~TLinkedList() {
	RemoveAll();
}

template< class T >
FORCEINLINE void TLinkedList< T >::operator = ( const TLinkedList< T > & other ) {
	RemoveAll();

	listnode_s * node = other.m_head;
	while ( node ) {
		Append( node->data );
		node = node->next;
	}
}

template< class T >
FORCEINLINE bool TLinkedList< T >::IsEmpty() const {
	return  m_head == NULL;
}

template< class T >
FORCEINLINE INT TLinkedList< T >::Count() const {
	return Num();
}

template< class T >
FORCEINLINE INT TLinkedList< T >::Num() const {
	return m_size;
}

//-------------------------------------------------
//	Adds an element at the end of the list.
//-------------------------------------------------
template< class T >
FORCEINLINE void TLinkedList< T >::Append( const T & value ) {
	listnode_s * node = new listnode_s( value );

	++m_size;

	if ( NULL == m_head ) {
		m_head = node;
	}
	node->prev = m_tail;

	if ( NULL != m_tail ) {
		m_tail->next = node;
	}

	m_tail = node;
}

//-------------------------------------------------
//	Adds an element at the beginning of the list.
//-------------------------------------------------
template< class T >
FORCEINLINE void TLinkedList< T >::Prepend( const T & value ) {
	listnode_s * node = new listnode_s( value );

	++m_size;

	if ( NULL = m_head ) {
		m_tail = node;
		m_head = node;
	} else {
		node->next = m_head;
		m_head->prev = node;
		m_head = node;
	}
}

//-------------------------------------------------
//	Clears the list. Removes all elements from the list.
//-------------------------------------------------
template< class T >
FORCEINLINE void TLinkedList< T >::RemoveAll() {
	listnode_s * node = m_head;
	while ( node ) {
		listnode_s *  next = node->next;
		delete	node;
		node = next;
	}

	m_head = NULL;
	m_tail = NULL;
	m_size = 0;
}

//-------------------------------------------------
// Deletes and removes all elements.
//-------------------------------------------------
template< class T >
FORCEINLINE void TLinkedList< T >::Clear() {
	listnode_s * node = m_head;
	while ( node ) {
		listnode_s *  next = node->next;
		delete node->data;
		delete node;
		node = next;
	}

	m_head = NULL;
	m_tail = NULL;
	m_size = 0;
}

//---------------------------------------------------------
//	Singly linked lists.
//---------------------------------------------------------

//---------------------------------------------------------
//
//	void PrependItem< T >( T *& pList, T * item )
//
//	Prepends the given item to the linked list.
//	Modifies both parameters !
//	The type 'T' must have a member function
//		void SetNext( T * elem );
//
/* Usage example:
		Face * face = polygonList;
		Face * OutPolys = NULL;
		while ( face )
		{
			Face * next = face->GetNext();
			PrependItem( OutPolys, face );
			face = next;
		}

		'OutPolys' are the same as 'polygonList'.
*/
template< typename T >
void PrependItem( T *& pList, T * item )
{	mxBREAK_IF( !item || (pList == item) );

	item->SetNext( pList );
	pList = item;
}

mxNAMESPACE_END

#endif // ! __MX_DOUBLY_LINKED_LIST_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//

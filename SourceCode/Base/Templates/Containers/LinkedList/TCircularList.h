/*
=============================================================================
	File:	TCircularList.h
	Desc:
=============================================================================
*/

#ifndef __MX_CONTAINTERS_CIRCULAR_LIST_H__
#define __MX_CONTAINTERS_CIRCULAR_LIST_H__

mxNAMESPACE_BEGIN

mxSWIPED("Id Software, idLib")
//
//	TCircularList
//
//	Swiped from Id Software.
//
template< typename T >
class TCircularList {
public:
						TCircularList( T* Owner = NULL );
						~TCircularList();

	bool				IsListEmpty( void ) const;
	bool				InList( void ) const;
	INT					Num( void ) const;
	void				Clear( void );

	void				InsertBefore( TCircularList &node );
	void				InsertAfter( TCircularList &node );
	void				AddToEnd( TCircularList &node );
	void				AddToFront( TCircularList &node );

	void				Remove( void );

	T *					Next( void ) const;
	T *					Prev( void ) const;

	T *					Owner( void ) const;
	void				SetOwner( T *object );

	TCircularList *		ListHead( void ) const;
	TCircularList *		NextNode( void ) const;
	TCircularList *		PrevNode( void ) const;

private:
	TCircularList *		head;
	TCircularList *		next;
	TCircularList *		prev;
	T *					owner;
};

/*
================
TCircularList<T>::TCircularList

Node is initialized to be the head of an empty list
================
*/
template< typename T >
TCircularList<T>::TCircularList( T* Owner = NULL )
{
	owner	= Owner;
	head	= this;
	next	= this;
	prev	= this;
}

/*
================
TCircularList<T>::~TCircularList

Removes the node from the list, or if it's the head of a list, removes
all the nodes from the list.
================
*/
template< typename T >
TCircularList<T>::~TCircularList() {
	Clear();
}

/*
================
TCircularList<T>::IsListEmpty

Returns true if the list is empty.
================
*/
template< typename T >
bool TCircularList<T>::IsListEmpty( void ) const {
	return head->next == head;
}

/*
================
TCircularList<T>::InList

Returns true if the node is in a list.  If called on the head of a list, will always return false.
================
*/
template< typename T >
bool TCircularList<T>::InList( void ) const {
	return head != this;
}

/*
================
TCircularList<T>::Num

Returns the number of nodes in the list.
================
*/
template< typename T >
INT TCircularList<T>::Num( void ) const {
	TCircularList<T>	*node;
	INT					num;

	num = 0;
	for( node = head->next; node != head; node = node->next ) {
		num++;
	}

	return num;
}

/*
================
TCircularList<T>::Clear

If node is the head of the list, clears the list.  Otherwise it just removes the node from the list.
================
*/
template< typename T >
void TCircularList<T>::Clear( void ) {
	if ( head == this ) {
		while( next != this ) {
			next->Remove();
		}
	} else {
		Remove();
	}
}

/*
================
TCircularList<T>::Remove

Removes node from list
================
*/
template< typename T >
void TCircularList<T>::Remove( void ) {
	prev->next = next;
	next->prev = prev;

	next = this;
	prev = this;
	head = this;
}

/*
================
TCircularList<T>::InsertBefore

Places the node before the existing node in the list.  If the existing node is the head,
then the new node is placed at the end of the list.
================
*/
template< typename T >
void TCircularList<T>::InsertBefore( TCircularList &node ) {
	Remove();

	next		= &node;
	prev		= node.prev;
	node.prev	= this;
	prev->next	= this;
	head		= node.head;
}

/*
================
TCircularList<T>::InsertAfter

Places the node after the existing node in the list.  If the existing node is the head,
then the new node is placed at the beginning of the list.
================
*/
template< typename T >
void TCircularList<T>::InsertAfter( TCircularList &node ) {
	Remove();

	prev		= &node;
	next		= node.next;
	node.next	= this;
	next->prev	= this;
	head		= node.head;
}

/*
================
TCircularList<T>::AddToEnd

Adds node at the end of the list
================
*/
template< typename T >
void TCircularList<T>::AddToEnd( TCircularList &node ) {
	InsertBefore( *node.head );
}

/*
================
TCircularList<T>::AddToFront

Adds node at the beginning of the list
================
*/
template< typename T >
void TCircularList<T>::AddToFront( TCircularList &node ) {
	InsertAfter( *node.head );
}

/*
================
TCircularList<T>::ListHead

Returns the head of the list.  If the node isn't in a list, it returns
a pointer to itself.
================
*/
template< typename T >
TCircularList<T> *TCircularList<T>::ListHead( void ) const {
	return head;
}

/*
================
TCircularList<T>::Next

Returns the next object in the list, or NULL if at the end.
================
*/
template< typename T >
T *TCircularList<T>::Next( void ) const {
	if ( !next || ( next == head ) ) {
		return NULL;
	}
	return next->owner;
}

/*
================
TCircularList<T>::Prev

Returns the previous object in the list, or NULL if at the beginning.
================
*/
template< typename T >
T *TCircularList<T>::Prev( void ) const {
	if ( !prev || ( prev == head ) ) {
		return NULL;
	}
	return prev->owner;
}

/*
================
TCircularList<T>::NextNode

Returns the next node in the list, or NULL if at the end.
================
*/
template< typename T >
TCircularList<T> *TCircularList<T>::NextNode( void ) const {
	if ( next == head ) {
		return NULL;
	}
	return next;
}

/*
================
TCircularList<T>::PrevNode

Returns the previous node in the list, or NULL if at the beginning.
================
*/
template< typename T >
TCircularList<T> *TCircularList<T>::PrevNode( void ) const {
	if ( prev == head ) {
		return NULL;
	}
	return prev;
}

/*
================
TCircularList<T>::Owner

Gets the object that is associated with this node.
================
*/
template< typename T >
T * TCircularList<T>::Owner( void ) const {
	return owner;
}

/*
================
TCircularList<T>::SetOwner

Sets the object that this node is associated with.
================
*/
template< typename T >
void TCircularList<T>::SetOwner( T *object ) {
	owner = object;
}

mxNAMESPACE_END

#endif // ! __MX_CONTAINTERS_CIRCULAR_LIST_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//

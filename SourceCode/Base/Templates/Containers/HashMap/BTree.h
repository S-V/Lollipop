/*
=============================================================================
	File:	BTree.h
	Desc:	Balanced Search Tree.
	Copyright (C) 2004 Id Software, Inc.
=============================================================================
*/

#ifndef __BTREE_H__
#define __BTREE_H__

//#define BTREE_CHECK

mxNAMESPACE_BEGIN
mxSWIPED("Id Software");
//
//	mxBTreeNode< typename objType, typename keyType >
//
template< typename objType, typename keyType >
class mxBTreeNode {
public:
	keyType							key;			// key used for sorting
	objType *						object;			// if != NULL pointer to object stored in leaf node
	mxBTreeNode *					parent;			// parent node
	mxBTreeNode *					next;			// next sibling
	mxBTreeNode *					prev;			// prev sibling
	INT								numChildren;	// number of children
	mxBTreeNode *					firstChild;		// first child
	mxBTreeNode *					lastChild;		// last child
};

//
//	mxBTree< class objType, class keyType, INT maxChildrenPerNode >
//
template< typename objType, typename keyType, INT maxChildrenPerNode >
class mxBTree {
public:
									mxBTree( void );
									~mxBTree( void );

	void							Init( void );
	void							Shutdown( void );

	mxBTreeNode<objType,keyType> *	Add( objType *object, keyType key );						// add an object to the tree
	void							Remove( mxBTreeNode<objType,keyType> *node );				// remove an object node from the tree

	objType *						Find( keyType key ) const;									// find an object using the given key
	objType *						FindSmallestLargerEqual( keyType key ) const;				// find an object with the smallest key larger equal the given key
	objType *						FindLargestSmallerEqual( keyType key ) const;				// find an object with the largest key smaller equal the given key

	mxBTreeNode<objType,keyType> *	GetRoot( void ) const;										// returns the root node of the tree
	INT								GetNodeCount( void ) const;									// returns the total number of nodes in the tree
	mxBTreeNode<objType,keyType> *	GetNext( mxBTreeNode<objType,keyType> *node ) const;		// goes through all nodes of the tree
	mxBTreeNode<objType,keyType> *	GetNextLeaf( mxBTreeNode<objType,keyType> *node ) const;	// goes through all leaf nodes of the tree

private:
	mxBTreeNode<objType,keyType> *	root;
	mxBlockAlloc<mxBTreeNode<objType,keyType>,128>	nodeAllocator;

	mxBTreeNode<objType,keyType> *	AllocNode( void );
	void							FreeNode( mxBTreeNode<objType,keyType> *node );
	void							SplitNode( mxBTreeNode<objType,keyType> *node );
	mxBTreeNode<objType,keyType> *	MergeNodes( mxBTreeNode<objType,keyType> *node1, mxBTreeNode<objType,keyType> *node2 );

	void							CheckTree_r( mxBTreeNode<objType,keyType> *node, INT &numNodes ) const;
	void							CheckTree( void ) const;
};

template< typename objType, typename keyType, INT maxChildrenPerNode >
FORCEINLINE mxBTree<objType,keyType,maxChildrenPerNode>::mxBTree( void ) {
	Assert( maxChildrenPerNode >= 4 );
	root = NULL;
}

template< typename objType, typename keyType, INT maxChildrenPerNode >
FORCEINLINE mxBTree<objType,keyType,maxChildrenPerNode>::~mxBTree( void ) {
	Shutdown();
}

template< typename objType, typename keyType, INT maxChildrenPerNode >
FORCEINLINE void mxBTree<objType,keyType,maxChildrenPerNode>::Init( void ) {
	root = AllocNode();
}

template< typename objType, typename keyType, INT maxChildrenPerNode >
FORCEINLINE void mxBTree<objType,keyType,maxChildrenPerNode>::Shutdown( void ) {
	nodeAllocator.Shutdown();
	root = NULL;
}

template< typename objType, typename keyType, INT maxChildrenPerNode >
FORCEINLINE mxBTreeNode<objType,keyType> *mxBTree<objType,keyType,maxChildrenPerNode>::Add( objType *object, keyType key ) {
	mxBTreeNode<objType,keyType> *node, *child, *newNode;

	if ( root->numChildren >= maxChildrenPerNode ) {
		newNode = AllocNode();
		newNode->key = root->key;
		newNode->firstChild = root;
		newNode->lastChild = root;
		newNode->numChildren = 1;
		root->parent = newNode;
		SplitNode( root );
		root = newNode;
	}

	newNode = AllocNode();
	newNode->key = key;
	newNode->object = object;

	for ( node = root; node->firstChild != NULL; node = child ) {

		if ( key > node->key ) {
			node->key = key;
		}

		// find the first child with a key larger equal to the key of the new node
		for( child = node->firstChild; child->next; child = child->next ) {
			if ( key <= child->key ) {
				break;
			}
		}

		if ( child->object ) {

			if ( key <= child->key ) {
				// insert new node before child
				if ( child->prev ) {
					child->prev->next = newNode;
				} else {
					node->firstChild = newNode;
				}
				newNode->prev = child->prev;
				newNode->next = child;
				child->prev = newNode;
			} else {
				// insert new node after child
				if ( child->next ) {
					child->next->prev = newNode;
				} else {
					node->lastChild = newNode;
				}
				newNode->prev = child;
				newNode->next = child->next;
				child->next = newNode;
			}

			newNode->parent = node;
			node->numChildren++;

#ifdef BTREE_CHECK
			CheckTree();
#endif

			return newNode;
		}

		// make sure the child has room to store another node
		if ( child->numChildren >= maxChildrenPerNode ) {
			SplitNode( child );
			if ( key <= child->prev->key ) {
				child = child->prev;
			}
		}
	}

	// we only end up here if the root node is empty
	newNode->parent = root;
	root->key = key;
	root->firstChild = newNode;
	root->lastChild = newNode;
	root->numChildren++;

#ifdef BTREE_CHECK
	CheckTree();
#endif

	return newNode;
}

template< typename objType, typename keyType, INT maxChildrenPerNode >
FORCEINLINE void mxBTree<objType,keyType,maxChildrenPerNode>::Remove( mxBTreeNode<objType,keyType> *node ) {
	mxBTreeNode<objType,keyType> *parent;

	Assert( node->object != NULL );

	// unlink the node from it's parent
	if ( node->prev ) {
		node->prev->next = node->next;
	} else {
		node->parent->firstChild = node->next;
	}
	if ( node->next ) {
		node->next->prev = node->prev;
	} else {
		node->parent->lastChild = node->prev;
	}
	node->parent->numChildren--;

	// make sure there are no parent nodes with a single child
	for ( parent = node->parent; parent != root && parent->numChildren <= 1; parent = parent->parent ) {

		if ( parent->next ) {
			parent = MergeNodes( parent, parent->next );
		} else if ( parent->prev ) {
			parent = MergeNodes( parent->prev, parent );
		}

		// a parent may not use a key higher than the key of it's last child
		if ( parent->key > parent->lastChild->key ) {
			parent->key = parent->lastChild->key;
		}

		if ( parent->numChildren > maxChildrenPerNode ) {
			SplitNode( parent );
			break;
		}
	}
	for ( ; parent != NULL && parent->lastChild != NULL; parent = parent->parent ) {
		// a parent may not use a key higher than the key of it's last child
		if ( parent->key > parent->lastChild->key ) {
			parent->key = parent->lastChild->key;
		}
	}

	// free the node
	FreeNode( node );

	// remove the root node if it has a single internal node as child
	if ( root->numChildren == 1 && root->firstChild->object == NULL ) {
		mxBTreeNode<objType,keyType> *oldRoot = root;
		root->firstChild->parent = NULL;
		root = root->firstChild;
		FreeNode( oldRoot );
	}

#ifdef BTREE_CHECK
	CheckTree();
#endif
}

template< typename objType, typename keyType, INT maxChildrenPerNode >
FORCEINLINE objType *mxBTree<objType,keyType,maxChildrenPerNode>::Find( keyType key ) const {
	mxBTreeNode<objType,keyType> *node;

	for ( node = root->firstChild; node != NULL; node = node->firstChild ) {
		while( node->next ) {
			if ( node->key >= key ) {
				break;
			}
			node = node->next;
		}
		if ( node->object ) {
			if ( node->key == key ) {
				return node->object;
			} else {
				return NULL;
			}
		}
	}
	return NULL;
}

template< typename objType, typename keyType, INT maxChildrenPerNode >
FORCEINLINE objType *mxBTree<objType,keyType,maxChildrenPerNode>::FindSmallestLargerEqual( keyType key ) const {
	mxBTreeNode<objType,keyType> *node;

	for ( node = root->firstChild; node != NULL; node = node->firstChild ) {
		while( node->next ) {
			if ( node->key >= key ) {
				break;
			}
			node = node->next;
		}
		if ( node->object ) {
			if ( node->key >= key ) {
				return node->object;
			} else {
				return NULL;
			}
		}
	}
	return NULL;
}

template< typename objType, typename keyType, INT maxChildrenPerNode >
FORCEINLINE objType *mxBTree<objType,keyType,maxChildrenPerNode>::FindLargestSmallerEqual( keyType key ) const {
	mxBTreeNode<objType,keyType> *node;

	for ( node = root->lastChild; node != NULL; node = node->lastChild ) {
		while( node->prev ) {
			if ( node->key <= key ) {
				break;
			}
			node = node->prev;
		}
		if ( node->object ) {
			if ( node->key <= key ) {
				return node->object;
			} else {
				return NULL;
			}
		}
	}
	return NULL;
}

template< typename objType, typename keyType, INT maxChildrenPerNode >
FORCEINLINE mxBTreeNode<objType,keyType> *mxBTree<objType,keyType,maxChildrenPerNode>::GetRoot( void ) const {
	return root;
}

template< typename objType, typename keyType, INT maxChildrenPerNode >
FORCEINLINE INT mxBTree<objType,keyType,maxChildrenPerNode>::GetNodeCount( void ) const {
	return nodeAllocator.GetAllocCount();
}

template< typename objType, typename keyType, INT maxChildrenPerNode >
FORCEINLINE mxBTreeNode<objType,keyType> *mxBTree<objType,keyType,maxChildrenPerNode>::GetNext( mxBTreeNode<objType,keyType> *node ) const {
	if ( node->firstChild ) {
		return node->firstChild;
	} else {
		while( node && node->next == NULL ) {
			node = node->parent;
		}
		return node;
	}
}

template< typename objType, typename keyType, INT maxChildrenPerNode >
FORCEINLINE mxBTreeNode<objType,keyType> *mxBTree<objType,keyType,maxChildrenPerNode>::GetNextLeaf( mxBTreeNode<objType,keyType> *node ) const {
	if ( node->firstChild ) {
		while ( node->firstChild ) {
			node = node->firstChild;
		}
		return node;
	} else {
		while( node && node->next == NULL ) {
			node = node->parent;
		}
		if ( node ) {
			node = node->next;
			while ( node->firstChild ) {
				node = node->firstChild;
			}
			return node;
		} else {
			return NULL;
		}
	}
}

template< typename objType, typename keyType, INT maxChildrenPerNode >
FORCEINLINE mxBTreeNode<objType,keyType> *mxBTree<objType,keyType,maxChildrenPerNode>::AllocNode( void ) {
	mxBTreeNode<objType,keyType> *node = nodeAllocator.Alloc();
	node->key = 0;
	node->parent = NULL;
	node->next = NULL;
	node->prev = NULL;
	node->numChildren = 0;
	node->firstChild = NULL;
	node->lastChild = NULL;
	node->object = NULL;
	return node;
}

template< typename objType, typename keyType, INT maxChildrenPerNode >
FORCEINLINE void mxBTree<objType,keyType,maxChildrenPerNode>::FreeNode( mxBTreeNode<objType,keyType> *node ) {
	nodeAllocator.Free( node );
}

template< typename objType, typename keyType, INT maxChildrenPerNode >
FORCEINLINE void mxBTree<objType,keyType,maxChildrenPerNode>::SplitNode( mxBTreeNode<objType,keyType> *node ) {
	INT i;
	mxBTreeNode<objType,keyType> *child, *newNode;

	// allocate a new node
	newNode = AllocNode();
	newNode->parent = node->parent;

	// divide the children over the two nodes
	child = node->firstChild;
	child->parent = newNode;
	for ( i = 3; i < node->numChildren; i += 2 ) {
		child = child->next;
		child->parent = newNode;
	}

	newNode->key = child->key;
	newNode->numChildren = node->numChildren / 2;
	newNode->firstChild = node->firstChild;
	newNode->lastChild = child;

	node->numChildren -= newNode->numChildren;
	node->firstChild = child->next;

	child->next->prev = NULL;
	child->next = NULL;

	// add the new child to the parent before the split node
	Assert( node->parent->numChildren < maxChildrenPerNode );

	if ( node->prev ) {
		node->prev->next = newNode;
	} else {
		node->parent->firstChild = newNode;
	}
	newNode->prev = node->prev;
	newNode->next = node;
	node->prev = newNode;

	node->parent->numChildren++;
}

template< typename objType, typename keyType, INT maxChildrenPerNode >
FORCEINLINE mxBTreeNode<objType,keyType> *mxBTree<objType,keyType,maxChildrenPerNode>::MergeNodes( mxBTreeNode<objType,keyType> *node1, mxBTreeNode<objType,keyType> *node2 ) {
	mxBTreeNode<objType,keyType> *child;

	Assert( node1->parent == node2->parent );
	Assert( node1->next == node2 && node2->prev == node1 );
	Assert( node1->object == NULL && node2->object == NULL );
	Assert( node1->numChildren >= 1 && node2->numChildren >= 1 );

	for ( child = node1->firstChild; child->next; child = child->next ) {
		child->parent = node2;
	}
	child->parent = node2;
	child->next = node2->firstChild;
	node2->firstChild->prev = child;
	node2->firstChild = node1->firstChild;
	node2->numChildren += node1->numChildren;

	// unlink the first node from the parent
	if ( node1->prev ) {
		node1->prev->next = node2;
	} else {
		node1->parent->firstChild = node2;
	}
	node2->prev = node1->prev;
	node2->parent->numChildren--;

	FreeNode( node1 );

	return node2;
}

template< typename objType, typename keyType, INT maxChildrenPerNode >
FORCEINLINE void mxBTree<objType,keyType,maxChildrenPerNode>::CheckTree_r( mxBTreeNode<objType,keyType> *node, INT &numNodes ) const {
	INT numChildren;
	mxBTreeNode<objType,keyType> *child;

	numNodes++;

	// the root node may have zero children and leaf nodes always have zero children, all other nodes should have at least 2 and at most maxChildrenPerNode children
	Assert( ( node == root ) || ( node->object != NULL && node->numChildren == 0 ) || ( node->numChildren >= 2 && node->numChildren <= maxChildrenPerNode ) );
	// the key of a node may never be larger than the key of it's last child
	Assert( ( node->lastChild == NULL ) || ( node->key <= node->lastChild->key ) );

	numChildren = 0;
	for ( child = node->firstChild; child; child = child->next ) {
		numChildren++;
		// make sure the children are properly linked
		if ( child->prev == NULL ) {
			Assert( node->firstChild == child );
		} else {
			Assert( child->prev->next == child );
		}
		if ( child->next == NULL ) {
			Assert( node->lastChild == child );
		} else {
			Assert( child->next->prev == child );
		}
		// recurse down the tree
		CheckTree_r( child, numNodes );
	}
	// the number of children should equal the number of linked children
	Assert( numChildren == node->numChildren );
}

template< typename objType, typename keyType, INT maxChildrenPerNode >
FORCEINLINE void mxBTree<objType,keyType,maxChildrenPerNode>::CheckTree( void ) const {
	INT numNodes = 0;
	mxBTreeNode<objType,keyType> *node, *lastNode;

	CheckTree_r( root, numNodes );

	// the number of nodes in the tree should equal the number of allocated nodes
	Assert( numNodes == nodeAllocator.GetAllocCount() );

	// all the leaf nodes should be ordered
	lastNode = GetNextLeaf( GetRoot() );
	if ( lastNode ) {
		for ( node = GetNextLeaf( lastNode ); node; lastNode = node, node = GetNextLeaf( node ) ) {
			Assert( lastNode->key <= node->key );
		}
	}
}

mxNAMESPACE_END

#endif /* !__BTREE_H__ */

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//

// Copyright (C) 2004 Id Software, Inc.
//

#ifndef __MX_TEMPLATE_HIERARCHY_H__
#define __MX_TEMPLATE_HIERARCHY_H__

mxSWIPED("idLib");

mxNAMESPACE_BEGIN

/*
==============================================================================

	THierarchy

==============================================================================
*/

template< class TYPE >
class THierarchy {
public:

						THierarchy( TYPE* theOwner = nil );
						~THierarchy();
	
	void				SetOwner( TYPE *object );
	TYPE *				Owner( void ) const;
	void				ParentTo( THierarchy &node );
	void				MakeSiblingAfter( THierarchy &node );
	bool				ParentedBy( const THierarchy &node ) const;
	void				RemoveFromParent( void );
	void				RemoveFromHierarchy( void );

	TYPE *				GetParent( void ) const;		// parent of this node
	TYPE *				GetChild( void ) const;			// first child of this node
	TYPE *				GetSibling( void ) const;		// next node with the same parent
	TYPE *				GetPriorSibling( void ) const;	// previous node with the same parent
	TYPE *				GetNext( void ) const;			// goes through all nodes of the hierarchy
	TYPE *				GetNextLeaf( void ) const;		// goes through all leaf nodes of the hierarchy

private:
	THierarchy *		parent;
	THierarchy *		sibling;
	THierarchy *		child;
	TYPE *				owner;

	THierarchy<TYPE>	*GetPriorSiblingNode( void ) const;	// previous node with the same parent
};

/*
================
THierarchy<TYPE>::THierarchy
================
*/
template< class TYPE >
THierarchy<TYPE>::THierarchy( TYPE* theOwner ) {
	owner	= theOwner;
	parent	= NULL;	
	sibling	= NULL;
	child	= NULL;
}

/*
================
THierarchy<TYPE>::~THierarchy
================
*/
template< class TYPE >
THierarchy<TYPE>::~THierarchy() {
	RemoveFromHierarchy();
}

/*
================
THierarchy<TYPE>::Owner

Gets the object that is associated with this node.
================
*/
template< class TYPE >
TYPE *THierarchy<TYPE>::Owner( void ) const {
	return owner;
}

/*
================
THierarchy<TYPE>::SetOwner

Sets the object that this node is associated with.
================
*/
template< class TYPE >
void THierarchy<TYPE>::SetOwner( TYPE *object ) {
	owner = object;
}

/*
================
THierarchy<TYPE>::ParentedBy
================
*/
template< class TYPE >
bool THierarchy<TYPE>::ParentedBy( const THierarchy &node ) const {
	if ( parent == &node ) {
		return true;
	} else if ( parent ) {
		return parent->ParentedBy( node );
	}
	return false;
}

/*
================
THierarchy<TYPE>::ParentTo

Makes the given node the parent.
================
*/
template< class TYPE >
void THierarchy<TYPE>::ParentTo( THierarchy &node ) {
	RemoveFromParent();

	parent		= &node;
	sibling		= node.child;
	node.child	= this;
}

/*
================
THierarchy<TYPE>::MakeSiblingAfter

Makes the given node a sibling after the passed in node.
================
*/
template< class TYPE >
void THierarchy<TYPE>::MakeSiblingAfter( THierarchy &node ) {
	RemoveFromParent();
	parent	= node.parent;
	sibling = node.sibling;
	node.sibling = this;
}

/*
================
THierarchy<TYPE>::RemoveFromParent
================
*/
template< class TYPE >
void THierarchy<TYPE>::RemoveFromParent( void ) {
	THierarchy<TYPE> *prev;

	if ( parent ) {
		prev = GetPriorSiblingNode();
		if ( prev ) {
			prev->sibling = sibling;
		} else {
			parent->child = sibling;
		}
	}

	parent = NULL;
	sibling = NULL;
}

/*
================
THierarchy<TYPE>::RemoveFromHierarchy

Removes the node from the hierarchy and adds it's children to the parent.
================
*/
template< class TYPE >
void THierarchy<TYPE>::RemoveFromHierarchy( void ) {
	THierarchy<TYPE> *parentNode;
	THierarchy<TYPE> *node;

	parentNode = parent;
	RemoveFromParent();

	if ( parentNode ) {
		while( child ) {
			node = child;
			node->RemoveFromParent();
			node->ParentTo( *parentNode );
		}
	} else {
		while( child ) {
			child->RemoveFromParent();
		}
	}
}

/*
================
THierarchy<TYPE>::GetParent
================
*/
template< class TYPE >
TYPE *THierarchy<TYPE>::GetParent( void ) const {
	if ( parent ) {
		return parent->owner;
	}
	return NULL;
}

/*
================
THierarchy<TYPE>::GetChild
================
*/
template< class TYPE >
TYPE *THierarchy<TYPE>::GetChild( void ) const {
	if ( child ) {
		return child->owner;
	}
	return NULL;
}

/*
================
THierarchy<TYPE>::GetSibling
================
*/
template< class TYPE >
TYPE *THierarchy<TYPE>::GetSibling( void ) const {
	if ( sibling ) {
		return sibling->owner;
	}
	return NULL;
}

/*
================
THierarchy<TYPE>::GetPriorSiblingNode

Returns NULL if no parent, or if it is the first child.
================
*/
template< class TYPE >
THierarchy<TYPE> *THierarchy<TYPE>::GetPriorSiblingNode( void ) const {
	if ( !parent || ( parent->child == this ) ) {
		return NULL;
	}

	THierarchy<TYPE> *prev;
	THierarchy<TYPE> *node;

	node = parent->child;
	prev = NULL;
	while( ( node != this ) && ( node != NULL ) ) {
		prev = node;
		node = node->sibling;
	}

	if ( node != this ) {
		mxErr( "THierarchy::GetPriorSibling: could not find node in parent's list of children" );
	}

	return prev;
}

/*
================
THierarchy<TYPE>::GetPriorSibling

Returns NULL if no parent, or if it is the first child.
================
*/
template< class TYPE >
TYPE *THierarchy<TYPE>::GetPriorSibling( void ) const {
	THierarchy<TYPE> *prior;

	prior = GetPriorSiblingNode();
	if ( prior ) {
		return prior->owner;
	}

	return NULL;
}

/*
================
THierarchy<TYPE>::GetNext

Goes through all nodes of the hierarchy.
================
*/
template< class TYPE >
TYPE *THierarchy<TYPE>::GetNext( void ) const {
	const THierarchy<TYPE> *node;

	if ( child ) {
		return child->owner;
	} else {
		node = this;
		while( node && node->sibling == NULL ) {
			node = node->parent;
		}
		if ( node ) {
			return node->sibling->owner;
		} else {
			return NULL;
		}
	}
}

/*
================
THierarchy<TYPE>::GetNextLeaf

Goes through all leaf nodes of the hierarchy.
================
*/
template< class TYPE >
TYPE *THierarchy<TYPE>::GetNextLeaf( void ) const {
	const THierarchy<TYPE> *node;

	if ( child ) {
		node = child;
		while ( node->child ) {
			node = node->child;
		}
		return node->owner;
	} else {
		node = this;
		while( node && node->sibling == NULL ) {
			node = node->parent;
		}
		if ( node ) {
			node = node->sibling;
			while ( node->child ) {
				node = node->child;
			}
			return node->owner;
		} else {
			return NULL;
		}
	}
}

mxNAMESPACE_END

#endif /* !__MX_TEMPLATE_HIERARCHY_H__ */

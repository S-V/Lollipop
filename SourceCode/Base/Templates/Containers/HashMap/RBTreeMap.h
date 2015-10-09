/*
=============================================================================
	File:	RBTreeMap.h
	Desc:	Template for associative arrays using a red-black tree.
=============================================================================
*/

#ifndef __RB_TREE_MAP_H__
#define __RB_TREE_MAP_H__

mxNAMESPACE_BEGIN
mxSWIPED("Irrlicht");
//
//	RBTreeMap< class KeyType, class ValueType >
//
//	Operators 'equal-to' and 'less-than' should be implemented, e.g.:
//
//	bool KeyType::operator == ( const KeyType& other ) const;
//	bool KeyType::operator < ( const KeyType& other ) const;
//
template< class KeyType, class ValueType >
class RBTreeMap
{
	//
	// Red/Black tree for RBTreeMap.
	//
	template< class KeyTypeRB, class ValueTypeRB >
	class RBTree
	{
	public:
		RBTree( const KeyTypeRB& k, const ValueTypeRB& v)
			: mLeftChild( nil )
			, mRightChild( nil )
			, mParent( nil )
			, mKey( k )
			, mValue( v )
			, mIsRed( true )
		{}

		void setLeftChild( RBTree* p )
		{
			mLeftChild = p;
			if( p ) {
				p->setParent(this);
			}
		}

		void setRightChild( RBTree* p )
		{
			mRightChild = p;
			if( p ) {
				p->setParent(this);
			}
		}

		void setParent( RBTree* p )		{ mParent=p; }

		void setValue( const ValueTypeRB& v)	{ mValue = v; }

		void setRed()			{ mIsRed = true; }
		void setBlack()			{ mIsRed = false; }

		RBTree* GetLeftChild() const	{ return mLeftChild; }
		RBTree* GetRightChild() const	{ return mRightChild; }
		RBTree* getParent() const		{ return mParent; }

		ValueTypeRB GetValue() const
		{
			return mValue;
		}

		KeyTypeRB GetKey() const
		{
			return mKey;
		}

		bool isRoot() const
		{
			return mParent==0;
		}

		bool isLeftChild() const
		{
			return (mParent != 0) && (mParent->GetLeftChild() == this);
		}

		bool isRightChild() const
		{
			return (mParent != 0) && (mParent->GetRightChild() == this);
		}

		bool isLeaf() const
		{
			return (mLeftChild == 0) && (mRightChild == 0);
		}

		UINT getLevel() const
		{
			if( isRoot() ) {
				return 1;
			} else {
				return getParent()->getLevel() + 1;
			}
		}


		bool isRed() const
		{
			return mIsRed;
		}

		bool isBlack() const
		{
			return !mIsRed;
		}

		ValueTypeRB* GetValuePtr()
		{
			return &mValue;
		}

	private:
		RBTree();

		RBTree *	mLeftChild;
		RBTree *	mRightChild;

		RBTree *	mParent;

		KeyTypeRB	mKey;
		ValueTypeRB	mValue;

		bool		mIsRed;
	}; // RBTree

public:

	typedef RBTree<KeyType,ValueType> Node;

	// Normal Iterator
	class Iterator
	{
	public:

		Iterator()
			: mRoot( nil )
			, mCurrent( nil )
		{}

		// Constructor(Node*)
		Iterator( Node* root )
			: mRoot( root )
		{
			Reset();
		}

		// Copy constructor
		Iterator( const Iterator& src )
			: mRoot( src.mRoot )
			, mCurrent( src.mCurrent )
		{}

		void Reset( bool atLowest = true )
		{
			if ( atLowest ) {
				mCurrent = GetMin( mRoot );
			} else {
				mCurrent = GetMax( mRoot );
			}
		}

		bool AtEnd() const
		{
			return mCurrent==0;
		}

		Node* GetNode()
		{
			return mCurrent;
		}

		Iterator& operator = ( const Iterator& src)
		{
			mRoot = src.mRoot;
			mCurrent = src.mCurrent;
			return (*this);
		}

		void operator ++ (INT)
		{
			Inc();
		}

		void operator -- (INT)
		{
			Dec();
		}

		Node* operator -> ()
		{
			return GetNode();
		}

		Node& operator * ()
		{
			if (AtEnd())
				throw "Iterator at end";

			return *mCurrent;
		}

	private:

		Node* GetMin( Node* n )
		{
			while( n && n->GetLeftChild() )
			{
				n = n->GetLeftChild();
			}
			return n;
		}

		Node* GetMax( Node* n )
		{
			while( n && n->GetRightChild() )
			{
				n = n->GetRightChild();
			}
			return n;
		}

		void Inc()
		{
			// Already at end?
			if( mCurrent == nil ) {
				return;
			}

			if( mCurrent->GetRightChild() )
			{
				// If current node has a right child, the next higher node is the
				// node with lowest key beneath the right child.
				mCurrent = GetMin( mCurrent->GetRightChild() );
			}
			else if( mCurrent->isLeftChild() )
			{
				// No right child? Well if current node is a left child then
				// the next higher node is the parent
				mCurrent = mCurrent->getParent();
			}
			else
			{
				// Current node neither is left child nor has a right child.
				// Ie it is either right child or root
				// The next higher node is the parent of the first non-right
				// child (ie either a left child or the root) up in the
				// hierarchy. mRoot's parent is 0.
				while( mCurrent->isRightChild() )
				{
					mCurrent = mCurrent->getParent();
				}
				mCurrent = mCurrent->getParent();
			}
		}

		void Dec()
		{
			// Already at end?
			if( mCurrent == 0 ) {
				return;
			}
			if( mCurrent->GetLeftChild() )
			{
				// If current node has a left child, the next lower node is the
				// node with highest key beneath the left child.
				mCurrent = GetMax( mCurrent->GetLeftChild() );
			}
			else if( mCurrent->isRightChild() )
			{
				// No left child? Well if current node is a right child then
				// the next lower node is the parent
				mCurrent = mCurrent->getParent();
			}
			else
			{
				// Current node neither is right child nor has a left child.
				// Ie it is either left child or root
				// The next higher node is the parent of the first non-left
				// child (ie either a right child or the root) up in the
				// hierarchy. mRoot's parent is 0.

				while( mCurrent->isLeftChild() )
				{
					mCurrent = mCurrent->getParent();
				}
				mCurrent = mCurrent->getParent();
			}
		}

		Node* mRoot;
		Node* mCurrent;
	}; // Iterator



	// mParent First Iterator.
	/** Traverses the tree from top to bottom. Typical usage is
	when storing the tree structure, because when reading it
	later (and inserting elements) the tree structure will
	be the same. */
	class ParentFirstIterator
	{
	public:

		ParentFirstIterator()
			: mRoot( nil )
			, mCurrent( nil )
		{
		}

		explicit ParentFirstIterator( Node* root )
			: mRoot( root )
			, mCurrent( nil )
		{
			Reset();
		}

		void Reset()
		{
			mCurrent = mRoot;
		}

		bool AtEnd() const
		{
			
			return mCurrent==0;
		}

		Node* GetNode()
		{
			return mCurrent;
		}

		ParentFirstIterator& operator=( const ParentFirstIterator& src)
		{
			mRoot = src.mRoot;
			mCurrent = src.mCurrent;
			return (*this);
		}

		void operator++(INT)
		{
			Inc();
		}


		Node* operator -> ()
		{
			return GetNode();
		}

		Node& operator* ()
		{
			if (AtEnd())
				throw "ParentFirstIterator at end";
			return *GetNode();
		}

	private:

		void Inc()
		{
			// Already at end?
			if (mCurrent==0)
				return;

			// First we try down to the left
			if (mCurrent->GetLeftChild())
			{
				mCurrent = mCurrent->GetLeftChild();
			}
			else if (mCurrent->GetRightChild())
			{
				// No left child? The we go down to the right.
				mCurrent = mCurrent->GetRightChild();
			}
			else
			{
				// No children? Move up in the hierarcy until
				// we either reach 0 (and are finished) or
				// Find a right uncle.
				while (mCurrent!=0)
				{
					// But if parent is left child and has a right "uncle" the parent
					// has already been processed but the uncle hasn't. Move to
					// the uncle.
					if (mCurrent->isLeftChild() && mCurrent->getParent()->GetRightChild())
					{
						mCurrent = mCurrent->getParent()->GetRightChild();
						return;
					}
					mCurrent = mCurrent->getParent();
				}
			}
		}

		Node* mRoot;
		Node* mCurrent;

	}; // ParentFirstIterator


	// mParent Last Iterator
	/** Traverse the tree from bottom to top.
	Typical usage is when deleting all elements in the tree
	because you must delete the children before you delete
	their parent. */
	class ParentLastIterator
	{
	public:

		ParentLastIterator()
			: mRoot(0), mCurrent(0)
		{}

		explicit ParentLastIterator( Node* root )
			: mRoot(root), mCurrent(0)
		{
			Reset();
		}

		void Reset()
		{
			mCurrent = GetMin(mRoot);
		}

		bool AtEnd() const
		{
			
			return mCurrent==0;
		}

		Node* GetNode()
		{
			return mCurrent;
		}

		ParentLastIterator& operator=( const ParentLastIterator& src)
		{
			mRoot = src.mRoot;
			mCurrent = src.mCurrent;
			return (*this);
		}

		void operator++(INT)
		{
			Inc();
		}

		Node* operator -> ()
		{
			return GetNode();
		}

		Node& operator* ()
		{
			if (AtEnd())
				throw "ParentLastIterator at end";
			return *GetNode();
		}

	private:

		Node* GetMin(Node* n)
		{
			while( n!=0
				&& (n->GetLeftChild()!=0 || n->GetRightChild()!=0) )
			{
				if( n->GetLeftChild() ) {
					n = n->GetLeftChild();
				} else {
					n = n->GetRightChild();
				}
			}
			return n;
		}

		void Inc()
		{
			// Already at end?
			if( mCurrent == 0 ) {
				return;
			}
			// Note: Starting point is the node as far down to the left as possible.

			// If current node has an uncle to the right, go to the
			// node as far down to the left from the uncle as possible
			// else just go up a level to the parent.
			if( mCurrent->isLeftChild() && mCurrent->getParent()->GetRightChild() )
			{
				mCurrent = GetMin(mCurrent->getParent()->GetRightChild());
			}
			else {
				mCurrent = mCurrent->getParent();
			}
		}

		Node* mRoot;
		Node* mCurrent;
	}; // ParentLastIterator


	// AccessClass is a temporary class used with the [] operator.
	// It makes it possible to have different behavior in situations like:
	// myTree["Foo"] = 32;
	// If "Foo" already exists update its value else Insert a new element.
	// INT i = myTree["Foo"]
	// If "Foo" exists return its value, else throw an exception.
	class AccessClass
	{
		// Let RBTreeMap be the only one who can instantiate this class.
		friend class RBTreeMap<KeyType, ValueType>;

	public:

		// Assignment operator. Handles the myTree["Foo"] = 32; situation
		void operator=( const ValueType& value)
		{
			// Just use the Set method, it handles already exist/not exist situation
			Tree.Set( mKey, value );
		}

		// ValueType operator
		operator ValueType()
		{
			Node* node = Tree.Find(mKey);

			// Not found
			if ( node == 0 ) {
				//throw "Item not found";
				Error( "Item not found" );
			}
			
			return node->GetValue();
		}

	private:

		AccessClass( RBTreeMap& tree, const KeyType& key ) : Tree(tree), mKey(key) {}

		AccessClass();

		RBTreeMap& Tree;
		const KeyType& mKey;
	}; // AccessClass


	// Constructor.
	RBTreeMap() : mRoot(0), mSize(0) {}

	// Destructor
	~RBTreeMap()
	{
		Clear();
	}

	//------------------------------
	// Public Commands
	//------------------------------

	// Inserts a new node into the tree
	/** \param keyNew: the index for this value
	\param v: the value to Insert
	\return True if successful, false if it fails (already exists) */
	bool Insert( const KeyType& keyNew, const ValueType& v)
	{
		// First Insert node the "usual" way (no fancy balance logic yet)
		Node* newNode = new Node(keyNew,v);
		if (!Insert(newNode))
		{
			delete newNode;
			
			return false;
		}

		// Then attend a balancing party
		while (!newNode->isRoot() && (newNode->getParent()->isRed()))
		{
			if (newNode->getParent()->isLeftChild())
			{
				// If newNode is a left child, get its right 'uncle'
				Node* newNodesUncle = newNode->getParent()->getParent()->GetRightChild();
				if ( newNodesUncle!=0 && newNodesUncle->isRed())
				{
					// case 1 - change the colours
					newNode->getParent()->setBlack();
					newNodesUncle->setBlack();
					newNode->getParent()->getParent()->setRed();
					// Move newNode up the tree
					newNode = newNode->getParent()->getParent();
				}
				else
				{
					// newNodesUncle is a black node
					if ( newNode->isRightChild())
					{
						// and newNode is to the right
						// case 2 - move newNode up and rotate
						newNode = newNode->getParent();
						rotateLeft(newNode);
					}
					// case 3
					newNode->getParent()->setBlack();
					newNode->getParent()->getParent()->setRed();
					rotateRight(newNode->getParent()->getParent());
				}
			}
			else
			{
				// If newNode is a right child, get its left 'uncle'
				Node* newNodesUncle = newNode->getParent()->getParent()->GetLeftChild();
				if ( newNodesUncle!=0 && newNodesUncle->isRed())
				{
					// case 1 - change the colours
					newNode->getParent()->setBlack();
					newNodesUncle->setBlack();
					newNode->getParent()->getParent()->setRed();
					// Move newNode up the tree
					newNode = newNode->getParent()->getParent();
				}
				else
				{
					// newNodesUncle is a black node
					if (newNode->isLeftChild())
					{
						// and newNode is to the left
						// case 2 - move newNode up and rotate
						newNode = newNode->getParent();
						rotateRight(newNode);
					}
					// case 3
					newNode->getParent()->setBlack();
					newNode->getParent()->getParent()->setRed();
					rotateLeft(newNode->getParent()->getParent());
				}

			}
		}
		// Color the root black
		mRoot->setBlack();
		return true;
	}

	// Replaces the value if the key already exists, otherwise inserts a new element.
	/** \param k The index for this value
	\param v The new value of */
	void Set( const KeyType& k, const ValueType& v)
	{
		Node* p = Find( k );
		if( p ) {
			p->setValue( v );
		}
		else {
			Insert( k, v );
		}
	}

	// Removes a node from the tree and returns it.
	/** The returned node must be deleted by the user
	\param k the key to Remove
	\return A pointer to the node, or 0 if not found */
	Node* Delink( const KeyType& k)
	{
		Node* p = Find(k);
		if( p == 0 ) {
			return 0;
		}
		// Rotate p down to the left until it has no right child, will get there
		// sooner or later.
		while( p->GetRightChild() )
		{
			// "Pull up my right child and let it knock me down to the left"
			rotateLeft( p );
		}
		// p now has no right child but might have a left child
		Node* left = p->GetLeftChild();

		// Let p's parent point to p's child instead of point to p
		if( p->isLeftChild() ) {
			p->getParent()->setLeftChild( left );
		}
		else if( p->isRightChild() ) {
			p->getParent()->setRightChild( left );
		}
		else
		{
			// p has no parent => p is the root.
			// Let the left child be the new root.
			SetRoot( left );
		}

		// p is now gone from the tree in the sense that
		// no one is pointing at it, so return it.

		--mSize;
		return p;
	}

	// Removes a node from the tree and deletes it.
	/** \return True if the node was found and deleted */
	bool Remove( const KeyType& k)
	{
		Node* p = Find( k );
		if( p == nil ) {
			return false;
		}

		// Rotate p down to the left until it has no right child, will get there
		// sooner or later.
		while( p->GetRightChild() )
		{
			// "Pull up my right child and let it knock me down to the left"
			rotateLeft( p );
		}
		// p now has no right child but might have a left child
		Node* left = p->GetLeftChild();

		// Let p's parent point to p's child instead of point to p
		if( p->isLeftChild() ) {
			p->getParent()->setLeftChild( left );
		}
		else if( p->isRightChild() ) {
			p->getParent()->setRightChild( left );
		}
		else
		{
			// p has no parent => p is the root.
			// Let the left child be the new root.
			SetRoot( left );
		}

		// p is now gone from the tree in the sense that
		// no one is pointing at it. Let's get rid of it.
		delete p;

		--mSize;
		return true;
	}

	// Clear the entire tree
	void Clear()
	{
		ParentLastIterator i(getParentLastIterator());

		while(!i.AtEnd())
		{
			Node* p = i.GetNode();
			i++; // Increment it before it is deleted
				// else iterator will get quite confused.
			delete p;
		}
		mRoot = nil;
		mSize= 0;
	}

	// Is the tree empty?
	// \return Returns true if empty, false if not
	bool IsEmpty() const
	{
		return ( mRoot == nil );
	}

	// Search for a node with the specified key.
	// \param keyToFind: The key to Find
	// \return Returns 0 if node couldn't be found.
	Node* Find( const KeyType& keyToFind ) const
	{
		Node* pNode = mRoot;
		while( pNode != NULL )
		{
			KeyType  key( pNode->GetKey() );

			if ( keyToFind == key ) {
				return pNode;
			}
			else if ( keyToFind < key ) {
				pNode = pNode->GetLeftChild();
			}
			else // keyToFind > key
			{
				pNode = pNode->GetRightChild();
			}
		}
		return nil;
	}

	ValueType * FindValue( const KeyType& key ) const
	{
		if( Node* node = Find( key ) ) {
			return node->GetValuePtr();
		}
		return nil;
	}

	// Gets the root element.
	// \return Returns a pointer to the root node, or
	// 0 if the tree is empty.
	Node* GetRoot() const
	{
		return mRoot;
	}

	// Returns the number of nodes in the tree.
	UINT GetSize() const
	{
		return mSize;
	}

	//------------------------------
	// Public Iterators
	//------------------------------

	// Returns an iterator
	Iterator getIterator()
	{
		Iterator it(GetRoot());
		return it;
	}
	// Returns a ParentFirstIterator.
	// Traverses the tree from top to bottom. Typical usage is
	// when storing the tree structure, because when reading it
	// later (and inserting elements) the tree structure will
	// be the same.
	ParentFirstIterator getParentFirstIterator()
	{
		ParentFirstIterator it(GetRoot());
		return it;
	}
	// Returns a ParentLastIterator to traverse the tree from
	// bottom to top.
	// Typical usage is when deleting all elements in the tree
	// because you must delete the children before you delete
	// their parent.
	ParentLastIterator getParentLastIterator()
	{
		ParentLastIterator it(GetRoot());
		return it;
	}

	//------------------------------
	// Public Operators
	//------------------------------

	// operator [] for access to elements
	/** for example myMap["key"] */
	AccessClass operator[]( const KeyType& k)
	{
		return AccessClass(*this, k);
	}
	
private:

	//------------------------------
	// Disabled methods
	//------------------------------
	// Copy constructor and assignment operator deliberately
	// defined but not implemented. The tree should never be
	// copied, pass along references to it instead.
	explicit RBTreeMap( const RBTreeMap& src);
	RBTreeMap& operator = ( const RBTreeMap& src);

	// Set node as new root.
	/** The node will be Set to black, otherwise core dumps may arise
	(patch provided by rogerborg).
	\param newRoot Node which will be the new root
	*/
	void SetRoot( Node* newRoot )
	{
		mRoot = newRoot;
		if ( mRoot != NULL )
		{
			mRoot->setParent(0);
			mRoot->setBlack();
		}
	}

	// Insert a node into the tree without using any fancy balancing logic.
	/** \return false if that key already exist in the tree. */
	bool Insert( Node* newNode )
	{
		bool result = true; // Assume success

		if ( mRoot == NULL )
		{
			SetRoot(newNode);
			mSize = 1;
		}
		else
		{
			Node* pNode = mRoot;
			KeyType keyNew = newNode->GetKey();
			while ( pNode )
			{
				KeyType key( pNode->GetKey() );

				if ( keyNew == key ) {
					result = false;
					pNode = 0;
				}
				else if ( keyNew < key)
				{
					if ( pNode->GetLeftChild() == 0 )
					{
						pNode->setLeftChild( newNode );
						pNode = 0;
					}
					else
						pNode = pNode->GetLeftChild();
				}
				else // keyNew > key
				{
					if ( pNode->GetRightChild() == 0 )
					{
						pNode->setRightChild( newNode );
						pNode = 0;
					}
					else
					{
						pNode = pNode->GetRightChild();
					}
				}
			}

			if ( result ) {
				++mSize;
			}
		}

		return result;
	}

	// Rotate left.
	// Pull up node's right child and let it knock node down to the left
	void rotateLeft( Node* p )
	{
		Node* right = p->GetRightChild();

		p->setRightChild( right->GetLeftChild() );

		if ( p->isLeftChild() ) {
			p->getParent()->setLeftChild( right );
		}
		else if ( p->isRightChild() ) {
			p->getParent()->setRightChild( right );
		}
		else {
			SetRoot( right );
		}

		right->setLeftChild( p );
	}

	// Rotate right.
	// Pull up node's left child and let it knock node down to the right
	void rotateRight( Node* p )
	{
		Node* left = p->GetLeftChild();

		p->setLeftChild( left->GetRightChild() );

		if (p->isLeftChild()) {
			p->getParent()->setLeftChild( left );
		}
		else if ( p->isRightChild() ) {
			p->getParent()->setRightChild( left );
		}
		else {
			SetRoot( left );
		}

		left->setRightChild( p );
	}

	//------------------------------
	// Private Members
	//------------------------------
	Node *	mRoot; // The top node. 0 if empty.
	UINT	mSize; // Number of nodes in the tree
};

mxNAMESPACE_END

#endif // ! __RB_TREE_MAP_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//

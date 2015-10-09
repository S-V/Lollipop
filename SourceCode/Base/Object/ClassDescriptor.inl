
FORCEINLINE
bool mxClass::operator == ( const mxClass& other ) const
{
	return ( this == &other );
}

FORCEINLINE
bool mxClass::operator != ( const mxClass& other ) const
{
	return ( this != &other );
}

FORCEINLINE
PCSTR mxClass::GetTypeName() const
{
	return m_name;
}

FORCEINLINE
TypeGUIDArg mxClass::GetTypeGuid() const
{
	return m_guid;
}

FORCEINLINE
const mxClass * mxClass::GetParent() const
{
	return m_parent;
}

FORCEINLINE
SizeT mxClass::GetInstanceSize() const
{
	return m_instanceSize;
}

FORCEINLINE
bool mxClass::IsAbstract() const
{
	return m_constructor == nil;
}

FORCEINLINE
bool mxClass::IsConcrete() const
{
	return m_constructor != nil;
}

FORCEINLINE
F_CreateObject * mxClass::GetCreator() const
{
	return m_creator;
}

FORCEINLINE
F_ConstructObject *	mxClass::GetConstructor() const
{
	return m_constructor;
}

FORCEINLINE
F_DestructObject * mxClass::GetDestructor() const
{
	return m_destructor;
}

FORCEINLINE
SPerTypeUserData* mxClass::GetUserData()
{
	return m_userData;
}

FORCEINLINE
const mxClassMembers& mxClass::GetMembers() const
{
	return m_members;
}

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//

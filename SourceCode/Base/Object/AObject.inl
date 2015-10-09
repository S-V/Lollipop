
/*================================
			AObject
================================*/

FORCEINLINE
bool AObject::IsA( const mxClass& type ) const
{
	return this->rttiGetClass().IsDerivedFrom( type );
}

FORCEINLINE
bool AObject::IsA( TypeGUIDArg typeCode ) const
{
	return this->rttiGetClass().IsDerivedFrom( typeCode );
}

FORCEINLINE
bool AObject::IsInstanceOf( const mxClass& type ) const
{
	return ( this->rttiGetClass() == type );
}

FORCEINLINE
bool AObject::IsInstanceOf( TypeGUIDArg typeCode ) const
{
	return ( this->rttiGetClass().GetTypeGuid() == typeCode );
}

FORCEINLINE
const char* AObject::rttiGetTypeName() const
{
	return this->rttiGetClass().GetTypeName();
}

FORCEINLINE
TypeGUID AObject::rttiGetTypeGuid() const
{
	return this->rttiGetClass().GetTypeGuid();
}

FORCEINLINE
bool AObject::IsAbstract() const
{
	return this->rttiGetClass().IsAbstract();
}

FORCEINLINE
bool AObject::IsConcrete() const
{
	return this->rttiGetClass().IsConcrete();
}

FORCEINLINE
SizeT AObject::GetInstanceSize() const
{
	return this->rttiGetClass().GetInstanceSize();
}

FORCEINLINE
mxClass & AObject::StaticClass()
{
	return ms_staticTypeInfo;
}

FORCEINLINE
mxClass & AObject::rttiGetClass() const
{
	return ms_staticTypeInfo;
}

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//

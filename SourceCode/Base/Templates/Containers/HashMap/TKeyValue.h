/*
=============================================================================
	File:	TKeyValue.h
	Desc:	
=============================================================================
*/

#ifndef __MX_TEMPLATE_KEY_VALUE_H__
#define __MX_TEMPLATE_KEY_VALUE_H__

mxNAMESPACE_BEGIN

/*
====================================================================
	
	Key/Value pair objects are used by most associative container classes,
	like Dictionary or HashTable. 

====================================================================
*/
mxSWIPED("Nebula 3");
template< typename KEYTYPE, typename VALUETYPE >
class TKeyValue {
public:
	KEYTYPE key;
	VALUETYPE value;

public:
    FORCEINLINE TKeyValue();
    FORCEINLINE TKeyValue(const KEYTYPE& k, const VALUETYPE& v);
    FORCEINLINE TKeyValue(const KEYTYPE& k);
    FORCEINLINE TKeyValue(const TKeyValue<KEYTYPE, VALUETYPE>& rhs);

    FORCEINLINE void operator=(const TKeyValue<KEYTYPE, VALUETYPE>& rhs);
    FORCEINLINE bool operator==(const TKeyValue<KEYTYPE, VALUETYPE>& rhs) const;
    FORCEINLINE bool operator!=(const TKeyValue<KEYTYPE, VALUETYPE>& rhs) const;
    FORCEINLINE bool operator>(const TKeyValue<KEYTYPE, VALUETYPE>& rhs) const;
    FORCEINLINE bool operator>=(const TKeyValue<KEYTYPE, VALUETYPE>& rhs) const;
    FORCEINLINE bool operator<(const TKeyValue<KEYTYPE, VALUETYPE>& rhs) const;
    FORCEINLINE bool operator<=(const TKeyValue<KEYTYPE, VALUETYPE>& rhs) const;
    FORCEINLINE VALUETYPE& Value();
    FORCEINLINE const KEYTYPE& Key() const;
    FORCEINLINE const VALUETYPE& Value() const;

	FORCEINLINE friend AStreamWriter& operator << ( AStreamWriter& file, const Pair& o )
	{
		file << o.key << o.value;
		return file;
	}
	FORCEINLINE friend AStreamReader& operator >> ( AStreamReader& file, Pair& o )
	{
		file >> o.key >> o.value;
		return file;
	}
	FORCEINLINE friend mxArchive& operator && ( mxArchive& archive, Pair& o )
	{
		return archive && o.key && o.value;
	}
};

//------------------------------------------------------------------------------
template<class KEYTYPE, class VALUETYPE>
TKeyValue<KEYTYPE, VALUETYPE>::TKeyValue()
{
    // empty
}

//------------------------------------------------------------------------------
template<class KEYTYPE, class VALUETYPE>
TKeyValue<KEYTYPE, VALUETYPE>::TKeyValue(const KEYTYPE& k, const VALUETYPE& v) :
    key(k),
    value(v)
{
    // empty
}
//------------------------------------------------------------------------------
/**
    This strange constructor is useful for search-by-key if
    the key-value-pairs are stored in a Util::Array.
*/
template<class KEYTYPE, class VALUETYPE>
TKeyValue<KEYTYPE, VALUETYPE>::TKeyValue(const KEYTYPE& k) :
    key(k)
{
    // empty
}
//------------------------------------------------------------------------------
/**
*/
template<class KEYTYPE, class VALUETYPE>
TKeyValue<KEYTYPE, VALUETYPE>::TKeyValue(const TKeyValue<KEYTYPE, VALUETYPE>& rhs) :
    key(rhs.key),
    value(rhs.value)
{
    // empty
}
//------------------------------------------------------------------------------
/**
*/
template<class KEYTYPE, class VALUETYPE>
void
TKeyValue<KEYTYPE, VALUETYPE>::operator=(const TKeyValue<KEYTYPE, VALUETYPE>& rhs)
{
    this->key = rhs.key;
    this->value = rhs.value;
}

//------------------------------------------------------------------------------
/**
*/
template<class KEYTYPE, class VALUETYPE>
bool
TKeyValue<KEYTYPE, VALUETYPE>::operator==(const TKeyValue<KEYTYPE, VALUETYPE>& rhs) const
{
    return (this->key == rhs.key);
}

//------------------------------------------------------------------------------
/**
*/
template<class KEYTYPE, class VALUETYPE>
bool
TKeyValue<KEYTYPE, VALUETYPE>::operator!=(const TKeyValue<KEYTYPE, VALUETYPE>& rhs) const
{
    return (this->key != rhs.key);
}

//------------------------------------------------------------------------------
/**
*/
template<class KEYTYPE, class VALUETYPE>
bool
TKeyValue<KEYTYPE, VALUETYPE>::operator>(const TKeyValue<KEYTYPE, VALUETYPE>& rhs) const
{
    return (this->key > rhs.key);
}

//------------------------------------------------------------------------------
/**
*/
template<class KEYTYPE, class VALUETYPE>
bool
TKeyValue<KEYTYPE, VALUETYPE>::operator>=(const TKeyValue<KEYTYPE, VALUETYPE>& rhs) const
{
    return (this->key >= rhs.key);
}

//------------------------------------------------------------------------------
/**
*/
template<class KEYTYPE, class VALUETYPE>
bool
TKeyValue<KEYTYPE, VALUETYPE>::operator<(const TKeyValue<KEYTYPE, VALUETYPE>& rhs) const
{
    return (this->key < rhs.key);
}

//------------------------------------------------------------------------------
/**
*/
template<class KEYTYPE, class VALUETYPE>
bool
TKeyValue<KEYTYPE, VALUETYPE>::operator<=(const TKeyValue<KEYTYPE, VALUETYPE>& rhs) const
{
    return (this->key <= rhs.key);
}

//------------------------------------------------------------------------------
/**
*/
template<class KEYTYPE, class VALUETYPE>
VALUETYPE&
TKeyValue<KEYTYPE, VALUETYPE>::Value()
{
    return this->value;
}

//------------------------------------------------------------------------------
/**
*/
template<class KEYTYPE, class VALUETYPE>
const KEYTYPE&
TKeyValue<KEYTYPE, VALUETYPE>::Key() const
{
    return this->key;
}

//------------------------------------------------------------------------------
/**
*/
template<class KEYTYPE, class VALUETYPE>
const VALUETYPE&
TKeyValue<KEYTYPE, VALUETYPE>::Value() const
{
    return this->value;
}

mxNAMESPACE_END

#endif // !__MX_TEMPLATE_KEY_VALUE_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//

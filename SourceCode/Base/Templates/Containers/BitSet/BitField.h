/*
=============================================================================
	File:	TBitField.h
	Desc:	A large static bit array with a size multiple of 32 bits.
	Note:	 (C) 2009 Radon Labs GmbH
=============================================================================
*/

#ifndef __MX_BIT_FIELD_H__
#define __MX_BIT_FIELD_H__
mxSWIPED("Nebula3");
mxNAMESPACE_BEGIN

template< unsigned int NUMBITS >
class TBitField {
public:
    /// constructor
    TBitField();
    /// copy constructor
    TBitField(const TBitField<NUMBITS>& rhs);
    
    /// assignment operator
    void operator=(const TBitField<NUMBITS>& rhs);
    /// equality operator
    bool operator==(const TBitField<NUMBITS>& rhs) const;
    /// inequality operator
    bool operator!=(const TBitField<NUMBITS>& rhs) const;
    
    /// clear content
    void Clear();
    /// return true if all bits are 0
    bool IsNull() const;
    /// set a bit by index
    void SetBit(IndexT bitIndex);
    /// clear a bit by index
    void ClearBit(IndexT bitIndex);
    
    /// set bitfield to OR combination
    static TBitField<NUMBITS> Or(const TBitField<NUMBITS>& b0, const TBitField<NUMBITS>& b1);
    /// set bitfield to AND combination
    static TBitField<NUMBITS> And(const TBitField<NUMBITS>& b0, const TBitField<NUMBITS>& b1);

private:
    static const int size = ((NUMBITS + 31) / 32);
    UINT bits[ size ];
};

template<unsigned int NUMBITS>
TBitField<NUMBITS>::TBitField()
{
    IndexT i;
    for (i = 0; i < size; i++)
    {
        this->bits[i] = 0;
    }
}

template<unsigned int NUMBITS>
TBitField<NUMBITS>::TBitField(const TBitField<NUMBITS>& rhs)
{
    IndexT i;
    for (i = 0; i < size; i++)
    {
        this->bits[i] = rhs.bits[i];
    }
}

template<unsigned int NUMBITS> void
TBitField<NUMBITS>::operator=(const TBitField<NUMBITS>& rhs)
{
    IndexT i;
    for (i = 0; i < size; i++)
    {
        this->bits[i] = rhs.bits[i];
    }
}

template<unsigned int NUMBITS> bool
TBitField<NUMBITS>::operator==(const TBitField<NUMBITS>& rhs) const
{
    IndexT i;
    for (i = 0; i < size; i++)
    {
        if (this->bits[i] != rhs.bits[i])
        {
            return false;
        }
    }
    return true;
}

template<unsigned int NUMBITS> bool
TBitField<NUMBITS>::operator!=(const TBitField<NUMBITS>& rhs) const
{
    return !(*this == rhs);
}

template<unsigned int NUMBITS> void
TBitField<NUMBITS>::Clear()
{
    IndexT i;
    for (i = 0; i < size; i++)
    {
        this->bits[i] = 0;
    }
}

template<unsigned int NUMBITS> bool
TBitField<NUMBITS>::IsNull() const
{
    IndexT i;
    for (i = 0; i < size; i++)
    {
        if (this->bits[i] != 0)
        {
            return false;
        }
    }
    return true;
}

template<unsigned int NUMBITS> void
TBitField<NUMBITS>::SetBit(IndexT i)
{
    Assert(i < NUMBITS);
    this->bits[i / 32] |= (1 << (i % 32));
}

template<unsigned int NUMBITS> void
TBitField<NUMBITS>::ClearBit(IndexT i)
{
    Assert(i < NUMBITS);
    this->bits[i / 32] &= ~(1 << (i % 32));
}

template<unsigned int NUMBITS> TBitField<NUMBITS>
TBitField<NUMBITS>::Or(const TBitField<NUMBITS>& b0, const TBitField<NUMBITS>& b1)
{
    TBitField<NUMBITS> res;
    IndexT i;
    for (i = 0; i < size; i++)
    {
        res.bits[i] = b0.bits[i] | b1.bits[i];
    }
    return res;
}

template<unsigned int NUMBITS> TBitField<NUMBITS>
TBitField<NUMBITS>::And(const TBitField<NUMBITS>& b0, const TBitField<NUMBITS>& b1)
{
    TBitField<NUMBITS> res;
    IndexT i;
    for (i = 0; i < size; i++)
    {
        res.bits[i] = b0.bits[i] & b1.bits[i];
    }
    return res;
}

mxNAMESPACE_END

#endif // ! __MX_BIT_FIELD_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//

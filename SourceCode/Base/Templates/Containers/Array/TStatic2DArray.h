/*
=============================================================================
	File:	TStatic2DArray.h
	Desc:	A fixed-size 2-dimensional array.
	ToDo:	Stop reinventing the wheel.
=============================================================================
*/

#ifndef __MX_CONTAINTERS_STATIC_2D_ARRAY_H__
#define __MX_CONTAINTERS_STATIC_2D_ARRAY_H__

mxNAMESPACE_BEGIN

//
//	TStatic2DArray
//

template<
	typename TYPE,
	UINT	ROW_COUNT,
	UINT	COLUMN_COUNT
>
class TStatic2DArray {
public:
	FORCEINLINE TStatic2DArray()
	{}

	FORCEINLINE TStatic2DArray(EInitZero)
	{
		this->SetZero();
	}

	FORCEINLINE ~TStatic2DArray()
	{}

	FORCEINLINE UINT GetRowCount()
	{
		return ROW_COUNT;
	}
	FORCEINLINE UINT GetColumnCount()
	{
		return COLUMN_COUNT;
	}

	FORCEINLINE TYPE & Get( UINT row, UINT column )
	{	Assert( row < GetRowCount() && column < GetColumnCount() );
		return m_data[ row ][ column ];
	}

	FORCEINLINE const TYPE & Get( UINT row, UINT column ) const
	{	Assert( row < GetRowCount() && column < GetColumnCount() );
		return m_data[ row ][ column ];
	}

	FORCEINLINE void Set( UINT row, UINT column, const TYPE& newValue )
	{	Assert( row < GetRowCount() && column < GetColumnCount() );
		m_data[ row ][ column ] = newValue;
	}

	FORCEINLINE void SetZero()
	{
		ZERO_OUT( m_data );
	}


	FORCEINLINE TYPE * ToPtr()
	{ return m_data; }

	FORCEINLINE const TYPE * ToPtr() const
	{ return m_data; }

private:
	TYPE	m_data[ ROW_COUNT ][ COLUMN_COUNT ];
};

mxNAMESPACE_END

#endif // ! __MX_CONTAINTERS_STATIC_2D_ARRAY_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//

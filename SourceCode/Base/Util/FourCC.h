/*
=============================================================================
	File:	FourCC.h
	Desc:	Four-character-code.
=============================================================================
*/

#ifndef __FOUR_CC_H__
#define __FOUR_CC_H__
mxSWIPED("Nebula3");
mxNAMESPACE_BEGIN

//
//	FourCC - four-character code.
//
//	Four byte identifiers can be made up of four human-readable characters
//	while fitting in a 32-bit integer.
//
class FourCC
{
	UINT32	fourCC;

public:
	// Empty constructor.
	FourCC();

	// Construct from a 32-bit value (e.g. 'SOAP').
	FourCC( UINT32 u );

	// Construct from a string.
	FourCC( const String& str );

	// Comparison operators.
	bool	operator == ( const FourCC& other ) const;
	bool	operator != ( const FourCC& other ) const;

	// These are needed for putting FourCC's into associative containers.
	bool	operator < ( const FourCC& other ) const;
    bool	operator <= ( const FourCC& other ) const;
    bool	operator > ( const FourCC& other ) const;
    bool	operator >= ( const FourCC& other ) const;

	// Returns 'true' if this is a valid code.
	bool	IsValid() const;

	void	SetFromUInt( UINT32 n );
	void	SetFromString( const String& str );

	UINT32	AsInt() const;
	String	AsString() const;

	friend UINT mxGetHashCode( const FourCC& fourCC )
	{
		return fourCC.AsInt();
	}

public:

	// conversion utils

	static String	ToString( const FourCC& n );
	static FourCC	FromString( const String& s );
};

FORCEINLINE
FourCC::FourCC()
	: fourCC( 0 )
{}

FORCEINLINE
FourCC::FourCC( mxUInt u )
	: fourCC( u )
{}

FORCEINLINE
FourCC::FourCC( const String& str )
{
	this->SetFromString( str );
}

FORCEINLINE
bool FourCC::operator == ( const FourCC& other ) const
{
	return ( this->fourCC == other.fourCC );
}

FORCEINLINE
bool FourCC::operator != ( const FourCC& other ) const
{
	return ( this->fourCC != other.fourCC );
}

FORCEINLINE
bool FourCC::operator < ( const FourCC& other ) const
{
	return ( this->fourCC < other.fourCC );
}

FORCEINLINE
bool FourCC::operator <= ( const FourCC& other ) const
{
	return ( this->fourCC <= other.fourCC );
}

FORCEINLINE
bool FourCC::operator > ( const FourCC& other ) const
{
	return ( this->fourCC > other.fourCC );
}

FORCEINLINE
bool FourCC::operator >= ( const FourCC& other ) const
{
	return ( this->fourCC >= other.fourCC );
}

FORCEINLINE
bool FourCC::IsValid() const
{
	return ( this->fourCC != 0 );
}

FORCEINLINE
void FourCC::SetFromUInt( UINT32 n )
{
	Assert( n != 0 );
	this->fourCC = n;
}

FORCEINLINE
void FourCC::SetFromString( const String& str )
{
	this->fourCC = FourCC::FromString( str ).AsInt();
}

FORCEINLINE
UINT32 FourCC::AsInt() const
{
	return this->fourCC;
}

inline
String FourCC::AsString() const
{
	return FourCC::ToString( *this );
}

inline
/*static */
String  FourCC::ToString( const FourCC& n )
{
    Assert( n.IsValid() );
    String str("xzyw");
    str[0] = (char) ( (n.fourCC & 0xFF000000) >> 24 );
    str[1] = (char) ( (n.fourCC & 0x00FF0000) >> 16 );
    str[2] = (char) ( (n.fourCC & 0x0000FF00) >> 8 );
    str[3] = (char) ( n.fourCC & 0x000000FF );
    return str;
}

inline
/*static */
FourCC  FourCC::FromString( const String& s )
{
	Assert( s.GetLength() == 4 );
	return FourCC( UINT32( s[0] | s[1]<<8 | s[2]<<16 | s[3]<<24 ) );
}

mxNAMESPACE_END

#endif // !__FOUR_CC_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//

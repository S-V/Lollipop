/*
=============================================================================
	File:	TextUtils.h
	Desc:
=============================================================================
*/

#ifndef __MX_TEXT_UTILS_H__
#define __MX_TEXT_UTILS_H__

mxNAMESPACE_BEGIN


// searches the specified string in the given array of strings
// and returns the index of the string
// returns INDEX_NONE if not found
UINT BinaryStringSearch(const char* stringArray[], const UINT arraySize,
					   const char* theString);

//mxSWIPED("C4");
//long GetTextLength(const char *text);
//unsigned long GetTextHash(const char *text);
//
//long FindChar(const char *text, unsigned long k);
//long FindChar(const char *text, unsigned long k, long max);
//long FindUnquotedChar(const char *text, unsigned long k);
//
//long CopyText(const char *source, char *dest);
//long CopyText(const char *source, char *dest, long max);
//
//void ConvertToLowerCase(char *text);
//void ConvertToUpperCase(char *text);
//
//bool CompareText(const char *s1, const char *s2);
//bool CompareText(const char *s1, const char *s2, long max);
//bool CompareTextCaseless(const char *s1, const char *s2);
//bool CompareTextCaseless(const char *s1, const char *s2, long max);
//bool CompareTextLessThan(const char *s1, const char *s2);
//bool CompareTextLessThan(const char *s1, const char *s2, long max);
//bool CompareTextLessEqual(const char *s1, const char *s2);
//bool CompareTextLessEqual(const char *s1, const char *s2, long max);
//
//bool ContainsText(const char *s1, const char *s2);
//
//long IntegerToString(long num, char *text, long max);
//long StringToInteger(const char *text);
//long Integer64ToString(INT64 num, char *text, long max);
//INT64 StringToInteger64(const char *text);
//long FloatToString(float num, char *text, long max);
//float StringToFloat(const char *text);


void SkipSpaces(const char *& buffer);
UINT StripTrailingSlashes( char* s, const UINT len );
UINT IndexOfLastChar( const char* s, const UINT len, const char c );
OSFileName ExtractDirName( const char* s, const UINT len );

int ParseInteger(const char *p);
float ParseFloat(const char* p);
double ParseDouble(const char* p);

U4 StringToU32( const char* s, U4 minValue, U4 maxValue );
F4 StringToF32( const char* s, F4 minValue, F4 maxValue );

char* ConvertBaseNToBinary32( U4 x, UINT base, char buf[32] );
void Print4BitsInBinary( UINT number, char str[32] );
void Print8BitsInBinary( UINT number, char str[32] );
void Print32BitsInBinary( U4 number, char str[32], char space );

mxNAMESPACE_END

#endif // !__MX_TEXT_UTILS_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//

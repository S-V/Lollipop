/*
=======================================================================
	File:	Parse_Util.h
	Desc:	
=======================================================================
*/

#ifndef __MX_PARSE_UTIL_H__
#define __MX_PARSE_UTIL_H__

#include <Base/Text/TextBuffer.h>

mxNAMESPACE_BEGIN

// for use with OutputDebugString() in Microsoft Visual C++
// so that clicking on the line in the output window
// will send you to the specified location in the source file.
inline
void FileLineNumberInfoToString(
	const char* fileName,
	int line, int column,
	StackString & outInfo
)
{
	outInfo.Format("%s(%d,%d): ",
		fileName,line,column);
}

void ParseLineNumberInfo( const char* str, UINT &line, UINT &column );

// message in a compilation log
//
struct CompileMessage
{
	UINT		line;
	UINT		column;
	TextBuffer	text;

	friend AStreamWriter& operator << ( AStreamWriter& file, const CompileMessage& o )
	{
		return file << o.line << o.column << o.text;
	}
	friend AStreamReader& operator >> ( AStreamReader& file, CompileMessage& o )
	{
		return file >> o.line >> o.column >> o.text;
	}
};

mxNAMESPACE_END

#endif /* !__MX_PARSE_UTIL_H__ */

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//

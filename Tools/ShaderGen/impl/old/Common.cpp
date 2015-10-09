/*
=============================================================================
	File:	
	Desc:	
=============================================================================
*/
#include "stdafx.h"
#pragma hdrstop
#include "ShaderGen.h"
#include "Parser.h"

void* Lex_Alloc( SizeT bytes )
{
	return mxAlloc(bytes);
}

void Lex_Free( void* ptr )
{
	mxFree(ptr);
}







int ParseInteger(const char *p)
{
	int s = p[0]=='-' ? -1 : 1;
	p += (p[0]=='-') | (p[0]=='+');
	int r = 0;
	for(; p[0]>='0' && p[0]<='9'; ++p)
		r = r*10 + (int)(p[0] - '0');
	return r*s;
}
double ParseDouble(const char* p)
{
	float s = p[0]=='-' ? -1.0f : 1.0f;
	p += (p[0]=='-') | (p[0]=='+');
	double r = 0;
	for(; p[0]>='0' && p[0]<='9'; ++p)
		r = r*10.0 + (double)(p[0] - '0');
	if(p[0]=='.' || p[0]==',')
	{
		double k = 0.1;
		for(++p; p[0]>='0' && p[0]<='9'; ++p, k *= 0.1)
			r += k*(double)(p[0] - '0');
	}
	if(p[0]=='e' || p[0]=='E')
		r *= pow(10.0, (double)ParseInteger(p + 1));
	return r*s;
}


U32 StringToU32( const char* s, U32 minValue, U32 maxValue )
{
	U32 v = ParseInteger(s);
	Assert(v >= minValue);
	Assert(v <= maxValue);
	return v;
}
F32 StringToF32( const char* s, F32 minValue, F32 maxValue )
{
	F32 v = ParseDouble(s);
	Assert(v >= minValue);
	Assert(v <= maxValue);
	return v;
}

F32 CurrTokenToFloat( F32 minValue, F32 maxValue )
{
	return StringToF32( compiler.parser->CurrTokenText(), minValue, maxValue );
}

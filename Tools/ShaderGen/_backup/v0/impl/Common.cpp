#include "stdafx.h"
#pragma hdrstop
#include "Common.h"

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
float ParseFloat(const char* p)
{
	return cast(float)ParseDouble(p);
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


wrShaderConstantBuffer & wrSourceFile::NewConstantBuffer( Parser* parser )
{
	wrShaderConstantBuffer & newCB = shaders.GetLast().inputs.constantBuffers.Add();
	newCB.pos = parser->Location();
	return newCB;
}
wrShaderConstantBuffer & wrSourceFile::LastConstantBuffer()
{
	return shaders.GetLast().inputs.constantBuffers.GetLast();
}
wrShaderSamplerState & wrSourceFile::NewSamplerState( Parser* parser )
{
	wrShaderSamplerState & newSS = shaders.GetLast().inputs.samplers.Add();
	newSS.pos = parser->Location();
	return newSS;
}
wrShaderResource& wrSourceFile::NewTexture( Parser* parser )
{
	wrShaderResource & newTex = shaders.GetLast().inputs.resources.Add();
	newTex.pos = parser->Location();
	return newTex;
}

void SkipSpaces(const char *& buffer)
{
	while( *buffer && (*buffer < 32) )
		++buffer;
}

void ParseShaderCode( wrShaderProgram & shader, Parser * parser )
{
	shader.startOfCode = parser->Location();


	const int startPos = parser->scanner->pos;
	//const int currLine = parser->Location().line;

	const char* buffer = (const char*) parser->scanner->buffer->GetPtr() + startPos;

	//SkipSpaces(buffer);



	//parser->Expect(Parser::_lbrace);


	int nestLevel = 0;

	int lastPos = parser->scanner->pos;

	while (1)
	{
		lastPos = parser->scanner->pos;

		parser->Get();

		Token* token = parser->t;

		//mxPutf("%s",token->text.ToChars());


		if( Parser::_lbrace == token->kind )
		{
			++nestLevel;
		}

		if( Parser::_rbrace == token->kind )
		{
			--nestLevel;
		}

		if(!nestLevel)
			break;
	}


	// HACK: remove redundant '}'
	--lastPos;


	if( parser->scanner->pos <= startPos )
	{
		DEBUG_BREAK;
		return;
	}
	
	int codeLength = lastPos - startPos;

	shader.code.SetNum(codeLength);

	MemCopy(shader.code.ToPtr(), buffer, codeLength);


	shader.code.Add(0);	// append null terminator

	shader.codeBlob = shader.code;	// copy the original
}



MX_SWIPED("lifted and modified from Microsoft lib");

/***
*strstr.c - search for one string inside another
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*       defines strstr() - search for one string inside another
*
*******************************************************************************/

/***
*char *strstr(string1, string2) - search for string2 in string1
*
*Purpose:
*       finds the first occurrence of string2 in string1
*
*Entry:
*       char *string1 - string to search in
*       char *string2 - string to search for
*
*Exit:
*       returns a pointer to the first occurrence of string2 in
*       string1, or NULL if string2 does not occur in string1
*
*Uses:
*
*Exceptions:
*
*******************************************************************************/

char * __cdecl MyStrStr (
					   const char * str1,
					   const char * str2,
					   SizeT maxChars
					   )
{
	char *cp = (char *) str1;
	char *s1, *s2;

	if ( !*str2 )
		return((char *)str1);

	while (*cp && ((cp - str1) < maxChars) )
	{
		s1 = cp;
		s2 = (char *) str2;

		while ( *s1 && *s2 && !(*s1-*s2) )
			s1++, s2++;

		if (!*s2)
			return(cp);

		cp++;
	}

	return(NULL);

}

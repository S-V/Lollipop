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

UINT wrSourceElement::ms_counter = 0;

wrName wrSourceElement::GetUniqueName() const
{
	wrName		tmp;
	tmp.Format("N%u",(UINT)uniqueId);
	return tmp;
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
wrVertexDeclaration& wrSourceFile::NewVertex( Parser* parser )
{
	wrVertexDeclaration & newVtx = vertexDeclarations.Add();
	newVtx.pos = parser->Location();
	return newVtx;
}


void ParseCode( wrShaderCode & code, Parser * parser )
{
	code.start = parser->Location();


	const int startPos = parser->scanner->CurrPos();
	//const int currLine = parser->Location().line;

	const char* buffer = (const char*) parser->scanner->buffer->GetPtr() + startPos;

	//SkipSpaces(buffer);



	//parser->Expect(Parser::_lbrace);


	int nestLevel = 0;

	int lastPos = parser->scanner->CurrPos();

	while (1)
	{
		lastPos = parser->scanner->CurrPos();

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


	if( parser->scanner->CurrPos() <= startPos )
	{
		mxDEBUG_BREAK;
		return;
	}
	
	int codeLength = lastPos - startPos;

	code.SetNum(codeLength);

	MemCopy(code.ToPtr(), buffer, codeLength);


//	code.Add(0);	// append null terminator

	//shader.codeBlob = shader.code;	// copy the original
}





wrShaderLibrary::wrShaderLibrary(const ParseResults& results)
{
	for( UINT iSrcFile = 0;
		iSrcFile < results.parsedFiles.Num();
		iSrcFile++ )
	{
		const ParseFileOutput& parsedData = results.parsedFiles[ iSrcFile ];
		const wrSourceFile& srcFile = parsedData.fileData;

		this->renderTargets.Append( srcFile.renderTargets );
		this->multiRenderTargets.Append( srcFile.multiRenderTargets );

		this->samplerStates.Append( srcFile.samplerStates );
		this->depthStencilStates.Append( srcFile.depthStencilStates );
		this->rasterizerStates.Append( srcFile.rasterizerStates );
		this->blendStates.Append( srcFile.blendStates );
		this->stateBlocks.Append( srcFile.stateBlocks );

		this->sharedSections.Append( srcFile.sharedSections );

		this->shaders.Append( srcFile.shaders );

		this->vertexDeclarations.Append( srcFile.vertexDeclarations );
	}
}


void Warning( const wrSourceElement& element, const char* fmt, ... )
{
	char	temp[ MAX_STRING_CHARS ];
	MX_GET_VARARGS_ANSI( temp, fmt );

	const wrLocation & loc = element.pos;

	StackString	fileLineInfo;
	FileLineNumberInfoToString(
		loc.file.ToChars(),
		loc.line, loc.column, fileLineInfo
		);

	mxPutf("%s%s",fileLineInfo.ToChars(),temp);
}

wrSharedSection* wrShaderLibrary::FindSharedSectionByName( const char* sharedSectionName )
{
	for( UINT iSharedSection = 0;
		iSharedSection < this->sharedSections.Num();
		iSharedSection++ )
	{
		wrSharedSection & sharedSection = this->sharedSections[iSharedSection];

		if(sharedSection.name.Equals(sharedSectionName))
		{
			return &sharedSection;
		}
	}
	return nil;
}

UINT wrShaderLibrary::CalcNumUniqueShaderProgramCombinations() const
{
	UINT result = 0;

	for( UINT iShader = 0;
		iShader < this->shaders.Num();
		iShader++ )
	{
		const wrShaderProgram& shader = this->shaders[ iShader ];

		result += shader.CalcNumInstances();
	}
	return result;
}

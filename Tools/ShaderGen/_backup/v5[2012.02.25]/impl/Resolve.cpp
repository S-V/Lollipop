#include "stdafx.h"
#pragma hdrstop
#include "Common.h"

//===========================================================================
template< typename T >
void CollectPointers( TList< T > & elements, TList< T* > & pointers )
{
	const UINT num = elements.Num();

	for( UINT i = 0; i < num; i++ )
	{
		pointers.Add( &elements[i] );
	}
}
//===========================================================================
template< class T >	// where T : wrRegisterBound
void FindUnusedVars(
					TList< T > & elements, const wrShaderProgram& shader,
					bool bRemoveUnusedVars = false )
{
	//char* srcCode = (char*) shader.code.ToChars();

	TList< T >	usedVars;

	for( UINT i = 0; i < elements.Num(); i++ )
	{
		T & elem = elements[ i ];

#if 0
		// assume that the element is not referenced
		elem.bindFlags = 0;

		if( shader.vsEntryOffset )
		{
			const bool usedByVS = mxStrStrNAnsi(
				shader.vsEntryOffset, elem.name.ToChars(),
				shader.vsLength
				) != nil;
			if( usedByVS ) {
				elem.bindFlags |= CB_Bind_VS;
			}
		}


		if( shader.psEntryOffset )
		{
			const bool usedByPS = mxStrStrNAnsi(
				shader.psEntryOffset, elem.name.ToChars(),
				shader.psLength
				) != nil;
			if( usedByPS ) {
				elem.bindFlags |= CB_Bind_PS;
			}
		}


		if( elem.isUsed() ) {
			usedVars.Add(elem);
		}
		else {
			Warning(elem,"unreferenced variable: '%s'\n",elem.name.ToChars());
		}
#else
		usedVars.Add(elem);
#endif

	}

	if( bRemoveUnusedVars ) {
		if( usedVars.Num() < elements.Num() ) {
			elements = usedVars;
		}
	}
}
//===========================================================================

void FindUnusedVarsInCB(wrShaderProgram & shader, bool bRemoveUnusedVars = true)
{
	TList< wrShaderConstantBuffer >	usedConstantBuffers;

	for( UINT iCB = 0;
		iCB < shader.inputs.constantBuffers.Num();
		iCB++ )
	{
		wrShaderConstantBuffer& cb = shader.inputs.constantBuffers[ iCB ];


#if 0
		cb.bindFlags = 0;

		FindUnusedVars( cb.elements, shader, bRemoveUnusedVars );

		UINT numUsedVars = 0;

		for( UINT iCBElement = 0;
			iCBElement < cb.elements.Num();
			iCBElement++ )
		{
			const wrCBVar& elem = cb.elements[ iCBElement ];
			if( elem.isUsed() ) {
				++numUsedVars;
			}

			cb.bindFlags |= elem.bindFlags;
		}

		if( numUsedVars )
		{
			usedConstantBuffers.Add(cb);
		}
		else
		{
			// this constant buffer is unused
			//unreferenced
			Warning(cb,"constant buffer '%s' is unused\n",cb.name.ToChars());
		}
#else

		usedConstantBuffers.Add(cb);

#endif
	}

	shader.inputs.constantBuffers = usedConstantBuffers;

}

//===========================================================================
template< class T, class BIT_SET >
void AssignSlots( TList< T* > & elements, BIT_SET & bitSlots, UINT maxSlot )
{
	Assert( bitSlots.Capacity() >= maxSlot );

	//bitSlots.setAll( 0 );

	for( UINT i = 0; i < elements.Num(); i++ )
	{
		T & elem = *elements[ i ];

		if( elem.iRegister != INDEX_NONE )
		{
			// the bind point was specified using the register keyword

			Assert(elem.iRegister <= maxSlot);

			if( bitSlots.get( elem.iRegister ) )
			{
				Warning( elem, "The slot %u for %s is already taken\n"
					,elem.iRegister
					,elem.name.ToChars()
					);
			}

			// mark this slot as taken
			bitSlots.set( elem.iRegister );
		}
		else
		{
			// Find a free slot.

			const int freeSlot = bitSlots.firstZeroBit();

			if ( freeSlot > maxSlot )
			{
				Warning( elem,"Out of free slots for %s\n", elem.name.ToChars() );

				elem.iRegister = maxSlot;
			}
			else
			{
				elem.iRegister = freeSlot;

				// mark this slot as taken
				bitSlots.set( freeSlot );
			}
		}
	}
}
//===========================================================================
template< class T >
void SortBySlots( TList< T* > & elements )
{
	struct MyCompareZZ
	{
		static bool Compare( const T* a, const T* b )
		{
			return a->iRegister > b->iRegister;
		}
	};

	InsertionSort< T*, MyCompareZZ >(
		elements.ToPtr(),
		0,
		elements.Num()-1
		);
}

//===========================================================================
template< class T >
void put_elements_in_sorted_order( TList< T > & elements )
{
	const UINT num = elements.Num();
	if(num < 2) {
		return;
	}

	TList< T* >	sortedPtrs;
	CollectPointers(elements,sortedPtrs);

	// sort the list of pointers

	SortBySlots(sortedPtrs);

	TList< T >	sortedElements;

	for( UINT i = 0; i < num; i++ )
	{
		sortedElements.Add( *sortedPtrs[ i ] );
	}

	elements = sortedElements;
}
//===========================================================================
template< class T >
void GetUsedElementsMask( const TList< T > & elements, BitSet32 & mask )
{
	mask.setAll(0);

	for( UINT i = 0; i < elements.Num(); i++ )
	{
		const T & elem = elements[ i ];

		Assert( elem.iRegister != INDEX_NONE );
		Assert(elem.iRegister <= mask.Capacity());

		Assert( mask.get( elem.iRegister ) == 0 );

		mask.set( elem.iRegister );
	}
}
//===========================================================================

void Resolve_Shader_Constant_Buffers( TList< wrShaderConstantBuffer* > & constantBuffers, BitSet32& bitSlots )
{
	AssignSlots( constantBuffers, bitSlots, MAX_CONSTANT_BUFFER_SLOTS );

	//SortBySlots(constantBuffers);

	//for( UINT iCB = 0;
	//	iCB < constantBuffers.Num();
	//	iCB++ )
	//{
	//	wrShaderConstantBuffer& cb = constantBuffers[ iCB ];
	//}
}
//===========================================================================
void Resolve_Shader_Sampler_States( TList< wrShaderSamplerState* > & samplers, BitSet32& bitSlots )
{
	AssignSlots( samplers, bitSlots, MAX_SAMPLER_STATE_SLOTS );

	//SortBySlots(samplers);

	//for( UINT iSamplerState = 0;
	//	iSamplerState < samplers.Num();
	//	iSamplerState++ )
	//{
	//	wrShaderSamplerState& ss = samplers[ iSamplerState ];
	//	(void)ss;
	//}
}
//===========================================================================
void Resolve_Shader_Resources( TList< wrShaderResource* > & resources, BitSet32& bitSlots )
{
	AssignSlots( resources, bitSlots, MAX_SHADER_RESOURCE_SLOTS );

	//SortBySlots(resources);

	//for( UINT iShaderResource = 0;
	//	iShaderResource < resources.Num();
	//	iShaderResource++ )
	//{
	//	wrShaderResource& tex = resources[ iShaderResource ];
	//	(void)tex;
	//}
}
//===========================================================================

UINT MeasureBracedSectionLength( const char* s )
{
	const char* p = s;

	while( *p && *p != '{') {
		++p;
	}

	UINT braceDepth = 1;
	while(*p && braceDepth)
	{
		++p;
		if( *p == '{' ) {
			braceDepth++;
		}
		if( *p == '}' ) {
			braceDepth--;
		}
	}

	if( braceDepth > 0 ) {
		mxErrf("no matching closing brace");
	}
	return p - s;
}

//===========================================================================
void scan_shader_code(wrShaderProgram & shader)
{
	Assert(!shader.vertexShader.IsEmpty());
	Assert(!shader.pixelShader.IsEmpty());

	shader.vsEntryOffset = strstr(
		(char*)shader.code.ToChars(),
		shader.vertexShader.ToChars() );

	if( shader.vsEntryOffset )
	{
		shader.vsLength = MeasureBracedSectionLength(shader.vsEntryOffset);
	}
	else //if( !shader.vsEntryOffset )
	{
		shader.vsLength = 0;
		mxWarnf("Failed to find vertex shader entry point '%s' in shader '%s'"
			,shader.vertexShader.ToChars(),shader.name.ToChars());
	}


	if( shader.pixelShader != "nil" )
	{
		shader.psEntryOffset = strstr(
			(char*)shader.code.ToChars(),
			shader.pixelShader.ToChars() );

		AssertPtr(shader.psEntryOffset);

		if( !shader.psEntryOffset ) {
			mxErrf("Failed to find pixel shader entry point '%s' in shader '%s'"
				,shader.pixelShader.ToChars(),shader.name.ToChars());
		}

		shader.psLength = MeasureBracedSectionLength(shader.psEntryOffset);
	}

}
//===========================================================================
void resolve_shader_program( wrShaderLibrary& shaderLib, wrShaderProgram & shader, const Options& config )
{
	scan_shader_code(shader);

	shader.generatedCode.Reserve(shader.code.Num());
	shader.generatedCode.Empty();



	FindUnusedVarsInCB(shader, config.bStripUnusedVars);
	FindUnusedVars( shader.inputs.samplers, shader, config.bStripUnusedVars );
	FindUnusedVars( shader.inputs.resources, shader, config.bStripUnusedVars );




	TList< wrShaderConstantBuffer* >	constantBuffersPtrs;
	TList< wrShaderSamplerState* >		shaderSamplerStatesPtrs;
	TList< wrShaderResource* >			shaderResourcesPtrs;

	CollectPointers(shader.inputs.constantBuffers,constantBuffersPtrs);
	CollectPointers(shader.inputs.samplers,shaderSamplerStatesPtrs);
	CollectPointers(shader.inputs.resources,shaderResourcesPtrs);



	BitSet32	constantBuffersSlots(0);
	BitSet32	samplerStatesSlots(0);
	BitSet32	shaderResourcesSlots(0);

	for( UINT iSharedSection = 0;
		iSharedSection < shader.inputs.sharedSections.Num();
		iSharedSection++ )
	{
		const wrBuffer & sharedSectionName = shader.inputs.sharedSections[ iSharedSection ];

		const wrSharedSection * sharedSection = shaderLib.FindSharedSectionByName(sharedSectionName.ToChars());

		if(!sharedSection)
		{
			Warning(shader,"failed to find shared section '%s'\n",sharedSectionName.ToChars());
			continue;
		}

		constantBuffersSlots.v |= sharedSection->vars.constantBuffersSlots.v;
		samplerStatesSlots.v |= sharedSection->vars.samplerStatesSlots.v;
		shaderResourcesSlots.v |= sharedSection->vars.shaderResourcesSlots.v;

		/*CollectPointers(sharedSection.vars.constantBuffers,constantBuffersPtrs);
		CollectPointers(sharedSection.vars.samplers,shaderSamplerStatesPtrs);
		CollectPointers(sharedSection.vars.resources,shaderResourcesPtrs);*/

		shader.generatedCode.Append(sharedSection->bindCode);
		shader.generatedCode.Append(sharedSection->code);
	}


	Resolve_Shader_Constant_Buffers( constantBuffersPtrs,constantBuffersSlots );
	Resolve_Shader_Sampler_States( shaderSamplerStatesPtrs,samplerStatesSlots );
	Resolve_Shader_Resources( shaderResourcesPtrs,shaderResourcesSlots );

	put_elements_in_sorted_order(shader.inputs.constantBuffers);
	put_elements_in_sorted_order(shader.inputs.samplers);
	put_elements_in_sorted_order(shader.inputs.resources);


	// Append original shader source code.

	TextBuffer::OStream codeWriter = shader.generatedCode.GetOStream();
	mxTextWriter	tw(codeWriter);

	tw << "\n";
	{
		TextBuffer	temp;
		shader.inputs.generate_hlsl_code_for_binding(temp,config);
		shader.generatedCode.Append(temp);
	}


	if( config.bEmitHLSLComments )
	{
		tw << "\n";
		tw.Putf(
			"#line %d \"%s\"\n",
			shader.code.start.line,
			shader.code.start.file.ToChars()
			);
	}

	//append original code
	shader.generatedCode.Append(shader.code);




#if 0//MX_DEBUG
	StackString	dumpFileName;
	dumpFileName.Format("R:/%s.hlsl",shader.name.ToChars());

	FileWriter	fileWriter(dumpFileName.ToChars());
	mxTextWriter	tw2(&fileWriter);
	fileWriter.Write(shader.generatedCode.ToChars(),shader.generatedCode.GetDataSize());
	//tw2.Emit( finalSrcCode.ToChars() );
#endif
}

//===========================================================================

//===========================================================================

void generate_shared_section_hlsl_code(wrSharedSection & sharedSection, const Options& config)
{
	sharedSection.vars.generate_hlsl_code_for_binding(sharedSection.bindCode, config);
}
//===========================================================================
void resolve_shared_sections(wrShaderLibrary& shaderLib, const Options& config)
{
	TList< wrShaderConstantBuffer* >	constantBuffersPtrs;
	TList< wrShaderSamplerState* >		shaderSamplerStatesPtrs;
	TList< wrShaderResource* >			shaderResourcesPtrs;

	for( UINT iSharedSection = 0;
		iSharedSection < shaderLib.sharedSections.Num();
		iSharedSection++ )
	{
		wrSharedSection & sharedSection = shaderLib.sharedSections[ iSharedSection ];

		CollectPointers(sharedSection.vars.constantBuffers,constantBuffersPtrs);
		CollectPointers(sharedSection.vars.samplers,shaderSamplerStatesPtrs);
		CollectPointers(sharedSection.vars.resources,shaderResourcesPtrs);
	}


	BitSet32 bitSlots;

	bitSlots.setAll(0);
	Resolve_Shader_Constant_Buffers(constantBuffersPtrs,bitSlots);

	bitSlots.setAll(0);
	Resolve_Shader_Sampler_States(shaderSamplerStatesPtrs,bitSlots);

	bitSlots.setAll(0);
	Resolve_Shader_Resources(shaderResourcesPtrs,bitSlots);


	for( UINT iSharedSection = 0;
		iSharedSection < shaderLib.sharedSections.Num();
		iSharedSection++ )
	{
		wrSharedSection & sharedSection = shaderLib.sharedSections[ iSharedSection ];

		put_elements_in_sorted_order(sharedSection.vars.constantBuffers);
		put_elements_in_sorted_order(sharedSection.vars.samplers);
		put_elements_in_sorted_order(sharedSection.vars.resources);

		GetUsedElementsMask(sharedSection.vars.constantBuffers,sharedSection.vars.constantBuffersSlots);
		GetUsedElementsMask(sharedSection.vars.samplers,sharedSection.vars.samplerStatesSlots);
		GetUsedElementsMask(sharedSection.vars.resources,sharedSection.vars.shaderResourcesSlots);

		generate_shared_section_hlsl_code(sharedSection, config);
	}
}
//===========================================================================
void sort_render_targets_by_size( TList< wrRenderTarget > & renderTargets )
{

}
//===========================================================================
void sort_shaders_by_name_and_generate_unique_ids( TList< wrShaderProgram > & shaders )
{
	if( !shaders.Num() )
		return;

	// sorted in ascending order
	TList< wrShaderProgram* >	shadersSortedByName;

	for( UINT iShader = 0;
		iShader < shaders.Num();
		iShader++ )
	{
		wrShaderProgram& shader = shaders[ iShader ];
		shadersSortedByName.Add(&shader);
	}


	if(shaders.Num() > 1)
	{
		struct MyCompareZZ
		{
			static bool Compare( const wrShaderProgram* a, const wrShaderProgram* b )
			{
				// strcmp returns a positive integer if s1 is greater than s2, according to the lexicographical order.
				return strcmp( a->name.ToChars(), b->name.ToChars() ) > 0;
			}
		};


		InsertionSort< wrShaderProgram*, MyCompareZZ >(
			shadersSortedByName.ToPtr(),
			0,
			shadersSortedByName.Num()-1
			);
	}


	TList< wrShaderProgram >	sortedShadersList;

	// assign unique indices to shaders

	for( UINT iShader = 0;
		iShader < shadersSortedByName.Num();
		iShader++ )
	{
		wrShaderProgram* shader = shadersSortedByName[ iShader ];

		shader->uniqueIndex = iShader;

		sortedShadersList.Add(*shader);

		DBGOUT("%s\n",shader->name.ToChars());
	}

	shaders = sortedShadersList;
}
//===========================================================================
void resolve_shaders( wrShaderLibrary& shaderLib,const Options& config )
{
	const UINT numShaders = shaderLib.shaders.Num();

	if( !numShaders ) {
		return;
	}

	for( UINT iShader = 0;
		iShader < numShaders;
		iShader++ )
	{
		wrShaderProgram& shader = shaderLib.shaders[ iShader ];

		resolve_shader_program( shaderLib,shader,config );
	}
}
//===========================================================================
bool wrShaderLibrary::Resolve(const Options& config)
{
	sort_render_targets_by_size(this->renderTargets);
	sort_shaders_by_name_and_generate_unique_ids(this->shaders);

	resolve_shared_sections(*this,config);

	resolve_shaders(*this,config);



	for( UINT iVtx = 0;
		iVtx < this->vertexDeclarations.Num();
		iVtx++ )
	{
		wrVertexDeclaration& vtx = this->vertexDeclarations[ iVtx ];
		vtx.uniqueIndex = iVtx;
	}

	return true;
}


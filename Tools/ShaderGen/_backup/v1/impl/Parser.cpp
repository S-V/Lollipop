

//BEGIN
#include "stdafx.h"
#pragma hdrstop
//#include <Base/Memory/OverrideGlobalNewDelete.h>
#include "Common.h"
//END

#include <wchar.h>
#include "Parser.h"
#include "Scanner.h"




void Parser::SynErr(int n) {
	if (errDist >= minErrDist) errors->SynErr(la->line, la->col, n);
	errDist = 0;
}

void Parser::SemErr(const wchar_t* msg) {
	if (errDist >= minErrDist) errors->Error(t->line, t->col, msg);
	errDist = 0;
}

void Parser::Get() {
	for (;;) {
		t = la;
		la = scanner->Scan();
		if (la->kind <= maxT) { ++errDist; break; }
		if (la->kind == 97) {
		}
		if (la->kind == 98) {
		}
		if (la->kind == 99) {
		}
		if (la->kind == 100) {
		}
		if (la->kind == 101) {
		}
		if (la->kind == 102) {
		}
		if (la->kind == 103) {
		}

		if (dummyToken != t) {
			dummyToken->kind = t->kind;
			dummyToken->pos = t->pos;
			dummyToken->col = t->col;
			dummyToken->line = t->line;
			dummyToken->next = NULL;
			coco_string_delete(dummyToken->val);
			dummyToken->val = coco_string_create(t->val);
			t = dummyToken;
		}
		la = t;
	}
}

void Parser::Expect(int n) {
	if (la->kind==n) Get(); else { SynErr(n); }
}

void Parser::ExpectWeak(int n, int follow) {
	if (la->kind == n) Get();
	else {
		SynErr(n);
		while (!StartOf(follow)) Get();
	}
}

bool Parser::WeakSeparator(int n, int syFol, int repFol) {
	if (la->kind == n) {Get(); return true;}
	else if (StartOf(repFol)) {return false;}
	else {
		SynErr(n);
		while (!(StartOf(syFol) || StartOf(repFol) || StartOf(0))) {
			Get();
		}
		return StartOf(syFol);
	}
}

void Parser::HLSL() {
		declaration();
		while (StartOf(1)) {
			declaration();
		}
}

void Parser::declaration() {
		switch (la->kind) {
		case 39: {
			render_target_declaration();
			break;
		}
		case 46: {
			sampler_state_declaration();
			break;
		}
		case 51: {
			depth_stencil_state_declaration();
			break;
		}
		case 56: {
			rasterizer_state_declaration();
			break;
		}
		case 67: {
			blend_state_declaration();
			break;
		}
		case 77: {
			state_block_declaration();
			break;
		}
		case 82: {
			shader_program_declaration();
			break;
		}
		case 91: {
			vertex_declaration();
			break;
		}
		case 94: {
			shared_section_declaration();
			break;
		}
		case 1: case 2: case 3: case 4: case 5: case 6: case 7: case 8: case 9: case 10: case 11: case 12: case 13: case 14: case 15: case 16: case 17: case 18: case 19: case 20: case 21: case 22: case 23: case 24: case 25: case 26: case 27: case 28: case 29: case 30: case 31: case 32: case 33: case 34: case 35: case 36: case 37: case 38: case 40: case 41: case 42: case 43: case 44: case 45: case 47: case 48: case 49: case 50: case 52: case 53: case 54: case 55: case 57: case 58: case 59: case 60: case 61: case 62: case 63: case 64: case 65: case 66: case 68: case 69: case 70: case 71: case 72: case 73: case 74: case 75: case 76: case 78: case 79: case 80: case 81: case 83: case 84: case 85: case 86: case 87: case 88: case 89: case 90: case 92: case 93: case 95: case 96: {
			Get();
			break;
		}
		default: SynErr(97); break;
		}
}

void Parser::render_target_declaration() {
		wrRenderTarget& newRT = ParsedFile().renderTargets.Add(); newRT.pos = Location(); 
		Expect(39);
		Expect(1);
		newRT.name = TokenBuf(); 
		Expect(36);
		if (la->kind == 40) {
			Get();
			Expect(3);
			newRT.sizeX.SetAbsoluteSize( TokenToU32() ); 
		} else if (la->kind == 41) {
			Get();
			Expect(2);
			newRT.sizeX.SetRelativeSize( TokenToF32() ); 
		} else SynErr(98);
		if (la->kind == 42) {
			Get();
			Expect(3);
			newRT.sizeY.SetAbsoluteSize( TokenToU32() ); 
		} else if (la->kind == 43) {
			Get();
			Expect(2);
			newRT.sizeY.SetRelativeSize( TokenToF32() ); 
		} else SynErr(99);
		Expect(44);
		Expect(1);
		newRT.format = TokenBuf(); 
		if (la->kind == 45) {
			Get();
			Expect(5);
		}
		newRT.info = TokenBuf(); 
		Expect(37);
}

void Parser::sampler_state_declaration() {
		wrSamplerState& newSS = ParsedFile().samplerStates.Add(); newSS.pos = Location(); 
		Expect(46);
		Expect(1);
		newSS.name = TokenBuf(); 
		Expect(36);
		Expect(47);
		Expect(1);
		newSS.Filter = TokenBuf(); 
		if (la->kind == 48) {
			Get();
			Expect(1);
			newSS.AddressU = TokenBuf(); 
		}
		if (la->kind == 49) {
			Get();
			Expect(1);
			newSS.AddressV = TokenBuf(); 
		}
		if (la->kind == 50) {
			Get();
			Expect(1);
			newSS.AddressW = TokenBuf(); 
		}
		Expect(37);
}

void Parser::depth_stencil_state_declaration() {
		wrDepthStencilState& newDS = ParsedFile().depthStencilStates.Add(); newDS.pos = Location(); 
		Expect(51);
		Expect(1);
		newDS.name = TokenBuf(); 
		Expect(36);
		Expect(52);
		boolean_constant();
		newDS.DepthEnable = TokenBuf(); 
		Expect(53);
		bit_mask();
		newDS.DepthWriteMask = TokenBuf(); 
		Expect(54);
		Expect(1);
		newDS.DepthFunc = TokenBuf(); 
		Expect(55);
		boolean_constant();
		newDS.StencilEnable = TokenBuf(); 
		Expect(37);
}

void Parser::rasterizer_state_declaration() {
		wrRasterizerState& newRS = ParsedFile().rasterizerStates.Add(); newRS.pos = Location(); 
		Expect(56);
		Expect(1);
		newRS.name = TokenBuf(); 
		Expect(36);
		Expect(57);
		Expect(1);
		newRS.FillMode = TokenBuf(); 
		Expect(58);
		Expect(1);
		newRS.CullMode = TokenBuf(); 
		if (la->kind == 59) {
			Get();
			boolean_constant();
			newRS.FrontCounterClockwise = TokenBuf(); 
		}
		if (la->kind == 60) {
			Get();
			Expect(3);
			newRS.DepthBias = TokenBuf(); 
		}
		if (la->kind == 61) {
			Get();
			Expect(2);
			newRS.DepthBiasClamp = TokenBuf(); 
		}
		if (la->kind == 62) {
			Get();
			Expect(2);
			newRS.SlopeScaledDepthBias = TokenBuf(); 
		}
		if (la->kind == 63) {
			Get();
			boolean_constant();
			newRS.DepthClipEnable = TokenBuf(); 
		}
		if (la->kind == 64) {
			Get();
			boolean_constant();
			newRS.ScissorEnable = TokenBuf(); 
		}
		if (la->kind == 65) {
			Get();
			boolean_constant();
			newRS.MultisampleEnable = TokenBuf(); 
		}
		if (la->kind == 66) {
			Get();
			boolean_constant();
			newRS.AntialiasedLineEnable = TokenBuf(); 
		}
		Expect(37);
}

void Parser::blend_state_declaration() {
		wrBlendState& newBS = ParsedFile().blendStates.Add(); newBS.pos = Location(); 
		Expect(67);
		Expect(1);
		newBS.name = TokenBuf(); 
		Expect(36);
		Expect(68);
		boolean_constant();
		newBS.BlendEnable = TokenBuf(); 
		if (la->kind == 69) {
			Get();
			boolean_constant();
			newBS.AlphaToCoverageEnable = TokenBuf(); 
		}
		if (la->kind == 70) {
			Get();
			Expect(1);
			newBS.SrcBlend = TokenBuf(); 
		}
		if (la->kind == 71) {
			Get();
			Expect(1);
			newBS.DestBlend = TokenBuf(); 
		}
		if (la->kind == 72) {
			Get();
			Expect(1);
			newBS.BlendOp= TokenBuf(); 
		}
		if (la->kind == 73) {
			Get();
			Expect(1);
			newBS.SrcBlendAlpha = TokenBuf(); 
		}
		if (la->kind == 74) {
			Get();
			Expect(1);
			newBS.DestBlendAlpha = TokenBuf(); 
		}
		if (la->kind == 75) {
			Get();
			Expect(1);
			newBS.BlendOpAlpha = TokenBuf(); 
		}
		if (la->kind == 76) {
			Get();
			bit_mask();
			newBS.RenderTargetWriteMask = TokenBuf(); 
		}
		Expect(37);
}

void Parser::state_block_declaration() {
		wrStateBlock& newSB = ParsedFile().stateBlocks.Add(); newSB.pos = Location(); 
		Expect(77);
		Expect(1);
		newSB.name = TokenBuf(); 
		Expect(36);
		if (la->kind == 56) {
			Get();
			Expect(1);
			newSB.rasterizerState = TokenBuf(); 
		}
		if (la->kind == 51) {
			Get();
			Expect(1);
			newSB.depthStencilState = TokenBuf(); 
		}
		if (la->kind == 78) {
			Get();
			Expect(3);
			newSB.stencilRef = TokenBuf(); 
		}
		if (la->kind == 67) {
			Get();
			Expect(1);
			newSB.blendState = TokenBuf(); 
		}
		if (la->kind == 79) {
			Get();
			rgba_color(newSB.blendFactorRGBA );
		}
		if (la->kind == 80) {
			Get();
			bit_mask();
			newSB.sampleMask = TokenBuf(); 
		}
		Expect(37);
}

void Parser::shader_program_declaration() {
		wrShaderProgram& newSP = ParsedFile().NewElement( this, ParsedFile().shaders ); SETPOS(newSP); 
		Expect(82);
		Expect(1);
		newSP.name = TokenBuf(); 
		Expect(36);
		if (la->kind == 87) {
			shader_inputs();
		}
		Expect(83);
		ParseCode( newSP.code, this ); 
		Expect(37);
		Expect(84);
		Expect(36);
		Expect(85);
		Expect(1);
		newSP.vertexShader = TokenBuf(); 
		Expect(86);
		Expect(1);
		newSP.pixelShader = TokenBuf(); 
		Expect(37);
}

void Parser::vertex_declaration() {
		Expect(91);
		wrVertexDeclaration& newVtx = ParsedFile().NewVertex( this ); 
		Expect(1);
		newVtx.name = TokenBuf(); 
		Expect(36);
		vertex_element();
		while (la->kind == 1) {
			vertex_element();
		}
		Expect(37);
}

void Parser::shared_section_declaration() {
		Expect(94);
		wrSharedSection & sharedSection = ParsedFile().sharedSections.Add(); SETPOS(sharedSection); 
		Expect(1);
		sharedSection.name = TokenBuf(); 
		Expect(36);
		shared_variables_declaration(sharedSection );
		if (la->kind == 83) {
			shared_code_declaration(sharedSection );
		}
		Expect(37);
}

void Parser::boolean_constant() {
		if (la->kind == 1) {
			Get();
		} else if (la->kind == 3) {
			Get();
		} else SynErr(100);
}

void Parser::bit_mask() {
		if (la->kind == 1) {
			Get();
		} else if (la->kind == 3) {
			Get();
		} else SynErr(101);
}

void Parser::rgba_color(FColor & rgba ) {
		Expect(81);
		Expect(2);
		rgba.R = TokenToF32( 0.0f, 1.0f ); 
		Expect(2);
		rgba.G = TokenToF32( 0.0f, 1.0f ); 
		Expect(2);
		rgba.B = TokenToF32( 0.0f, 1.0f ); 
		Expect(2);
		rgba.A = TokenToF32( 0.0f, 1.0f ); 
		Expect(33);
}

void Parser::shader_inputs() {
		Expect(87);
		wrShaderProgram & shader = ParsedFile().shaders.GetLast(); 
		Expect(36);
		shader.startOfInputs = Location(); 
		shader_variables_list_decl(shader.inputs );
		Expect(37);
}

void Parser::shader_variables_list_decl(wrShaderVariables & vars ) {
		shader_variable_decl(vars );
		while (la->kind == 8 || la->kind == 46 || la->kind == 88) {
			shader_variable_decl(vars );
		}
}

void Parser::shader_variable_decl(wrShaderVariables & vars ) {
		if (la->kind == 8) {
			constant_buffer_decl(vars );
		} else if (la->kind == 46) {
			sampler_state_decl(vars );
		} else if (la->kind == 88) {
			shader_resource_decl(vars );
		} else SynErr(102);
}

void Parser::constant_buffer_decl(wrShaderVariables & vars ) {
		wrShaderConstantBuffer& newCB = vars.constantBuffers.Add(); SETPOS(newCB); 
		Expect(8);
		Expect(1);
		newCB.name = TokenBuf(); 
		if (la->kind == 30) {
			constant_buffer_register(newCB);
		}
		Expect(36);
		constant_buffer_element(newCB);
		while (la->kind == 1) {
			constant_buffer_element(newCB);
		}
		Expect(37);
		Expect(29);
}

void Parser::sampler_state_decl(wrShaderVariables & vars ) {
		wrShaderSamplerState& newSS = vars.samplers.Add(); SETPOS(newSS); 
		Expect(46);
		Expect(1);
		newSS.name = TokenBuf(); 
		if (la->kind == 30) {
			Get();
			Expect(18);
			Expect(32);
			Expect(3);
			newSS.iRegister = TokenToU32(0,MAX_SAMPLER_STATE_SLOTS-1); 
			Expect(33);
		}
		if (la->kind == 84) {
			Get();
			Expect(1);
			newSS.initializer = TokenBuf(); 
		}
		Expect(29);
}

void Parser::shader_resource_decl(wrShaderVariables & vars ) {
		wrShaderResource& newRes = vars.resources.Add(); SETPOS(newRes); 
		Expect(88);
		if (la->kind == 89) {
			Get();
			type_name();
			Expect(90);
		}
		Expect(1);
		newRes.name = TokenBuf(); 
		if (la->kind == 30) {
			Get();
			Expect(18);
			Expect(32);
			Expect(3);
			newRes.iRegister = TokenToU32(0,MAX_SHADER_RESOURCE_SLOTS-1); 
			Expect(33);
		}
		if (la->kind == 84) {
			Get();
			Expect(1);
			newRes.initializer = TokenBuf(); 
		}
		Expect(29);
}

void Parser::constant_buffer_register(wrShaderConstantBuffer & cb ) {
		Expect(30);
		Expect(18);
		Expect(32);
		Expect(3);
		cb.iRegister = TokenToU32(0,MAX_CONSTANT_BUFFER_SLOTS-1); 
		Expect(33);
}

void Parser::constant_buffer_element(wrShaderConstantBuffer & cb ) {
		wrCBVar& newCBElem = cb.elements.Add(); SETPOS(newCBElem); 
		type_name();
		newCBElem.typeName = TokenBuf(); 
		Expect(1);
		newCBElem.name = TokenBuf(); 
		Expect(29);
}

void Parser::type_name() {
		Expect(1);
}

void Parser::vertex_element() {
		wrVertexElement& newVtxElem = ParsedFile().vertexDeclarations.GetLast().elements.Add(); 
		Expect(1);
		newVtxElem.name = TokenBuf(); 
		if (la->kind == 5) {
			Get();
			newVtxElem.info = TokenBuf(); 
		}
		Expect(36);
		Expect(92);
		Expect(1);
		newVtxElem.SemanticName = TokenBuf(); 
		Expect(93);
		Expect(3);
		newVtxElem.SemanticIndex = TokenToU32(0,32); 
		Expect(44);
		Expect(1);
		newVtxElem.Format = TokenBuf(); 
		Expect(37);
}

void Parser::shared_variables_declaration(wrSharedSection & sharedSection ) {
		Expect(95);
		Expect(36);
		shader_variables_list_decl(sharedSection.vars );
		Expect(37);
}

void Parser::shared_code_declaration(wrSharedSection & sharedSection ) {
		Expect(83);
		ParseCode( sharedSection.code, this ); 
}



void Parser::Parse() {
	t = NULL;
	la = dummyToken = new Token();
	la->val = coco_string_create(L"Dummy Token");
	Get();
	HLSL();
	Expect(0);
}

Parser::Parser( Scanner *scanner, const ParseFileInput& parseInput, ParseFileOutput &parseOutput )
	: input( parseInput ), output( parseOutput ), numWarnings( 0 ), numErrors( 0 )
{
	maxT = 96;

	dummyToken = NULL;
	t = la = NULL;
	minErrDist = 2;
	errDist = minErrDist;
	this->scanner = scanner;
	errors = new Errors(this);
}

bool Parser::StartOf(int s) {
	const bool T = true;
	const bool x = false;

	static bool set[2][98] = {
		{T,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x},
		{x,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,x}
	};



	return set[s][la->kind];
}

Parser::~Parser() {
	delete errors;
	delete dummyToken;
}

Errors::Errors( Parser* p ) {
	count = 0;
	parser = p;
}

void Errors::SynErr(int line, int col, int n) {
	wchar_t* s;
	switch (n) {
			case 0: s = coco_string_create(L"EOF expected"); break;
			case 1: s = coco_string_create(L"identifier expected"); break;
			case 2: s = coco_string_create(L"float_constant expected"); break;
			case 3: s = coco_string_create(L"integer_constant expected"); break;
			case 4: s = coco_string_create(L"char_constant expected"); break;
			case 5: s = coco_string_create(L"string_constant expected"); break;
			case 6: s = coco_string_create(L"auto expected"); break;
			case 7: s = coco_string_create(L"case expected"); break;
			case 8: s = coco_string_create(L"cbuffer expected"); break;
			case 9: s = coco_string_create(L"char expected"); break;
			case 10: s = coco_string_create(L"const expected"); break;
			case 11: s = coco_string_create(L"default expected"); break;
			case 12: s = coco_string_create(L"double expected"); break;
			case 13: s = coco_string_create(L"enum expected"); break;
			case 14: s = coco_string_create(L"extern expected"); break;
			case 15: s = coco_string_create(L"float expected"); break;
			case 16: s = coco_string_create(L"int expected"); break;
			case 17: s = coco_string_create(L"long expected"); break;
			case 18: s = coco_string_create(L"register expected"); break;
			case 19: s = coco_string_create(L"short expected"); break;
			case 20: s = coco_string_create(L"signed expected"); break;
			case 21: s = coco_string_create(L"static expected"); break;
			case 22: s = coco_string_create(L"struct expected"); break;
			case 23: s = coco_string_create(L"typedef expected"); break;
			case 24: s = coco_string_create(L"union expected"); break;
			case 25: s = coco_string_create(L"unsigned expected"); break;
			case 26: s = coco_string_create(L"void expected"); break;
			case 27: s = coco_string_create(L"volatile expected"); break;
			case 28: s = coco_string_create(L"comma expected"); break;
			case 29: s = coco_string_create(L"semicolon expected"); break;
			case 30: s = coco_string_create(L"colon expected"); break;
			case 31: s = coco_string_create(L"star expected"); break;
			case 32: s = coco_string_create(L"lpar expected"); break;
			case 33: s = coco_string_create(L"rpar expected"); break;
			case 34: s = coco_string_create(L"lbrack expected"); break;
			case 35: s = coco_string_create(L"rbrack expected"); break;
			case 36: s = coco_string_create(L"lbrace expected"); break;
			case 37: s = coco_string_create(L"rbrace expected"); break;
			case 38: s = coco_string_create(L"ellipsis expected"); break;
			case 39: s = coco_string_create(L"\"RenderTarget\" expected"); break;
			case 40: s = coco_string_create(L"\"SizeX\" expected"); break;
			case 41: s = coco_string_create(L"\"ScaleX\" expected"); break;
			case 42: s = coco_string_create(L"\"SizeY\" expected"); break;
			case 43: s = coco_string_create(L"\"ScaleY\" expected"); break;
			case 44: s = coco_string_create(L"\"Format\" expected"); break;
			case 45: s = coco_string_create(L"\"Info\" expected"); break;
			case 46: s = coco_string_create(L"\"SamplerState\" expected"); break;
			case 47: s = coco_string_create(L"\"Filter\" expected"); break;
			case 48: s = coco_string_create(L"\"AddressU\" expected"); break;
			case 49: s = coco_string_create(L"\"AddressV\" expected"); break;
			case 50: s = coco_string_create(L"\"AddressW\" expected"); break;
			case 51: s = coco_string_create(L"\"DepthStencilState\" expected"); break;
			case 52: s = coco_string_create(L"\"DepthEnable\" expected"); break;
			case 53: s = coco_string_create(L"\"DepthWriteMask\" expected"); break;
			case 54: s = coco_string_create(L"\"DepthFunc\" expected"); break;
			case 55: s = coco_string_create(L"\"StencilEnable\" expected"); break;
			case 56: s = coco_string_create(L"\"RasterizerState\" expected"); break;
			case 57: s = coco_string_create(L"\"FillMode\" expected"); break;
			case 58: s = coco_string_create(L"\"CullMode\" expected"); break;
			case 59: s = coco_string_create(L"\"FrontCounterClockwise\" expected"); break;
			case 60: s = coco_string_create(L"\"DepthBias\" expected"); break;
			case 61: s = coco_string_create(L"\"DepthBiasClamp\" expected"); break;
			case 62: s = coco_string_create(L"\"SlopeScaledDepthBias\" expected"); break;
			case 63: s = coco_string_create(L"\"DepthClipEnable\" expected"); break;
			case 64: s = coco_string_create(L"\"ScissorEnable\" expected"); break;
			case 65: s = coco_string_create(L"\"MultisampleEnable\" expected"); break;
			case 66: s = coco_string_create(L"\"AntialiasedLineEnable\" expected"); break;
			case 67: s = coco_string_create(L"\"BlendState\" expected"); break;
			case 68: s = coco_string_create(L"\"BlendEnable\" expected"); break;
			case 69: s = coco_string_create(L"\"AlphaToCoverageEnable\" expected"); break;
			case 70: s = coco_string_create(L"\"SrcBlend\" expected"); break;
			case 71: s = coco_string_create(L"\"DestBlend\" expected"); break;
			case 72: s = coco_string_create(L"\"BlendOp\" expected"); break;
			case 73: s = coco_string_create(L"\"SrcBlendAlpha\" expected"); break;
			case 74: s = coco_string_create(L"\"DestBlendAlpha\" expected"); break;
			case 75: s = coco_string_create(L"\"BlendOpAlpha\" expected"); break;
			case 76: s = coco_string_create(L"\"RenderTargetWriteMask\" expected"); break;
			case 77: s = coco_string_create(L"\"StateBlock\" expected"); break;
			case 78: s = coco_string_create(L"\"StencilRef\" expected"); break;
			case 79: s = coco_string_create(L"\"BlendFactor\" expected"); break;
			case 80: s = coco_string_create(L"\"SampleMask\" expected"); break;
			case 81: s = coco_string_create(L"\"RGBA(\" expected"); break;
			case 82: s = coco_string_create(L"\"Shader\" expected"); break;
			case 83: s = coco_string_create(L"\"Code\" expected"); break;
			case 84: s = coco_string_create(L"\"=\" expected"); break;
			case 85: s = coco_string_create(L"\"VertexShader\" expected"); break;
			case 86: s = coco_string_create(L"\"PixelShader\" expected"); break;
			case 87: s = coco_string_create(L"\"Inputs\" expected"); break;
			case 88: s = coco_string_create(L"\"Texture2D\" expected"); break;
			case 89: s = coco_string_create(L"\"<\" expected"); break;
			case 90: s = coco_string_create(L"\">\" expected"); break;
			case 91: s = coco_string_create(L"\"Vertex\" expected"); break;
			case 92: s = coco_string_create(L"\"SemanticName\" expected"); break;
			case 93: s = coco_string_create(L"\"SemanticIndex\" expected"); break;
			case 94: s = coco_string_create(L"\"Shared\" expected"); break;
			case 95: s = coco_string_create(L"\"Variables\" expected"); break;
			case 96: s = coco_string_create(L"??? expected"); break;
			case 97: s = coco_string_create(L"invalid declaration"); break;
			case 98: s = coco_string_create(L"invalid render_target_declaration"); break;
			case 99: s = coco_string_create(L"invalid render_target_declaration"); break;
			case 100: s = coco_string_create(L"invalid boolean_constant"); break;
			case 101: s = coco_string_create(L"invalid bit_mask"); break;
			case 102: s = coco_string_create(L"invalid shader_variable_decl"); break;

		default:
		{
			wchar_t format[20];
			coco_swprintf(format, 20, L"error %d", n);
			s = coco_string_create(format);
		}
		break;
	}
	wprintf(L"-- line %d col %d: %ls\n", line, col, s);
	
//BEGIN
	parser->SyntaxError(line,col,MX_TO_ANSI(s));
//END
	
	coco_string_delete(s);
	count++;
}

void Errors::Error(int line, int col, const wchar_t *s) {
	wprintf(L"-- line %d col %d: %ls\n", line, col, MX_TO_ANSI(s));
	
//BEGIN
	parser->Error(line,col,MX_TO_ANSI(s));
//END

	count++;
}

void Errors::Warning(int line, int col, const wchar_t *s) {
	wprintf(L"-- line %d col %d: %ls\n", line, col, MX_TO_ANSI(s));
	
//BEGIN
	parser->Warning(line,col,MX_TO_ANSI(s));
//END
}

void Errors::Warning(const wchar_t *s) {
	wprintf(L"%ls\n", s);
//BEGIN
	parser->Warning(MX_TO_ANSI(s));
//END
}

void Errors::Exception(const wchar_t* s) {
	wprintf(L"%ls", s); 
//BEGIN
	parser->Message(MX_TO_ANSI(s));
//END
	exit(1);
}



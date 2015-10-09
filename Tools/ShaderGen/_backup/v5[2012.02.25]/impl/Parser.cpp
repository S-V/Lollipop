

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
		if (la->kind == 110) {
		}
		if (la->kind == 111) {
		}
		if (la->kind == 112) {
		}
		if (la->kind == 113) {
		}
		if (la->kind == 114) {
		}
		if (la->kind == 115) {
		}
		if (la->kind == 116) {
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
		case 42: {
			render_target_declaration();
			break;
		}
		case 49: {
			multi_render_target_declaration();
			break;
		}
		case 51: case 52: {
			sampler_state_declaration();
			break;
		}
		case 63: {
			depth_stencil_state_declaration();
			break;
		}
		case 68: {
			rasterizer_state_declaration();
			break;
		}
		case 79: {
			blend_state_declaration();
			break;
		}
		case 89: {
			state_block_declaration();
			break;
		}
		case 94: {
			shader_program_declaration();
			break;
		}
		case 103: {
			vertex_declaration();
			break;
		}
		case 107: {
			shared_section_declaration();
			break;
		}
		case 1: case 2: case 3: case 4: case 5: case 6: case 7: case 8: case 9: case 10: case 11: case 12: case 13: case 14: case 15: case 16: case 17: case 18: case 19: case 20: case 21: case 22: case 23: case 24: case 25: case 26: case 27: case 28: case 29: case 30: case 31: case 32: case 33: case 34: case 35: case 36: case 37: case 38: case 39: case 40: case 41: case 43: case 44: case 45: case 46: case 47: case 48: case 50: case 53: case 54: case 55: case 56: case 57: case 58: case 59: case 60: case 61: case 62: case 64: case 65: case 66: case 67: case 69: case 70: case 71: case 72: case 73: case 74: case 75: case 76: case 77: case 78: case 80: case 81: case 82: case 83: case 84: case 85: case 86: case 87: case 88: case 90: case 91: case 92: case 93: case 95: case 96: case 97: case 98: case 99: case 100: case 101: case 102: case 104: case 105: case 106: case 108: case 109: {
			Get();
			break;
		}
		default: SynErr(110); break;
		}
}

void Parser::render_target_declaration() {
		wrRenderTarget& newRT = ParsedFile().renderTargets.Add(); newRT.pos = Location(); 
		Expect(42);
		Expect(1);
		newRT.name = TokenBuf(); 
		Expect(33);
		if (la->kind == 43) {
			Get();
			Expect(3);
			newRT.sizeX.SetAbsoluteSize( TokenToU32() ); 
		} else if (la->kind == 44) {
			Get();
			Expect(2);
			newRT.sizeX.SetRelativeSize( TokenToF32() ); 
		} else SynErr(111);
		if (la->kind == 45) {
			Get();
			Expect(3);
			newRT.sizeY.SetAbsoluteSize( TokenToU32() ); 
		} else if (la->kind == 46) {
			Get();
			Expect(2);
			newRT.sizeY.SetRelativeSize( TokenToF32() ); 
		} else SynErr(112);
		Expect(47);
		Expect(1);
		newRT.format = TokenBuf(); 
		if (la->kind == 48) {
			Get();
			Expect(5);
		}
		newRT.info = TokenBuf(); 
		Expect(34);
}

void Parser::multi_render_target_declaration() {
		wrMultiRenderTarget& newMRT = ParsedFile().multiRenderTargets.Add(); SetPos(newMRT); 
		Expect(49);
		Expect(1);
		newMRT.name = TokenBuf(); 
		Expect(33);
		multi_render_target_list();
		if (la->kind == 48) {
			Get();
			Expect(5);
		}
		newMRT.info = TokenBuf(); 
		Expect(34);
}

void Parser::sampler_state_declaration() {
		wrSamplerState& newSS = ParsedFile().samplerStates.Add(); newSS.pos = Location(); 
		if (la->kind == 51) {
			Get();
			Expect(1);
			newSS.name = TokenBuf(); 
		} else if (la->kind == 52) {
			Get();
			Expect(1);
			newSS.name = TokenBuf(); newSS.bIsSamplerCmpState = true; 
		} else SynErr(113);
		Expect(33);
		Expect(53);
		Expect(1);
		newSS.Filter = TokenBuf(); 
		if (la->kind == 54) {
			Get();
			Expect(1);
			newSS.AddressU = TokenBuf(); 
		}
		if (la->kind == 55) {
			Get();
			Expect(1);
			newSS.AddressV = TokenBuf(); 
		}
		if (la->kind == 56) {
			Get();
			Expect(1);
			newSS.AddressW = TokenBuf(); 
		}
		if (la->kind == 57) {
			Get();
			Expect(2);
			newSS.MipLODBias = TokenToF32(); 
		}
		if (la->kind == 58) {
			Get();
			Expect(3);
			newSS.MaxAnisotropy = TokenToU32(0,16); 
		}
		if (la->kind == 59) {
			Get();
			Expect(1);
			newSS.ComparisonFunc = TokenBuf(); 
		}
		if (la->kind == 60) {
			Get();
			rgba_color(newSS.BorderColor );
		}
		if (la->kind == 61) {
			Get();
			Expect(2);
			newSS.MinLOD = TokenToF32(); 
		}
		if (la->kind == 62) {
			Get();
			Expect(2);
			newSS.MaxLOD = TokenToF32(); 
		}
		Expect(34);
}

void Parser::depth_stencil_state_declaration() {
		wrDepthStencilState& newDS = ParsedFile().depthStencilStates.Add(); newDS.pos = Location(); 
		Expect(63);
		Expect(1);
		newDS.name = TokenBuf(); 
		Expect(33);
		Expect(64);
		boolean_constant();
		newDS.DepthEnable = TokenBuf(); 
		Expect(65);
		bit_mask();
		newDS.DepthWriteMask = TokenBuf(); 
		Expect(66);
		Expect(1);
		newDS.DepthFunc = TokenBuf(); 
		Expect(67);
		boolean_constant();
		newDS.StencilEnable = TokenBuf(); 
		Expect(34);
}

void Parser::rasterizer_state_declaration() {
		wrRasterizerState& newRS = ParsedFile().rasterizerStates.Add(); newRS.pos = Location(); 
		Expect(68);
		Expect(1);
		newRS.name = TokenBuf(); 
		Expect(33);
		Expect(69);
		Expect(1);
		newRS.FillMode = TokenBuf(); 
		Expect(70);
		Expect(1);
		newRS.CullMode = TokenBuf(); 
		if (la->kind == 71) {
			Get();
			boolean_constant();
			newRS.FrontCounterClockwise = TokenBuf(); 
		}
		if (la->kind == 72) {
			Get();
			Expect(3);
			newRS.DepthBias = TokenBuf(); 
		}
		if (la->kind == 73) {
			Get();
			Expect(2);
			newRS.DepthBiasClamp = TokenBuf(); 
		}
		if (la->kind == 74) {
			Get();
			Expect(2);
			newRS.SlopeScaledDepthBias = TokenBuf(); 
		}
		if (la->kind == 75) {
			Get();
			boolean_constant();
			newRS.DepthClipEnable = TokenBuf(); 
		}
		if (la->kind == 76) {
			Get();
			boolean_constant();
			newRS.ScissorEnable = TokenBuf(); 
		}
		if (la->kind == 77) {
			Get();
			boolean_constant();
			newRS.MultisampleEnable = TokenBuf(); 
		}
		if (la->kind == 78) {
			Get();
			boolean_constant();
			newRS.AntialiasedLineEnable = TokenBuf(); 
		}
		Expect(34);
}

void Parser::blend_state_declaration() {
		wrBlendState& newBS = ParsedFile().blendStates.Add(); newBS.pos = Location(); 
		Expect(79);
		Expect(1);
		newBS.name = TokenBuf(); 
		Expect(33);
		Expect(80);
		boolean_constant();
		newBS.BlendEnable = TokenBuf(); 
		if (la->kind == 81) {
			Get();
			boolean_constant();
			newBS.AlphaToCoverageEnable = TokenBuf(); 
		}
		if (la->kind == 82) {
			Get();
			Expect(1);
			newBS.SrcBlend = TokenBuf(); 
		}
		if (la->kind == 83) {
			Get();
			Expect(1);
			newBS.DestBlend = TokenBuf(); 
		}
		if (la->kind == 84) {
			Get();
			Expect(1);
			newBS.BlendOp= TokenBuf(); 
		}
		if (la->kind == 85) {
			Get();
			Expect(1);
			newBS.SrcBlendAlpha = TokenBuf(); 
		}
		if (la->kind == 86) {
			Get();
			Expect(1);
			newBS.DestBlendAlpha = TokenBuf(); 
		}
		if (la->kind == 87) {
			Get();
			Expect(1);
			newBS.BlendOpAlpha = TokenBuf(); 
		}
		if (la->kind == 88) {
			Get();
			bit_mask();
			newBS.RenderTargetWriteMask = TokenBuf(); 
		}
		Expect(34);
}

void Parser::state_block_declaration() {
		wrStateBlock& newSB = ParsedFile().stateBlocks.Add(); newSB.pos = Location(); 
		Expect(89);
		Expect(1);
		newSB.name = TokenBuf(); 
		Expect(33);
		if (la->kind == 68) {
			Get();
			Expect(1);
			newSB.rasterizerState = TokenBuf(); 
		}
		if (la->kind == 63) {
			Get();
			Expect(1);
			newSB.depthStencilState = TokenBuf(); 
		}
		if (la->kind == 90) {
			Get();
			Expect(3);
			newSB.stencilRef = TokenBuf(); 
		}
		if (la->kind == 79) {
			Get();
			Expect(1);
			newSB.blendState = TokenBuf(); 
		}
		if (la->kind == 91) {
			Get();
			rgba_color(newSB.blendFactorRGBA );
		}
		if (la->kind == 92) {
			Get();
			bit_mask();
			newSB.sampleMask = TokenBuf(); 
		}
		Expect(34);
}

void Parser::shader_program_declaration() {
		wrShaderProgram& newSP = ParsedFile().NewElement( this, ParsedFile().shaders ); SetPos(newSP); 
		Expect(94);
		Expect(1);
		newSP.name = TokenBuf(); 
		if (la->kind == 99) {
			shader_defines(newSP );
		}
		Expect(33);
		if (la->kind == 101) {
			shader_inputs();
		}
		Expect(95);
		ParseCode( newSP.code, this ); 
		Expect(34);
		Expect(96);
		Expect(33);
		Expect(97);
		Expect(1);
		newSP.vertexShader = TokenBuf(); 
		Expect(98);
		Expect(1);
		newSP.pixelShader = TokenBuf(); 
		Expect(34);
}

void Parser::vertex_declaration() {
		Expect(103);
		wrVertexDeclaration& newVtx = ParsedFile().NewVertex( this ); 
		Expect(1);
		newVtx.name = TokenBuf(); 
		Expect(33);
		vertex_element();
		while (la->kind == 1) {
			vertex_element();
		}
		Expect(34);
}

void Parser::shared_section_declaration() {
		Expect(107);
		wrSharedSection & sharedSection = ParsedFile().sharedSections.Add(); SetPos(sharedSection); 
		Expect(1);
		sharedSection.name = TokenBuf(); 
		Expect(33);
		shared_variables_declaration(sharedSection );
		if (la->kind == 95) {
			shared_code_declaration(sharedSection );
		}
		Expect(34);
}

void Parser::multi_render_target_list() {
		wrMultiRenderTarget& newMRT = ParsedFile().multiRenderTargets.GetLast(); 
		Expect(42);
		Expect(1);
		wrRenderTarget& newRT = newMRT.renderTargets.Add();  SetPos(newRT); newRT.name = TokenBuf();
		Expect(33);
		if (la->kind == 50) {
			Get();
			rgba_color(newRT.clearColor );
		}
		Expect(34);
		while (la->kind == 42) {
			multi_render_target_list();
		}
}

void Parser::rgba_color(FColor & rgba ) {
		Expect(93);
		Expect(2);
		rgba.R = TokenToF32( 0.0f, 1.0f ); 
		Expect(2);
		rgba.G = TokenToF32( 0.0f, 1.0f ); 
		Expect(2);
		rgba.B = TokenToF32( 0.0f, 1.0f ); 
		Expect(2);
		rgba.A = TokenToF32( 0.0f, 1.0f ); 
		Expect(30);
}

void Parser::boolean_constant() {
		if (la->kind == 1) {
			Get();
		} else if (la->kind == 3) {
			Get();
		} else SynErr(114);
}

void Parser::bit_mask() {
		if (la->kind == 1) {
			Get();
		} else if (la->kind == 3) {
			Get();
		} else SynErr(115);
}

void Parser::shader_defines(wrShaderProgram & shader ) {
		Expect(99);
		shader_define(shader );
		while (la->kind == 1) {
			shader_define(shader );
		}
		Expect(100);
}

void Parser::shader_inputs() {
		Expect(101);
		wrShaderProgram & shader = ParsedFile().shaders.GetLast(); 
		if (la->kind == 27) {
			using_shared_sections_decls_list(shader.inputs.sharedSections );
		}
		Expect(33);
		SetPos(shader.inputs); 
		if (StartOf(2)) {
			shader_variables_list_decl(shader.inputs );
		}
		Expect(34);
}

void Parser::shader_define(wrShaderProgram & shader ) {
		wrShaderDefine& newDefine = shader.defines.Add(); SetPos(newDefine); 
		Expect(1);
		newDefine.name = TokenBuf(); 
		Expect(96);
		Expect(3);
		newDefine.defaultValue = TokenToU32(0,1); 
}

void Parser::using_shared_sections_decls_list(SharedSectionsList& sharedSections ) {
		Expect(27);
		using_shared_sections_decl(sharedSections);
		while (la->kind == 1) {
			using_shared_sections_decl(sharedSections);
		}
}

void Parser::shader_variables_list_decl(wrShaderVariables & vars ) {
		shader_variable_decl(vars );
		while (StartOf(2)) {
			shader_variable_decl(vars );
		}
}

void Parser::shader_variable_decl(wrShaderVariables & vars ) {
		if (la->kind == 8) {
			constant_buffer_decl(vars );
		} else if (la->kind == 51 || la->kind == 52) {
			sampler_state_decl(vars );
		} else if (la->kind == 102) {
			shader_resource_decl(vars );
		} else SynErr(116);
}

void Parser::constant_buffer_decl(wrShaderVariables & vars ) {
		wrShaderConstantBuffer& newCB = vars.constantBuffers.Add(); SetPos(newCB); 
		Expect(8);
		Expect(1);
		newCB.name = TokenBuf(); 
		if (la->kind == 29) {
			shader_stage_bind_flags(newCB );
		}
		if (la->kind == 27) {
			constant_buffer_register(newCB);
		}
		Expect(33);
		constant_buffer_element(newCB);
		while (la->kind == 1) {
			constant_buffer_element(newCB);
		}
		Expect(34);
		Expect(26);
}

void Parser::sampler_state_decl(wrShaderVariables & vars ) {
		wrShaderSamplerState& newSS = vars.samplers.Add(); SetPos(newSS); 
		if (la->kind == 51) {
			Get();
			Expect(1);
			newSS.name = TokenBuf(); 
		} else if (la->kind == 52) {
			Get();
			Expect(1);
			newSS.name = TokenBuf(); newSS.bIsSamplerCmpState = true; 
		} else SynErr(117);
		if (la->kind == 29) {
			shader_stage_bind_flags(newSS );
		}
		if (la->kind == 27) {
			Get();
			Expect(15);
			Expect(29);
			Expect(3);
			newSS.iRegister = TokenToU32(0,MAX_SAMPLER_STATE_SLOTS-1); 
			Expect(30);
		}
		if (la->kind == 96) {
			Get();
			Expect(1);
			newSS.initializer = TokenBuf(); 
		}
		Expect(26);
}

void Parser::shader_resource_decl(wrShaderVariables & vars ) {
		wrShaderResource& newRes = vars.resources.Add(); SetPos(newRes); 
		Expect(102);
		if (la->kind == 99) {
			Get();
			type_name();
			Expect(100);
		}
		Expect(1);
		newRes.name = TokenBuf(); 
		if (la->kind == 29) {
			shader_stage_bind_flags(newRes );
		}
		if (la->kind == 27) {
			Get();
			Expect(15);
			Expect(29);
			Expect(3);
			newRes.iRegister = TokenToU32(0,MAX_SHADER_RESOURCE_SLOTS-1); 
			Expect(30);
		}
		if (la->kind == 96) {
			Get();
			Expect(1);
			newRes.initializer = TokenBuf(); 
		}
		Expect(26);
}

void Parser::using_shared_sections_decl(SharedSectionsList& sharedSections ) {
		Expect(1);
		sharedSections.Add(TokenBuf()); 
}

void Parser::shader_stage_bind_flags(wrRegisterBound & o ) {
		Expect(29);
		o.bindFlags = 0; 
		if (la->kind == 36) {
			Get();
			o.bindFlags |= CB_Bind_HS; 
		}
		if (la->kind == 37) {
			Get();
			o.bindFlags |= CB_Bind_DS; 
		}
		if (la->kind == 38) {
			Get();
			o.bindFlags |= CB_Bind_TS; 
		}
		if (la->kind == 39) {
			Get();
			o.bindFlags |= CB_Bind_VS; 
		}
		if (la->kind == 40) {
			Get();
			o.bindFlags |= CB_Bind_GS; 
		}
		if (la->kind == 41) {
			Get();
			o.bindFlags |= CB_Bind_PS; 
		}
		Expect(30);
		if( o.bindFlags == 0 ) Error("Object is not bound to any shader stage"); 
}

void Parser::constant_buffer_register(wrShaderConstantBuffer & cb ) {
		Expect(27);
		Expect(15);
		Expect(29);
		Expect(3);
		cb.iRegister = TokenToU32(0,MAX_CONSTANT_BUFFER_SLOTS-1); 
		Expect(30);
}

void Parser::constant_buffer_element(wrShaderConstantBuffer & cb ) {
		wrCBVar& newCBElem = cb.elements.Add(); SetPos(newCBElem); 
		type_name();
		newCBElem.typeName = TokenBuf(); 
		Expect(1);
		newCBElem.name = TokenBuf(); 
		if (la->kind == 31) {
			Get();
			Expect(1);
			Assert(newCBElem.arrayDim.IsEmpty()); newCBElem.arrayDim = TokenBuf(); 
			Expect(32);
		}
		Expect(26);
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
		Expect(33);
		Expect(104);
		Expect(1);
		newVtxElem.SemanticName = TokenBuf(); 
		Expect(105);
		Expect(3);
		newVtxElem.SemanticIndex = TokenToU32(0,32); 
		Expect(47);
		Expect(1);
		newVtxElem.Format = TokenBuf(); 
		if (la->kind == 106) {
			Get();
			Expect(3);
			newVtxElem.InputSlot = TokenToU32(0,32); 
		}
		Expect(34);
}

void Parser::shared_variables_declaration(wrSharedSection & sharedSection ) {
		Expect(108);
		Expect(33);
		shader_variables_list_decl(sharedSection.vars );
		Expect(34);
}

void Parser::shared_code_declaration(wrSharedSection & sharedSection ) {
		Expect(95);
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
	maxT = 109;

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

	static bool set[3][111] = {
		{T,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x},
		{x,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,x},
		{x,x,x,x, x,x,x,x, T,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,T, T,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,T,x, x,x,x,x, x,x,x}
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
			case 15: s = coco_string_create(L"register expected"); break;
			case 16: s = coco_string_create(L"short expected"); break;
			case 17: s = coco_string_create(L"signed expected"); break;
			case 18: s = coco_string_create(L"static expected"); break;
			case 19: s = coco_string_create(L"struct expected"); break;
			case 20: s = coco_string_create(L"typedef expected"); break;
			case 21: s = coco_string_create(L"union expected"); break;
			case 22: s = coco_string_create(L"unsigned expected"); break;
			case 23: s = coco_string_create(L"void expected"); break;
			case 24: s = coco_string_create(L"volatile expected"); break;
			case 25: s = coco_string_create(L"comma expected"); break;
			case 26: s = coco_string_create(L"semicolon expected"); break;
			case 27: s = coco_string_create(L"colon expected"); break;
			case 28: s = coco_string_create(L"star expected"); break;
			case 29: s = coco_string_create(L"lpar expected"); break;
			case 30: s = coco_string_create(L"rpar expected"); break;
			case 31: s = coco_string_create(L"lbrack expected"); break;
			case 32: s = coco_string_create(L"rbrack expected"); break;
			case 33: s = coco_string_create(L"lbrace expected"); break;
			case 34: s = coco_string_create(L"rbrace expected"); break;
			case 35: s = coco_string_create(L"ellipsis expected"); break;
			case 36: s = coco_string_create(L"BIND_HS expected"); break;
			case 37: s = coco_string_create(L"BIND_DS expected"); break;
			case 38: s = coco_string_create(L"BIND_TS expected"); break;
			case 39: s = coco_string_create(L"BIND_VS expected"); break;
			case 40: s = coco_string_create(L"BIND_GS expected"); break;
			case 41: s = coco_string_create(L"BIND_PS expected"); break;
			case 42: s = coco_string_create(L"\"RenderTarget\" expected"); break;
			case 43: s = coco_string_create(L"\"SizeX\" expected"); break;
			case 44: s = coco_string_create(L"\"ScaleX\" expected"); break;
			case 45: s = coco_string_create(L"\"SizeY\" expected"); break;
			case 46: s = coco_string_create(L"\"ScaleY\" expected"); break;
			case 47: s = coco_string_create(L"\"Format\" expected"); break;
			case 48: s = coco_string_create(L"\"Info\" expected"); break;
			case 49: s = coco_string_create(L"\"MultiRenderTarget\" expected"); break;
			case 50: s = coco_string_create(L"\"ClearColor\" expected"); break;
			case 51: s = coco_string_create(L"\"SamplerState\" expected"); break;
			case 52: s = coco_string_create(L"\"SamplerComparisonState\" expected"); break;
			case 53: s = coco_string_create(L"\"Filter\" expected"); break;
			case 54: s = coco_string_create(L"\"AddressU\" expected"); break;
			case 55: s = coco_string_create(L"\"AddressV\" expected"); break;
			case 56: s = coco_string_create(L"\"AddressW\" expected"); break;
			case 57: s = coco_string_create(L"\"MipLODBias\" expected"); break;
			case 58: s = coco_string_create(L"\"MaxAnisotropy\" expected"); break;
			case 59: s = coco_string_create(L"\"ComparisonFunc\" expected"); break;
			case 60: s = coco_string_create(L"\"BorderColor\" expected"); break;
			case 61: s = coco_string_create(L"\"MinLOD\" expected"); break;
			case 62: s = coco_string_create(L"\"MaxLOD\" expected"); break;
			case 63: s = coco_string_create(L"\"DepthStencilState\" expected"); break;
			case 64: s = coco_string_create(L"\"DepthEnable\" expected"); break;
			case 65: s = coco_string_create(L"\"DepthWriteMask\" expected"); break;
			case 66: s = coco_string_create(L"\"DepthFunc\" expected"); break;
			case 67: s = coco_string_create(L"\"StencilEnable\" expected"); break;
			case 68: s = coco_string_create(L"\"RasterizerState\" expected"); break;
			case 69: s = coco_string_create(L"\"FillMode\" expected"); break;
			case 70: s = coco_string_create(L"\"CullMode\" expected"); break;
			case 71: s = coco_string_create(L"\"FrontCounterClockwise\" expected"); break;
			case 72: s = coco_string_create(L"\"DepthBias\" expected"); break;
			case 73: s = coco_string_create(L"\"DepthBiasClamp\" expected"); break;
			case 74: s = coco_string_create(L"\"SlopeScaledDepthBias\" expected"); break;
			case 75: s = coco_string_create(L"\"DepthClipEnable\" expected"); break;
			case 76: s = coco_string_create(L"\"ScissorEnable\" expected"); break;
			case 77: s = coco_string_create(L"\"MultisampleEnable\" expected"); break;
			case 78: s = coco_string_create(L"\"AntialiasedLineEnable\" expected"); break;
			case 79: s = coco_string_create(L"\"BlendState\" expected"); break;
			case 80: s = coco_string_create(L"\"BlendEnable\" expected"); break;
			case 81: s = coco_string_create(L"\"AlphaToCoverageEnable\" expected"); break;
			case 82: s = coco_string_create(L"\"SrcBlend\" expected"); break;
			case 83: s = coco_string_create(L"\"DestBlend\" expected"); break;
			case 84: s = coco_string_create(L"\"BlendOp\" expected"); break;
			case 85: s = coco_string_create(L"\"SrcBlendAlpha\" expected"); break;
			case 86: s = coco_string_create(L"\"DestBlendAlpha\" expected"); break;
			case 87: s = coco_string_create(L"\"BlendOpAlpha\" expected"); break;
			case 88: s = coco_string_create(L"\"RenderTargetWriteMask\" expected"); break;
			case 89: s = coco_string_create(L"\"StateBlock\" expected"); break;
			case 90: s = coco_string_create(L"\"StencilRef\" expected"); break;
			case 91: s = coco_string_create(L"\"BlendFactor\" expected"); break;
			case 92: s = coco_string_create(L"\"SampleMask\" expected"); break;
			case 93: s = coco_string_create(L"\"RGBA(\" expected"); break;
			case 94: s = coco_string_create(L"\"Shader\" expected"); break;
			case 95: s = coco_string_create(L"\"Code\" expected"); break;
			case 96: s = coco_string_create(L"\"=\" expected"); break;
			case 97: s = coco_string_create(L"\"VertexShader\" expected"); break;
			case 98: s = coco_string_create(L"\"PixelShader\" expected"); break;
			case 99: s = coco_string_create(L"\"<\" expected"); break;
			case 100: s = coco_string_create(L"\">\" expected"); break;
			case 101: s = coco_string_create(L"\"Inputs\" expected"); break;
			case 102: s = coco_string_create(L"\"Texture2D\" expected"); break;
			case 103: s = coco_string_create(L"\"Vertex\" expected"); break;
			case 104: s = coco_string_create(L"\"SemanticName\" expected"); break;
			case 105: s = coco_string_create(L"\"SemanticIndex\" expected"); break;
			case 106: s = coco_string_create(L"\"InputSlot\" expected"); break;
			case 107: s = coco_string_create(L"\"Shared\" expected"); break;
			case 108: s = coco_string_create(L"\"Variables\" expected"); break;
			case 109: s = coco_string_create(L"??? expected"); break;
			case 110: s = coco_string_create(L"invalid declaration"); break;
			case 111: s = coco_string_create(L"invalid render_target_declaration"); break;
			case 112: s = coco_string_create(L"invalid render_target_declaration"); break;
			case 113: s = coco_string_create(L"invalid sampler_state_declaration"); break;
			case 114: s = coco_string_create(L"invalid boolean_constant"); break;
			case 115: s = coco_string_create(L"invalid bit_mask"); break;
			case 116: s = coco_string_create(L"invalid shader_variable_decl"); break;
			case 117: s = coco_string_create(L"invalid sampler_state_decl"); break;

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



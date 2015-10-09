/*
=============================================================================
	File:	D3D11Shaders.cpp
	Desc:	
=============================================================================
*/
#include "Graphics_PCH.h"
#pragma hdrstop
#include "Graphics_DX11.h"

//CRC32_BlockChecksum
#include <Base/Math/Hashing/CRC32.h>

//for mxSessionInfo
#include <Core/Core.h>

#include "DX11Private.h"

//-------------------------------------------------------------------------------------------------------------//

static const char* D3DBlobToChars( const dxPtr< ID3DBlob >& msgBlob )
{
	return (const char*) msgBlob->GetBufferPointer();
}

// Stripping comments from Shader bytecodes
// See: http://www.devmaster.net/forums/showthread.php?t=13595
// See: http://entland.homelinux.com/blog/2009/01/15/stripping-comments-from-shader-bytecodes/
// NOTE: it didn't worked in D3D11 (on the opposite, file size went up slightly)
//
static ID3DBlob* CreateBlobWithoutComments( ID3DBlob* code )
{
	// Calculates the new size (without comments)
	const int* codeData = static_cast<int*>(code->GetBufferPointer());
	const unsigned int sizeInWords = code->GetBufferSize() / 4;

	unsigned int strippedSizeInWords = sizeInWords;

	for (unsigned int i = 0; i < sizeInWords; i++)
	{
		if ((codeData[i] & 0xffff) == D3DSIO_COMMENT)
		{
			const int commentSize = codeData[i] >> 16;
			strippedSizeInWords -= 1 + commentSize;
			i += commentSize;
		}
	}

	// Creates a new buffer with the original code but omitting the comments
	ID3DBlob* strippedCode;
	dxchk(D3DCreateBlob( strippedSizeInWords * 4, &strippedCode ));

	int* strippedCodeData = static_cast<int*>(strippedCode->GetBufferPointer());
	size_t offset = 0;

	for (unsigned int i = 0; i < sizeInWords; i++)
	{
		if ((codeData[i] & 0xffff) == D3DSIO_COMMENT)
		{
			int commentSize = codeData[i] >> 16;
			i += commentSize;
		}
		else
		{
			strippedCodeData[offset++] = codeData[i];
		}
	}

	return strippedCode;
}


static void DumpShaderSource( const char* sourceCode )
{
	::OutputDebugStringA("\n=== Dumping shader source code ===\n");
	::OutputDebugStringA(sourceCode);
	::OutputDebugStringA("\n=== End of shader source code ===\n");
}

static bool DumpShaderSourceToFile( const char* message, const char* sourceCode, const char* fileName = nil )
{
	StackString		generatedFileName;
	if( fileName == nil )
	{
		// we need to generate unique file name
		const FileTime timeStamp(FileTime::CurrentTime());
		generatedFileName.Format("ShaderDump_%u[",timeStamp.time.dwLowDateTime);

		GetCurrentDateTimeString( generatedFileName );
		generatedFileName += "].txt";
		fileName = generatedFileName.ToChars();
	}

	FileWriter	file( fileName, FileWrite_NoErrors );
	if( file.IsOpen() )
	{
		dxMsgf("Dumping shader source code to file '%s'.\n", fileName );

		char	buffer[ MAX_STRING_CHARS ];
		MX_SPRINTF_ANSI(buffer,"// %s\n",message);
		file.Write( buffer, mxStrLenAnsi(buffer) );

		file.Write( sourceCode, mxStrLenAnsi(sourceCode) );
		return true;
	}
	else
	{
		dxMsgf("Failed to save shader source code to file '%s'.\n", fileName );
	}
	return false;
}
//-------------------------------------------------------------------------------------------------------------//

static void OnCompilationSuccess( TPtr< ID3DBlob >& code )
{
	(void)code;
}

static void OnCompilationFailure( const char* fileName, HRESULT hr, const char* profile,
								 const char* entryPoint, MessageListener& messageListener )
{
	Assert(FAILED(hr));
	AssertPtr(profile);

	// can be null for some shaders (e.g. shadow depth shader)
	//AssertPtr(entryPoint);
	if( nil == entryPoint ) {
		entryPoint = "<null>";
	}

	StackString	fileNameBuffer;
	if( fileName != nil )
	{
		fileNameBuffer.Format(" '%s'",fileName);
		fileName = fileNameBuffer.ToChars();
	}
	else
	{
		fileName = "";
	}

	char	msg[ MAX_STRING_CHARS ];
	MX_SPRINTF_ANSI( msg,
		"Failed to compile shader '%s'. Profile: '%s', entry point: '%s'.\n"
		"Reason: '%s'.\n",
		fileName, profile, entryPoint,
		D3D_GetErrorCodeString(hr)
	);
	messageListener.Put( msg );

#if MX_DEVELOPER && MX_EDITOR
	mxBeep(500);
	//MX_DEBUG_BREAK;
#endif // MX_DEVELOPER && MX_EDITOR
}
//-------------------------------------------------------------------------------------------------------------//
static UINT Get_HLSL_Compilation_Flags()
{
	UINT hlslCompileFlags = 0;

	hlslCompileFlags |= D3DCOMPILE_ENABLE_STRICTNESS;

	// Specifying this flag enables strictness which may not allow for legacy syntax.
	hlslCompileFlags |= D3DCOMPILE_IEEE_STRICTNESS;

	hlslCompileFlags |= D3DCOMPILE_WARNINGS_ARE_ERRORS;

	if( RX_DEBUG_SHADERS )
	{
mxFIXME("commented out because bytecode contains source text");
		//hlslCompileFlags |= D3DCOMPILE_DEBUG;	// Insert debug file/line/type/symbol information.
		//hlslCompileFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;	// Skip optimization during code generation; generally recommended for debug only.
	}
	else
	{
		// Lowest optimization level. May produce slower code but will do so more quickly.
		// This may be useful in a highly iterative shader development cycle.
		//hlslCompileFlags |= D3DCOMPILE_OPTIMIZATION_LEVEL0;

		// Second lowest optimization level.
		//hlslCompileFlags |= D3DCOMPILE_OPTIMIZATION_LEVEL1;

		// Second highest optimization level.
		//hlslCompileFlags |= D3DCOMPILE_OPTIMIZATION_LEVEL2;

		// Highest optimization level. Will produce best possible code but may take significantly longer to do so.
		// This will be useful for final builds of an application where performance is the most important factor.
		hlslCompileFlags |= D3DCOMPILE_OPTIMIZATION_LEVEL3;
	}

	if( !RX_DEBUG_RENDERER && !RX_DEBUG_SHADERS ) {
		// Do not validate the generated code against known capabilities and constraints.
		// Only use this with shaders that have been successfully compiled in the past.
		// Shaders are always validated by DirectX before they are set to the device.
		hlslCompileFlags |= D3DCOMPILE_SKIP_VALIDATION;
	}


	// D3DCOMPILE_AVOID_FLOW_CONTROL	Tell compiler to not allow flow-control (when possible).
	hlslCompileFlags |= D3DCOMPILE_AVOID_FLOW_CONTROL;

	// D3DCOMPILE_PARTIAL_PRECISION	Force all computations to be done with partial precision; this may run faster on some hardware.
	// D3DCOMPILE_PREFER_FLOW_CONTROL	Tell compiler to use flow-control (when possible).


	const bool bShaderMatricesRowMajor = true;
	if( bShaderMatricesRowMajor )
	{
		hlslCompileFlags |= D3DCOMPILE_PACK_MATRIX_ROW_MAJOR;
	}

	return hlslCompileFlags;
}
//-------------------------------------------------------------------------------------------------------------//
ID3DBlob* D3D_CompileShader(
	const char* sourceCode,
	SizeT sourceCodeLength,
	const char* profile,
	const char* entryPoint,
	const D3D_SHADER_MACRO* defines,
	ID3DInclude* pInclude,
	MessageListener& messageListener
)
{
	AssertPtr(sourceCode);
	Assert_GZ(sourceCodeLength);

	TPtr< ID3DBlob >	byteCode;
	dxPtr< ID3DBlob >	errorMessages;

	UINT hlslCompileFlags = Get_HLSL_Compilation_Flags();

#if 1
	const HRESULT hr = ::D3DX11CompileFromMemory(
		sourceCode,				// buffer contains shader source code
		sourceCodeLength,		// length of shader source code
		nil,					// Optional. The name of the shader file. Use either this or pSrcData.
		defines,				// Optional. An array of nil-terminated macro definitions (see D3D_SHADER_MACRO).
		pInclude,				// Optional. A pointer to an ID3D10Include for handling include files. Setting this to nil will cause a compile error if a shader contains a #include.
		entryPoint,				// The name of the shader entry point function.
		profile,				// The shader target or set of shader features to compile against.
		hlslCompileFlags,		// Shader compile options
		0,						// Effect compile options.
		nil,					// ID3DX11ThreadPump* pPump
		&byteCode.Ptr,			// The address of a ID3D10Blob that contains the compiled code
		&errorMessages.Ptr,		// Optional. A pointer to an ID3D10Blob that contains compiler error messages, or nil if there were no errors.
		nil						// HRESULT* pHResult
	);
#else
	const HRESULT hr = ::D3DCompile(
		sourceCode,				// buffer contains shader source code
		sourceCodeLength,		// length of shader source code
		nil,					// Optional. The name of the shader file. Use either this or pSrcData.
		defines,				// Optional. An array of nil-terminated macro definitions (see D3D_SHADER_MACRO).
		pInclude,				// Optional. A pointer to an ID3D10Include for handling include files. Setting this to nil will cause a compile error if a shader contains a #include.
		entryPoint,				// The name of the shader entry point function.
		profile,				// The shader target or set of shader features to compile against.
		hlslCompileFlags,		// Shader compile options
		0,						// Effect compile options.
		&byteCode.Ptr,			// The address of a ID3D10Blob that contains the compiled code
		&errorMessages.Ptr		// Optional. A pointer to an ID3D10Blob that contains compiler error messages, or nil if there were no errors.
	);
#endif
	if( errorMessages != nil )
	{
		messageListener.Put( D3DBlobToChars(errorMessages) );
		DumpShaderSourceToFile( D3DBlobToChars(errorMessages), sourceCode );
	}

	if( FAILED( hr ) )
	{
		OnCompilationFailure( nil, hr, profile, entryPoint, messageListener );
		return nil;
	}

	OnCompilationSuccess(byteCode);

	RX_ASSERT(byteCode);
	return byteCode;
}
//-------------------------------------------------------------------------------------------------------------//
ID3DBlob* D3D_CompileShaderFromFile(
	const char* fileName,
	const char* profile,
	const char* entryPoint,
	const D3D_SHADER_MACRO* defines,
	ID3DInclude* pInclude,
	MessageListener& messageListener
)
{
	AssertPtr(fileName);

	TPtr< ID3DBlob >	byteCode;
	dxPtr< ID3DBlob >	errorMessages;

	UINT hlslCompileFlags = Get_HLSL_Compilation_Flags();


	const HRESULT hr = ::D3DX11CompileFromFileA(
		fileName,				// LPCSTR pSrcFile
		defines,				// Optional. An array of nil-terminated macro definitions (see D3D_SHADER_MACRO).
		pInclude,				// Optional. A pointer to an ID3D10Include for handling include files. Setting this to nil will cause a compile error if a shader contains a #include.
		entryPoint,				// The name of the shader entry point function.
		profile,				// The shader target or set of shader features to compile against.
		hlslCompileFlags,		// Shader compile options
		0,						// Effect compile options.
		nil,					// ID3DX11ThreadPump* pPump
		&byteCode.Ptr,			// The address of a ID3D10Blob that contains the compiled code
		&errorMessages.Ptr,		// Optional. A pointer to an ID3D10Blob that contains compiler error messages, or nil if there were no errors.
		nil						// HRESULT* pHResult
	);

	if( errorMessages != nil )
	{
		messageListener.Put( D3DBlobToChars(errorMessages) );
	}

	if( FAILED( hr ) )
	{
		OnCompilationFailure( fileName, hr, profile, entryPoint, messageListener );
		return nil;
	}

	OnCompilationSuccess(byteCode);

	RX_ASSERT(byteCode);
	return byteCode;
}

//-------------------------------------------------------------------------------------------------------------//

// Takes a binary shader and returns a buffer containing text assembly.
//
ID3DBlob* D3D_DisassembleShader(
	const void* compiledByteCode,
	SizeT byteCodeLength,
	bool bHTML,
	const char* comments
)
{
	AssertPtr(compiledByteCode);
	Assert_GZ(byteCodeLength);

	TPtr< ID3DBlob >	disassembly;

	UINT disasmFlags = 0;

	if( bHTML )
	{
		disasmFlags |= D3D_DISASM_ENABLE_COLOR_CODE;	// Enable the output of color codes.
		disasmFlags |= D3D_DISASM_ENABLE_DEFAULT_VALUE_PRINTS;	// Enable the output of default values.
		disasmFlags |= D3D_DISASM_ENABLE_INSTRUCTION_NUMBERING;	// Enable instruction numbering.
	}
//	disasmFlags |= D3D_DISASM_ENABLE_INSTRUCTION_CYCLE;	// No effect. NOTE: the function fails with this flag enabled!
	if( !RX_DEBUG_SHADERS )
	{
		disasmFlags |= D3D_DISASM_DISABLE_DEBUG_INFO;
	}

	const HRESULT hr = ::D3DDisassemble(
		compiledByteCode,		// A pointer to source data as compiled HLSL code.
		byteCodeLength,			// length of compiled HLSL code
		disasmFlags,			// Flags affecting the behavior of D3DDisassemble.
		comments,				// The optional comment string at the top of the shader that identifies the shader constants and variables.
		&disassembly.Ptr		// A pointer to a buffer that receives the ID3D10Blob interface that accesses assembly text.
	);
	if( FAILED( hr ) )
	{
		dxErrf( hr, "Failed to disassemble shader." );
		return nil;
	}
	RX_ASSERT(disassembly);
	return disassembly;
}
//-------------------------------------------------------------------------------------------------------------//
ID3D11VertexShader* D3D_CreateVertexShader(
	const void* compiledBytecode,
	SizeT bytecodeLength,
	ID3D11ClassLinkage* linkage
	)
{
	AssertPtr(compiledBytecode);
	Assert_GZ(bytecodeLength);

	if( !compiledBytecode || !bytecodeLength ) {
		return nil;
	}
	ID3D11VertexShader *	vertexShader = nil;

	const HRESULT hr = D3DDevice->CreateVertexShader(
		compiledBytecode,
		bytecodeLength,
		linkage,
		&vertexShader
	);
	if( FAILED( hr ) )
	{
		dxErrf( hr, "Failed to create vertex shader from compiled bytecode.\n" );
		return nil;
	}

	return vertexShader;
}
//-------------------------------------------------------------------------------------------------------------//
ID3D11GeometryShader* D3D_CreateGeometryShader(
	const void* compiledBytecode,
	SizeT bytecodeLength,
	ID3D11ClassLinkage* linkage
	)
{
	AssertPtr(compiledBytecode);
	Assert_GZ(bytecodeLength);

	if( !compiledBytecode || !bytecodeLength ) {
		return nil;
	}
	ID3D11GeometryShader *	geometryShader = nil;

	const HRESULT hr = D3DDevice->CreateGeometryShader(
		compiledBytecode,
		bytecodeLength,
		linkage,
		&geometryShader
	);
	if( FAILED( hr ) )
	{
		dxErrf( hr, "Failed to create geometry shader from compiled bytecode.\n" );
		return nil;
	}

	return geometryShader;
}
//-------------------------------------------------------------------------------------------------------------//
ID3D11PixelShader* D3D_CreatePixelShader(
	const void* compiledBytecode,
	SizeT bytecodeLength,
	ID3D11ClassLinkage* linkage
	)
{
	AssertPtr(compiledBytecode);
	Assert_GZ(bytecodeLength);

	if( !compiledBytecode || !bytecodeLength ) {
		return nil;
	}
	ID3D11PixelShader *	pixelShader = nil;

	const HRESULT hr = D3DDevice->CreatePixelShader(
		compiledBytecode,
		bytecodeLength,
		linkage,
		&pixelShader
	);
	if( FAILED( hr ) )
	{
		dxErrf( hr, "Failed to create pixel shader from compiled bytecode.\n" );
		return nil;
	}

	return pixelShader;
}
//-------------------------------------------------------------------------------------------------------------//
ID3D11Buffer* D3D_CreateConstantBuffer(
	UINT size,
	const void* data
){
	// this buffer must be updated with UpdateSubresource()
	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.BindFlags	= D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.Usage		= D3D11_USAGE_DEFAULT;	// must be D3D11_USAGE_DEFAULT if UpdateSubresource() will be used
	bufferDesc.ByteWidth	= ALIGN16( size );
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags	= 0;
	bufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = data;
	initData.SysMemPitch = 0;
	initData.SysMemSlicePitch = 0;

	ID3D11Buffer *	newBuffer = nil;
	dxchk(D3DDevice->CreateBuffer(
		&bufferDesc,
		data ? &initData : nil,
		&newBuffer
	));

	return newBuffer;
}
//-------------------------------------------------------------------------------------------------------------//
ID3D11Buffer* D3D_CreateMappableConstantBuffer(
	UINT size,
	const void* data
){
	// this buffer must be updated with Map()/Unmap()
	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.BindFlags	= D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.Usage		= D3D11_USAGE_DYNAMIC;
	bufferDesc.ByteWidth	= ALIGN16( size );
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.MiscFlags	= 0;
	bufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = data;
	initData.SysMemPitch = 0;
	initData.SysMemSlicePitch = 0;

	ID3D11Buffer *	newBuffer = nil;
	dxchk(D3DDevice->CreateBuffer(
		&bufferDesc,
		data ? &initData : nil,
		&newBuffer
	));

	return newBuffer;
}

/*================================
		FileIncludeProxy
================================*/

FileIncludeProxy::FileIncludeProxy( FileIncludeInterface* fileInclude )
	: mFileInclude( fileInclude )
{
}
//-------------------------------------------------------------------------------------------------------------//
FileIncludeProxy::~FileIncludeProxy()
{
}
//-------------------------------------------------------------------------------------------------------------//
HRESULT FileIncludeProxy::Open(
	THIS_ D3D_INCLUDE_TYPE IncludeType,
	LPCSTR pFileName,
	LPCVOID pParentData,
	LPCVOID *ppData,
	UINT *pBytes )
{
	const bool bOk = mFileInclude->Open( pFileName, ppData, pBytes );

	return bOk ? S_OK : S_FALSE;
}
//-------------------------------------------------------------------------------------------------------------//
HRESULT FileIncludeProxy::Close( THIS_ LPCVOID pData )
{
	mFileInclude->Close( pData );

	return S_OK;
}
//-------------------------------------------------------------------------------------------------------------//


/*================================
		FileIncludeManager
================================*/

FileIncludeManager::FileIncludeManager()
{
}
//-------------------------------------------------------------------------------------------------------------//
FileIncludeManager::~FileIncludeManager()
{
}
//-------------------------------------------------------------------------------------------------------------//
void FileIncludeManager::Add( FileIncludeInterface* p )
{
	mFileIncludes.Add(p);
}
//-------------------------------------------------------------------------------------------------------------//
HRESULT FileIncludeManager::Open(
	THIS_ D3D_INCLUDE_TYPE IncludeType,
	LPCSTR pFileName,
	LPCVOID pParentData,
	LPCVOID *ppData,
	UINT *pBytes )
{
	for( UINT i = 0; i < mFileIncludes.Num(); i++ )
	{
		FileIncludeInterface* p = mFileIncludes[ i ];

		if( p->Open( pFileName, ppData, pBytes ) ) {
			return S_OK;
		}
	}

	dxErrf("Failed to open shader source file: '%s'.\n",pFileName);

	return S_FALSE;
}
//-------------------------------------------------------------------------------------------------------------//
HRESULT FileIncludeManager::Close( THIS_ LPCVOID pData )
{
	for( UINT i = 0; i < mFileIncludes.Num(); i++ )
	{
		FileIncludeInterface* p = mFileIncludes[ i ];

		if( p->Close( pData ) ) {
			return S_OK;
		}
	}

	return S_FALSE;
}
//-------------------------------------------------------------------------------------------------------------//

/*================================
		CompilationLog
================================*/

CompilationLog::CompilationLog()
{
}
//-------------------------------------------------------------------------------------------------------------//
void CompilationLog::Attach( MessageListener* listener )
{
	this->listeners.AddUnique(listener);
}
//-------------------------------------------------------------------------------------------------------------//
void CompilationLog::Detach( MessageListener* listener )
{
	this->listeners.Remove(listener);
}
//-------------------------------------------------------------------------------------------------------------//
void CompilationLog::Put( const char* message )
{
	MessageListener::defaultInstance.Put(message);

	for( UINT i=0; i < listeners.Num(); i++ )
	{
		this->listeners[i]->Put(message);

		//include null terminator
		this->log.Add(message,mxStrLenAnsi(message)+1);
	}
}

/*================================
		LoggingMessageListener
================================*/

LoggingMessageListener::LoggingMessageListener( CompilationLog& theLog )
	: log(theLog)
{
	this->log.Attach(this);
}

LoggingMessageListener::~LoggingMessageListener()
{
	this->log.Detach(this);
}

void LoggingMessageListener::Put( const char* message )
{
	CompileMessage & newMsg = this->messages.Add();

	ParseLineNumberInfo(message,newMsg.line,newMsg.column);
	newMsg.text.SetString(message);
}

//-------------------------------------------------------------------------------------------------------------//

const char* FindPattern( const char* data, UINT dataLength, const char* pattern, UINT patternLength )
{
	AssertPtr(data);
	Assert(dataLength > 0);
	AssertPtr(pattern);
	Assert(patternLength > 0);
	Assert(dataLength > patternLength);

	for( UINT i = 0; i < dataLength - patternLength; i++ )
	{
		bool match = true;
		for( UINT k = 0; k < patternLength; k++ )
		{
			if( data[i + k] != pattern[k] )
			{
				match = false;
				break;
			}
		}
		if( match )
		{
			return data + i;
		}
	}
	return nil;
}

void D3D11CompiledShader::Construct( ID3DBlob* byteCode )
{
	AssertPtr(byteCode);

	const UINT codeSize = byteCode->GetBufferSize();
	const char* pSource = c_cast(const char*) byteCode->GetBufferPointer();

	// Search for the string "Microsoft (R) HLSL Shader Compiler"
	// and strip that from compiled shader byte code.

	//static const ConstCharPtr	signature("Microsoft (R) HLSL Shader Compiler");
	//const char* pComment = FindPattern( pSource, codeSize, signature.ToChars(), signature.Length() );

	this->data.SetNum( codeSize );
	MemCopy( this->data.ToPtr(), pSource, codeSize );
}

//-------------------------------------------------------------------------------------------------------------//

static const char* dxUtil_GetGPUVendorFourCC()
{
	switch( D3D_GetDeviceVendor() )
	{
	case EDeviceVendor::Vendor_ATI :	return "ATI";
	case EDeviceVendor::Vendor_NVidia :	return "NV";
	case EDeviceVendor::Vendor_Intel :	return "INTL";
	default:	return "UNKN";
	}
}

// returns name of file where compiled shaders will be stored
static void dxUtil_ComposeShaderCacheFileName( StackString & outFileName, bool includeVendorInfo = false, bool includeDateTimeInfo = false )
{
	outFileName.Empty();
	outFileName += "PC_";
	outFileName += D3D_GetShaderModel();

	if( includeVendorInfo )
	{
		outFileName += "_";
		outFileName += dxUtil_GetGPUVendorFourCC();
	}


	if( includeDateTimeInfo )
	{
		Unimplemented;
		//String	currDate, currTime;
		//GetCurrentDateString( currDate );
		//GetCurrentTimeOfDayString( currTime, '-' );

		//outFileName += '[';
		//outFileName += currDate;
		//outFileName += '_';
		//outFileName += currTime;
		//outFileName += ']';
	}

#define RX_SHADER_CACHE_FILE_EXTENSION	".mxs"

	outFileName += RX_SHADER_CACHE_FILE_EXTENSION;

#undef RX_SHADER_CACHE_FILE_EXTENSION
}

static void ComposeFullPathToShaderCache( PCSTR pathName, StackString &fullPath )
{
	fullPath.SetString( pathName );
	F_NormalizePath( fullPath );

	StackString	shaderCacheFileName;
	dxUtil_ComposeShaderCacheFileName( shaderCacheFileName );

	fullPath.Append( shaderCacheFileName );
}

/*================================
		ShaderCacheHeader
================================*/

// used mainly for serialization
struct ShaderCacheHeader
{
#pragma pack (push,1)

	mxSessionInfo	m_session;

	FileTime	m_timeStamp;
	U4			m_checksum;
	U4			m_numShaders;
	U4			m_flags;
	U4			m_bD3D11;

#pragma pack (pop)

public:
	ShaderCacheHeader()
	{
		m_session.fourCC = MAGIC_NUM;

		m_timeStamp = FileTime::CurrentTime();
		m_checksum = 0;
		m_numShaders = 0;
		m_flags = 0;
		m_bD3D11 = RX_HW_D3D11;
	}

	static const U4 MAGIC_NUM = MAKEFOURCC('S','H','I','T');
};
mxDECLARE_POD_TYPE(ShaderCacheHeader);

/*================================
		D3D11ShaderCache
================================*/

D3D11ShaderCache::D3D11ShaderCache()
{
}
//-------------------------------------------------------------------------------------------------------------//
D3D11ShaderCache::~D3D11ShaderCache()
{
}
//-------------------------------------------------------------------------------------------------------------//
void D3D11ShaderCache::RegisterNewShader( GrShaderData * shaderInfo )
{
	AssertPtr(shaderInfo);
	//Assert(shaderInfo->internalIndex == INDEX_NONE);

	const UINT internalIndex = m_shadersList.Num();
	(void)internalIndex;

	m_shadersList.Add(shaderInfo);

#if MX_EDITOR
	this->Add(new EdShaderDataProxy(*shaderInfo,internalIndex));
#endif // MX_EDITOR
}
//-------------------------------------------------------------------------------------------------------------//
void D3D11ShaderCache::Save( AStreamWriter& stream )
{
	DBGOUT("Saved '%u' shaders.\n",m_shadersList.Num());

	ShaderCacheHeader	header;

	header.m_timeStamp = FileTime::CurrentTime();
	header.m_checksum = 0;
	header.m_numShaders = m_shadersList.Num();
	header.m_flags = 0;

	stream << header;

	stream << m_compiledCodeMap;
	stream << m_createdShaders;

	// number of registered shaders
	const UINT numShaders = m_shadersList.Num();

	for(UINT iShader = 0;
		iShader < numShaders;
		iShader++)
	{
		const GrShaderData* shaderData = m_shadersList[ iShader ];

		const UINT numShaderInstances = shaderData->numInstances;

		for(UINT iShaderInstance = 0;
			iShaderInstance < numShaderInstances;
			iShaderInstance++)
		{
			const ShaderInstanceData& instanceData = shaderData->instancesData[ iShaderInstance ];

			stream << instanceData;

			//DBGOUT("--- saving VS %u, PS %u\n",instanceData.VS_CodeIndex,instanceData.PS_CodeIndex);
		}
	}
}
//-------------------------------------------------------------------------------------------------------------//
bool D3D11ShaderCache::Load( AStreamReader& stream, const FileTime& srcTimeStamp, rxShaderCompiler& compiler )
{
	const U8 startTime = mxGetTimeInMicroseconds();

	ShaderCacheHeader	header;
	stream >> header;

	mxSessionInfo	thisSession;
	thisSession.fourCC = ShaderCacheHeader::MAGIC_NUM;

	if( !mxSessionInfo::AreCompatible(header.m_session, thisSession) )
	{
		dxMsgf("Invalid shader cache header.\n");
		return false;
	}

	if( m_shadersList.Num() != header.m_numShaders )
	{
		dxMsgf("Shader cache is not up-to-date\n");
		return false;
	}

	char	szCacheTimeStamp[128];	// lastTimeCompiled
	FS_Win32_FileTimeStampToDateTimeString( header.m_timeStamp.time, szCacheTimeStamp, ARRAY_SIZE(szCacheTimeStamp));

	if( header.m_timeStamp < srcTimeStamp )
	{
		char	szSourceTimeStamp[128];	// timeLastModified
		FS_Win32_FileTimeStampToDateTimeString( srcTimeStamp.time, szSourceTimeStamp, ARRAY_SIZE(szSourceTimeStamp));

		dxMsgf("Shader cache is out of date (source: %s, cache: %s).\n", szSourceTimeStamp, szCacheTimeStamp);
		return false;
	}

	dxMsgf( "Loading shader cache (last modified: %s).\n", szCacheTimeStamp );


	stream >> m_compiledCodeMap;
	stream >> m_createdShaders;

	// number of registered shaders
	const UINT numShaders = m_shadersList.Num();

	// number of D3D shader objects
	const UINT numShaderObjects = m_createdShaders.Num();

	if( header.m_numShaders != numShaders
		|| m_createdShaders.Num() != numShaderObjects )
	{
		dxMsgf("Shader cache must be rebuilt\n");
		return false;
	}

	// stats
	TStaticArray_InitZeroed< UINT, NumShaderTypes > shaderCounts;

	for(UINT iShaderObject = 0;
		iShaderObject < numShaderObjects;
		iShaderObject++)
	{
		D3D11CreatedShader& shaderInstance = m_createdShaders[ iShaderObject ];

		const D3D11CompiledShader& code = m_compiledCodeMap.GetPairs()[ shaderInstance.iCodeIndex ].key;
		const EShaderType shaderType = c_cast(EShaderType) shaderInstance.eHWShaderType;
		shaderInstance.pD3DShader = this->CreateShader( compiler, code, shaderType );

		shaderCounts[shaderType]++;
	}

	// Load shader instances and initialize shaders.

	for(UINT iShader = 0;
		iShader < numShaders;
		iShader++)
	{
		GrShaderData* shaderData = m_shadersList[ iShader ];

		AssertPtr(shaderData);

		DBGOUT("Loading %s (%s, %s)\n",
			shaderData->name, shaderData->VS_EntryPoint, shaderData->PS_EntryPoint
		);

		const UINT numShaderInstances = shaderData->numInstances;

		for(UINT iShaderInstance = 0;
			iShaderInstance < numShaderInstances;
			iShaderInstance++)
		{
			ShaderInstance& instance = shaderData->instances[ iShaderInstance ];
			ShaderInstanceData& instanceData = shaderData->instancesData[ iShaderInstance ];

			stream >> instanceData;

			Assert(instanceData.isOk());

			//DBGOUT("--- loading VS %u, PS %u\n",instanceData.VS_CodeIndex,instanceData.PS_CodeIndex);

			instance.vertexShader = m_createdShaders[ instanceData.VS_CodeIndex ].GetShader<ID3D11VertexShader>();

			// null pixel shader is used when rendering into depth shadow map
			if( instanceData.PS_CodeIndex != INDEX_NONE )
			{
				instance.pixelShader = m_createdShaders[ instanceData.PS_CodeIndex ].GetShader<ID3D11PixelShader>();
			}
		}
	}

	DEVOUT("Loaded '%u' shaders (VS %u, PS %u).\n",
		header.m_numShaders, shaderCounts[ST_Vertex_Shader], shaderCounts[ST_Pixel_Shader]
	);

	const U8 endTime = mxGetTimeInMicroseconds();
	const U8 elapsedTime = endTime - startTime;
	UINT minutes, seconds;
	ConvertMicrosecondsToMinutesSeconds(elapsedTime, minutes, seconds);
	DBGOUT("Loaded shader cache in %u minutes %u seconds.\n", minutes, seconds);

	return true;
}
//-------------------------------------------------------------------------------------------------------------//
void D3D11ShaderCache::RecompileAllShaders( rxShaderCompiler& compiler )
{
	const U8 startTime = mxGetTimeInMicroseconds();

	const UINT numShaders = m_shadersList.Num();

	for(UINT iShader = 0; iShader < numShaders; iShader++)
	{
		GrShaderData* shaderData = m_shadersList[ iShader ];

		this->RecompileShader( compiler, shaderData );
	}

	const U8 endTime = mxGetTimeInMicroseconds();
	const U8 elapsedTime = endTime - startTime;
	UINT minutes, seconds;
	ConvertMicrosecondsToMinutesSeconds(elapsedTime, minutes, seconds);
	DEVOUT("Shader compilation took %u minutes %u seconds.\n", minutes, seconds);
}
//-------------------------------------------------------------------------------------------------------------//
void D3D11ShaderCache::RecompileShader( rxShaderCompiler& compiler, GrShaderData* shaderData )
{
	DEVOUT("Compiling %s (%s, %s)\n",
		shaderData->name, shaderData->VS_EntryPoint, shaderData->PS_EntryPoint
	);

	const UINT numInstances = shaderData->numInstances;

	for(UINT iShaderInstance = 0;
		iShaderInstance < numInstances;
		iShaderInstance++)
	{
		this->CompileShaderInstance( compiler, shaderData, iShaderInstance );
	}
}
//-------------------------------------------------------------------------------------------------------------//
void D3D11ShaderCache::CompileShaderInstance( rxShaderCompiler& compiler, GrShaderData* shaderData, UINT iShaderInstance )
{
	ShaderInstance & shaderInstance = shaderData->instances[ iShaderInstance ];
	ShaderInstanceData & shaderInstanceData = shaderData->instancesData[ iShaderInstance ];

	dxShaderMacros	shaderDefines;
	Assert( shaderData->getDefines != nil );
	shaderData->getDefines( iShaderInstance, shaderDefines );

#if MX_EDITOR
	UINT VS_CodeIndex, PS_CodeIndex;

	ID3D11VertexShader* newVS = TGetCachedShader< ID3D11VertexShader, ST_Vertex_Shader >( compiler, shaderData, shaderDefines.ToPtr(), VS_CodeIndex );
	ID3D11PixelShader* newPS = TGetCachedShader< ID3D11PixelShader, ST_Pixel_Shader >( compiler, shaderData, shaderDefines.ToPtr(), PS_CodeIndex );

	if( newVS != nil )
	{
		shaderInstance.vertexShader = newVS;
		shaderInstanceData.VS_CodeIndex = VS_CodeIndex;
	}
	else
	{
		DBGOUT("Failed to compile Vertex Shader\n");
	}

	if( newPS != nil )
	{
		shaderInstance.pixelShader = newPS;
		shaderInstanceData.PS_CodeIndex = PS_CodeIndex;
	}
	else
	{
		DBGOUT("Failed to compile Pixel Shader\n");
	}

#else // !MX_EDITOR

	shaderInstance.vertexShader = TGetCachedShader< ID3D11VertexShader, ST_Vertex_Shader >( compiler, shaderData, shaderDefines.ToPtr(), shaderInstanceData.VS_CodeIndex );
	shaderInstance.pixelShader = TGetCachedShader< ID3D11PixelShader, ST_Pixel_Shader >( compiler, shaderData, shaderDefines.ToPtr(), shaderInstanceData.PS_CodeIndex );

#endif // !MX_EDITOR

}
//-------------------------------------------------------------------------------------------------------------//
U4 D3D11ShaderCache::CalculateCheckSum()
{
	U4 crc32 = 0;

	TMap< D3D11CompiledShader, UINT >::ConstIterator iter( m_compiledCodeMap );
	while( iter )
	{
		const D3D11CompiledShader& code = iter.Key();

		crc32 ^= CRC32_BlockChecksum( code.GetBufferPointer(), code.GetBufferSize() );

		++iter;
	}

	return crc32;
}
//-------------------------------------------------------------------------------------------------------------//


/*
--------------------------------------------------------------
	D3D11ShaderSystem
--------------------------------------------------------------
*/
D3D11ShaderSystem::D3D11ShaderSystem()
{
}
//-------------------------------------------------------------------------------------------------------------//
D3D11ShaderSystem::~D3D11ShaderSystem()
{
}
//-------------------------------------------------------------------------------------------------------------//
void D3D11ShaderSystem::Create_ConstantBuffer(
	ID3D11BufferPtr &r,
	UINT size,
	const void* data
)
{
	Assert(r.IsNull());

	ID3D11Buffer* newBuffer = D3D_CreateConstantBuffer( size, data );
	r = newBuffer;
}
//-------------------------------------------------------------------------------------------------------------//
void D3D11ShaderSystem::Destroy_ConstantBuffer(
	ID3D11BufferPtr &r
)
{
	r = nil;
}
//-------------------------------------------------------------------------------------------------------------//
void D3D11ShaderSystem::RegisterShader(
	GrShaderData * shaderInfo
)
{
	m_shaderCache.RegisterNewShader(shaderInfo);
}
//-------------------------------------------------------------------------------------------------------------//
void D3D11ShaderSystem::GetShaderSource(
	const ANSICHAR* fileName,
	const ANSICHAR*&fileData,
	UINT &fileSize
)
{
	AssertPtr(fileName);

#if MX_EDITOR


	fileData = nil;
	fileSize = nil;

	mxPERM("this shouldn't be left in release exe, shaders should be compiled offline with a separate tool");
	mxHACK(:);mxUNSAFE;mxMT_UNSAFE;

	enum { MAX_SRC_CODE_CHARS = 1024*32 };	// 32 Kb
	static ANSICHAR buffer[MAX_SRC_CODE_CHARS];


	OSPathName		fullPath;
	GetFullPathToFile( fileName, fullPath );

	FileHandle file = FS_OpenFile( fullPath.ToChars(), EAccessMode::ReadAccess );

	if( InvalidFileHandle == file ) {
		dxErrf("Failed to open file '%s' for reading\n",fileName);
		return;
	}

	fileSize = FS_GetFileSize( file );

	// reserve one char for null terminator
	Assert(fileSize+1 < sizeof buffer);
	FS_ReadFile( file, buffer, fileSize );
	buffer[fileSize] = nil;	// null terminator

	fileData = buffer;

	FS_CloseFile( file );

#else

	mxDBG_UNIMPLEMENTED;

#endif // MX_EDITOR
}
//-------------------------------------------------------------------------------------------------------------//
void D3D11ShaderSystem::ReloadShader(
	GrShaderData * shaderInfo,
	const ANSICHAR* source,
	const UINT sourceLength
)
{
#if MX_EDITOR

	OSPathName		fullPath;
	GetFullPathToFile( shaderInfo->file, fullPath );

	FileHandle file = FS_OpenFile( fullPath.ToChars(), EAccessMode::WriteAccess );

	if( InvalidFileHandle == file ) {
		dxErrf("Failed to open file '%s' for writing\n",fullPath.ToChars());
		return;
	}

	FS_WriteFile( file, source, sourceLength );

	FS_CloseFile( file );

	m_shaderCache.RecompileShader( *this, shaderInfo );

#else

	mxDBG_UNIMPLEMENTED;

#endif // MX_EDITOR
}
//-------------------------------------------------------------------------------------------------------------//
void D3D11ShaderSystem::CompileShaders(
	const GrShaderCacheInfo& settings
)
{
	// Try to load shader cache first.

	m_pathToSources.SetString( settings.pathToShaderSource );

	//bool bUseShaderCache = 0;

	StackString	shaderCacheFileName;
	ComposeFullPathToShaderCache( settings.pathToShaderCache, shaderCacheFileName );

	FileReader	file( shaderCacheFileName.ToChars(), FileRead_NoErrors );

	if( !file.IsOpen() ) {
		goto L_ShaderCacheLoadFailed;
	}

	Assert( file.IsOpen() );

	dxMsgf("Loading shader cache from file '%s'.\n",shaderCacheFileName.ToChars());

	// Create all shader instances from cache.
	const bool bOk = m_shaderCache.Load( file, settings.timeStamp, *this );
	if( !bOk ) {
		goto L_ShaderCacheLoadFailed;
	}

	return;	// success


L_ShaderCacheLoadFailed:

#if 1//MX_EDITOR

		dxWarnf("Failed to load shader cache from file '%s'.\n",shaderCacheFileName.ToChars());

		// Recompile all shader instances.
		m_shaderCache.RecompileAllShaders(*this);

	#if MX_DEBUG
		if( !m_compilationLog.Text().IsEmpty() )
		{
			DebugOut("\n==== DUMPING COMPILATION LOG =====================\n");
			DebugOut(m_compilationLog.Text().ToChars());
			DebugOut("\n==================================================\n");
		}
	#endif
		//mxWarn("Some shaders have been compiled incorrectly.\n");

#else

	mxFatalf("Failed to load shader cache from file '%s'.\n",shaderCacheFileName.ToChars());

#endif // MX_EDITOR
}

//-------------------------------------------------------------------------------------------------------------//
void D3D11ShaderSystem::SaveCompiledShaders(
	PCSTR pathToShaderCache
)
{
	StackString	shaderCacheFileName;
	ComposeFullPathToShaderCache( pathToShaderCache, shaderCacheFileName );

	FS_MakeDirectory(pathToShaderCache);

	FileWriter	file( shaderCacheFileName.ToChars(), FileWrite_NoErrors );

	if( file.IsOpen() )
	{
		dxMsgf("Saving shader cache to file '%s'.\n",shaderCacheFileName.ToChars());
		m_shaderCache.Save( file );
	}
	else
	{
		dxWarnf("Failed to save shader cache to file '%s'.\n",shaderCacheFileName.ToChars());
	}
}
//-------------------------------------------------------------------------------------------------------------//
const char* D3D11ShaderSystem::GetCompilationErrors()
{
	if( !m_compilationLog.Text().IsEmpty() )
	{
		return m_compilationLog.Text().ToChars();
	}
	return nil;
}
//-------------------------------------------------------------------------------------------------------------//
ID3DBlob* D3D11ShaderSystem::CompileShaderFromFile(
	const char* fileName,
	const char* profile,
	const char* entryPoint,
	const D3D_SHADER_MACRO* defines
)
{
	OSPathName		fullPath;
	GetFullPathToFile( fileName, fullPath );

	ID3DBlob* byteCode = D3D_CompileShaderFromFile(
		fullPath.ToChars(),
		profile,
		entryPoint,
		defines,
		nil,
		m_compilationLog
	);
	return byteCode;
}
//-------------------------------------------------------------------------------------------------------------//
ID3D11VertexShader* D3D11ShaderSystem::NewVertexShader(
	const void* compiledBytecode,
	SizeT bytecodeLength,
	ID3D11ClassLinkage* linkage
)
{
	return D3D_CreateVertexShader( compiledBytecode, bytecodeLength, linkage );
}
//-------------------------------------------------------------------------------------------------------------//
ID3D11GeometryShader* D3D11ShaderSystem::NewGeometryShader(
	const void* compiledBytecode,
	SizeT bytecodeLength,
	ID3D11ClassLinkage* linkage
)
{
	return D3D_CreateGeometryShader( compiledBytecode, bytecodeLength, linkage );
}
//-------------------------------------------------------------------------------------------------------------//
ID3D11PixelShader* D3D11ShaderSystem::NewPixelShader(
	const void* compiledBytecode,
	SizeT bytecodeLength,
	ID3D11ClassLinkage* linkage
)
{
	return D3D_CreatePixelShader( compiledBytecode, bytecodeLength, linkage );
}

//-------------------------------------------------------------------------------------------------------------//
#if MX_EDITOR
const char* D3D11ShaderSystem::edToChars( UINT column ) const
{
	return "Shaders";
}
#endif // MX_EDITOR
//-------------------------------------------------------------------------------------------------------------//
void D3D11ShaderSystem::GetFullPathToFile( PCSTR fileName, OSPathName &OutPath )
{
	OutPath = m_pathToSources;
	OutPath.Append( fileName );
}




#if MX_EDITOR

mxDEFINE_ABSTRACT_CLASS(EdShaderDataProxy);

EdShaderDataProxy::EdShaderDataProxy( GrShaderData & r, UINT index )
	: d( r ), internalIndex( index )
{
	//internalIndex = INDEX_NONE;
}
EdShaderDataProxy::~EdShaderDataProxy()
{

}

UINT EdShaderDataProxy::edNumRows() const
{
	return 0;
}
AEditable* EdShaderDataProxy::edItemAt( UINT index )
{
	return nil;
}
UINT EdShaderDataProxy::edIndexOf( const AEditable* child ) const
{
	return INDEX_NONE;
}
const char* EdShaderDataProxy::edToChars( UINT column ) const
{
	return d.name;
}
AEditable* EdShaderDataProxy::edGetParent()
{
	D3D11ShaderSystem* shaderSystem = checked_cast< D3D11ShaderSystem* >( graphics.shaders.Ptr );
	return &shaderSystem->GetShaderCache();
}

#endif // MX_EDITOR

//-------------------------------------------------------------------------------------------------------------//
#if MX_EDITOR

const char* D3D11ShaderCache::edToChars( UINT column ) const
{
	return "Shaders";
}

AEditable* D3D11ShaderCache::edGetParent()
{
	return graphics.shaders;
}

#endif // MX_EDITOR

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//

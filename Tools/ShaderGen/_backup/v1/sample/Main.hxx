namespace GPU
{
	// Shader library info
	enum { TotalNumberOfShaders = 2 };
	
	UINT ShaderNameToIndex( const char* str );
	const char* ShaderIndexToName( UINT idx );
	
	// Function declarations
	void InitializeGPU();
	void ShutdownGPU();
}

#ifndef H_VERTEX_LIB_AUTO
#define H_VERTEX_LIB_AUTO

struct Vertex_P3f
{
	float3	position : Position; //  position 
};

struct Vertex_P3f_TEX2f
{
	float3	position : Position; //  position 
	float2	texCoords : TexCoord; //  texture coordinates 
};

struct Vertex_P3f_TEX2f_N4Ub_T4Ub
{
	float3	position : Position; //  position 
	float2	texCoords : TexCoord; //  texture coordinates 
	uint4	normal : Normal; //  normal (T') 
	uint4	tangent : Tangent; //  tangent (P') 
};

struct Vertex_P3f_TEX2f_COL4Ub
{
	float3	position : Position; //  transformed position 
	float2	texCoords : TexCoord; //  texture coordinates 
	float4	color : Color; //  vertex color 
};

#endif // !H_VERTEX_LIB_AUTO


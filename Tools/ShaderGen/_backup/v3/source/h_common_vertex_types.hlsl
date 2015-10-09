/*===============================================
		Vertex types
===============================================*/

// this vertex format is used for rendering static meshes
//
struct Vertex_Static
{
	float3 xyz : Position;
	float2 uv : TexCoord;
	uint4 normal : Normal;
	uint4 tangent : Tangent;
};

float3 UnpackVertexNormal( in float3 packedNormal )
{
	return packedNormal * (1.0f / 127.5f) - 1.0f;
}


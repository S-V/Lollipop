/*===============================================
		Vertex types
===============================================*/

struct vtx_static
{
	float3 xyz : Position;
	uint4 tangent : Tangent;
	uint4 normal : Normal;
	float2 uv : TexCoord;
};

float3 UnpackVertexNormal( in float3 packedNormal )
{
	return packedNormal * (1.0f / 127.5f) - 1.0f;
}


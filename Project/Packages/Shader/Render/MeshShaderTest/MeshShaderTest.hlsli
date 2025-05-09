
struct VertexInput {
	float4 position : POSITION0;
	float3 normal : NORMAL;
	float2 uv : TEXCOORD0;
};

struct Index {
	uint3 index;
};

struct BufferLength {
	uint vertexInputLength : INDEX0;
	uint indexLength : INDEX1;
};

struct PrimitiveOutput {
	uint primitiveId : INDEX0;
};

struct VertexOutput {
	float4 position : SV_POSITION;
	float3 normal : NORMAL;
	float2 uv : TEXCOORD;
	uint index : INDEX0;
};

struct PayloadType {
	uint meshletIndex;
};


struct Meshlet {
	uint vertexOffset;
	uint triangleOffset;
	uint vertexCount;
	uint triangleCount;
	
	/// culling
	//float3 boudingSphereCenter;
	//float boudingSphereRadius;
};

/// ============================================================
/// Global Buffer
/// ============================================================

ConstantBuffer<BufferLength> bufferLength : register(b0);
//StructuredBuffer<Meshlet>    meshlets     : register(t0);

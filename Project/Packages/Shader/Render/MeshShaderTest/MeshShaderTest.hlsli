
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

struct MSOutput {
	float4 position : SV_POSITION;
	float3 normal : NORMAL;
	float2 uv : TEXCOORD;
};

struct PayloadType {
	uint meshletIndex;
};

//struct Meshlet {
//	uint vertexOffset;
//	uint indexOffset;
//	uint vertexCount;
//	uint indexCount;
	
//	/// culling
//	float3 boudingSphereCenter;
//	float boudingSphereRadius;
//};

/// ============================================================
/// Global Buffer
/// ============================================================

ConstantBuffer<BufferLength> bufferLength : register(b0);
//StructuredBuffer<Meshlet>    meshlets     : register(t0);

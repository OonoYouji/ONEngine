
struct VertexInput {
	float4 position : POSITION0;
	float3 normal : NORMAL;
	float2 uv : TEXCOORD0;
};

struct Index {
	uint index;
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
	uint someData;
};


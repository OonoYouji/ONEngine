
struct MSInput {
	
};

struct MSOutput {
	float4 position : SV_POSITION;
	float4 color : COLOR;
};

struct PayloadType {
	uint someData;
	float4 color;
};

struct VertexOut {
	float3 position : POSITION;
	float3 normal : NORMAL;
	float2 uv : TEXCOORD;
};

struct PSInput {
	float4 position : SV_POSITION;
	float3 normal : NORMAL;
	float2 uv : TEXCOORD;
};

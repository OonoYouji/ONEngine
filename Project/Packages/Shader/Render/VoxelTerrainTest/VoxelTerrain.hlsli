struct Vertex {
    float4 position : POSITION;
	float4 color : COLOR0;
    float3 normal : NORMAL;
};

struct VSOutput {
    float4 position : SV_POSITION;
    float4 worldPos : TEXCOORD0;
    float3 normal : NORMAL;
	float4 color : COLOR0;
};

struct PSOutput {
    float4 color : SV_TARGET0;
    float4 worldPos : SV_TARGET1;
    float4 normal : SV_TARGET2;
    float4 flags : SV_TARGET3;
};
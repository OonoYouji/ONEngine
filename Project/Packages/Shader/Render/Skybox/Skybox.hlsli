struct VSInput {
	float4 position      : POSITION;
};

struct VSOutput {
	float4 position      : SV_POSITION;
	float4 worldPosition : TEXCOORD0;
	float3 normal        : NORMAL0;
	float3 uv            : TEXCOORD2;
};

struct PSOutput {
	float4 color         : SV_TARGET0;
	float4 worldPosition : SV_TARGET1;
	float4 normal        : SV_TARGET2;
	float4 flags         : SV_TARGET3;
};


struct TexIndex {
	uint id;
};
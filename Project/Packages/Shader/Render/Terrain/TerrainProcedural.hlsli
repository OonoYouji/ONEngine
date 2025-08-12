struct VSInput {
	float4 position : POSITION0;
	float3 normal : NORMAL0;
	float2 uv : TEXCOORD0;
};

struct VSOutput {
	float4 position : SV_POSITION;
	float4 wPosition : POSITION0;
	float3 normal : NORMAL0;
	float2 uv : TEXCOORD0;
};

struct PSOutput {
	float4 color : SV_TARGET0;
	float4 wPosition : SV_TARGET1;
	float4 normal : SV_TARGET2;
	float4 flags : SV_TARGET3;
};
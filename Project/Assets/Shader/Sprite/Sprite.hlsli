struct VSInput {
	float4 position : SV_POSITION;
	float2 uv       : TEXCOORD0;
};

struct VSOutput {
	float4 position : SV_POSITION;
	float2 uv       : TEXCOORD0;
};

struct PSOutput {
	float4 color : SV_TARGET0;
};
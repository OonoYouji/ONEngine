struct VSInput {
	float4 position : POSITION0;
	float2 texcoord : TEXCOORD0;
};

struct VSOutput {
	float4 position : SV_POSITION;
	float2 texcoord : TEXCOORD0;
};


struct PSOutput {
	float4 color1 : SV_TARGET0;
	float4 color2 : SV_TARGET1;
};
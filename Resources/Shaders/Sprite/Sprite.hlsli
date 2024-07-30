struct VSInput {
	float4 position : POSITION0;
	float2 texcoord : TEXCOORD0;
};

struct VSOutput {
	float4 position : SV_POSITION;
	float2 texcoord : TEXCOORD0;
};




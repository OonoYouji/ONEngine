struct VSInput {
	float4 position : POSITION0;
	float4 color    : COLOR0;
};

struct VSOutput {
	float4 position : SV_POSITION;
	float4 color    : COLOR0;
};

struct PSOutput {
	float4 color    : SV_TARGET0;
};


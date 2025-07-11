struct VSInput {
	float4 position : POSITION0;
	float4 color    : COLOR0;
};

struct VSOutput {
	float4 position : SV_POSITION;
	float4 color    : COLOR0;
	float4 worldPosition : POSITION0;
};

struct PSOutput {
	float4 color : SV_TARGET0;
	float4 worldPosition : SV_TARGET1;
	float4 normal : SV_TARGET2;
	float4 flags : SV_TARGET3;
};


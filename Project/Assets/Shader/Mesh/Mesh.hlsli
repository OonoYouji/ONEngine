struct VSInput {
	float4 position : POSITION0;
};


struct VSOutput {
	float4 position : SV_POSITION;
};


struct PSOutput {
	float4 color : SV_TARGET0;
};
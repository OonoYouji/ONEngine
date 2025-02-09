struct VSInput {
	float4 position : POSITION0;
	float3 normal   : NORMAL0;
	float2 uv       : TEXCOORD0;
};


struct VSOutput {
	float4 position : SV_POSITION;
	float3 normal   : NORMAL0;
	float2 uv       : TEXCOORD0;
};


struct PSOutput {
	float4 color : SV_TARGET0;
};
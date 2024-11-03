struct VSInput {
    float4 position : POSITION0;
    float4 color : COLOR0;
};

struct VSOutput {
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

struct PSOutput {
	float4 color : SV_TARGET;
};

struct GSInput {
	float4 position : SV_POSITION;
	float4 color : COLOR;
};

struct GSOutput {
	float4 position : SV_POSITION;
	float4 color : COLOR;
};
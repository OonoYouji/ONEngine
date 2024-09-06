struct VSInput {
    float4 position : POSITION0;
    float4 color : COLOR0;
    float4 screenSize : TEXCOORD0;
};

struct VSOutput {
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float4 screenSize : TEXCOORD0;
};
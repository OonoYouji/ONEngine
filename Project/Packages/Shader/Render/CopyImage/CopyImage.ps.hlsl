#include "CopyImage.hlsli"

struct PSOutput {
	float4 color : SV_TARGET0;
};

Texture2D<float4> sceneTexture : register(t0);
SamplerState samplerState : register(s0);


PSOutput main(VSOutput input) {
	PSOutput output;
	output.color = sceneTexture.Sample(samplerState, input.texcoord);
	return output;
}
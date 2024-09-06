#include "BlendRenderTexture.hlsli"

Texture2D gRtvTexBack : register(t0);
Texture2D gRtvTexFront : register(t1);
SamplerState gSampler : register(s0);

PSOutput main(VSOutput input) {
	PSOutput output;
	
	float4 frontColor = gRtvTexFront.Sample(gSampler, input.texcoord);
	float4 backColor = gRtvTexBack.Sample(gSampler, input.texcoord);
	
	float alpha = frontColor.a; // frontRenderTex のアルファチャンネルを使用
	output.color = lerp(backColor, frontColor, alpha);

	if (output.color.a == 0.0f) {
		discard;
	}

	return output;
}
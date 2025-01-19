#include "BlendRenderTexture.hlsli"

Texture2D gRtvTexBack : register(t0);
Texture2D gRtvTexFront : register(t1);
SamplerState gSampler : register(s0);

PSOutput main(VSOutput input) {
	PSOutput output;

	float4 frontColor = gRtvTexFront.Sample(gSampler, input.texcoord);
	float4 backColor  = gRtvTexBack.Sample(gSampler, input.texcoord);
	
	/// frontRenderTex のアルファチャンネルを使用
	float alpha      = frontColor.a; 
	output.color.rgb = lerp(backColor.rgb, frontColor.rgb, alpha);
	output.color.a   = 1.0f;
	return output;
}
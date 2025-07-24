
#include "../../../ConstantBufferData/Material.hlsli"

/// texture
Texture2D<float4> colorTex : register(t0);
Texture2D<float4> flagsTex : register(t1);
RWTexture2D<float4> outputTex : register(u0);
SamplerState textureSampler : register(s0);

[numthreads(16, 16, 1)]
void main(uint3 dispatchId : SV_DispatchThreadID) {

	float4 flags = flagsTex[dispatchId.xy];
	if (!IsPostEffectEnabled((int)flags.x, PostEffectFlags_Grayscale)) {
		return;
	}

	float2 texCoord = float2(dispatchId.x / 1920.0f, dispatchId.y / 1080.0f);

	float4 color = colorTex.Sample(textureSampler, texCoord);

	float value = dot(color.rgb, float3(0.2125f, 0.7154f, 0.0721f));
	outputTex[dispatchId.xy] = float4(value, value, value, 1.0f);
	
}
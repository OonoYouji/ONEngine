#include "BrushPreview.hlsli"
#include "../../ConstantBufferData/Material.hlsli"

struct PSOutput {
	float4 color : SV_Target0;
	float4 worldPos : SV_Target1;
	float4 normal : SV_Target2;
	float4 flags : SV_Target3;
};

ConstantBuffer<ConstantBufferMaterial> material : register(b4);

PSOutput main(VertexOut input) {
	PSOutput output;

	// 1. 法線の正規化
	float3 N = normalize(input.normal);

	output.color = float4(0.7, 0.1, 0.1, 0.5);
	output.worldPos = input.worldPosition;
	output.normal = float4(N, 1);
	output.flags = float4(PostEffectFlags_Lighting, material.intValues.y, 0, 1);

	if (output.color.a <= 0.001f) {
		discard;
	}
	
	return output;
}
#include "TerrainProcedural.hlsli"

#include "../../ConstantBufferData/Material.hlsli"

PSOutput main(VSOutput input) {
	PSOutput output;
	
	output.color = float4(1, 1, 0, 1);
	output.normal = float4(input.normal, 1);
	output.wPosition = input.wPosition;
	output.flags = float4(PostEffectFlags_Lighting, 0, 0, 1);

	return output;
}
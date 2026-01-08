#include "VoxelTerrain.hlsli"
#include "../../ConstantBufferData/Material.hlsli"

ConstantBuffer<ConstantBufferMaterial> material : register(b1);

PSOutput main(VSOutput input) {
	PSOutput output;

	output.color = material.baseColor * input.color;
	//output.color = material.baseColor;
	output.worldPos = input.worldPos;
	output.normal = float4(normalize(input.normal), 1.0f);
	output.flags = float4(material.intValues.y, material.intValues.x, 0.0f, 1.0f);

	return output;
}

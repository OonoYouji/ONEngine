#include "../../ConstantBufferData/Material.hlsli"
#include "Transvoxel.hlsli"

struct PSOutput {
	float4 color : SV_Target0;
	float4 worldPos : SV_Target1;
	float4 normal : SV_Target2;
	float4 flags : SV_Target3;
};

ConstantBuffer<ConstantBufferMaterial> material : register(b3);

PSOutput main(VertexOut input) {
	PSOutput output;

    // output.color = material.baseColor;
	// output.color = float32_t4(0.7, 0.2, 0.1, 1);
    output.color = input.color;
	output.worldPos = input.worldPosition;
	output.normal = float4(normalize(input.normal.xyz), 1);
	output.flags = float4(material.intValues.x, material.intValues.y, 0, 1);

	if (output.color.a <= 0.001f) {
		discard;
	}

	return output;
}
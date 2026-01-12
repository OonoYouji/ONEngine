#include "Transvoxel.hlsli"
#include "../../ConstantBufferData/Material.hlsli"

struct PSOutput {
	float4 color : SV_Target0;
	float4 worldPos : SV_Target1;
	float4 normal : SV_Target2;
	float4 flags : SV_Target3;
};

ConstantBuffer<ConstantBufferMaterial> material : register(b3);

PSOutput main(VertexOut _out) {
	PSOutput output;
	
    output.color = material.baseColor;
	output.worldPos = _out.worldPos;
	output.normal = float4(normalize(_out.normal.xyz), 1);
	output.flags = float4(material.intValues.x, material.intValues.y, 0, 1);

	if (output.color.a <= 0.001f) {
		discard;
	}
	
	return output;
}
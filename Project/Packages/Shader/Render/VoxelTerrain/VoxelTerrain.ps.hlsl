#include "VoxelTerrain.hlsli"

struct PSOutput {
	float4 color : SV_Target0;
	float4 worldPos : SV_Target1;
	float4 normal : SV_Target2;
	float4 flags : SV_Target3;
};


PSOutput main(VertexOut _out) {
	PSOutput output;
	
	//float3 direction1to2 = normalize(_out.otherVertexPos2.xyz - _out.otherVertexPos1.xyz);
	//float3 direction1toSelf = normalize(_out.worldPosition.xyz - _out.otherVertexPos1.xyz);
	//float3 normal = cross(direction1to2, direction1toSelf);

	//float3 normal = float3(0, 1, 0);

	output.color    = _out.color;
	output.worldPos = _out.worldPosition;
	output.normal = float4(normalize(_out.normal), 1);
	output.flags    = float4(1, 0, 0, 1);

	if (output.color.a <= 0.001f) {
		discard;
	}
	
	return output;
}
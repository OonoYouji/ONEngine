#include "VoxelTerrain.hlsli"

struct PSOutput {
	float4 color : SV_Target0;
	float4 worldPos : SV_Target1;
	float4 normal : SV_Target2;
	float4 flags : SV_Target3;
};


PSOutput main(VertexOut _out) {
	PSOutput output;

	output.color = _out.color;
	output.worldPos = _out.worldPosition;
	output.normal = float4(normalize(_out.normal), 1);
	output.flags = float4(1, 0, 0, 1);

	if (output.color.a <= 0.001f) {
		discard;
	}
	
	return output;
}
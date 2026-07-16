#include "Gizmo3D.hlsli"

PSOutput main(VSOutput input) {
	PSOutput output;
	output.color = input.color;
	output.worldPosition = input.worldPosition;
	output.normal = float4(0, 0, 0, 1);
	output.flags = float4(0, 0, 0, 0);
	return output;
}
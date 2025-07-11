#include "Line3D.hlsli"

PSOutput main(VSOutput input) {
	PSOutput output;
	output.color = input.color;
	output.worldPosition = input.worldPosition;
	output.normal = float4(0, 0, 0, 1); // No normal data for lines)
	output.flags = float4(0, 0, 0, 0); // No flags data for lines
	return output;
}
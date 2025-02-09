#include "Mesh.hlsli"


PSOutput main(VSOutput input) {
	PSOutput output;

	output.color = float4(input.normal, 1.0f);

	return output;
}
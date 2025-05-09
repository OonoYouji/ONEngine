#include "MeshShaderTest.hlsli"

static const float4 colors[5] = {
	float4(1, 0, 0, 1),
	float4(0, 1, 0, 1),
	float4(0, 0, 1, 1),
	float4(1, 1, 0, 1),
	float4(0, 1, 1, 1)
};


[shader("pixel")]
float4 main(VertexOutput input) : SV_Target {

	return colors[input.index % 5];

	//return float4(input.normal * 0.5f + 0.5f, 1);
}
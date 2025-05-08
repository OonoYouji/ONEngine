#include "MeshShaderTest.hlsli"


[shader("pixel")]
float4 main(MSOutput input) : SV_Target {
	return float4(input.normal * 0.5f + 0.5f, 1);
	//return float4(1, 0, 0, 1);
}
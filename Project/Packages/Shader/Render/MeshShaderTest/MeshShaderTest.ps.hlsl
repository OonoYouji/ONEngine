#include "MeshShaderTest.hlsli"


[shader("pixel")]
float4 main(MSOutput input) : SV_Target {
	return float4(1, 0, 0, 1);
	//return float4(input.uv, 0.5, 1.0); // UV可視化
}
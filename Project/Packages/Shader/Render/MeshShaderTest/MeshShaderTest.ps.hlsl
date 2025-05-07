#include "MeshShaderTest.hlsli"


[shader("pixel")]
float4 main(PSInput input) : SV_Target {
	return float4(input.uv, 0.5, 1.0); // UV可視化
}
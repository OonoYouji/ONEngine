#include "MeshShaderTest.hlsli"


[shader("pixel")]
float4 main(MSOutput input) : SV_Target {
	return float4(1, 0, 0, 1);
}
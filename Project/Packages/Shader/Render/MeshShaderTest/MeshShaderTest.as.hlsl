#include "MeshShaderTest.hlsli"


[shader("amplification")]
[numthreads(1, 1, 1)]
void main(uint3 dispatchThreadID : SV_DispatchThreadID) {
	PayloadType payload;
	DispatchMesh(1, 1, 1, payload);
}
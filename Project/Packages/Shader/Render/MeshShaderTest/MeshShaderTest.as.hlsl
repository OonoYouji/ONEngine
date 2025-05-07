#include "MeshShaderTest.hlsli"


[shader("amplification")]
[numthreads(1, 1, 1)]
void main(uint3 dispatchThreadID : SV_DispatchThreadID) {
	PayloadType payload;
	payload.someData = 0;
	DispatchMesh(1, 1, 1, payload);
}
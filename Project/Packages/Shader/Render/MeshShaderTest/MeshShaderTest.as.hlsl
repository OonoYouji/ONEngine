#include "MeshShaderTest.hlsli"


[shader("amplification")]
[numthreads(32, 1, 1)]
void main(uint3 dispatchThreadID : SV_DispatchThreadID) {
	PayloadType payload;
	payload.someData = 0;
	DispatchMesh(dispatchThreadID.x, dispatchThreadID.y, dispatchThreadID.z, payload);
}
#include "MeshShaderTest.hlsli"


bool IsVisible(float3 _center, float _radius) {
	return true;
}


[shader("amplification")]
[numthreads(1, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID) {
	PayloadType payload;
	//uint meshletIndex = DTid.x;
	
	//Meshlet meshlet = meshlets[meshletIndex];

	///// 簡易カリング
	//if (!IsVisible(meshlet.boudingSphereCenter, meshlet.boudingSphereRadius)) {
	//	return;
	//}
	
	//payload.meshletIndex = meshletIndex;
	
	DispatchMesh(1, 1, 1, payload);
}
//#include "Grass.hlsli"
#include "BladeInstance.hlsli"


[shader("amplification")]
[numthreads(1, 1, 1)]
void ASMain(uint3 DTid : SV_DispatchThreadID,
			uint3 Gid : SV_GroupID) {
	uint index = startIndex + DTid.x;
	Payload payload;

	// 草のデータを取得
	//BladeInstance instance = bladeInstances[index];
	
	for (int i = 0; i < kMaxGrassDataSize; ++i) {
		payload.grassData[i].isCulled = false;
		payload.grassData[i].index = index + i;
	}

	// DispatchMeshを呼び出す
	DispatchMesh(1, 1, 1, payload);
}

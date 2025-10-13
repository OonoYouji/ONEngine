//#include "Grass.hlsli"
#include "BladeInstance.hlsli"


[shader("amplification")]
[numthreads(1, 1, 1)]
void ASMain(uint3 DTid : SV_DispatchThreadID) {
	uint globalIndex = DTid.x;
	uint index = startIndex + globalIndex;

	Payload payload;

	// 草のデータを取得
	BladeInstance instance = bladeInstances[index];

	// カリング条件を設定（例: x座標が500以上の場合はカリング）
	if (instance.position.x > 500.0f) {
		payload.grassData.isCulled = true;
		payload.grassData.index = index;
	} else {
		payload.grassData.isCulled = false;
		payload.grassData.index = index;
	}

	// DispatchMeshを呼び出す
	DispatchMesh(1, 1, 1, payload);
}

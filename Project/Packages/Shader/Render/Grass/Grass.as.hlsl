//#include "Grass.hlsli"
#include "BladeInstance.hlsli"

float3 Min3(float3 a, float3 b) {
	return float3(min(a.x, b.x), min(a.y, b.y), min(a.z, b.z));
}

float3 Max3(float3 a, float3 b) {
	return float3(max(a.x, b.x), max(a.y, b.y), max(a.z, b.z));
}

bool IsVisible(float3 min, float3 max) {
    // 仮の描画範囲
	float3 rangeMin = float3(200, 0, 0);
	float3 rangeMax = float3(700, 0, 300);

    // XZ平面でAABBが範囲内に重なっているか
	bool overlapX = max.x >= rangeMin.x && min.x <= rangeMax.x;
	bool overlapZ = max.z >= rangeMin.z && min.z <= rangeMax.z;

	return overlapX && overlapZ;
}



[shader("amplification")]
[numthreads(1, 1, 1)]
void ASMain(uint3 DTid : SV_DispatchThreadID,
			uint gIndex : SV_GroupIndex,
			uint3 Gid : SV_GroupID) {
	uint index = startIndices[gIndex].value /*+ DTid.x*/;
	Payload payload;
	
	BladeInstance frontInstance = bladeInstances[index];
	float3 min = frontInstance.position;
	float3 max = frontInstance.position;

	// 草のデータを取得
	for (int i = 0; i < kMaxRenderingGrassSize; ++i) {
		payload.grassData[i].isCulled = false;
		payload.grassData[i].index = index + i;
		
		BladeInstance instance = bladeInstances[index + i];
		///min maxの更新
		min = Min3(min, instance.position);
		max = Max3(max, instance.position);
	}
	
	
	//uint grassCount = 1;
	//if (!IsVisible(min, max)) {
	//	grassCount = 0;
	//}


	// DispatchMeshを呼び出す
	DispatchMesh(1, 1, 1, payload);
}

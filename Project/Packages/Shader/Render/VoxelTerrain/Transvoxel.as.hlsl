#include "Transvoxel.hlsli"



uint GetChunkLOD(uint3 groupId) {
    // LOD計算のロジックをここに実装
    // 例: 距離に基づくLOD計算
	return 0; // 仮の戻り値
}

uint GetNeighborLOD(uint3 neighborGroupId) {
	// 隣接チャンクのLODを取得するロジックをここに実装
	return 0; // 仮の戻り値
}

// ---------------------------------------------------
// Amplification Shader
// ---------------------------------------------------
[shader("amplification")]
[numthreads(1, 1, 1)]
void AS_Transvoxel(
    uint3 DTid : SV_DispatchThreadID,
    uint3 groupId : SV_GroupID,
    uint groupIndex : SV_GroupIndex) {
    // チャンク原点
	uint3 chunkOrigin = groupId * voxelTerrainInfo.chunkSize;

    // 自チャンク LOD
	uint myLOD = GetChunkLOD(groupId);

    // 隣接チャンク LOD取得（簡易例）
	uint neighborLOD_XP = GetNeighborLOD(groupId + uint3(1, 0, 0));
	uint neighborLOD_XN = GetNeighborLOD(groupId - uint3(1, 0, 0));
	uint neighborLOD_ZP = GetNeighborLOD(groupId + uint3(0, 0, 1));
	uint neighborLOD_ZN = GetNeighborLOD(groupId - uint3(0, 0, 1));
	uint neighborLOD_YP = GetNeighborLOD(groupId + uint3(0, 1, 0));
	uint neighborLOD_YN = GetNeighborLOD(groupId - uint3(0, 1, 0));

    // 境界面ごとに必要なTransvoxel Dispatchを決定
    [unroll]
	for (uint face = 0; face < 6; ++face) {
		bool needTransvoxel = false;
		uint neighborLOD = 0;
		switch (face) {
			case 0:
				neighborLOD = neighborLOD_XP;
				needTransvoxel = (myLOD > neighborLOD);
				break;
			case 1:
				neighborLOD = neighborLOD_XN;
				needTransvoxel = (myLOD > neighborLOD);
				break;
			case 2:
				neighborLOD = neighborLOD_ZP;
				needTransvoxel = (myLOD > neighborLOD);
				break;
			case 3:
				neighborLOD = neighborLOD_ZN;
				needTransvoxel = (myLOD > neighborLOD);
				break;
			case 4:
				neighborLOD = neighborLOD_YP;
				needTransvoxel = (myLOD > neighborLOD);
				break;
			case 5:
				neighborLOD = neighborLOD_YN;
				needTransvoxel = (myLOD > neighborLOD);
				break;
		}

		if (needTransvoxel) {
			Payload payload;
			payload.face = face;
			payload.myLOD = myLOD;
			payload.neighborLOD = neighborLOD;
			payload.chunkOrigin = chunkOrigin;
			payload.cellCount = voxelTerrainInfo.chunkSize.x >> myLOD; // X面セル数例

            // DispatchMesh: 1スレッド=1境界セル
			DispatchMesh(payload.cellCount, payload.cellCount, 1, payload);
		}
	}
}
#include "BladeInstance.hlsli"

cbuffer constants : register(b1) {
	uint startIndex;
	uint currentInstanceCount;
};

[shader("amplification")]
[numthreads(16, 1, 1)] // numthreads 属性を追加
void ASMain(uint3 DTid : SV_DispatchThreadID) {
    // Payload を構築
	uint globalIndex = DTid.x;
	uint index = startIndex + globalIndex;

	Payload payload;
	payload.grassData[globalIndex].index = index;
	//payload.startIndex = startIndex;
	//payload.currentInstanceCount = currentInstanceCount;
	//payload.index = index;

	//BladeInstance instance = bladeInstances[index];
	/// 適当にカリングを行ってみる
	/// 仮にxが500以上ならスルーする
	//payload.isCulled = false;
	//if (instance.position.x > 500.0f) {
	//	payload.isCulled = true;
	//	// DispatchMeshは全スレッドで呼ぶ
	//}

	
	DispatchMesh(1, 1, 1, payload);
}

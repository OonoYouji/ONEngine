#include "Grass.hlsli"
//#include "BladeInstance.hlsli"

cbuffer constants : register(b1) {
	uint startIndex;
	uint currentInstanceCount;
};

[shader("amplification")]
[numthreads(32, 1, 1)] // numthreads 属性を追加
void ASMain(uint3 DTid : SV_DispatchThreadID) {
    // Payload を構築
	uint globalIndex = DTid.x;
	uint index = startIndex + globalIndex;

	Payload payload;
	payload.grassData.index = index;
	//payload.grassData.index = index;
	
	DispatchMesh(1, 1, 1, payload);
}

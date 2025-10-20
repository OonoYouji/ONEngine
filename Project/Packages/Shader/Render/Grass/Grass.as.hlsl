//#include "Grass.hlsli"
#include "BladeInstance.hlsli"

float3 Min3(float3 a, float3 b) {
	return float3(min(a.x, b.x), min(a.y, b.y), min(a.z, b.z));
}

float3 Max3(float3 a, float3 b) {
	return float3(max(a.x, b.x), max(a.y, b.y), max(a.z, b.z));
}

cbuffer constants : register(b3) {
	uint asDispatchCount; /// asの起動回数
	uint asDispatchSize; /// asのdispatchSize
};


[shader("amplification")]
[numthreads(1, 1, 1)]
void ASMain(uint3 DTid : SV_DispatchThreadID,
			uint gIndex : SV_GroupIndex,
			uint3 Gid : SV_GroupThreadID) {

	uint meshShaderDipatchCount = 32;
	uint grassPerMS = meshShaderDipatchCount * 51; /// 1つのMSあたりの草の本数
	uint groupIndex = DTid.x; /// DispatchMesh(x,y,z) の x に対応

	uint index = groupIndex * grassPerMS;
	Payload payload;
	
	/// 51本分の開始インデックスをセット
	for (int i = 0; i < meshShaderDipatchCount; ++i) {
		payload.startIndices[i] = index + i * 51;
	}

	
	/// 32個の草のグループごとにAABBでカリングする
	Frustum frustum = CreateFrustumFromMatrix(viewProjection.matVP);
	
	uint dispatchSize = 0;
	uint drawIndices[32];
	
	for (int i = 0; i < meshShaderDipatchCount; ++i) {
		AABB aabb;

		uint startIndex = index + i * 51;
		BladeInstance frontInstance = bladeInstances[startIndex];
		//aabb.min = frontInstance.position;
		//aabb.max = frontInstance.position;

		///// 各インスタンスの位置を考慮して範囲を計算
		//for (int j = 0; j < 51; ++j) {
		//	BladeInstance instance = bladeInstances[startIndex + j];

		//	/// AABBの最小・最大を更新
		//	aabb.min = Min3(aabb.min, instance.position /*- expand*/);
		//	aabb.max = Max3(aabb.max, instance.position /*+ expand*/);
		//}

		/// 座標系をfrustumと合わせる
		//aabb.min = mul(float4(aabb.min, 1.0f), viewProjection.matView).xyz;
		//aabb.max = mul(float4(aabb.max, 1.0f), viewProjection.matView).xyz;
		
		/// AABBとFrustumのどちらもWorld座標系で計算する
		/// ここでフラスタムとの交差判定を行う
		//if (IsVisible(aabb, frustum)) {
		//	drawIndices[dispatchSize] = startIndex;
		//	dispatchSize += 1;
		//}
		
		
		drawIndices[dispatchSize] = startIndex;
		dispatchSize += 1;
	}

	for (int i = 0; i < dispatchSize; ++i) {
		payload.startIndices[i] = drawIndices[i];
	}
	
	/// Mesh Shaderを呼び出す
	DispatchMesh(dispatchSize, 1, 1, payload);
}

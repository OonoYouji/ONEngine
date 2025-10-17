//#include "Grass.hlsli"
#include "BladeInstance.hlsli"

float3 Min3(float3 a, float3 b) {
	return float3(min(a.x, b.x), min(a.y, b.y), min(a.z, b.z));
}

float3 Max3(float3 a, float3 b) {
	return float3(max(a.x, b.x), max(a.y, b.y), max(a.z, b.z));
}

//bool IsVisible(float3 min, float3 max) {
//    // 仮の描画範囲
//	float3 rangeMin = float3(200, 0, 0);
//	float3 rangeMax = float3(700, 0, 300);

//    // XZ平面でAABBが範囲内に重なっているか
//	bool overlapX = max.x >= rangeMin.x && min.x <= rangeMax.x;
//	bool overlapZ = max.z >= rangeMin.z && min.z <= rangeMax.z;

//	return overlapX && overlapZ;
//}


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
	uint grassPerMS = meshShaderDipatchCount * 51; // 1 MSあたりの草数
	uint groupIndex = DTid.x; // DispatchMesh(x,y,z)のxに対応

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
		aabb.min = frontInstance.position;
		aabb.max = frontInstance.position;

    // track maximum local extents for safety (optional)
		float maxWidth = 0.0;
		float maxHeight = 0.0;

    // accumulate positions and compute per-instance extents
		for (int j = 0; j < 51; ++j) {
			BladeInstance instance = bladeInstances[startIndex + j];

        // compute extents used in MS:
			float width = instance.scale; // blade horizontal half-extent approximate
			float height = 2.0 * instance.scale; // blade vertical extent approximate

        // include sway amplitude (match MS's 0.1 factor or tune)
			float swayAmp = 0.12; // choose slightly larger than MS's 0.1 for safety

        // expand AABB by the full potential extents of this blade
			float3 expand = float3(width + swayAmp, height, width + swayAmp);

        // update min/max using expanded box around instance.position
			aabb.min = Min3(aabb.min, instance.position - expand);
			aabb.max = Max3(aabb.max, instance.position + expand);

        // optional tracking
			maxWidth = max(maxWidth, width + swayAmp);
			maxHeight = max(maxHeight, height);
		}

		// now test against the frustum
		aabb.min = mul(float4(aabb.min, 1.0f), viewProjection.matView).xyz;
		aabb.max = mul(float4(aabb.max, 1.0f), viewProjection.matView).xyz;
		
		if (IsVisible(aabb, frustum)) {
			drawIndices[dispatchSize] = startIndex;
			dispatchSize += 1;
		}
	}

	for(int i = 0; i < dispatchSize; ++i) {
		payload.startIndices[i] = drawIndices[i];
	}
	

	// DispatchMeshを呼び出す
	DispatchMesh(dispatchSize, 1, 1, payload);
}

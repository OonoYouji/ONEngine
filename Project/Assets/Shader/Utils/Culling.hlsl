#include "../Schema/Schema.hlsli"

struct FrustumPlanes {
    float4 planes[6];
};

struct CullingParams {
    uint targetModelIndex;
    uint maxInstances;
    uint instanceOffset; // 入力バッファ内の開始位置
    uint batchIndex;     // このバッチが使用するカウンタ/バッファの番号
    uint forceVisible;   // 1 ならカリングをスキップ (デバッグ用)
    uint subMeshIndex;   // 追加
};

ConstantBuffer<SceneData> gSceneData : register(b0);
ConstantBuffer<FrustumPlanes> gFrustum : register(b1);
ConstantBuffer<CullingParams> gCullingParams : register(b2);

StructuredBuffer<InstanceData> gInputInstances : register(t0);
StructuredBuffer<MeshInfo> gMeshInfos : register(t1);

RWStructuredBuffer<InstanceData> gOutputInstances : register(u0);
RWStructuredBuffer<uint> gCountBuffer : register(u1); // 各バッチごとのカウントを保持

bool IsVisible(InstanceData inst) {
    if (gCullingParams.forceVisible != 0) return true;

    float4x4 world = inst.world;
    float3 minP = inst.aabbMin.xyz;
    float3 maxP = inst.aabbMax.xyz;

    // AABBの8角をワールド空間へ変換
    float3 corners[8];
    corners[0] = mul(float4(minP.x, minP.y, minP.z, 1.0f), world).xyz;
    corners[1] = mul(float4(maxP.x, minP.y, minP.z, 1.0f), world).xyz;
    corners[2] = mul(float4(minP.x, maxP.y, minP.z, 1.0f), world).xyz;
    corners[3] = mul(float4(maxP.x, maxP.y, minP.z, 1.0f), world).xyz;
    corners[4] = mul(float4(minP.x, minP.y, maxP.z, 1.0f), world).xyz;
    corners[5] = mul(float4(maxP.x, minP.y, maxP.z, 1.0f), world).xyz;
    corners[6] = mul(float4(minP.x, maxP.y, maxP.z, 1.0f), world).xyz;
    corners[7] = mul(float4(maxP.x, maxP.y, maxP.z, 1.0f), world).xyz;

    for (int i = 0; i < 6; ++i) {
        int outCount = 0;
        for (int j = 0; j < 8; ++j) {
            if (dot(gFrustum.planes[i], float4(corners[j], 1.0f)) < -0.01f) {
                outCount++;
            }
        }
        if (outCount == 8) return false;
    }
    return true;
}

[numthreads(64, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID) {
    uint localIdx = DTid.x;
    if (localIdx >= gCullingParams.maxInstances) return;

    // このバッチの絶対インデックス
    uint globalIdx = gCullingParams.instanceOffset + localIdx;
    InstanceData inst = gInputInstances[globalIdx];
    
    // モデルインデックスが一致するか確認
    if (inst.modelIndex != gCullingParams.targetModelIndex) return;

    // カリング判定
    if (IsVisible(inst)) {
        uint outputIdx;
        InterlockedAdd(gCountBuffer[gCullingParams.batchIndex], 1, outputIdx);
        
        // 出力バッファに書き込み
        uint writeIdx = gCullingParams.batchIndex * 2048 + outputIdx;
        if (outputIdx < 2048) {
            gOutputInstances[writeIdx] = inst;
        }
    }
}

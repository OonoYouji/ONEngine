#include "../Schema/Schema.hlsli"

struct FrustumPlanes {
    float4 planes[6];
};

struct DrawIndexedArguments {
    uint indexCountPerInstance;
    uint instanceCount;
    uint startIndexLocation;
    int  baseVertexLocation;
    uint startInstanceLocation;
};

struct CullingParams {
    uint targetModelIndex;
    uint maxInstances;
    uint instanceOffset; // 入力バッファ内の開始位置
    uint batchIndex;     // このバッチが使用するカウンタ/バッファの番号
    uint forceVisible;   // 1 ならカリングをスキップ (デバッグ用)
};

ConstantBuffer<SceneData> gSceneData : register(b0);
ConstantBuffer<FrustumPlanes> gFrustum : register(b1);
ConstantBuffer<CullingParams> gCullingParams : register(b2);

StructuredBuffer<InstanceData> gInputInstances : register(t0);
StructuredBuffer<MeshInfo> gMeshInfos : register(t1);

RWStructuredBuffer<InstanceData> gOutputInstances : register(u0);
RWStructuredBuffer<DrawIndexedArguments> gOutCommands : register(u1);
RWStructuredBuffer<uint> gCountBuffer : register(u2); // 各バッチごとのカウントを保持

bool IsVisible(InstanceData inst) {
    if (gCullingParams.forceVisible != 0) return true;

    float4x4 world = inst.world;
    float3 minP = inst.aabbMin.xyz;
    float3 maxP = inst.aabbMax.xyz;

    // AABBの8角をワールド空間へ変換 (Row-major: v * M)
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
            // 平面の外側 (d < 0) にあるか判定 (gFrustum.planes は内側向きとする)
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
    
    // モデルインデックスが一致するか再確認
    if (inst.modelIndex != gCullingParams.targetModelIndex) return;

    // カリング判定
    bool visible = IsVisible(inst);

    if (visible) {
        MeshInfo modelInfo = gMeshInfos[inst.modelIndex];
        uint meshCount = modelInfo.meshCount;
        if (meshCount == 0) meshCount = 1; // フォールバック

        // 必要なコマンド数を一括確保
        uint startCmdIdx;
        InterlockedAdd(gCountBuffer[gCullingParams.batchIndex], meshCount, startCmdIdx);
        
        // サブメッシュごとにコマンド生成
        for (uint i = 0; i < meshCount; ++i) {
            uint infoIdx = (modelInfo.meshCount > 0) ? (modelInfo.vertexOffset + i) : inst.modelIndex;
            MeshInfo mesh = gMeshInfos[infoIdx];

            uint outputIdx = startCmdIdx + i;
            uint writeIdx = gCullingParams.batchIndex * 2048 + outputIdx;

            if (outputIdx < 2048) {
                // インスタンスデータは全サブメッシュで共有だが、modelIndex は各サブメッシュの情報を指すように更新
                InstanceData subInst = inst;
                subInst.modelIndex = infoIdx;
                gOutputInstances[writeIdx] = subInst;

                DrawIndexedArguments args;
                args.indexCountPerInstance = mesh.indexCount;
                args.instanceCount = 1;
                args.startIndexLocation = mesh.indexOffset;
                args.baseVertexLocation = 0;
                args.startInstanceLocation = outputIdx; 

                gOutCommands[writeIdx] = args;
            }
        }
    }
}


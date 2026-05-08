#include "../Schema/Schema.hlsli"

struct DrawIndexedArguments {
    uint indexCountPerInstance;
    uint instanceCount;
    uint startIndexLocation;
    int  baseVertexLocation;
    uint startInstanceLocation;
    uint padding[3]; // 32 bytes alignment
};

// バッチごとのメッシュ情報を保持するバッファ
// 0-255: モデルヘッダー, 256+: サブメッシュ情報
StructuredBuffer<MeshInfo> gMeshInfos : register(t0);
StructuredBuffer<uint> gCountBuffer : register(t1);

RWStructuredBuffer<DrawIndexedArguments> gOutCommands : register(u0);

struct BuildParams {
    uint modelIndex;
    uint batchIndex;
    uint maxBatches;
    uint padding;
};
ConstantBuffer<BuildParams> gParams : register(b0);

[numthreads(1, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID) {
    uint batchIdx = gParams.batchIndex;
    uint instanceCount = gCountBuffer[batchIdx];
    
    if (instanceCount == 0) {
        // 描画なしコマンド
        DrawIndexedArguments emptyArgs = (DrawIndexedArguments)0;
        gOutCommands[batchIdx] = emptyArgs;
        return;
    }

    MeshInfo modelHeader = gMeshInfos[gParams.modelIndex];
    
    // サブメッシュ情報の取得 (ParticleSystem::UpdateMeshInfoBuffer の仕様に合わせる)
    // 最初のサブメッシュのみを対象とする
    uint infoIdx = (modelHeader.meshCount > 0) ? modelHeader.vertexOffset : gParams.modelIndex;
    MeshInfo mesh = gMeshInfos[infoIdx];

    DrawIndexedArguments args;
    args.indexCountPerInstance = mesh.indexCount;
    args.instanceCount = instanceCount;
    args.startIndexLocation = mesh.indexOffset;
    args.baseVertexLocation = 0; // シェーダー (CelShader.hlsl 等) 内で inst.vertexOffset を足しているため 0 固定
    args.startInstanceLocation = 0;
    args.padding[0] = 0;
    args.padding[1] = 0;
    args.padding[2] = 0;

    gOutCommands[batchIdx] = args;
}

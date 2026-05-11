#include "../Schema/Schema.hlsli"

struct Vertex {
    float4 position;
    float4 normal;
    float2 uv;
    float2 _pad;
};

ConstantBuffer<SkinningParams> gParams : register(b0);
StructuredBuffer<BoneData> gBones : register(t0);
StructuredBuffer<Vertex> gInputVertices : register(t1);
StructuredBuffer<BoneWeightData> gBoneWeights : register(t2);
RWStructuredBuffer<Vertex> gOutputVertices : register(u0);

[numthreads(64, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID) {
    uint vIdx = DTid.x;
    if (vIdx >= gParams.vertexCount) return;

    uint inIdx = gParams.inputVertexOffset + vIdx;
    uint outIdx = gParams.outputVertexOffset + vIdx;

    Vertex v = gInputVertices[inIdx];

    if (gParams.skinningEnabled != 0) {
        BoneWeightData w = gBoneWeights[inIdx];
        
        float3 skinnedPos = 0;
        float3 skinnedNormal = 0;
        float totalWeight = 0;

        [unroll]
        for (int i = 0; i < 4; ++i) {
            if (w.boneWeights[i] > 0.0001f) {
                float4x4 m = gBones[gParams.boneOffset + w.boneIndices[i]].transform;
                
                // 位置の計算: mul(vector, matrix)
                skinnedPos += mul(float4(v.position.xyz, 1.0f), m).xyz * w.boneWeights[i];
                
                // 法線の計算: 回転成分 (float3x3) のみを使用
                skinnedNormal += mul(v.normal.xyz, (float3x3)m) * w.boneWeights[i];
                
                totalWeight += w.boneWeights[i];
            }
        }

        if (totalWeight > 0.0001f) {
            // W 成分を強制固定して破綻を防ぐ
            v.position = float4(skinnedPos / totalWeight, 1.0f);
            v.normal = float4(normalize(skinnedNormal), 0.0f);
        }
    }

    gOutputVertices[outIdx] = v;
}

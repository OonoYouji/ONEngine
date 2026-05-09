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
        
        float4 skinnedPos = 0;
        float4 skinnedNormal = 0;
        float totalWeight = 0;

        // Bone 0
        if (w.boneWeights.x > 0.0001f) {
            float4x4 m = gBones[gParams.boneOffset + w.boneIndices[0]].transform;
            skinnedPos += mul(v.position, m) * w.boneWeights.x;
            skinnedNormal += mul(v.normal, m) * w.boneWeights.x;
            totalWeight += w.boneWeights.x;
        }
        // Bone 1
        if (w.boneWeights.y > 0.0001f) {
            float4x4 m = gBones[gParams.boneOffset + w.boneIndices[1]].transform;
            skinnedPos += mul(v.position, m) * w.boneWeights.y;
            skinnedNormal += mul(v.normal, m) * w.boneWeights.y;
            totalWeight += w.boneWeights.y;
        }
        // Bone 2
        if (w.boneWeights.z > 0.0001f) {
            float4x4 m = gBones[gParams.boneOffset + w.boneIndices[2]].transform;
            skinnedPos += mul(v.position, m) * w.boneWeights.z;
            skinnedNormal += mul(v.normal, m) * w.boneWeights.z;
            totalWeight += w.boneWeights.z;
        }
        // Bone 3
        if (w.boneWeights.w > 0.0001f) {
            float4x4 m = gBones[gParams.boneOffset + w.boneIndices[3]].transform;
            skinnedPos += mul(v.position, m) * w.boneWeights.w;
            skinnedNormal += mul(v.normal, m) * w.boneWeights.w;
            totalWeight += w.boneWeights.w;
        }

        if (totalWeight > 0.0001f) {
            v.position = skinnedPos;
            v.normal = float4(normalize(skinnedNormal.xyz), 0.0f);
        }
        // totalWeight が 0 の場合は、元の v のまま (Fallback)
    }

    gOutputVertices[outIdx] = v;
}

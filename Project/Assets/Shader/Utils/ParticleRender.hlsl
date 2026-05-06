#include "../Schema/Schema.hlsli"

struct VSOutput {
    float4 position : SV_POSITION;
    float3 worldPos : POSITION;
    float2 uv : TEXCOORD;
    float4 color : COLOR;
    nointerpolation uint particleIdx : TEXCOORD1;
};

struct MeshVertex {
    float4 position;
    float4 normal;
    float2 uv;
    float2 _pad;
};

ConstantBuffer<SceneData> gSceneData : register(b0);
StructuredBuffer<ParticleGPUData> gParticles : register(t0);
StructuredBuffer<MeshVertex> gVertices : register(t1);
StructuredBuffer<uint> gIndices : register(t2);
StructuredBuffer<MeshInfo> gMeshInfos : register(t3);

Texture2D gTextures[] : register(t0, space1);
SamplerState gSampler : register(s0);

float3 rotate_vector(float3 v, float4 q) {
    return v + 2.0 * cross(q.xyz, cross(q.xyz, v) + q.w * v);
}

float4 euler_to_quat(float3 e) {
    float3 c = cos(e * 0.5);
    float3 s = sin(e * 0.5);
    return float4(
        s.x * c.y * c.z - c.x * s.y * s.z,
        c.x * s.y * c.z + s.x * c.y * s.z,
        c.x * c.y * s.z - s.x * s.y * c.z,
        c.x * c.y * c.z + s.x * s.y * s.z
    );
}

[numthreads(128, 1, 1)]
[outputtopology("triangle")]
void ms_main(
    uint gtid : SV_GroupThreadID,
    uint gid : SV_GroupID,
    out vertices VSOutput verts[128],
    out indices uint3 tris[128]
) {
    ParticleGPUData p = gParticles[gid];
    
    uint vCount = 0;
    uint primCount = 0;

    if (p.age < p.maxLifetime) {
        if (p.modelIndex == 0) {
            vCount = 4;
            primCount = 2;
        } else {
            MeshInfo info = gMeshInfos[p.modelIndex];
            vCount = min(info.vertexCount, 128);
            primCount = min(info.indexCount, 128 * 3) / 3;
        }
    }

    SetMeshOutputCounts(vCount, primCount);

    if (vCount > 0) {
        if (p.modelIndex == 0) {
            // --- Sprite (Quad) Mode ---
            if (gtid < 4) {
                float2 uv = float2(gtid % 2, gtid / 2);
                float2 localPos = (uv * 2.0f - 1.0f) * p.scale;
                float3 forward = normalize(gSceneData.cameraPos - p.position);
                float3 right = normalize(cross(float3(0, 1, 0), forward));
                float3 up = cross(forward, right);
                float3 worldPos = p.position + (right * localPos.x) + (up * localPos.y);

                verts[gtid].position = mul(float4(worldPos, 1.0f), gSceneData.viewProj);
                verts[gtid].worldPos = worldPos;
                verts[gtid].uv = uv;
                verts[gtid].color = p.color;
                verts[gtid].particleIdx = gid;
            }
            if (gtid == 0) {
                tris[0] = uint3(0, 1, 2);
                tris[1] = uint3(2, 1, 3);
            }
        } else {
            // --- Mesh Mode ---
            MeshInfo info = gMeshInfos[p.modelIndex];

            if (gtid < vCount) {
                MeshVertex mv = gVertices[info.vertexOffset + gtid];
                float3 pos = mv.position.xyz * p.scale;
                pos = rotate_vector(pos, euler_to_quat(p.rotation));
                pos += p.position;

                verts[gtid].position = mul(float4(pos, 1.0f), gSceneData.viewProj);
                verts[gtid].worldPos = pos;
                verts[gtid].uv = mv.uv;
                verts[gtid].color = p.color;
                verts[gtid].particleIdx = gid;
            }

            if (gtid < primCount) {
                tris[gtid] = uint3(
                    gIndices[info.indexOffset + gtid * 3 + 0],
                    gIndices[info.indexOffset + gtid * 3 + 1],
                    gIndices[info.indexOffset + gtid * 3 + 2]
                );
            }
        }
    }
}

struct PSOutput {
    float4 color : SV_Target0;
    float4 normal : SV_Target1;
    uint2 idFlags : SV_Target2;
};

PSOutput ps_main(VSOutput input) {
    ParticleGPUData p = gParticles[input.particleIdx];
    float4 texColor = gTextures[NonUniformResourceIndex(p.textureIndex)].Sample(gSampler, input.uv);
    
    PSOutput output;
    
    if (p.modelIndex == 0) {
        output.color = texColor * input.color;
        output.normal = float4(0.5f, 0.5f, 1.0f, 1.0f);
    } else {
        float3 lightDir = normalize(float3(1, 1, -1));
        float diff = max(dot(normalize(input.worldPos - gSceneData.cameraPos), -lightDir), 0.5);
        output.color = input.color * diff;
        output.normal = float4(0.5f, 0.5f, 1.0f, 1.0f); // メッシュ用法線の取得は別途必要だが一旦固定
    }
    
    output.idFlags = uint2(p.entityID, p.postProcessFlags);
    return output;
}

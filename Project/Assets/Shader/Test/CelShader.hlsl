#include "../Schema/Buffers.hlsli"
#include "../Utils/Clustered/ClusteredLight.hlsli"

// --- Resource Declarations (Consistent Order with BindlessTest) ---
struct Vertex {
    float4 position;
    float4 normal;
    float2 uv;
    float2 _pad;
};

StructuredBuffer<Vertex> gVertices : register(t0, space0); 
ConstantBuffer<SceneData> gSceneData : register(b0);
ConstantBuffer<BatchData> gBatchData : register(b1);
StructuredBuffer<InstanceData> gInstances : register(t1, space0);
StructuredBuffer<PointLightData> gPointLights : register(t2, space0);
StructuredBuffer<LightGrid> gLightGrid : register(t3, space0);
StructuredBuffer<uint> gLightIndexList : register(t4, space0);
StructuredBuffer<MeshInfo> gMeshInfos : register(t5, space0);

// Bindless Textures (Space 1)
Texture2D gTextures[] : register(t0, space1); 
SamplerState gSampler : register(s0);

// --- VS/PS Structures ---
struct VSOutput {
    float4 position : SV_POSITION;
    float3 worldPos : POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    nointerpolation uint instanceID : TEXCOORD10;
};

struct PSOutput {
    float4 color : SV_Target0;
    float4 normal : SV_Target1;
    uint2 idFlags : SV_Target2;
};

// --- Vertex Shader ---
VSOutput vs_main(uint vID : SV_VertexID, uint iID : SV_InstanceID) {
    VSOutput output;
    
    uint instanceIdx = gBatchData.instanceOffset + iID;
    InstanceData inst = gInstances[instanceIdx];
    
    Vertex v = gVertices[inst.vertexOffset + vID];
    
    float4 worldPos = mul(v.position, inst.world);
    output.position = mul(worldPos, gSceneData.viewProj);
    output.worldPos = worldPos.xyz;
    output.uv = v.uv;
    output.normal = mul(v.normal.xyz, (float3x3)inst.world);
    output.instanceID = instanceIdx;
    
    return output;
}

// --- Pixel Shader ---
PSOutput ps_main(VSOutput input) {
    InstanceData inst = gInstances[input.instanceID];
    
    // --- DEBUG: 一定の色で塗りつぶす (頂点カラー風) ---
    float3 finalRGB = float3(0.7f, 0.7f, 0.8f);
    if (inst.entityID % 2 == 0) finalRGB = float3(0.8f, 0.7f, 0.7f);
    
    // 選択ハイライト
    if (inst.entityID == gSceneData.selectedEntityID) {
        finalRGB += float3(0.4f, 0.4f, 0.0f);
    }

    PSOutput output;
    output.color = float4(finalRGB, 1.0f);
    output.normal = float4(input.normal * 0.5f + 0.5f, 1.0f);
    output.idFlags = uint2(inst.entityID, inst.postProcessFlags);
    
    return output;
}

#include "../Schema/Buffers.hlsli"
#include "../Utils/Clustered/ClusteredLight.hlsli"

// --- Resource Declarations ---
ConstantBuffer<SceneData> gSceneData : register(b0);
StructuredBuffer<InstanceData> gInstances : register(t1);
StructuredBuffer<PointLightData> gPointLights : register(t2);
StructuredBuffer<LightGrid> gLightGrid : register(t3);
StructuredBuffer<uint> gLightIndexList : register(t4);
StructuredBuffer<MeshInfo> gMeshInfos : register(t5);

// Bindless Textures (Space 1)
Texture2D gTextures[] : register(t0, space1); 
SamplerState gSampler : register(s0);

// Geometry Pool
struct Vertex {
    float4 position;
    float4 normal;
    float2 uv;
    float2 _pad;
};
StructuredBuffer<Vertex> gVertices : register(t0); 

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
    
    // インスタンスデータを取得 (SV_InstanceID は 0..instanceCount-1)
    InstanceData inst = gInstances[iID];
    
    // 頂点取得 (inst.vertexOffset を使用)
    Vertex v = gVertices[inst.vertexOffset + vID];
    
    float4 worldPos = mul(v.position, inst.world);
    output.position = mul(worldPos, gSceneData.viewProj);
    output.worldPos = worldPos.xyz;
    output.uv = v.uv;
    output.normal = mul(v.normal.xyz, (float3x3)inst.world);
    output.instanceID = iID;
    
    return output;
}

// --- Helper Functions ---
float SmoothStepThreshold(float v, float threshold, float smoothness) {
    return smoothstep(threshold - smoothness, threshold + smoothness, v);
}

// --- Pixel Shader ---
PSOutput ps_main(VSOutput input) {
    InstanceData inst = gInstances[input.instanceID];
    
    // ベースカラーを初期値にする
    float4 finalColor = inst.baseColor;

    // テクスチャがあれば乗算する (テクスチャが真っ黒な場合に備え、一旦 0.5 を加算してデバッグ表示)
    if (inst.textureIndex != 0xFFFFFFFF) {
        float4 texColor = gTextures[NonUniformResourceIndex(inst.textureIndex)].Sample(gSampler, input.uv);
        finalColor.rgb *= (texColor.rgb + 0.1f); // テクスチャが黒くても微かに見えるように
    }

    PSOutput output;
    output.color = float4(finalColor.rgb, 1.0f);
    output.normal = float4(input.normal * 0.5f + 0.5f, 1.0f);
    output.idFlags = uint2(inst.entityID, inst.postProcessFlags);
    
    return output;
}

#include "../Schema/Schema.hlsli"
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
    float4 texColor = gTextures[NonUniformResourceIndex(inst.textureIndex)].Sample(gSampler, input.uv);

    float3 normal = normalize(input.normal);
    float3 viewDir = normalize(gSceneData.cameraPos - input.worldPos);
    
    float3 diffuseTotal = 0.0f;
    float3 specularTotal = 0.0f;
    
    // アニメ調パラメータ
    float shadowThreshold = 0.4f;
    float shadowSmoothness = 0.02f;
    float3 shadowColorMul = float3(0.6f, 0.6f, 0.8f);

    // Directional Light
    {
        float3 lightDir = normalize(-gSceneData.dirLightDirection);
        float3 lightColor = gSceneData.dirLightColor * gSceneData.dirLightIntensity;
        float halfLambert = dot(normal, lightDir) * 0.5f + 0.5f;
        float lightIntensity = SmoothStepThreshold(halfLambert, shadowThreshold, shadowSmoothness);
        diffuseTotal += lerp(lightColor * shadowColorMul, lightColor, lightIntensity);

        float3 halfDir = normalize(lightDir + viewDir);
        specularTotal += lightColor * step(0.5f, pow(max(dot(normal, halfDir), 0.0f), 64.0f));
    }
    
    // Clustered Point Lights
    uint clusterX = (uint)(input.position.x / (gSceneData.screenWidth / CLUSTER_GRID_X));
    uint clusterY = (uint)(input.position.y / (gSceneData.screenHeight / CLUSTER_GRID_Y));
    
    float viewZ = mul(float4(input.worldPos, 1.0f), gSceneData.view).z;
    uint clusterZ = (uint)(max(0.0f, log(viewZ / gSceneData.nearZ) / log(gSceneData.farZ / gSceneData.nearZ)) * CLUSTER_GRID_Z);
    
    clusterX = min(clusterX, CLUSTER_GRID_X - 1);
    clusterY = min(clusterY, CLUSTER_GRID_Y - 1);
    clusterZ = min(clusterZ, CLUSTER_GRID_Z - 1);
    
    uint clusterIdx = clusterX + clusterY * CLUSTER_GRID_X + clusterZ * CLUSTER_GRID_X * CLUSTER_GRID_Y;
    LightGrid grid = gLightGrid[clusterIdx];

    for (uint i = 0; i < grid.count; ++i) {
        uint lightIdx = gLightIndexList[grid.offset + i];
        PointLightData light = gPointLights[lightIdx];
        
        float3 lightVec = light.position - input.worldPos;
        float dist = length(lightVec);
        float3 lightDir = normalize(lightVec);
        float attenuation = saturate(1.0f - (dist / light.radius));
        attenuation *= attenuation;
        
        float halfLambert = dot(normal, lightDir) * 0.5f + 0.5f;
        float lightIntensity = SmoothStepThreshold(halfLambert, shadowThreshold, shadowSmoothness);
        diffuseTotal += lerp(light.color * light.intensity * shadowColorMul, light.color * light.intensity, lightIntensity) * attenuation;
    }
    
    float3 ambient = 0.2f * inst.baseColor.rgb;
    float3 finalRGB = texColor.rgb * inst.baseColor.rgb * (diffuseTotal + ambient) + specularTotal;
    
    PSOutput output;
    output.color = float4(finalRGB, texColor.a * inst.baseColor.a);
    output.normal = float4(normal * 0.5f + 0.5f, 1.0f);
    output.idFlags = uint2(inst.entityID, inst.postProcessFlags);
    
    return output;
}

struct Vertex {
    float4 position; // 16 bytes
    float4 normal;   // 16 bytes
    float2 uv;       // 8 bytes
    float2 _pad;     // 8 bytes (Total 48 bytes)
};

struct VSOutput {
    float4 position : SV_POSITION;
    float3 worldPos : POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    nointerpolation uint instanceID : TEXCOORD10; // SV_InstanceID から変更
};

#include "../Schema/Schema.hlsli"
#include "../Utils/Clustered/ClusteredLight.hlsli"

ConstantBuffer<SceneData> gSceneData : register(b0);
StructuredBuffer<InstanceData> gInstances : register(t1, space0);
StructuredBuffer<PointLightData> gPointLights : register(t2, space0);

// クラスタライト用バッファ
StructuredBuffer<LightGrid> gLightGrid : register(t3, space0);
StructuredBuffer<uint> gLightIndexList : register(t4, space0);

// --- Bindless Resources ---
Texture2D gTextures[] : register(t0, space1); 
SamplerState gSampler : register(s0);

// --- Proper Mesh Resources ---
StructuredBuffer<Vertex> gVertices : register(t0, space0); 

VSOutput vs_main(uint vID : SV_VertexID, uint iID : SV_InstanceID) {
    VSOutput output;
    InstanceData inst = gInstances[iID];
    
    // vID は既に絶対的な頂点インデックス (オフセット込み) なので直接使用する
    Vertex v = gVertices[vID];
    
    float4 worldPos = mul(v.position, inst.world);
    output.position = mul(worldPos, gSceneData.viewProj);
    output.worldPos = worldPos.xyz;
    output.uv = v.uv;
    output.normal = mul(v.normal.xyz, (float3x3)inst.world);
    output.instanceID = iID;
    
    return output;
}

struct PSOutput {
    float4 color : SV_Target0;
    float4 normal : SV_Target1;
    uint2 idFlags : SV_Target2;
};

// セルルック用の階調化関数
float Posterize(float v, int steps) {
    return floor(v * (float)steps) / max((float)steps - 1.0f, 1.0f);
}

// 滑らかなステップ関数 (境界を少しぼかす)
float SmoothStepThreshold(float v, float threshold, float smoothness) {
    return smoothstep(threshold - smoothness, threshold + smoothness, v);
}

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

    // --- Directional Light ---
    {
        float3 lightDir = normalize(-gSceneData.dirLightDirection);
        float3 lightColor = gSceneData.dirLightColor * gSceneData.dirLightIntensity;
        float halfLambert = dot(normal, lightDir) * 0.5f + 0.5f;
        float lightIntensity = SmoothStepThreshold(halfLambert, shadowThreshold, shadowSmoothness);
        diffuseTotal += lerp(lightColor * shadowColorMul, lightColor, lightIntensity);

        float3 halfDir = normalize(lightDir + viewDir);
        specularTotal += lightColor * step(0.5f, pow(max(dot(normal, halfDir), 0.0f), 64.0f));
    }
    
    // --- Clustered Point Lights ---
    // 1. クラスタインデックスの計算
    uint clusterX = (uint)(input.position.x / (gSceneData.screenWidth / CLUSTER_GRID_X));
    uint clusterY = (uint)(input.position.y / (gSceneData.screenHeight / CLUSTER_GRID_Y));
    
    // Zクラスタ (指数分割の逆算)
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
    
    // 環境光
    float3 ambient = 0.2f * inst.baseColor.rgb;

    float3 finalRGB = texColor.rgb * inst.baseColor.rgb * (diffuseTotal + ambient) + specularTotal;
    
    PSOutput output;
    output.color = float4(finalRGB, texColor.a * inst.baseColor.a);
    output.normal = float4(normal * 0.5f + 0.5f, 1.0f);
    output.idFlags = uint2(inst.entityID, inst.postProcessFlags);
    
	return output;
}

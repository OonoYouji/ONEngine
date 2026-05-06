struct Vertex {
    float3 position;
    float3 normal;
    float2 uv;
};

struct VSOutput {
    float4 position : SV_POSITION;
    float3 worldPos : POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    nointerpolation uint instanceID : SV_InstanceID;
};

// --- Bindless Resources ---
Texture2D gTextures[] : register(t0, space1); 
SamplerState gSampler : register(s0);

// --- Proper Mesh Resources ---
StructuredBuffer<Vertex> gVertices : register(t0, space0); 

#include "../Schema/Schema.hlsli"

ConstantBuffer<SceneData> gSceneData : register(b0);
StructuredBuffer<InstanceData> gInstances : register(t1, space0);
StructuredBuffer<PointLightData> gPointLights : register(t2, space0);

VSOutput vs_main(uint vID : SV_VertexID, uint iID : SV_InstanceID) {
    VSOutput output;
    
    // SV_InstanceID を直接インデックスとして使用 (ExecuteIndirectのオフセットが含まれる)
    InstanceData inst = gInstances[iID];
    
    // vID は既に絶対的な頂点インデックス (オフセット込み) なので直接使用する
    Vertex v = gVertices[vID];
    
    // Position
    float4 worldPos = mul(float4(v.position, 1.0f), inst.world);
    output.position = mul(worldPos, gSceneData.viewProj);
    output.worldPos = worldPos.xyz;
    output.uv = v.uv;
    
    // Normal
    output.normal = mul(v.normal.xyz, (float3x3)inst.world);
    output.instanceID = iID;
    
    return output;
}

struct PSOutput {
    float4 color : SV_Target0;
    float4 normal : SV_Target1;
    uint2 idFlags : SV_Target2;
};

PSOutput ps_main(VSOutput input) {
    InstanceData inst = gInstances[input.instanceID];
    float4 texColor = gTextures[NonUniformResourceIndex(inst.textureIndex)].Sample(gSampler, input.uv);

    float3 normal = normalize(input.normal);
    float3 viewDir = normalize(gSceneData.cameraPos - input.worldPos);
    
    float3 diffuseTotal = 0;
    float3 specularTotal = 0;
    
    // --- Directional Light ---
    {
        float3 lightDir = normalize(-gSceneData.dirLightDirection);
        float3 lightColor = gSceneData.dirLightColor * gSceneData.dirLightIntensity;
        
        // Diffuse (Lambert)
        diffuseTotal += lightColor * max(dot(normal, lightDir), 0.0f);
        
        // Specular (Blinn-Phong)
        float3 halfDir = normalize(lightDir + viewDir);
        specularTotal += lightColor * pow(max(dot(normal, halfDir), 0.0f), 32.0f);
    }
    
    // --- Point Lights ---
    for (uint i = 0; i < gSceneData.numPointLights; ++i) {
        PointLightData light = gPointLights[i];
        float3 lightVec = light.position - input.worldPos;
        float dist = length(lightVec);
        float3 lightDir = normalize(lightVec);
        
        // 減衰計算
        float attenuation = saturate(1.0f - (dist / light.radius));
        attenuation *= attenuation; // 2乗減衰
        
        float3 lightIntensity = light.color * light.intensity * attenuation;
        
        // Diffuse
        diffuseTotal += lightIntensity * max(dot(normal, lightDir), 0.0f);
        
        // Specular
        float3 halfDir = normalize(lightDir + viewDir);
        specularTotal += lightIntensity * pow(max(dot(normal, halfDir), 0.0f), 32.0f);
    }
    
    // 環境光
    float3 ambient = 0.1f * inst.baseColor.rgb;

    float3 finalRGB = texColor.rgb * inst.baseColor.rgb * (diffuseTotal + ambient) + specularTotal;
    
    PSOutput output;
    output.color = float4(finalRGB, texColor.a * inst.baseColor.a);
    output.normal = float4(normal * 0.5f + 0.5f, 1.0f); // 0.0~1.0 範囲にマッピング
    output.idFlags = uint2(inst.entityID, inst.postProcessFlags);
    
	return output;
}

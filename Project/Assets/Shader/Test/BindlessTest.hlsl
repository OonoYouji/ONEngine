struct Vertex {
    float3 position;
    float2 uv;
    float3 normal;
};

struct VSOutput {
    float4 position : SV_POSITION;
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

VSOutput vs_main(uint vID : SV_VertexID, uint iID : SV_InstanceID) {
    VSOutput output;
    Vertex v = gVertices[vID];
    InstanceData inst = gInstances[iID];
    
    float4 worldPos = mul(float4(v.position, 1.0f), inst.world);
    output.position = mul(worldPos, gSceneData.viewProj);
    output.uv = v.uv;
    
    // 法線の変換（回転のみ適用するため world 行列の 3x3 部分を使用）
    output.normal = mul(v.normal, (float3x3)inst.world);
    output.instanceID = iID;
    
    return output;
}

float4 ps_main(VSOutput input) : SV_TARGET {
    InstanceData inst = gInstances[input.instanceID];
    float4 texColor = gTextures[NonUniformResourceIndex(inst.textureIndex)].Sample(gSampler, input.uv);

    // ライティング計算
    float3 normal = normalize(input.normal);
    float3 lightDir = normalize(-gSceneData.dirLightDirection); // ライトの向きの逆
    
    // Lambert
    float diffuse = max(dot(normal, lightDir), 0.0f);
    float3 lightColor = gSceneData.dirLightColor * gSceneData.dirLightIntensity;
    
    // 環境光 (Ambient) - 簡易的に定数
    float3 ambient = 0.2f;

    float3 finalRGB = texColor.rgb * inst.baseColor.rgb * (lightColor * diffuse + ambient);
    
	return float4(finalRGB, texColor.a * inst.baseColor.a);
}

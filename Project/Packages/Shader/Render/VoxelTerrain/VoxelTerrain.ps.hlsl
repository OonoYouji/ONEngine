#include "VoxelTerrain.hlsli"
#include "../../ConstantBufferData/Material.hlsli"
#include "../../Texture.hlsli"

struct PSOutput {
	float4 color : SV_Target0;
	float4 worldPos : SV_Target1;
	float4 normal : SV_Target2;
	float4 flags : SV_Target3;
};

ConstantBuffer<ConstantBufferMaterial> material : register(b4);
Texture2D<float4> textures[kMaxTextureCount] : register(t2050);
SamplerState textureSampler : register(s1);


// ---------------------------------------------
// Triplanar Sampling
// ---------------------------------------------
float4 SampleTriplanar(Texture2D<float4> tex, float3 worldPos, float3 normal, float tiling)
{
    float3 blend = abs(normal);
    blend = normalize(max(blend, 0.00001));
    blend /= (blend.x + blend.y + blend.z);

    // 各軸UV
    float2 uvX = worldPos.yz * tiling;
    float2 uvY = worldPos.xz * tiling;
    float2 uvZ = worldPos.xy * tiling;

    float4 texX = tex.Sample(textureSampler, uvX);
    float4 texY = tex.Sample(textureSampler, uvY);
    float4 texZ = tex.Sample(textureSampler, uvZ);

    return
        texX * blend.x +
        texY * blend.y +
        texZ * blend.z;
}

float3 SampleTriplanarNormal(Texture2D<float4> tex, float3 worldPos, float3 N, float tiling)
{
    float3 blend = abs(N);
    blend /= (blend.x + blend.y + blend.z);

    float2 uvX = worldPos.yz * tiling;
    float2 uvY = worldPos.xz * tiling;
    float2 uvZ = worldPos.xy * tiling;

    float3 nX = tex.Sample(textureSampler, uvX).xyz * 2 - 1;
    float3 nY = tex.Sample(textureSampler, uvY).xyz * 2 - 1;
    float3 nZ = tex.Sample(textureSampler, uvZ).xyz * 2 - 1;

    // ----- X projection -----
    float3 worldNX = float3(
        nX.z,
        nX.y,
        nX.x
    );

    // ----- Y projection -----
    float3 worldNY = float3(
        nY.x,
        nY.z,
        nY.y
    );

    // ----- Z projection -----
    float3 worldNZ = float3(
        nZ.x,
        nZ.y,
        nZ.z
    );

    float3 result =
        worldNX * blend.x +
        worldNY * blend.y +
        worldNZ * blend.z;

    return normalize(result);
}



PSOutput main(VertexOut input) {
	PSOutput output;

	float3 N = normalize(input.normal);
    float3 worldPos = input.worldPosition.xyz;

    const float tiling = 0.1f; // スケール調整
	float4 terrainColor = SampleTriplanar(textures[material.intValues.z], worldPos, N, tiling);
	output.color = material.baseColor * terrainColor;


    float3 triplanarNormal = SampleTriplanarNormal(textures[material.intValues.w], worldPos, N, tiling);
    output.normal = float4(triplanarNormal, 1);

	output.worldPos = input.worldPosition;
	output.flags = float4(material.intValues.x, material.intValues.y, 0, 1);

	if (output.color.a <= 0.001f) {
		discard;
	}
	
	return output;
}
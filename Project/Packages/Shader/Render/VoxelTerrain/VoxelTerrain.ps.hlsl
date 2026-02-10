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
    blend = normalize(max(blend, 0.00001));
    blend /= (blend.x + blend.y + blend.z);

    float2 uvX = worldPos.yz * tiling;
    float2 uvY = worldPos.xz * tiling;
    float2 uvZ = worldPos.xy * tiling;

    float3 nX = tex.Sample(textureSampler, uvX).xyz * 2 - 1;
    float3 nY = tex.Sample(textureSampler, uvY).xyz * 2 - 1;
    float3 nZ = tex.Sample(textureSampler, uvZ).xyz * 2 - 1;

    // ----- Projection basis -----

    // X projection
    float3 tX = float3(0, 0, 1);
    float3 bX = float3(0, 1, 0);
    float3 nBaseX = float3(1, 0, 0);

    float3 worldNX =
        nX.x * tX +
        nX.y * bX +
        nX.z * nBaseX;

    // Y projection
    float3 tY = float3(1, 0, 0);
    float3 bY = float3(0, 0, 1);
    float3 nBaseY = float3(0, 1, 0);

    float3 worldNY =
        nY.x * tY +
        nY.y * bY +
        nY.z * nBaseY;

    // Z projection
    float3 tZ = float3(1, 0, 0);
    float3 bZ = float3(0, 1, 0);
    float3 nBaseZ = float3(0, 0, 1);

    float3 worldNZ =
        nZ.x * tZ +
        nZ.y * bZ +
        nZ.z * nBaseZ;

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
    output.normal = float4(-N, 1);

	output.worldPos = input.worldPosition;
	output.flags = float4(material.intValues.x, material.intValues.y, 0, 1);

	if (output.color.a <= 0.001f) {
		discard;
	}
	
	return output;
}
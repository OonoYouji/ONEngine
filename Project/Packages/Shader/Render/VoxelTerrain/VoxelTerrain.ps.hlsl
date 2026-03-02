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
ConstantBuffer<ConstantBufferMaterial> cliffMaterial : register(b5);
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
    // 各軸のブレンド率を計算
    float3 blend = abs(N);
    blend /= (blend.x + blend.y + blend.z);

    // UVのサンプリング
    float2 uvX = worldPos.yz * tiling;
    float2 uvY = worldPos.xz * tiling;
    float2 uvZ = worldPos.xy * tiling;

    // 法線マップからサンプリング (0~1 -> -1~1)
    // Z成分を再構成（もし法線マップが2チャンネルなら）するか、xyzを使用
    float3 nX = tex.Sample(textureSampler, uvX).xyz * 2.0 - 1.0;
    float3 nY = tex.Sample(textureSampler, uvY).xyz * 2.0 - 1.0;
    float3 nZ = tex.Sample(textureSampler, uvZ).xyz * 2.0 - 1.0;

    // 各平面における法線の向きを、元の法線 N の符号に合わせて修正
    // これを行わないと、反対側の面で法線が逆向きに計算されてしまいます
    float3 axisSign = sign(N);
    
    // 各軸の法線を接線空間からワールド空間的に変換
    // 符号 (axisSign) を掛けることで、裏側の面でも正しく摂動が乗るようになります
    nX.z *= axisSign.x;
    nY.z *= axisSign.y;
    nZ.z *= axisSign.z;

    // 合成 (UDN Blend 簡易版)
    // 元の法線 N に対して、各平面のディテール (xy成分) をブレンド
    float3 worldNX = float3(nX.z, nX.y, nX.x); // X面
    float3 worldNY = float3(nY.x, nY.z, nY.y); // Y面
    float3 worldNZ = float3(nZ.x, nZ.y, nZ.z); // Z面

    // 最終的な法線の合成と正規化
    float3 result = normalize(
        worldNX * blend.x +
        worldNY * blend.y +
        worldNZ * blend.z
    );

    return result;
}




PSOutput main(VertexOut input) {
	PSOutput output;

	float3 N = normalize(input.normal);
    float3 worldPos = input.worldPosition.xyz;

    float cliffFactor = saturate((1.0 - abs(N.y) - 0.2) * 2.0);

    const float tiling = 0.1f; // スケール調整
	float4 terrainColor = SampleTriplanar(textures[material.intValues.z], worldPos, N, tiling);
    float4 cliffColor = SampleTriplanar(textures[cliffMaterial.intValues.z], worldPos, N, tiling);
    terrainColor = lerp(material.baseColor * terrainColor, cliffMaterial.baseColor * cliffColor, cliffFactor);
	output.color = terrainColor;

    float3 terrainNormal = SampleTriplanarNormal(textures[material.intValues.w], worldPos, N, tiling);
    float3 cliffNormal = SampleTriplanarNormal(textures[cliffMaterial.intValues.w], worldPos, N, tiling);
    float3 blendedNormal = normalize(lerp(terrainNormal, cliffNormal, cliffFactor));
    output.normal = float4(blendedNormal, 1);

	output.worldPos = input.worldPosition;
	output.flags = float4(material.intValues.x, material.intValues.y, 0, 1);

	if (output.color.a <= 0.001f) {
		discard;
	}
	
	return output;
}
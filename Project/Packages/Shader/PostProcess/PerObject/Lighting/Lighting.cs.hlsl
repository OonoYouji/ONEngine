#include "../../../ConstantBufferData/ViewProjection.hlsli"
#include "../../../ConstantBufferData/Material.hlsli"

struct DirectionalLight {
	float4 position;
	float4 color;
	float3 direction;
	float intensity;
};

//StructuredBuffer<DirectionalLight> lights : register(t0);
ConstantBuffer<DirectionalLight> light : register(b0);
ConstantBuffer<Camera> camera : register(b1);


/// texture
Texture2D<float4> colorTex : register(t0);
Texture2D<float4> positionTex : register(t1);
Texture2D<float4> normalTex : register(t2);
Texture2D<float4> flagsTex : register(t3);
TextureCube<float4> environmentTexture : register(t4); /// 環境マップ
RWTexture2D<float4> outputTex : register(u0);
SamplerState textureSampler : register(s0);

 

[numthreads(16, 16, 1)]
void main(uint3 dispatchId : SV_DispatchThreadID) {

	float2 texCoord = float2(dispatchId.x / 1920.0f, dispatchId.y / 1080.0f);
	float4 color = colorTex.Sample(textureSampler, texCoord);
	float4 flags = flagsTex.Sample(textureSampler, texCoord);

	/// ライティング処理なし
	if (!IsPostEffectEnabled((int) flags.x, PostEffectFlags_Lighting)) {
		outputTex[dispatchId.xy] = color;
		return;
	}
	
	float4 position = positionTex.Sample(textureSampler, texCoord);
	float4 normal = normalTex.Sample(textureSampler, texCoord);
	
	/// lighting
	float3 outputColor = float3(0.0f, 0.0f, 0.0f);

	float3 toEye = float3(normalize(camera.position - position).xyz);
	float3 halfVector = normalize(-light.direction + toEye);
	float3 reflectLight = reflect(light.direction, normalize(normal.xyz));
	float NdotH = dot(normalize(normal.xyz), halfVector);
	float specularPow = pow(saturate(NdotH), 32.0f);

	float NdotL = dot(normalize(normal.xyz), -light.direction);
	float cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
	float3 diffuse = (color * light.color * cos * light.intensity).rgb;
	float3 specular = light.color.rgb * light.intensity * specularPow;

	outputColor = diffuse + specular;
	
	if (flags.y == 1) {
		float3 cameraToPosition = normalize(position.xyz - camera.position.xyz);
		float3 reflectedVector = reflect(cameraToPosition, normal.xyz);
		float3 environmentColor = environmentTexture.Sample(textureSampler, reflectedVector).rgb;
		outputColor += environmentColor;
	}
	
	outputTex[dispatchId.xy] = float4(outputColor, 1.0f);
	
	
}
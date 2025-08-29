#include "../../../ConstantBufferData/ViewProjection.hlsli"
#include "../../../ConstantBufferData/Material.hlsli"

struct DirectionalLight {
	float4 position;
	float4 color;
	float3 direction;
	float intensity;
};

ConstantBuffer<DirectionalLight> light : register(b0);
ConstantBuffer<Camera> camera : register(b1);

Texture2D<float4> colorTex : register(t0);
Texture2D<float4> positionTex : register(t1);
Texture2D<float4> normalTex : register(t2);
Texture2D<float4> flagsTex : register(t3);
TextureCube<float4> environmentTexture : register(t4); /// 環境マップ
RWTexture2D<float4> outputTex : register(u0);
SamplerState textureSampler : register(s0);

 
float3 HalfLambertReflectance(float3 _color, float3 _normal) {
	float3 resultColor = float3(1, 1, 1);
	
	float NdotL = dot(normalize(_normal), -light.direction);
	float cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
	resultColor = _color * light.color.rgb * cos * light.intensity;
	
	return resultColor;
}

float3 EnvironmentReflection(float3 _position, float3 _normal) {
	float3 cameraToPosition = normalize(_position - camera.position.xyz);
	float3 reflectedVector = reflect(cameraToPosition, _normal);
	return environmentTexture.Sample(textureSampler, reflectedVector).rgb;
}


[numthreads(16, 16, 1)]
void main(uint3 dispatchId : SV_DispatchThreadID) {

	float2 texCoord = float2(dispatchId.xy + 0.5f) / float2(1920.0f, 1080.0f);
	float4 color = colorTex.Sample(textureSampler, texCoord);
	float4 position = positionTex.Sample(textureSampler, texCoord);
	float4 normal = normalTex.Sample(textureSampler, texCoord);
	float4 flags = flagsTex.Sample(textureSampler, texCoord);
	
	/// ライティング処理なし
	if (!IsPostEffectEnabled((int) flags.x, PostEffectFlags_Lighting)) {
		float3 outputColor = color.rgb;
		/// 天球の環境反射を適用する
		if (IsPostEffectEnabled((int) flags.x, PostEffectFlags_EnvironmentReflection)) {
			outputColor += EnvironmentReflection(position.xyz, normal.xyz);
		}

		outputTex[dispatchId.xy] = float4(outputColor, 1.0f);
		return;
	}
	
	/// lighting
	float3 outputColor = float3(1, 1, 1);
	
	outputColor = HalfLambertReflectance(color.rgb, normal.xyz);
	

	/// 天球の環境反射を適用する
	if (IsPostEffectEnabled((int) flags.x, PostEffectFlags_EnvironmentReflection)) {
		outputColor += EnvironmentReflection(position.xyz, normal.xyz);
	}
	
	outputTex[dispatchId.xy] = float4(outputColor, 1.0f);
	
	
}
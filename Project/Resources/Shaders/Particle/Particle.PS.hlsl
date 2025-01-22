#include "Particle.hlsli"

#include "../Material/Material.hlsli"
#include "../Light/DirectionalLight.hlsli"
#include "../Light/PointLight.hlsli"
#include "../Camera/Camera.hlsli"

ConstantBuffer<Material>           gMaterial : register(b0);
ConstantBuffer<CameraPosition>     gCamera   : register(b1);

Texture2D<float4>                  gTexture  : register(t0);
SamplerState                       gSampler  : register(s0);

StructuredBuffer<DirectionalLight> gDirLights : register(t1);
StructuredBuffer<PointLight>       gPointLights : register(t2);



PSOutput main(VSOutput input) {
	PSOutput output;

	float2 texcoord = mul(float3(input.texcoord, 1), (float3x3) gMaterial.uvTransform).xy;
	float4 texColor = gTexture.Sample(gSampler, texcoord);
	float3 shaderColor = float3(0.0f, 0.0f, 0.0f);
	
	/// Half Lambert
	if (gMaterial.isLighting) {
		/// Half Lambert
		/// Directional Light
		for (int i = 0; i < 5; ++i) {
			
			if (gDirLights[i].active != 0) {
				float3 toEye = normalize(gCamera.position - input.worldPosition);
				float3 halfVector = normalize(-gDirLights[i].direction + toEye);
				float3 reflectLight = reflect(gDirLights[i].direction, normalize(input.normal));
				float NdotH = dot(normalize(input.normal), halfVector);
				float specularPow = pow(saturate(NdotH), gMaterial.shininess);

				float NdotL = dot(normalize(input.normal), -gDirLights[i].direction);
				float cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
				float3 diffuse = (gMaterial.color * texColor * gDirLights[i].color * cos * gDirLights[i].intensity).rgb;
				float3 specular = gDirLights[i].color.rgb * gDirLights[i].intensity * specularPow * float3(1.0f, 1.0f, 1.0f);

				shaderColor += diffuse + specular;
			}
		}

		/// Point Light
		for (i = 0; i < 5; ++i) {

			/// lightが有効の時のみ計算する
			if (gPointLights[i].active != 0) {
				float3 toEye = normalize(gCamera.position - input.worldPosition);
				float3 pointLightDirection = normalize(gPointLights[i].position - input.worldPosition);
				float distance = length(gPointLights[i].position - input.worldPosition);
				float factor = pow(saturate(-distance / gPointLights[i].radius + 1.0f), gPointLights[i].decay);

				float3 reflectLight = reflect(pointLightDirection, normalize(input.normal));
				float NdotH = dot(normalize(input.normal), normalize(pointLightDirection + toEye));
				float specularPow = pow(saturate(NdotH), gMaterial.shininess);

				float NdotL = dot(normalize(input.normal), pointLightDirection);
				float cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
				float3 diffuse = (gMaterial.color * texColor * gPointLights[i].color * cos * gPointLights[i].intensity).rgb;
				float3 specular = gPointLights[i].color.rgb * gPointLights[i].intensity * specularPow * float3(1.0f, 1.0f, 1.0f);

				//shaderColor += gPointLights[i].color.rgb * gPointLights[i].intensity * factor;
				shaderColor += diffuse + specular;
			}
		}

		output.color.rgb = shaderColor;
		output.color.a = gMaterial.color.a * texColor.a;
	} else {
		output.color   = gMaterial.color * texColor;
	}
	
	/// pixelの破棄
	if (output.color.a == 0.0f) {
		discard;
	}

	return output;
}
#include "Model.hlsli"
#include "../Light/DirectionalLight.hlsli"
#include "../Material/Material.hlsli"
#include "../Camera/Camera.hlsli"

ConstantBuffer<Material>         gMaterial : register(b0);
ConstantBuffer<DirectionalLight> gDirLight : register(b1);
ConstantBuffer<CameraPosition>   gCamera   : register(b2);

Texture2D<float4>                gTexture  : register(t0);
SamplerState                     gSampler  : register(s0);


PSOutput main(VSOutput input) {
	PSOutput output;

	float2 texcoord = mul(float3(input.texcoord, 1), (float3x3) gMaterial.uvTransform).xy;
	float4 texColor = gTexture.Sample(gSampler, texcoord);

	///- Half Lambert
	if (gMaterial.isLighting) {

		float3 toEye        = normalize(gCamera.position - input.worldPosition);
		float3 halfVector   = normalize(-gDirLight.direction + toEye);
		float3 reflectLight = reflect(gDirLight.direction, normalize(input.normal));
		float NdotH         = dot(normalize(input.normal), halfVector);
		float specularPow   = pow(saturate(NdotH), gMaterial.shininess);

		float NdotL         = dot(normalize(input.normal), -gDirLight.direction);
		float cos           = pow(NdotL * 0.5f + 0.5f, 2.0f);
		float3 diffuse      = (gMaterial.color * texColor * gDirLight.color * cos * gDirLight.intensity).rgb;
		float3 specular     = gDirLight.color.rgb * gDirLight.intensity * specularPow * float3(1.0f, 1.0f, 1.0f);

		output.color.rgb    = diffuse + specular;
		output.color.a      = gMaterial.color.a * texColor.a;

	} else {
		output.color        = gMaterial.color * texColor;
	}
	
	/// pixelの破棄
	if (output.color.a == 0.0f) {
		discard;
	}

	return output;
}
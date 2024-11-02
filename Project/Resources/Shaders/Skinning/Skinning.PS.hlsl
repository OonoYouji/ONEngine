#include "Skinning.hlsli"


PSOutput main(VSOutout input) {
	PSOutput output;

	float NdotL = dot(normalize(input.normal), normalize(float3(1, -1, 1)));
	float cos = pow(NdotL * 0.5f + 0.5f, 2.0f);

	output.color = float4(1, 1, 1, 1) * cos;
	output.color.a = 1.0f;

	return output;
}
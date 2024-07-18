#include "Model.hlsli"

//struct Transform {
//	float4x4 matWorld;
//};

//struct ViewProjection {
//	float4x4 matVp;
//};


//ConstantBuffer<Transform> gTransform;
//ConstantBuffer<ViewProjection> gViewProjection;

VSOutput main(VSInput input) {
	VSOutput output;

	output.position = input.position;
	//output.position = mul(input.position, gTransform.matWorld);
	//output.texcoord = input.texcoord;
	//output.normal = normalize(mul(float4(input.normal, 1), gTransform.matWorld)).xyz;

	return output;
}
#include "Model.hlsli"


struct Material {
	float4 color;
};

ConstantBuffer<Material> gMaterial : register(b0);

float4 main(VSOutput input) : SV_TARGET0 {
	return gMaterial.color;
}
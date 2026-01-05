#include "VoxelTerrain.hlsli"
#include "../../ConstantBufferData/ViewProjection.hlsli"

ConstantBuffer<ViewProjection> viewProjection : register(b0);

VSOutput main(Vertex input) {
	VSOutput output;

	output.position = mul(input.position, viewProjection.matVP);
	output.worldPos = input.position;
	output.normal = input.normal;

	return output;
}
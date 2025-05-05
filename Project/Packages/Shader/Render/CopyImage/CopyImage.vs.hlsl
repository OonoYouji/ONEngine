#include "CopyImage.hlsli"

static const float4 positions[3] = {
	float4(-1, -1, 0, 1),
	float4(3, 1, 0, 1),
	float4(-1, 3, 0, 1)
};

static const float2 texcoords[3] = {
	{ 0, 0 },
	{ 2, 0 },
	{ 0, 2 }
};

VSOutput main(uint id : SV_VertexID) {
	VSOutput output;
	output.position = positions[id];
	output.texcoord = texcoords[id];
	return output;
}
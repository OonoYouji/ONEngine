struct VSInput {
	float4 position : POSITION0;
	float2 texcoord : TEXCOORD0;
	float3 normal   : NORMAL0;
	float4 weight   : WEIGHT0;
	int4   index    : INDEX0;
};

struct VSOutput {
	float4 position : SV_Position;
	float2 texcoord : TEXCOORD0;
	float3 normal   : NORMAL0;
};

struct PSOutput {
	float4 color : SV_TARGET0;
};

struct Skinned {
	float4 position;
	float3 normal;
};

struct Well {
	float4x4 matSkeletonSpace;
	float4x4 matSkeletonSpaceInverseTranspose;
};

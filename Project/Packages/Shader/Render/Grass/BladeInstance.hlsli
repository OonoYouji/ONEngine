#include "Grass.hlsli"
#include "../../ConstantBufferData/ViewProjection.hlsli"

struct StartIndex {
	uint value;
};


StructuredBuffer<BladeInstance> bladeInstances : register(t0);
StructuredBuffer<StartIndex> startIndices : register(t1);
ConstantBuffer<ViewProjection> viewProjection : register(b0);

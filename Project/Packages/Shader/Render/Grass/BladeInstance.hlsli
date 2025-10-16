#include "Grass.hlsli"

struct StartIndex {
	uint value;
};


StructuredBuffer<BladeInstance> bladeInstances : register(t0);
StructuredBuffer<StartIndex> startIndices : register(t1);

cbuffer constants : register(b1) {
	uint startIndex;
	uint currentInstanceCount;
};

#include "Grass.hlsli"

StructuredBuffer<BladeInstance> bladeInstances : register(t0);
cbuffer constants : register(b1) {
	uint startIndex;
	uint currentInstanceCount;
};

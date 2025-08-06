struct TerrainVertex {
	float4 position;
	float3 normal;
	float2 uv;
	float4 splatBlend;
	int index;
};

struct TerrainInfo {
	int entityId;
};

struct TerrainSize {
	uint terrainWidth;
	uint terrainHeight;
};

static const float maxHeight = 255.0f;
static const float minHeight = -255.0f;

float GetHeight(float height) {
	return clamp(height, minHeight, maxHeight);
}

float NormalizeHeight(float height) {
	height = clamp(height, minHeight, maxHeight);
	return (height - minHeight) / (maxHeight - minHeight);
}

float DenormalizeHeight(float normalizedHeight) {
	return minHeight + normalizedHeight * (maxHeight - minHeight);
}
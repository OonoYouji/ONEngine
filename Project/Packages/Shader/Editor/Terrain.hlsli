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

float3 CalculateNormal(uint _index, RWStructuredBuffer<TerrainVertex> _vertices, int _width, int _height) {
	int x = _index % _width;
	int z = _index / _width;

	if (x <= 0 || x >= _width - 1 || z <= 0 || z >= _height - 1) {
		return float3(0.0f, 1.0f, 0.0f);
	}

	float3 left = _vertices[_index - 1].position.xyz;
	float3 right = _vertices[_index + 1].position.xyz;
	float3 down = _vertices[_index - _width].position.xyz;
	float3 up = _vertices[_index + _width].position.xyz;

	float3 dx = right - left;
	float3 dz = up - down;

	return normalize(cross(dz, dx));
}
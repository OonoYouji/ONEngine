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

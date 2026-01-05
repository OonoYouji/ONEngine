#pragma once

/// engine
#include "Engine/Core/Utility/Utility.h"
#include "Engine/Asset/Assets/Mesh/Mesh.h"
#include "Engine/Graphics/Buffer/StructuredBuffer.h"


namespace ONEngine {

class VoxelTerrainChunk {
	friend class VoxelTerrain;
public:

	struct Vertex {
		Vector4 position;
		Vector3 normal;
	};

	//using TerrainMesh = Mesh<Vertex>;

public:

	VoxelTerrainChunk();
	~VoxelTerrainChunk();



private:

	StructuredBuffer<Vertex> rwVertices_;
	uint32_t vertexCount_;

};

} /// namespace ONEngine
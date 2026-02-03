#pragma once

/// engine
#include "../../Interface/IComponent.h"
#include "Engine/Core/Utility/Utility.h"

namespace ONEngine {

/// ///////////////////////////////////////////////////
/// ボクセル地形のコライダーコンポーネント
/// ///////////////////////////////////////////////////
class VoxelTerrainCollider : public IComponent {
public:
	
	struct Mesh {
		std::vector<Vector3> vertices;
		std::vector<uint32_t> indices;
	};

	struct BVHNode {
		Cube bounds;
		uint32_t firstTriangle;
		uint32_t triangleCount;
		BVHNode* leftNode;
		BVHNode* rightNode;
	};

	struct ChunkCollider {
		Mesh mesh;
		BVHNode* bvhRoot;
	};


public:
	/// =========================================
	/// public : methods
	/// =========================================

	VoxelTerrainCollider();
	~VoxelTerrainCollider();

private:
	/// =========================================
	/// private : objects
	/// =========================================


};


}
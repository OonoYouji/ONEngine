#include "Block.h"

/// engine
#include <Engine/Entity/Collection/EntityCollection.h>
#include <Engine/Component/RendererComponents/Mesh/MeshRenderer.h>

/// user
#include "../EntityConfig/EntityConfig.h"


/// ///////////////////////////////////////////////////
/// Block
/// ///////////////////////////////////////////////////

Block::Block() {}
Block::~Block() {}

void Block::Initialize() {
	/// メッシュの生成
	MeshRenderer* meshRenderer = AddComponent<MeshRenderer>();
	meshRenderer->SetMeshPath("Assets/Models/primitive/cube.obj");
	meshRenderer->SetTexturePath("Assets/Textures/white.png");

}

void Block::Update() {

	MeshRenderer* meshRenderer = GetComponent<MeshRenderer>();

	switch (type_) {
	case static_cast<int>(Type::black):
		meshRenderer->SetColor(Vector4(0.0f, 0.0f, 0.0f, 1.0f));
		break;
	case static_cast<int>(Type::white):
		meshRenderer->SetColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
		break;
	}

}



/// ///////////////////////////////////////////////////
/// BlockManager
/// ///////////////////////////////////////////////////

BlockManager::BlockManager() {}
BlockManager::~BlockManager() {}

void BlockManager::Initialize() {


	/// 仮のマップデータ
	std::vector<std::vector<int>> mapData = {
		{ 1, 1, 1, 1 },
		{ 1, 0, 0, 1 },
		{ 1, 0, 0, 1 },
		{ 1, 1, 1, 1 }
	};

	CreateBlock(mapData);

}

void BlockManager::Update() {


}

void BlockManager::CreateBlock(const std::vector<std::vector<int>>& _mapData) {

	/// 元々のブロックをクリア
	blocks_.clear();

	/// マップデータのサイズを取得
	const size_t kMaxRow = _mapData.size();
	const size_t kMaxCol = _mapData[0].size();


	for (size_t r = 0; r < kMaxRow; r++) {

		blocks_.push_back(std::vector<Block*>());

		for (size_t c = 0; c < kMaxCol; c++) {

			/// ブロックの生成
			Block* block = entityCollection_->GenerateEntity<Block>();

			/// ブロックの位置を設定
			block->SetPosition(Vector3(c * 2.0f, 0.0f, r * 2.0f));
			block->SetType(_mapData[r][c]);

			/// ブロックの二次元配列に追加
			blocks_[r].push_back(block);

		}
	}


}

#include "BlockManager.h"

/// engine
#include "Engine/Entity/Collection/EntityCollection.h"

BlockManager::BlockManager() {}
BlockManager::~BlockManager() {}

void BlockManager::Initialize() {

	LoadStage(
		{
			{ 1, 0, 0, 0, 1 },
			{ 0, 1, 0, 1, 0 },
			{ 0, 0, 1, 0, 0 },
			{ 0, 0, 0, 1, 0 },
			{ 0, 1, 0, 0, 1 },
		}
	);

}

void BlockManager::Update() {



}



void BlockManager::LoadStage(const std::vector<std::vector<int>>& _stageMap) {

	blockVerticalNum_   = _stageMap.size();
	blockHorizontalNum_	= _stageMap[0].size();

	/// 現在のブロックを削除
	for (auto& blockRow : blocks_) {
		for (auto& block : blockRow) {
			entityCollection_->RemoveEntity(block);
		}
	}

	blocks_.resize(blockVerticalNum_);
	for (auto& block : blocks_) {
		block.resize(blockHorizontalNum_);
	}

	/// _stageMapのxを反転
	std::vector<std::vector<int>> stageMap = _stageMap;
	std::reverse(stageMap.begin(), stageMap.end());

	/// 新しいブロックを生成
	for (size_t r = 0; r < blockVerticalNum_; ++r) {
		for (size_t c = 0; c < blockHorizontalNum_; ++c) {
			auto& block = blocks_[r][c];
			block = entityCollection_->GenerateEntity<Block>();
			block->SetPosition(Vector3(
				static_cast<float>(c) * 2.0f,
				-1.0f,
				static_cast<float>(r) * 2.0f
			));

			block->SetType(stageMap[r][c]);
		}
	}

}

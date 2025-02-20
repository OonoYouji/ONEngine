#include "BlockManager.h"

/// engine
#include "Engine/Entity/Collection/EntityCollection.h"

BlockManager::BlockManager() {}
BlockManager::~BlockManager() {}

void BlockManager::Initialize() {

	blocks_.resize(blockHorizontalNum_);
	for (auto& block : blocks_) {
		block.resize(blockVerticalNum_);
	}

	
	for (size_t r = 0; r < blockHorizontalNum_; ++r) {
		for (size_t c = 0; c < blockVerticalNum_; ++c) {

			auto& block = blocks_[r][c];
			block = entityCollection_->GenerateEntity<Block>();
			block->SetPosition(Vector3(
				static_cast<float>(r) * 2.0f,
				0.0f,
				static_cast<float>(c) * 2.0f
			));
		}
	}

}

void BlockManager::Update() {}

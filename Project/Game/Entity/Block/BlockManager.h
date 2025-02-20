#pragma once

/// std
#include <vector>

/// engine
#include "Engine/Entity/Interface/IEntity.h"

/// user
#include "Block.h"

/// ==================================================================================
/// Block クラスの管理クラス
/// ==================================================================================
class BlockManager final : public IEntity {
public:
	
	BlockManager();
	~BlockManager();

	void Initialize() override;
	void Update()     override;


private:

	size_t blockHorizontalNum_ = 10;
	size_t blockVerticalNum_   = 10;

	std::vector<std::vector<Block*>> blocks_;

};


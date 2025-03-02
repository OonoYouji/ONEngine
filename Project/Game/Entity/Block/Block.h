#pragma once

/// std
#include <vector>

/// engine
#include <Engine/Entity/Interface/IEntity.h>

/// ///////////////////////////////////////////////////
/// Block
/// ///////////////////////////////////////////////////
class Block : public IEntity{
public:

	/// ==================================================
	/// public : methods
	/// ==================================================

	Block();
	~Block();

	void Initialize() override;
	void Update()     override;

private:

	/// ==================================================
	/// private : objects
	/// ==================================================

	int type_; ///< ブロックの種類


public:

	/// ==================================================
	/// public : accessors
	/// ==================================================

	/// @brief ブロックのタイプをセット
	/// @param _type 
	void SetType(int _type) { type_ = _type; }

};



/// ///////////////////////////////////////////////////
/// BlockManager
/// ///////////////////////////////////////////////////
class BlockManager : public IEntity {
public:
	
	/// ==================================================
	/// public : methods
	/// ==================================================
	
	BlockManager();
	~BlockManager();
	
	void Initialize() override;
	void Update()     override;


	/// @brief csvなどのマップデータからブロックを生成
	/// @param _mapData ブロックの配置データ
	void CreateBlock(const std::vector<std::vector<int>>& _mapData);

private:

	/// ==================================================
	/// private : objects
	/// ==================================================

	std::vector<std::vector<Block*>> blocks_; ///< ブロックの二次元配列


};
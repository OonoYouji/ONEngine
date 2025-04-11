#pragma once

/// std
#include <vector>
#include <utility>

/// engine
//#include <Engine/ECS/Entity/Interface/IEntity.h>
#include <Engine/ECS/EntityComponentSystem/EntityComponentSystem.h>

/// user
#include "../EntityConfig/EntityConfig.h"

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


	/// @brief このブロックのタイプを取得
	/// @return ブロックのタイプを返す
	int GetType() const { return type_; }

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


	/// @brief 引数のアドレスがステージ内か確認し、ステージ内ならばそのブロックの種類を返す
	/// @param _address 確認したいアドレス
	/// @return ステージ内かどうか、ブロックの種類
	std::pair<bool, int> CheckAddress(const Address& _address) const;

private:

	/// ==================================================
	/// private : objects
	/// ==================================================

	std::vector<std::vector<Block*>> blocks_; ///< ブロックの二次元配列


};
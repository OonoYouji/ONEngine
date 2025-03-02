#pragma once

/// engine
#include "Engine/Entity/Interface/IEntity.h"

/// user
#include "../EntityConfig/EntityConfig.h" 

/// ///////////////////////////////////////////////////
/// Player
/// ///////////////////////////////////////////////////
class Player : public IEntity {
public:
	
	/// ===================================================
	/// public : methods
	/// ===================================================

	Player();
	~Player();

	void Initialize() override;
	void Update()     override;

	/// @brief 移動先をセットする
	/// @param _address 移動先のアドレス
	void Move(const Address& _address);

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	int     type_;           ///< プレイヤーのタイプ

	Address currentAddress_; ///< 現在のアドレス
	Address nextAddress_;    ///< 次のアドレス

	float moveAnimationTime_; ///< 移動アニメーションの時間
	float currentMoveTime_;   ///< 現在の移動時間
	bool  isAnimation_;       ///< アニメーション中か
	bool  isMove_;            ///< 移動中か

public:

	/// ===================================================
	/// public : accessor
	/// ===================================================

	/// @brief 次のステージ上のアドレスをセット
	/// @param _address ステージ上のどこにいるかを示すアドレス
	void SetNextAddress(const Address& _address) { nextAddress_ = _address; }

	/// @brief 現在のアドレスを更新する
	/// @param _address ステージ上のどこにいるかを示すアドレス
	void SetCurrentAddress(const Address& _address) { currentAddress_ = _address; }


	/// @brief 現在のアドレスを取得
	/// @return 現在のアドレス
	const Address& GetCurrentAddress() const { return currentAddress_; }

};


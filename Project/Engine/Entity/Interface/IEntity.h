#pragma once


/// ===================================================
/// エンティティインターフェース
/// ===================================================
class IEntity {
public:

	/// @brief 仮想デストラクタ
	virtual ~IEntity() {}

	/// @brief 初期化の純粋仮想関数
	virtual void Initialize() = 0;

	/// @brief 更新の純粋仮想関数
	virtual void Update()     = 0;

	/// @brief 描画の純粋仮想関数
	virtual void Draw()       = 0;

protected:

};


#pragma once

#include <GameObjectManager.h>

/// ===================================================
/// 前方宣言
/// ===================================================
class Player;


/// ===================================================
/// 竜巻を表現するクラス
/// ===================================================
class Tornado final : public BaseGameObject {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	Tornado() { CreateTag(this); }
	~Tornado() {}

	void Initialize() override;
	void Update()     override;
	void Debug()      override;


	void SetPlayer(Player* _player);

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	Player* pPlayer_ = nullptr;

};
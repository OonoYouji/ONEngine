#pragma once

#include <GameObjectManager.h>


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

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	Transform pivot_;

};
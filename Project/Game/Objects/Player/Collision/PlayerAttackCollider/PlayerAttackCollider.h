#pragma once

#include "GameObjectManager/BaseGameObject.h"

class PlayerAttackCollider : public BaseGameObject {
public:

	enum MODE {
		MODE_WEAK_ATTACK,	  /// 弱攻撃のSRT
		MODE_STRONG_ATTACK, /// 強攻撃のSRT
		COUNT
	};

public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	PlayerAttackCollider(class Player* _player, class GameCamera* _gameCamera);
	~PlayerAttackCollider();

	void Initialize() override;
	void Update()     override;
	void Debug()      override;

	void OnCollisionEnter(BaseGameObject* const _collision) override;

	void AddVariables();
	void LoadVariables();
	void ApplyVariables();

	
	/// <summary>
	/// 当たり判定のモードを切り替える
	/// </summary>
	/// <param name="_mode"></param>
	void SetMode(int _mode);

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	class BoxCollider* boxCollider_ = nullptr;

	class Player*     pPlayer_ = nullptr;
	class GameCamera* pGameCamera_ = nullptr;

	bool isCollisionEnter_ = false;
	bool isCollisionStay_ = false;

	int mode_;

	Transform weakAttackTransform_;
	Transform strongAttackTransform_;

};

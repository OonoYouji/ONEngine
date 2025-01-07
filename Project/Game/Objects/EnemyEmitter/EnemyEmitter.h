#pragma once

/// std
#include <list>

#include "GameObjectManager/BaseGameObject.h"

/// ===================================================
/// EnemyEmitterのクラス
/// ===================================================
class EnemyEmitter : public BaseGameObject {
public:

	/// ===================================================
	/// public : sub class 
	/// ===================================================

	/// <summary>
	/// Emitterの設定
	/// </summary>
	struct Config {
		float    activionTime;
		uint32_t emitEnemyNum;
		float    radius;
	};


public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	EnemyEmitter(class Player* _player, class EnemyManager* _manager, class TrackingCamera* _camera);
	~EnemyEmitter();

	void Initialize() override;
	void Update()     override;
	void Debug()      override;


	void Start();

	void Emit();

	void AddVariables();
	void ApplyVariables();


private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	class MeshRenderer* meshRenderer_ = nullptr;
	class Player*       pPlayer_      = nullptr;
	class EnemyManager* pManager_     = nullptr;
	class TrackingCamera* pTrackingCamera_ = nullptr;

	Config config_;

	bool  isStart_;
	bool  isEnd_;
	float animationTime_;
	float maxAnimationTime_;

	float minScale_;

	std::list<class Enemy*> enemyList_;
	
public:

	/// ===================================================
	/// public : accessor
	/// ===================================================

	const Config& GetConfig() const { return config_; }

	void SetConfig(const Config& _config) { config_ = _config; }

	std::list<class Enemy*>& GetEnemyList() { return enemyList_; }

	bool GetIsEnd() const { return isEnd_; }

};

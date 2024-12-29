#pragma once

/// std
#include <list>

/// engine
#include "GameObjectManager/BaseGameObject.h"

/// user
#include "../EnemyEmitter/EnemyEmitter.h"


/// ===================================================
/// Enemyの管理クラス
/// ===================================================
class EnemyManager : public BaseGameObject {

	using Emitter = EnemyEmitter;
	
	/// <summary>
	/// 
	/// </summary>
	struct EmitterData {
		Vec3            position;
		Emitter::Config config;
	};

public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	EnemyManager();
	~EnemyManager();

	void Initialize() override;
	void Update()     override;
	void Debug()      override;

	void EmitterEdit();


	/// <summary>
	/// Enemyを生成する
	/// </summary>
	/// <param name="_position"></param>
	void GenerateEnemy(const Vec3& _position);


private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	std::list<class Enemy*>        enemyList_;
	std::list<class EnemyEmitter*> enemyEmitterList_;



};

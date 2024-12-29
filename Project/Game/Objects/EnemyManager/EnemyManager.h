#pragma once

/// std
#include <list>
#include <vector>

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


private:

	/// ===================================================
	/// private : methods
	/// ===================================================

	/// <summary>
	/// Emitterの編集
	/// </summary>
	void EmitterEdit();

	/// <summary>
	/// EmitterDataのImGuiデバッグ
	/// </summary>
	/// <param name="_data"></param>
	void EmitterDataImGuiDebug(EmitterData& _data);


private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	std::list<class Enemy*>        enemyList_;
	std::list<class EnemyEmitter*> enemyEmitterList_;

	EmitterData              sourceEmitterData_;
	std::vector<EmitterData> emitterDatas_;

};

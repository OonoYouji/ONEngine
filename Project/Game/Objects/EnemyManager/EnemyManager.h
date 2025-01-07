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

	EnemyManager(class Player* _playerPtr);
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

	/// <summary>
	/// EmitterDataをJsonファイルに保存
	/// </summary>
	void EmitterDataSaveToJsonFile(const std::string& _directoryPath, bool _isDrawPopupWindow);

	/// <summary>
	/// JsonファイルからEmitterDataを読み込む
	/// </summary>
	void EmitterDataLoadFromJsonFile(const std::string& _directoryPath, bool _isDrawPopupWindow);

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	class Player* pPlayer_ = nullptr;

	std::list<class Enemy*>        enemyList_;
	std::list<class EnemyEmitter*> enemyEmitterList_;


	const std::string        directoryPath_ = "./Resources/Parameters/Objects/EnemyManager.json";
	EmitterData              sourceEmitterData_;
	std::vector<EmitterData> emitterDatas_; /// jsonに保存する用

	uint32_t defeatEnemiesCount_ = 0;

public:

	/// ===================================================
	/// public : accessor
	/// ===================================================

	const std::list<class Enemy*>& GetEnemyList() const { return enemyList_; }

};

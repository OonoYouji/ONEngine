#pragma once

/// std
#include <vector>
#include <string>

/// engine
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"


/// ===================================================
/// 各シーンの基底クラス
/// ===================================================
class IScene {
	friend class SceneManager;
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	virtual ~IScene() = default;

	/// @brief シーンの初期化
	virtual void Initialize() = 0;

	/// @brief シーンの更新
	virtual void Update()     = 0;

private:

	/// ===================================================
	/// private : methods
	/// ===================================================

	/// @brief scene manager へのポインタをセットする
	/// @param _sceneManager 
	void SetSceneManagerPtr(class SceneManager* _sceneManager);

	/// @brief entity collection へのポインタをセットする
	/// @param _entityCollection 
	void SetEntityComponentSystem(EntityComponentSystem* _pEntityComponentSystem);


protected:

	/// ===================================================
	/// protected : objects
	/// ===================================================
	
	EntityComponentSystem*   pEntityComponentSystem_ = nullptr;
	std::vector<std::string> loadResourcePaths_;
	std::vector<std::string> unloadResourcePaths_;


public:

	/// ===================================================
	/// public : accessor
	/// ===================================================

	/// @brief このシーンに切り替わるときに読み込むリソースのパスを取得する
	/// @return リソースのパス配列
	const std::vector<std::string>& GetLoadResourcePaths() const {
		return loadResourcePaths_;
	}

	/// @brief このシーンに切り替わるときにアンロードするリソースのパスを取得する
	/// @return リソースのパス配列
	const std::vector<std::string>& GetUnloadResourcePaths() const {
		return unloadResourcePaths_;
	}


private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	class SceneManager* pSceneManager_     = nullptr;

};

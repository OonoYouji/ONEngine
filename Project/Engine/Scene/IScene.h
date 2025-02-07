#pragma once

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
	void SetEntityCollectionPtr(class EntityCollection* _entityCollection);


protected:

	/// ===================================================
	/// protected : methods
	/// ===================================================
	

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	class SceneManager*     pSceneManager_     = nullptr;
	class EntityCollection* pEntityCollection_ = nullptr;

};


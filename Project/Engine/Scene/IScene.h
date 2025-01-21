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

	virtual void Initialize() = 0;
	virtual void Update()     = 0;

private:

	/// ===================================================
	/// private : methods
	/// ===================================================

	/// <summary>
	/// SceneManagerのポインタを設定する
	/// </summary>
	/// <param name="_sceneManager"> : SceneManagerへのポインタ </param>
	void SetSceneManagerPtr(class SceneManager* _sceneManager);


protected:

	/// ===================================================
	/// protected : methods
	/// ===================================================
	

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	class SceneManager* pSceneManager_ = nullptr;


};


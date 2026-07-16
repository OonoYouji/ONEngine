#pragma once

/// std
#include <memory>
#include <vector>
#include <string>

/// externals
#include <mono/jit/jit.h>

/// engine
#include "Loader/SceneIO.h"

namespace ONEngine {
class EntityComponentSystem;
}

namespace ONEngine::Asset {
class AssetCollection;
}



/// ///////////////////////////////////////////////////
/// シーンの管理を行うクラス
/// ///////////////////////////////////////////////////
namespace ONEngine {

/**
 * @class SceneManager
 * @brief シーンの遷移、保存、ロードを管理するクラス
 */
class SceneManager final {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	/**
	 * @brief コンストラクタ
	 * @param _entityCompnentSystem ECS管理クラスへのポインタ
	 */
	SceneManager(EntityComponentSystem* _entityCompnentSystem);

	/**
	 * @brief デストラクタ
	 */
	~SceneManager();

	/**
	 * @brief シーンマネージャーの初期化を行います。
	 * @param _assetCollection アセットコレクションへのポインタ
	 */
	void Initialize(Asset::AssetCollection* _assetCollection);

	/**
	 * @brief シーンの更新（シーン切り替え処理等）を行います。
	 */
	void Update();

	/**
	 * @brief 次のフレームで切り替えるシーンを設定します。
	 * @param _sceneName 次のシーンの名前
	 */
	void SetNextScene(const std::string& _sceneName);

	/**
	 * @brief 指定したECSグループのエンティティをシーン名で保存します。
	 * @param _name シーン名
	 * @param _ecsGroup 保存対象のECSグループ
	 */
	void SaveScene(const std::string& _name, class ECSGroup* _ecsGroup);

	/**
	 * @brief 現在のシーンをディスクに保存します。
	 */
	void SaveCurrentScene();
	/**
	 * @brief 現在のシーンをメモリ上に一時保存します。
	 */
	void SaveCurrentSceneTemporary();


	/**
	 * @brief シーンを読み込んで現在のECSグループを初期化します。
	 * @param _sceneName ロードするシーン名
	 */
	void LoadScene(const std::string& _sceneName);

	/**
	 * @brief 現在のシーンを再読み込みします。
	 * @param _isTemporary 一時保存されたシーンから読み込む場合はtrue
	 */
	void ReloadScene(bool _isTemporary);


	/**
	 * @brief シーンの入出力を担当するSceneIOのインスタンスを取得します。
	 * @return SceneIOのポインタ
	 */
	SceneIO* GetSceneIO();

	/**
	 * @brief 最後に開かれたシーン名を取得します。履歴ファイルから読み込みます。
	 * @return 最後に開いたシーン名
	 */
	std::string LastOpenSceneName();


	/**
	 * @brief シーンの変更フラグ（Dirtyフラグ）を立てます。
	 */
	void MarkDirty();

	/**
	 * @brief シーンが変更されたかどうかを取得します。
	 * @return 変更されている場合はtrue
	 */
	bool IsDirty() const;

	/**
	 * @brief シーンの変更フラグを明示的に設定します。
	 * @param _isDirty 設定するDirtyフラグの値
	 */
	void SetDirty(bool _isDirty);

private:
	/// ===================================================
	/// private : methods
	/// ===================================================

	/**
	 * @brief 次のシーンへのロードを実行し、現在のシーンとして設定します。
	 * @param _isTemporary 一時的なシーン読み込みの場合はtrue
	 */
	void MoveNextToCurrentScene(bool _isTemporary);


private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	/// ----- other class ----- ///
	EntityComponentSystem* pEcs_;
	Asset::AssetCollection* pAssetCollection_;

	std::string currentScene_;
	std::string nextScene_;

	bool isDirty_ = false;

	std::unique_ptr<SceneIO> sceneIO_;


public:
	/// ===================================================
	/// public : accessor
	/// ===================================================

	/**
	 * @brief 現在のアクティブなシーン名を取得します。
	 * @return 現在のシーン名
	 */
	const std::string& GetCurrentSceneName() const;
};



namespace MonoInternalMethods {

/**
 * @brief C#（Mono）側からシーンをロードするために呼び出される内部メソッド。
 * @param _sceneName ロードするシーン名（MonoString形式）
 */
void InternalLoadScene(MonoString* _sceneName);
}

} /// ONEngine

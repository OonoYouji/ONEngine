#pragma once

#include <wrl/client.h>
#include <d3d12.h>

#include <memory>
#include <string>

class BaseScene;

using namespace Microsoft::WRL;


enum RTVIndex {
	kCurrentScene,
	kDebugScene
};

/// ===================================================
/// シーンの管理クラス
/// ===================================================
class SceneManager final {
	SceneManager() = default;
	~SceneManager() = default;
public:

	/// <summary>
	/// インスタンス確保
	/// </summary>
	static SceneManager* GetInstance();

	/// <summary>
	/// 初期化処理
	/// </summary>
	void Initialize();

	/// <summary>
	/// 終了処理
	/// </summary>
	void Finalize();


	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw();

private:

	std::unique_ptr<BaseScene> scene_ = nullptr;

private:
	SceneManager(const SceneManager&) = delete;
	SceneManager(SceneManager&&) = delete;
	SceneManager& operator= (const SceneManager&) = delete;
	SceneManager& operator= (SceneManager&&) = delete;
};

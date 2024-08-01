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



	/// <summary>
	/// 描画前の処理
	/// </summary>
	void PreDraw();

	/// <summary>
	/// 描画後の処理
	/// </summary>
	void PostDraw();


	void SceneDraw();

	void ImGuiDraw(const std::string& name, RTVIndex index);

	ID3D12Resource* GetResource() const { return renderTargets_[kCurrentScene].Get(); }

private:

	std::unique_ptr<BaseScene> scene_ = nullptr;



	

	ComPtr<ID3D12Resource> renderTargets_[2];
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles_[2];
	D3D12_CPU_DESCRIPTOR_HANDLE srvCPUHandles_[2];
	D3D12_GPU_DESCRIPTOR_HANDLE srvGPUHandles_[2];

private:
	SceneManager(const SceneManager&) = delete;
	SceneManager(SceneManager&&) = delete;
	SceneManager& operator= (const SceneManager&) = delete;
	SceneManager& operator= (SceneManager&&) = delete;
};

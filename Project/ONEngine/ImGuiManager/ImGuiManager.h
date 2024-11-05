#pragma once

#include <d3d12.h>
#include <wrl/client.h>

#include "Externals/imgui/imgui.h"
#include "Externals/imgui/imgui_impl_dx12.h"
#include "Externals/imgui/imgui_impl_win32.h"

#include <cmath>
#include <memory>

#include "GraphicManager/RenderTextureManager/RenderTexture.h"


namespace ONE {
	class WinApp;
	class DxCommon;
	class DxDescriptor;
}

using namespace Microsoft::WRL;


/// ===================================================
/// Imguiの管理クラス
/// ===================================================
class ImGuiManager final {
	ImGuiManager() = default;
	~ImGuiManager() = default;
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	/// <summary>
	/// インスタンス確保
	/// </summary>
	/// <returns></returns>
	static ImGuiManager* GetInstance();



	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(ONE::WinApp* winApp, ONE::DxCommon* dxCommon);

	/// <summary>
	/// 終了処理
	/// </summary>
	void Finalize();



	/// <summary>
	/// フレームの最初に行う処理
	/// </summary>
	void BeginFrame();

	/// <summary>
	/// フレームの最後に行う処理
	/// </summary>
	void EndFrame();


	bool GetIsAcitive() const { return isActive_; }

	RenderTexture* GetRenderTexture() const {
		return renderTexture_.get();
	}

private:

	/// ===================================================
	/// private : methods
	/// ===================================================

	/// <summary>
	/// imguiの見た目の設定
	/// </summary>
	void StyleSetting();

	/// <summary>
	/// カラーコードをimguiのvector4に変換する
	/// </summary>
	ImVec4 ColorToVec4(uint32_t color);


private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	ONE::DxCommon*     pDxCommon_ = nullptr;
	ONE::WinApp*       pWinApp_ = nullptr;

	bool isActive_ = true;

	std::unique_ptr<RenderTexture> renderTexture_ = nullptr;

private:
	ImGuiManager(const ImGuiManager&) = delete;
	ImGuiManager(ImGuiManager&&) = delete;
	ImGuiManager& operator= (const ImGuiManager&) = delete;
	ImGuiManager& operator= (ImGuiManager&&) = delete;
};


#include "Mouse.h"

/// engine
#include "Engine/Core/Utility/Tools/Assert.h"
#include "Engine/Core/ImGui/ImGuiManager.h"
#include "Engine/Core/Window/WindowManager.h"

void Mouse::Initialize(IDirectInput8* _directInput, WindowManager* _windowManager, ImGuiManager* _imGuiManager) {
	pImGuiManager_ = _imGuiManager;
	Assert(pImGuiManager_ != nullptr, "pImGuiManager_ == nullptr");

	HRESULT hr = _directInput->CreateDevice(
		GUID_SysMouse, &mouse_, NULL);

	Assert(SUCCEEDED(hr), "マウスデバイスの生成に失敗しました");

	/// 入力データ形式のセット
	hr = mouse_->SetDataFormat(&c_dfDIMouse2); ///< 標準形式
	Assert(SUCCEEDED(hr), "マウスデバイスの生成に失敗しました");

	/// 排他制御レベルのセット
	/*
		DISCL_FOREGROUND   : 画面が手前にある場合のみ入力を受け付ける
		DISCL_NONEXCLUSIVE : デバイスをこのアプリだけで占有しない
		DISCL_NOWINKEY     : Windowsキーを無効にする
	*/

	hr = mouse_->SetCooperativeLevel(
		_windowManager->GetMainWindow()->GetHwnd(),
		DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY
	);
	Assert(SUCCEEDED(hr), "マウスデバイスの生成に失敗しました");
}

void Mouse::Update(Window* _window) {
	mouse_->SetCooperativeLevel(
		_window->GetHwnd(),
		DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY
	);

	/// マウス情報の取得開始
	preState_ = state_; ///< 前フレームの入力を保存
	mouse_->Acquire();
	mouse_->GetDeviceState(sizeof(state_), &state_);

	POINT mousePos{};
	GetCursorPos(&mousePos);
	ScreenToClient(_window->GetHwnd(), &mousePos);

	position_ = Vector2(
		static_cast<float>(mousePos.x),
		static_cast<float>(mousePos.y)
	);

	velocity_ = Vector2(
		static_cast<float>(state_.lX),
		static_cast<float>(state_.lY)
	);

	wheel_ = static_cast<float>(state_.lZ);
}

const Vector2& Mouse::GetImGuiImageMousePosition(const std::string& _name) {
	const ImGuiSceneImageInfo* imageInfo = pImGuiManager_->GetSceneImageInfo(_name);
	/// imageInfoが見つからない場合は、デフォルトの位置を返す
	if (!imageInfo) {
		static Vector2 defaultPosition(0.0f, 0.0f);
		return defaultPosition;
	}

	// 2. 現在のマウス位置を取得  
	ImVec2 mousePos = ImGui::GetMousePos();

	// 3. 画像の左上の位置 (imageInfo.pos) と画像のサイズ (imageInfo.size) を取得  
	ImVec2 imageMin = imageInfo->position;
	ImVec2 imageMax = ImVec2(
		imageMin.x + imageInfo->size.x,
		imageMin.y + imageInfo->size.y
	);

	// 4. 画像内での相対位置を計算 (画像の左上を基準にした位置)  
	ImVec2 mousePosInImage = ImVec2(
		mousePos.x - imageMin.x,
		mousePos.y - imageMin.y
	);

	// 6. 1280x720のRTVサイズに合わせて正規化  
	imageMousePosition.x = (mousePosInImage.x / imageInfo->size.x) * 1280.0f;
	imageMousePosition.y = (mousePosInImage.y / imageInfo->size.y) * 720.0f;

	return imageMousePosition;
}

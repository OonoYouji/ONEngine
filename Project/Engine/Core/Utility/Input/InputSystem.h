#pragma once

/// std
#include <memory>

/// engine
#include "Engine/Core/DirectX12/ComPtr/ComPtr.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "Gamepad.h"

namespace Editor {
class ImGuiManager;
}

/// //////////////////////////////////////////////////
/// 入力処理クラス
/// //////////////////////////////////////////////////
namespace ONEngine {

/**
 * @class InputSystem
 * @brief DirectInput および XInput 等の各種入力制御を一元管理するシステムクラス
 */
class InputSystem final {
	friend class Input;
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	/**
	 * @brief コンストラクタ
	 */
	InputSystem();

	/**
	 * @brief デストラクタ
	 */
	~InputSystem();

	/**
	 * @brief DirectInputの生成および各入力デバイスのインスタンス生成・初期化を行います。
	 * @param _windowManager ウィンドウ管理クラスポインタ
	 * @param _imGuiManager ImGui管理クラスポインタ
	 */
	void Initialize(class WindowManager* _windowManager, Editor::ImGuiManager* _imGuiManager);

	/**
	 * @brief 毎フレーム呼び出され、全入力デバイスのデータをポーリングして状態を更新します。
	 */
	void Update();

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	/// ----- other class ----- ///
	class WindowManager* pWindowManager_ = nullptr;

	ComPtr<IDirectInput8>     directInput_;
	std::unique_ptr<Keyboard> keyboard_;
	std::unique_ptr<Mouse>    mouse_;
	std::unique_ptr<Gamepad>  gamepad_;

};

namespace MonoInternalMethods {
	/**
	 * @brief C#（Mono）側からゲームパッドの入力値を取得するためのバインド関数。
	 * @param _axisIndex スティックのインデックス（0:左、1:右など）
	 * @param _x X軸入力値を格納するポインタ
	 * @param _y Y軸入力値を格納するポインタ
	 */
	void InternalGetGamepadThumb(int _axisIndex, float* _x, float* _y);

	/**
	 * @brief C#（Mono）側からマウスのフレーム移動量を取得するためのバインド関数。
	 * @param _x X移動量を格納するポインタ
	 * @param _y Y移動量を格納するポインタ
	 */
	void InternalGetMouseVelocity(float* _x, float* _y);

	/**
	 * @brief C#（Mono）側からマウスのスクリーン座標を取得するためのバインド関数。
	 * @param _x X座標を格納するポインタ
	 * @param _y Y座標を格納するポインタ
	 */
	void InternalGetMousePosition(float* _x, float* _y);

	/**
	 * @brief C#（Mono）側からマウスホイールの回転量を取得するためのバインド関数。
	 * @param _wheel ホイール回転量を格納するポインタ
	 */
	void InternalGetMouseWheel(float* _wheel);
}

} /// ONEngine

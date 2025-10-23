#pragma once

/// engine
#include "Keyboard.h"
#include "Mouse.h"
#include "Gamepad.h"

/// ===================================================
/// 入力処理クラス
/// ===================================================
class Input final {
	friend class GameFramework;
	friend class MonoScriptEngine;

	static void Initialize(class WindowManager* _windowManager, class ImGuiManager* _imguiManager);
	static void Update();
	static void Finalize();

public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	/// ----- Keyboard ----- ///

	/// @brief キーが押されているか
	/// @param _key DIK_*** 定数
	/// @return true: 押されている, false: 押されていない
	static bool PressKey(int _key);

	/// @brief キーがトリガーされたか
	/// @param _key DIK_*** 定数
	/// @return true: トリガーされた, false: トリガーされていない
	static bool TriggerKey(int _key);

	/// @brief キーが離されたか
	/// @param _key DIK_*** 定数
	/// @return true: 離された, false: 離されていない
	static bool ReleaseKey(int _key);


	
	/// ----- mouse ----- ///

	/// @brief マウスボタンが押されているか
	/// @param _button Mouse::*** 定数
	/// @return true: 押されている, false: 押されていない
	static bool PressMouse(int _button);

	/// @brief マウスボタンがトリガーされたか
	/// @param _button Mouse::*** 定数
	/// @return true: トリガーされた, false: トリガーされていない
	static bool TriggerMouse(int _button);

	/// @brief マウスボタンが離されたか
	/// @param _button Mouse::*** 定数
	/// @return true: 離された, false: 離されていない
	static bool ReleaseMouse(int _button);


	/// @brief マウスのホイールの回転量を取得
	/// @return 回転量 (上方向: 正の値, 下方向: 負の値)
	static float GetMouseWheel();

	/// @brief マウスの位置を取得
	static const Vector2& GetMousePosition();

	/// @brief マウスの移動量を取得
	static const Vector2& GetMouseVelocity();


	/// @brief ImGuiImage状でのマウス位置を正規化した座標で取得
	/// @param _imageName ImGuiImageの名前
	/// @return 1280x720内でのマウス位置
	static const Vector2& GetImGuiImageMousePosNormalized(const std::string& _imageName);

	/// @brief ImGuiImageの位置を取得
	/// @param _imageName Imageの名前
	static const Vector2& GetImGuiImagePos(const std::string& _imageName);

	/// @brief ImGuiImageのサイズを取得
	/// @param _imageName Imageの名前
	static const Vector2& GetImGuiImageSize(const std::string& _imageName);



	/// ----- gamepad ----- ///

	/// @brief Gamepadのボタンが押されているか
	/// @param _button Gamepad::*** 定数
	/// @return true: 押されている, false: 押されていない
	static bool PressGamepad(int _button);

	/// @brief Gamepadのボタンがトリガーされたか
	/// @param _button Gamepad::*** 定数
	/// @return true: トリガーされた, false: トリガーされていない
	static bool TriggerGamepad(int _button);

	/// @brief Gamepadのボタンが離されたか
	/// @param _button Gamepad::*** 定数
	/// @return rue: 離された, false: 離されていない
	static bool ReleaseGamepad(int _button);


	/// @brief Gamepadの左スティックの値を取得
	static Vector2 GetGamepadLeftThumb();

	/// @brief Gamepadの右スティックの値を取得
	static Vector2 GetGamepadRightThumb();

};


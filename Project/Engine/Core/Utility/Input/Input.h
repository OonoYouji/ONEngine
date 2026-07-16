#pragma once

/// engine
#include "Keyboard.h"
#include "Mouse.h"
#include "Gamepad.h"

namespace Editor {
	class ImGuiManager;
} /// Editor

/// //////////////////////////////////////////////////
/// 入力処理クラス
/// ///////////////////////////////////////////////////
namespace ONEngine {

/**
 * @class Input
 * @brief キーボード、マウス、ゲームパッドからの入力を一括して管理し、情報を提供する静的クラス
 */
class Input final {
	friend class GameFramework;
	friend class MonoScriptEngine;

	/**
	 * @brief 入力システムを初期化します。
	 * @param _windowManager ウィンドウ管理クラスのインスタンス
	 * @param _imguiManager ImGui管理クラスのインスタンス
	 */
	static void Initialize(class WindowManager* _windowManager, Editor::ImGuiManager* _imguiManager);

	/**
	 * @brief 毎フレーム呼び出され、各入力デバイスの入力状態を更新します。
	 */
	static void Update();

	/**
	 * @brief 入力システムの終了処理（デバイスの破棄など）を行います。
	 */
	static void Finalize();

public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	/// ----- Keyboard ----- ///

	/**
	 * @brief 指定したキーが押されているか判定します。
	 * @param _key DIK_*** 定数（DirectInput キーコード）
	 * @return 押されていれば true、それ以外は false
	 */
	static bool PressKey(int _key);

	/**
	 * @brief 指定したキーが今フレームで押された瞬間（トリガー）か判定します。
	 * @param _key DIK_*** 定数
	 * @return 押された瞬間であれば true、それ以外は false
	 */
	static bool TriggerKey(int _key);

	/**
	 * @brief 指定したキーが今フレームで離された瞬間か判定します。
	 * @param _key DIK_*** 定数
	 * @return 離された瞬間であれば true、それ以外は false
	 */
	static bool ReleaseKey(int _key);


	
	/// ----- mouse ----- ///

	/**
	 * @brief 指定したマウスボタンが押されているか判定します。
	 * @param _button Mouse::*** 定数（ボタンインデックス）
	 * @return 押されていれば true、それ以外は false
	 */
	static bool PressMouse(int _button);

	/**
	 * @brief 指定したマウスボタンが今フレームで押された瞬間（トリガー）か判定します。
	 * @param _button Mouse::*** 定数
	 * @return 押された瞬間であれば true、それ以外は false
	 */
	static bool TriggerMouse(int _button);

	/**
	 * @brief 指定したマウスボタンが今フレームで離された瞬間か判定します。
	 * @param _button Mouse::*** 定数
	 * @return 離された瞬間であれば true、それ以外は false
	 */
	static bool ReleaseMouse(int _button);


	/**
	 * @brief マウスホイールの回転量（ホイールの移動量）を取得します。
	 * @return ホイール回転量（奥に回すと正の値、手前に回すと負の値）
	 */
	static float GetMouseWheel();

	/**
	 * @brief マウスの現在のスクリーン座標（ウィンドウ内）を取得します。
	 * @return マウスの座標 (Vector2)
	 */
	static const Vector2& GetMousePosition();

	/**
	 * @brief マウスのフレーム移動量（速度）を取得します。
	 * @return マウス移動量ベクトル (Vector2)
	 */
	static const Vector2& GetMouseVelocity();


	/**
	 * @brief 指定された ImGui 画像ウインドウ内における、マウス位置を正規化した座標（通常 1280x720 基準）で取得します。
	 * @param _imageName 対象の ImGui 画像領域の名前
	 * @return 正規化されたマウス座標 (Vector2)
	 */
	static const Vector2& GetImGuiImageMousePosNormalized(const std::string& _imageName);

	/**
	 * @brief 指定された ImGui 画像ウインドウのスクリーン座標を取得します。
	 * @param _imageName 対象の ImGui 画像領域の名前
	 * @return ウィンドウ位置座標 (Vector2)
	 */
	static const Vector2& GetImGuiImagePos(const std::string& _imageName);

	/**
	 * @brief 指定された ImGui 画像ウインドウのサイズを取得します。
	 * @param _imageName 対象の ImGui 画像領域の名前
	 * @return ウィンドウサイズ (Vector2)
	 */
	static const Vector2& GetImGuiImageSize(const std::string& _imageName);



	/// ----- gamepad ----- ///

	/**
	 * @brief ゲームパッドのボタンが押されているか判定します。
	 * @param _button Gamepad::*** 定数（XINPUT_GAMEPAD_*** 定数など）
	 * @return 押されていれば true、それ以外は false
	 */
	static bool PressGamepad(int _button);

	/**
	 * @brief ゲームパッドのボタンが今フレームで押された瞬間（トリガー）か判定します。
	 * @param _button Gamepad::*** 定数
	 * @return 押された瞬間であれば true、それ以外は false
	 */
	static bool TriggerGamepad(int _button);

	/**
	 * @brief ゲームパッドのボタンが今フレームで離された瞬間か判定します。
	 * @param _button Gamepad::*** 定数
	 * @return 離された瞬間であれば true、それ以外は false
	 */
	static bool ReleaseGamepad(int _button);


	/**
	 * @brief ゲームパッドの左スティックの倒し量ベクトル（各軸 -1.0 〜 1.0）を取得します。
	 * @return 左スティックの入力ベクトル (Vector2)
	 */
	static Vector2 GetGamepadLeftThumb();

	/**
	 * @brief ゲームパッドの右スティックの倒し量ベクトル（各軸 -1.0 〜 1.0）を取得します。
	 * @return 右スティックの入力ベクトル (Vector2)
	 */
	static Vector2 GetGamepadRightThumb();

};


} /// ONEngine

#pragma once

#include "DInput.h"
/// directX12
#include <XInput.h>

/// engine
#include "Engine/Core/DirectX12/ComPtr/ComPtr.h"

/// ///////////////////////////////////////////////////
/// ゲームパッド入力処理クラス
/// ///////////////////////////////////////////////////
namespace ONEngine {

/**
 * @class Gamepad
 * @brief XInputを使用したゲームパッド入力（ボタン、アナログスティック）の取得を制御するクラス
 */
class Gamepad final {
	friend class Input;
public:

	/**
	 * @brief ゲームパッドのボタン定数（XInput値にマッピング）
	 */
	enum Button {
		DPadUp = XINPUT_GAMEPAD_DPAD_UP,         ///< 方向パッド上
		DPadDown = XINPUT_GAMEPAD_DPAD_DOWN,     ///< 方向パッド下
		DPadLeft = XINPUT_GAMEPAD_DPAD_LEFT,     ///< 方向パッド左
		DPadRight = XINPUT_GAMEPAD_DPAD_RIGHT,   ///< 方向パッド右
		Start = XINPUT_GAMEPAD_START,            ///< STARTボタン
		Back = XINPUT_GAMEPAD_BACK,              ///< BACKボタン
		LeftThumb = XINPUT_GAMEPAD_LEFT_THUMB,   ///< 左スティック押し込み
		RightThumb = XINPUT_GAMEPAD_RIGHT_THUMB, ///< 右スティック押し込み
		LeftShoulder = XINPUT_GAMEPAD_LEFT_SHOULDER,   ///< LBボタン
		RightShoulder = XINPUT_GAMEPAD_RIGHT_SHOULDER, ///< RBボタン
		A = XINPUT_GAMEPAD_A, ///< Aボタン
		B = XINPUT_GAMEPAD_B, ///< Bボタン
		X = XINPUT_GAMEPAD_X, ///< Xボタン
		Y = XINPUT_GAMEPAD_Y  ///< Yボタン
	};

	/**
	 * @brief スティック（アナログ入力）軸の識別用列挙型
	 */
	enum Axis {
		Left, ///< 左アナログスティック
		Right ///< 右アナログスティック
	};

public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	/**
	 * @brief コンストラクタ
	 */
	Gamepad();

	/**
	 * @brief デストラクタ
	 */
	~Gamepad();

	/**
	 * @brief ゲームパッド接続の初期状態セットアップ等を行います。
	 * @param _directInput IDirectInput8インスタンスポインタ（XInput主体のため現状未使用）
	 * @param _windowManager ウィンドウ管理クラスのポインタ
	 */
	void Initialize(IDirectInput8* _directInput, class WindowManager* _windowManager);

	/**
	 * @brief 毎フレームゲームパッドの接続状態および最新の入力フレーム（XINPUT_STATE）を取得・保存します。
	 * @param _window 現在アクティブなウィンドウのポインタ
	 */
	void Update(class Window* _window);


private:
	/// ===================================================
	/// private : methods
	/// ===================================================

	ComPtr<IDirectInputDevice8> gamepadDevice_; ///< （フォールバック用）DirectInput デバイス
	
	XINPUT_STATE state_;     ///< 今フレームのゲームパッド状態
	XINPUT_STATE prevState_; ///< 前フレームのゲームパッド状態

	int stickDeadZone_ = 8000; ///< アナログスティックの遊び（デッドゾーン）閾値
};


} /// ONEngine

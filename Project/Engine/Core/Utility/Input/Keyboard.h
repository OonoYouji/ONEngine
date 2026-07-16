#pragma once

/// std
#include <cmath>
#include <array>

/// engine
#include "DInput.h"

/// //////////////////////////////////////////////////
/// キーボード入力処理クラス
/// //////////////////////////////////////////////////
namespace ONEngine {

/**
 * @class Keyboard
 * @brief DirectInputを使用したキーボード入力を処理するクラス
 */
class Keyboard final {
	friend class Input;
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	/**
	 * @brief コンストラクタ
	 */
	Keyboard();

	/**
	 * @brief デストラクタ
	 */
	~Keyboard();

	/**
	 * @brief キーボード入力デバイスの初期化およびデータフォーマット、協調レベルの設定を行います。
	 * @param _directInput IDirectInput8インスタンスのポインタ
	 * @param _windowManager ウィンドウ管理クラスのポインタ
	 */
	void Initialize(IDirectInput8* _directInput, class WindowManager* _windowManager);

	/**
	 * @brief 毎フレームキーボードの最新のキー状態を取得し、前フレームの状態を保存します。
	 * @param _window 現在アクティブなウィンドウのポインタ
	 */
	void Update(class Window* _window);

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	Microsoft::WRL::ComPtr<IDirectInputDevice8> keyboard_; ///< キーボード用 DirectInput デバイス

	std::array<BYTE, 256> keys_;	///< 今フレームの全キー状態（押下されていると非ゼロ値）
	std::array<BYTE, 256> preKeys_; ///< 前フレームの全キー状態

};


} /// ONEngine

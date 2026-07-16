#pragma once

/// directX12
#include <dinput.h>
#include <wrl/client.h>

/// engine
#include "Engine/Core/Utility/Math/Vector2.h"

namespace Editor {
class ImGuiManager;
}

/// /////////////////////////////////////////////
/// Mouseの入力
/// /////////////////////////////////////////////
namespace ONEngine {

/**
 * @class Mouse
 * @brief DirectInputを使用したマウス入力（ボタン、位置、移動量、ホイール）の取得およびImGuiウィンドウと座標連動する機能を持つクラス
 */
class Mouse final {
	friend class Input;
public:
	/// =========================================
	/// public : enum
	/// =========================================

	enum {
		Left,              ///< 左ボタン
		Right,             ///< 右ボタン
		Whell, Center = Whell, ///< 中央ホイール（ホイールクリック）
		Side1,             ///< サイドボタン1
		Side2,             ///< サイドボタン2
		Side3,             ///< サイドボタン3
		Side4,             ///< サイドボタン4
		Count, // 使用禁止
	};

public:
	/// =========================================
	/// public : methods
	/// =========================================

	/**
	 * @brief コンストラクタ
	 */
	Mouse();

	/**
	 * @brief デストラクタ
	 */
	~Mouse();

	/**
	 * @brief マウス入力デバイスの初期化およびデータフォーマット、協調レベルの設定を行います。
	 * @param _directInput IDirectInput8インスタンスのポインタ
	 * @param _windowManager ウィンドウ管理クラスのポインタ
	 * @param _imGuiManager ImGui管理クラスのポインタ
	 */
	void Initialize(IDirectInput8* _directInput, class WindowManager* _windowManager, Editor::ImGuiManager* _imGuiManager);

	/**
	 * @brief 毎フレームマウスの最新の入力状態を取得し、前フレームの情報を保存して各種移動量等を更新します。
	 * @param _window 現在アクティブなウィンドウのポインタ
	 */
	void Update(class Window* _window);


	/**
	 * @brief 特定の ImGui ウインドウ内でのマウス相対位置を、ゲーム画面標準の 1280x720 サイズに正規化した座標で取得します。
	 * @param _name 対象の ImGui ウインドウ（画像領域）名
	 * @return 正規化されたマウス座標 (Vector2)
	 */
	const Vector2& GetImGuiImageMousePosNormalized(const std::string& _name);

	/**
	 * @brief 指定された ImGui 画像ウインドウのスクリーン座標を取得します。
	 * @param _name 対象の ImGui 画像領域名
	 * @return ウィンドウ座標 (Vector2)
	 */
	const Vector2& GetImGuiImagePos(const std::string& _name);

	/**
	 * @brief 指定された ImGui 画像ウインドウのサイズを取得します。
	 * @param _name 対象の ImGui 画像領域名
	 * @return ウィンドウサイズ (Vector2)
	 */
	const Vector2& GetImGuiImageSize(const std::string& _name);


private:
	/// =========================================
	/// private : objects
	/// =========================================

	/// ----- other class ----- ///
	Editor::ImGuiManager* pImGuiManager_; ///< ImGuiManagerへのポインタ


	Microsoft::WRL::ComPtr<IDirectInputDevice8> mouse_; ///< マウス用 DirectInput デバイス

	DIMOUSESTATE2 state_;    ///< 今フレームのマウスデバイス状態
	DIMOUSESTATE2 preState_; ///< 前フレームのマウスデバイス状態

	Vector2 position_; ///< マウスの現在のスクリーン座標 (X, Y)
	Vector2 velocity_; ///< マウスのフレーム移動量 (X, Y)
	float wheel_;      ///< ホイールのフレーム回転移動量

	Vector2 imageMousePosition; ///< キャッシュ用 ImGui 上の正規化マウス座標
	Vector2 imageSize_;        ///< キャッシュ用 ImGui のウィンドウサイズ
};


} /// ONEngine

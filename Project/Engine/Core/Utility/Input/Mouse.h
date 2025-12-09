#pragma once

/// directX12
#include <dinput.h>
#include <wrl/client.h>

/// engine
#include "Engine/Core/Utility/Math/Vector2.h"

/// /////////////////////////////////////////////
/// Mouseの入力
/// /////////////////////////////////////////////
class Mouse final {
	friend class Input;
public:
	/// =========================================
	/// public : enum
	/// =========================================

	enum {
		Left,
		Right,
		Whell, Center = Whell,
		Side1,
		Side2,
		Side3,
		Side4,
		Count, // 使用禁止
	};

public:
	/// =========================================
	/// public : methods
	/// =========================================

	Mouse();
	~Mouse();

	/// @brief 初期化
	/// @param _directInput DirectInput8へのポインタ
	/// @param _windowManager WindowManagerへのポインタ
	/// @param _imGuiManager ImGuiManagerへのポインタ
	void Initialize(IDirectInput8* _directInput, class WindowManager* _windowManager, class ImGuiManager* _imGuiManager);

	/// @brief 更新
	/// @param _window 現在のウィンドウへのポインタ
	void Update(class Window* _window);


	/// @brief Image内でのマウス位置を1280x720に正規化した座標で取得
	/// @param _name Imageの名前
	/// @return 1280x720内でのマウス位置 
	const Vector2& GetImGuiImageMousePosNormalized(const std::string& _name);

	/// @brief Imageの位置を取得
	/// @param _name Imageの名前
	const Vector2& GetImGuiImagePos(const std::string& _name);

	/// @brief Imageのサイズを取得
	/// @param _name Imageの名前
	const Vector2& GetImGuiImageSize(const std::string& _name);


private:
	/// =========================================
	/// private : objects
	/// =========================================

	/// ----- other class ----- ///
	class ImGuiManager* pImGuiManager_; ///< ImGuiManagerへのポインタ


	Microsoft::WRL::ComPtr<IDirectInputDevice8> mouse_;

	DIMOUSESTATE2 state_;
	DIMOUSESTATE2 preState_;

	Vector2 position_;
	Vector2 velocity_;
	float wheel_;

	Vector2 imageMousePosition;
	Vector2 imageSize_;
};


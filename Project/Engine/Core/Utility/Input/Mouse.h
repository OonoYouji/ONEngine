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

	Mouse() = default;
	~Mouse() = default;

	void Initialize(IDirectInput8* _directInput, class WindowManager* _windowManager);
	void Update(class Window* _window);

private:
	/// =========================================
	/// private : objects
	/// =========================================


	Microsoft::WRL::ComPtr<IDirectInputDevice8> mouse_;

	DIMOUSESTATE2 state_;
	DIMOUSESTATE2 preState_;

	Vector2 position_;
	Vector2 velocity_;

};


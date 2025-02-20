#pragma once

/// std
#include <memory>

/// engine
#include "Engine/Core/DirectX12/ComPtr/ComPtr.h"
#include "Keyboard.h"

/// ===================================================
/// 入力処理クラス
/// ===================================================
class InputSystem final {
	friend class Input;
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	InputSystem();
	~InputSystem();

	void Initialize(class WindowManager* _windowManager);
	void Update();


private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	ComPtr<IDirectInput8>     directInput_;
	std::unique_ptr<Keyboard> keyboard_;


};


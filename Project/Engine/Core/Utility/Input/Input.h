#pragma once

/// engine
#include "Keyboard.h"

/// ===================================================
/// 入力処理クラス
/// ===================================================
class Input final {
	friend class GameFramework;

	static void Initialize(class WindowManager* _windowManager);
	static void Update();
	static void Finalize();

public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	static bool PressKey(int _key);
	static bool TriggerKey(int _key);
	static bool ReleaseKey(int _key);


};


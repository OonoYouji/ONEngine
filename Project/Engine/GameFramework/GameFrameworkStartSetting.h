#pragma once

/// std
#include <string>
#include <memory>

/// engine
#include "IGameLoop/IGameLoop.h"

/// lib
#include "Math/Vector2.h"


/// ===================================================
/// GameFrameworkクラスの初期化に使う設定構造体
/// ===================================================
struct GameFrameworkStartSetting final {
	/// window
	std::wstring windowName;
	Vec2         windowSize;

	/// game loop
	IGameLoop*   gameLoop;
};
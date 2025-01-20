#pragma once

/// std
#include <string>
#include <memory>

/// engine
#include "IGameLoop/IGameLoop.h"
#include "Engine/Scene/ISceneFactory.h"

/// lib
#include "Engine/Utility/Math/Vector2.h"


/// ===================================================
/// GameFrameworkクラスの初期化に使う設定構造体
/// ===================================================
struct GameFrameworkConfig final {
	/// window
	std::wstring   windowName;
	Vec2           windowSize;

	/// game loop
	IGameLoop*     gameLoop;
};
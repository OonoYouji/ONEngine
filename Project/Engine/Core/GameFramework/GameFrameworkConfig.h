#pragma once

/// std
#include <string>
#include <memory>

/// engine
#include "Engine/Core/Utility/Math/Vector2.h"


/// ///////////////////////////////////////////////////
/// GameFrameworkクラスの初期化に使う設定構造体
/// ///////////////////////////////////////////////////
struct GameFrameworkConfig final {
	/// window
	std::wstring   windowName;
	Vector2        windowSize;
};
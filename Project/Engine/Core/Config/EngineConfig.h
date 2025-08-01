#pragma once

#include "Engine/Core/Utility/Math/Vector2.h"

#if defined(_DEBUG) || defined(DEBUG_BUILD)
#define DEBUG_MODE
#endif


namespace EngineConfig {
	static const Vector2 kWindowSize = { 1920.0f, 1080.0f };
}

namespace DebugConfig {

	static const Vector2 kDebugWindowSize = Vector2(16.0f, 9.0f) * 110.0f; // 120がmax

	/// @brief デバッグ中かどうか
	extern bool isDebugging; 
	

	enum SELECTED_MODE {
		SELECTED_MODE_FILE,
		SELECTED_MODE_GAME,
		SELECTED_MODE_EDITOR,
	};
	extern int selectedMode_; ///< 選択中のデバッグウィンドウ

}
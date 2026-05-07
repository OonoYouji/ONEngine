#include "EngineConfig.h"

using namespace ONEngine;

namespace ONEngine::DebugConfig {
	/// @brief デバッグ中かどうか
	bool isDebugging = false;
	bool isShowDebugScene = true;
	bool isShowGameScene = false;

	int selectedMode_ = 0; ///< 選択中のデバッグウィンドウ
}

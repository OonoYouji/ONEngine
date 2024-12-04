#pragma once

/// externals
#include <imgui.h>

/// 対象
#include "Objects/Player/Player.h"

class PlayerMotionEditor {
public:

	PlayerMotionEditor(Player* _player);
	~PlayerMotionEditor();

	void Initialize();
	void Finalize();

	/// <summary>
	/// consoleに追加する関数
	/// </summary>
	/// <param name="_windowFlags"></param>
	void ImGuiDebug(ImGuiWindowFlags _windowFlags);

private:

	Player* pPlayer_ = nullptr;

};
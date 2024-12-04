#pragma once

#include "Objects/Player/Player.h"

class PlayerMotionEditor {
public:

	PlayerMotionEditor(Player* _player);
	~PlayerMotionEditor();

	void Initialize();
	void Finalize();

	void ImGuiDebug();

private:


};
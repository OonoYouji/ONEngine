#pragma once

#include <ImGuiManager.h>


class Console final {
public:

	Console() {}
	~Console() {}

	void Initialize();

	void Update();

private:

	void ParentWindow();

	void Inspector();
	void Herarchy();

	void Scene();
	void Debug();

	void Assets();

private:

	ImGuiWindowFlags parentWinFlags_;
	ImGuiWindowFlags imguiWinFlags_;
	bool             imguiWinIsOpen_ = true;

private:
	Console(const Console&)            = delete;
	Console(Console&&)                 = delete;
	Console& operator=(const Console&) = delete;
	Console& operator=(Console&&)      = delete;
};
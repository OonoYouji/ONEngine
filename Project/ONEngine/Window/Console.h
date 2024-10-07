#pragma once

#include <ImGuiManager.h>
#include <SceneManager.h>
#include <GameObjectManager.h>
#include <CommandLineInterface/CommandLineInterface.h>


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

	void PlayControl();

	void CLI();

private:

	ImGuiWindowFlags parentWinFlags_;
	ImGuiWindowFlags imguiWinFlags_;
	bool             imguiWinIsOpen_ = true;

	SceneManager*         pSceneManager_      = nullptr;
	GameObjectManager*    pGameObjectManager_ = nullptr;
	CommandLineInterface* pCLI_               = nullptr;
private:
	Console(const Console&)            = delete;
	Console(Console&&)                 = delete;
	Console& operator=(const Console&) = delete;
	Console& operator=(Console&&)      = delete;
};
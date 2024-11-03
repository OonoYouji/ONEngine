#pragma once

#include "ImGuiManager/ImGuiManager.h"
#include "Scenes/Manager/SceneManager.h"
#include "GameObjectManager/GameObjectManager.h"
#include "CommandManager/CommandLineInterface.h"
#include "VariableManager/VariableManager.h"

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

	void DescriptorHeapsMunuBar();
	void WindowSettingMenuBar();

	void VariableGroupArray();

	void CreateGameObjectMenuBar();

private:

	ImGuiWindowFlags parentWinFlags_;
	ImGuiWindowFlags imguiWinFlags_;
	bool             imguiWinIsOpen_ = true;

	SceneManager*         pSceneManager_      = nullptr;
	GameObjectManager*    pGameObjectManager_ = nullptr;
	CommandLineInterface* pCLI_               = nullptr;
	VariableManager*      pVariableManager_   = nullptr;
private:
	Console(const Console&)            = delete;
	Console(Console&&)                 = delete;
	Console& operator=(const Console&) = delete;
	Console& operator=(Console&&)      = delete;
};
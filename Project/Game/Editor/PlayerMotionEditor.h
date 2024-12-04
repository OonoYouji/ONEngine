#pragma once

/// std
#include <map>
#include <string>

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

	/// <summary>
	/// モーションのエディタ
	/// </summary>
	void AddMotionEdit();


	/// <summary>
	/// モーションの追加
	/// </summary>
	/// <param name="_motionName"></param>
	/// <param name="_value"></param>
	void AddMotion(const std::string& _motionName, int _value);


	void RenderMotionAll();

private:

	Player* pPlayer_ = nullptr;

	std::map<std::string, int> motions_;
	
	std::string addMotionName_;
	int addMotionValue_;
};


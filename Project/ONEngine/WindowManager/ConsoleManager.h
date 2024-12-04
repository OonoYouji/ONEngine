#pragma once

/// std
#include <functional>

/// externals
#include <imgui.h>




/// ===================================================
/// public : sub class
/// ===================================================

using DebugFunction = std::function<void(ImGuiWindowFlags)>;
using MenuFunction = std::function<void()>;



/// ===================================================
/// コンソールの管理クラス
/// ===================================================
class ConsoleManager final {
	ConsoleManager() {}
	~ConsoleManager() {}
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	static ConsoleManager* GetInstance() {
		static ConsoleManager instance;
		return &instance;
	}

	void Initialize();
	void Finalize();
	void Update();

private:

	/// ===================================================
	/// private : methods
	/// ===================================================

	/// <summary>
	/// 親windowの表示
	/// </summary>
	void ParentWindow();

	/// <summary>
	/// 子windowのimgui window flagsを切り替えるmenuを表示
	/// </summary>
	void ChildWindowSetting();

	
	
	/*##########################################################
		TODO : COMMENT
		ここに持たせるべき関数ではないので引っ越し先を探す
	##########################################################*/

	void DescriptorHeapUsedCount();


private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	std::vector<DebugFunction> debugFunctions_;
	std::vector<MenuFunction>  menuFunctions_;

	ImGuiWindowFlags           imguiWindowFlags_;    /// 各window
	ImGuiWindowFlags           parentImGuiWindowFlags_; /// 親window



public:

	/// ===================================================
	/// public : accessor
	/// ===================================================

	/// <summary>
	/// デバッグ関数を登録する
	/// </summary>
	/// <param name="_function"></param>
	void RegisterFunction(DebugFunction _function);

	/// <summary>
	/// メニューの表示関数を設定
	/// </summary>
	/// <param name="_menuFunction"></param>
	void RegisterMenuFunction(MenuFunction _menuFunction);

};
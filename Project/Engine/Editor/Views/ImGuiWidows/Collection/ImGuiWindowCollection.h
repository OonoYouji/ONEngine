#pragma once

/// std
#include <vector>
#include <memory>
#include <string>



namespace ONEngine {
/// 前方宣言
class DxManager;
class EntityComponentSystem;
class AssetCollection;
class SceneManager;
} /// ONEngine



namespace Editor {

/// 前方宣言
class ImGuiManager;
class EditorManager;

/// ///////////////////////////////////////////////////
/// 他のImGuiWindowを持ち、表示するためのクラス
/// ///////////////////////////////////////////////////
class ImGuiWindowCollection {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	ImGuiWindowCollection(
		ONEngine::DxManager* _dxManager,
		ONEngine::EntityComponentSystem* _ecs,
		ONEngine::AssetCollection* _assetCollection,
		ImGuiManager* _imGuiManager,
		EditorManager* _editorManager,
		ONEngine::SceneManager* _sceneManager
	);
	~ImGuiWindowCollection();

	/// @brief 更新
	void Update();

	/// @brief windowの追加
	void AddParentWindow(const std::string& _name, std::unique_ptr<class IImGuiParentWindow> _window);

private:
	/// ===================================================
	/// private : methods
	/// ===================================================

	/// @brief MainMenuの更新
	void MainMenuUpdate();

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	/// ----- other class ----- ///
	ImGuiManager* pImGuiManager_;

	/// ----- collection window ----- ///
	std::vector<std::unique_ptr<IImGuiParentWindow>> parentWindows_;
	std::vector<std::string> parentWindowNames_;
	int selectedMenuIndex_ = 0;
};


/// ///////////////////////////////////////////////////
/// ImGuiの親Windowクラス
/// ///////////////////////////////////////////////////
class IImGuiParentWindow {
	friend class ImGuiWindowCollection;
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	virtual ~IImGuiParentWindow() = default;
	virtual void ShowImGui() = 0;

	/// @brief 子windowの更新
	void UpdateChildren();

	/// @brief 子windowの追加
	/// @param _child 子window 
	class IImGuiChildWindow* AddChild(std::unique_ptr<class IImGuiChildWindow> _child);


protected:
	/// ===================================================
	/// protected : objects
	/// ===================================================
	ImGuiManager* pImGuiManager_ = nullptr; ///< ImGuiManagerへのポインタ
	std::vector<std::unique_ptr<class IImGuiChildWindow>> children_;
};


/// ///////////////////////////////////////////////////
/// ImGuiの子windowクラス
/// ///////////////////////////////////////////////////
class IImGuiChildWindow {
	friend class ImGuiWindowCollection;
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	virtual ~IImGuiChildWindow() = default;
	virtual void ShowImGui() = 0;

protected:
	/// ===================================================
	/// protected : objects
	/// ===================================================
	ImGuiManager* pImGuiManager_;
};


} /// Editor


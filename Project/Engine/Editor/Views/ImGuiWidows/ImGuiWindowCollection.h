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
/// 他のエディター用のViewを持ち、表示するためのクラス
/// ///////////////////////////////////////////////////
class EditorViewCollection {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	EditorViewCollection(
		ONEngine::DxManager* _dxm,
		ONEngine::EntityComponentSystem* _ecs,
		ONEngine::AssetCollection* _assetCollection,
		ImGuiManager* _imGuiManager,
		EditorManager* _editorManager,
		ONEngine::SceneManager* _sceneManager
	);
	~EditorViewCollection();

	/// @brief 更新
	void Update();

	/// @brief windowの追加
	void AddParentWindow(const std::string& _name, std::unique_ptr<class IEditorWindowContainer> _window);

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
	std::vector<std::unique_ptr<IEditorWindowContainer>> parentWindows_;
	std::vector<std::string> parentWindowNames_;
	int selectedMenuIndex_ = 0;
};


/// ///////////////////////////////////////////////////
/// (File, Edit, View...)などの大枠のImGui windowクラス
/// ///////////////////////////////////////////////////
class IEditorWindowContainer {
	friend class EditorViewCollection;
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	virtual ~IEditorWindowContainer() = default;
	virtual void ShowImGui() = 0;

	/// @brief 子windowの更新
	void UpdateViews();

	/// @brief 子windowの追加
	/// @param _child 子window 
	class IEditorWindow* AddView(std::unique_ptr<class IEditorWindow> _child);


protected:
	/// ===================================================
	/// protected : objects
	/// ===================================================
	ImGuiManager* pImGuiManager_ = nullptr; ///< ImGuiManagerへのポインタ
	std::vector<std::unique_ptr<class IEditorWindow>> children_;
};


/// ///////////////////////////////////////////////////
/// ImGuiの子windowクラス
/// ///////////////////////////////////////////////////
class IEditorWindow {
	friend class EditorViewCollection;
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	virtual ~IEditorWindow() = default;
	virtual void ShowImGui() = 0;

protected:
	/// ===================================================
	/// protected : objects
	/// ===================================================
	ImGuiManager* pImGuiManager_;
};


} /// Editor


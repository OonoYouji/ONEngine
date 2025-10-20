#pragma once

/// std
#include <vector>
#include <memory>
#include <string>


/// ///////////////////////////////////////////////////
/// 他のImGuiWindowを持ち、表示するためのクラス
/// ///////////////////////////////////////////////////
class ImGuiWindowCollection {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	ImGuiWindowCollection(
		class EntityComponentSystem* _ecs,
		class GraphicsResourceCollection* _grc,
		class ImGuiManager* _imGuiManager,
		class EditorManager* _editorManager,
		class SceneManager* _sceneManager
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
	class ImGuiManager* pImGuiManager_;

	/// ----- collection window ----- ///
	std::vector<std::unique_ptr<class IImGuiParentWindow>> parentWindows_;
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
	class ImGuiManager* pImGuiManager_ = nullptr; ///< ImGuiManagerへのポインタ
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
	class ImGuiManager* pImGuiManager_;
};
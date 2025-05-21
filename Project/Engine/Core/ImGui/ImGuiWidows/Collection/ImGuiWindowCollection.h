#pragma once

/// std
#include <vector>
#include <memory>
#include <string>


/// ///////////////////////////////////////////////////
/// ImGuiWindowCollection
/// ///////////////////////////////////////////////////
class ImGuiWindowCollection {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	ImGuiWindowCollection(
		class EntityComponentSystem* _pEntityComponentSystem,
		class GraphicsResourceCollection* _resourceCollection,
		class ImGuiManager* _imGuiManager,
		class EditorManager* _editorManager
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

	void MainMenuUpdate();


private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	class ImGuiManager* pImGuiManager_;

	std::vector<std::unique_ptr<class IImGuiParentWindow>> parentWindows_;
	std::vector<std::string> parentWindowNames_;
	int selectedMenuIndex_ = 0;


};


/// ///////////////////////////////////////////////////
/// ImGuiの親windowクラス
/// ///////////////////////////////////////////////////
class IImGuiParentWindow {
	friend class ImGuiWindowCollection;
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	virtual ~IImGuiParentWindow() = default;
	virtual void ImGuiFunc() = 0;

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
	virtual void ImGuiFunc() = 0;

protected:
	/// ===================================================
	/// protected : objects
	/// ===================================================
	class ImGuiManager* pImGuiManager_;
};
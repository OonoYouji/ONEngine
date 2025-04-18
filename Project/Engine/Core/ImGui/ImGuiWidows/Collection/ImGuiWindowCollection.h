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

	ImGuiWindowCollection(class EntityComponentSystem* _pEntityComponentSystem, class GraphicsResourceCollection* _resourceCollection);
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

	std::vector<std::unique_ptr<class IImGuiParentWindow>> parentWindows_;
	std::vector<std::string> parentWindowNames_;
	int selectedMenuIndex_ = 0;


	/// 何度も定義すると重たい一時変数

};


/// ///////////////////////////////////////////////////
/// ImGuiの親windowクラス
/// ///////////////////////////////////////////////////
class IImGuiParentWindow {
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
	class IImGuiChildWindow*  AddChild(std::unique_ptr<class IImGuiChildWindow> _child) {
		class IImGuiChildWindow* child = _child.get();
		children_.push_back(std::move(_child));
		return child;
	}


protected:
	/// ===================================================
	/// protected : objects
	/// ===================================================

	std::vector<std::unique_ptr<class IImGuiChildWindow>> children_;
};


/// ///////////////////////////////////////////////////
/// ImGuiの子windowクラス
/// ///////////////////////////////////////////////////
class IImGuiChildWindow {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	virtual ~IImGuiChildWindow() = default;
	virtual void ImGuiFunc() = 0;
};
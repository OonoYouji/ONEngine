#pragma once

/// std
#include <vector>
#include <functional>
#include <map>

/// engine
#include "../../Collection/ImGuiWindowCollection.h"
#include "Engine/ECS/Entity/GameEntity/GameEntity.h"

/// ///////////////////////////////////////////////////
/// ImGuiInspectorWindow
/// ///////////////////////////////////////////////////
class ImGuiInspectorWindow : public IImGuiChildWindow {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	ImGuiInspectorWindow(const std::string& _windowName, class EntityComponentSystem* _ecs, class AssetCollection* _resourceCollection, class EditorManager* _editorManager);
	~ImGuiInspectorWindow() {}

	/// @brief imgui windowの描画処理
	void ShowImGui() override;

	/// @brief Componentのデバッグ関数を登録する
	/// @tparam T Componentの型
	/// @param _func Componentのデバッグ関数
	template<typename T>
	void RegisterComponent(std::function<void(class IComponent*)> _func);

	/// @brief EntityのInspector表示処理
	void EntityInspector();

	/// edit target entity の setter getter
	void SetSelectedEntity(GameEntity* _entity);
	class GameEntity* GetSelectedEntity() const;

private:
	/// ===================================================
	/// private : methods
	/// ===================================================

	/// ----- other class ----- ///
	class EntityComponentSystem* pEcs_;
	class EditorManager* pEditorManager_;
	class AssetCollection* pGrc_;

	/// ----- edit target ----- ///
	GameEntity* selectedEntity_ = nullptr; ///< 選択したエンティティのポインタ

	std::string windowName_;
	class IComponent* selectedComponent_ = nullptr;
	std::vector<std::function<void()>> inspectorFunctions_;
	std::unordered_map<size_t, std::function<void(class IComponent*)>> componentDebugFuncs_;

	/* ----- add component ----- */
	std::map<size_t, std::string> componentNames_;

};

template<typename T>
inline void ImGuiInspectorWindow::RegisterComponent(std::function<void(class IComponent*)> _func) {
	/// hash計算
	std::string name = typeid(T).name();
	if (name.find("class ") == 0) {
		name = name.substr(6);
	}
	size_t hash = std::hash<std::string>()(name);

	componentDebugFuncs_[hash] = _func;
	componentNames_[hash] = name;
}

#pragma once

/// std
#include <vector>
#include <functional>
#include <map>

/// engine
#include "Engine/ECS/Entity/GameEntity/GameEntity.h"
#include "Engine/ECS/Component/Collection/ComponentHash.h"

/// editor
#include "../../EditorViewCollection.h"

namespace ONEngine {
/// 前方宣言
class IComponent;
class Texture;
class GameEntity;
}

namespace Editor {

/// ///////////////////////////////////////////////////
/// 選択された対象の情報を表示・編集する
/// ///////////////////////////////////////////////////
class InspectorWindow : public IEditorWindow {

	/// @brief コンポーネントの種類分け
	enum class ComponentType {
		Compute,	/// Transformを筆頭に計算に使うようなコンポーネント
		Renderer,	/// MeshRendererを筆頭に描画に用いるコンポーネント
		Collider,	/// BoxColliderを筆頭に衝突判定に用いるコンポーネント
	};

	using EditFunc = std::function<void(ONEngine::IComponent*)>;

	/// @brief エディタに表示するためのコンポーネントの情報
	struct ComponentUIBinding {
		std::string name;
		ComponentType type;
		EditFunc function;
	};

public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	InspectorWindow(
		const std::string& windowName,
		ONEngine::DxManager* dxm,
		ONEngine::EntityComponentSystem* ecs,
		ONEngine::AssetCollection* assetCollection,
		EditorManager* editorManager
	);
	~InspectorWindow() {}

	/// @brief imgui windowの描画処理
	void ShowImGui() override;


	/// @brief Componentのデバッグ関数を登録する
	/// @tparam T Componentの型
	/// @param _func Componentのデバッグ関数
	template<typename T>
	void RegisterComponent(ComponentType type, EditFunc func);


	/// --------------------------------------------------------------------------------------------------
	/// エンティティのエディタ表示用関数群
	///	--------------------------------------------------------------------------------------------------

	/// @brief EntityのInspector表示処理
	void EntityInspector();

	/// @brief 選択しているエンティティを検索
	/// @return 選択しているエンティティ、選択していなければnullptr
	ONEngine::GameEntity* GetSelectedEntity(const ONEngine::Guid& entityGuid);

	/// @brief 選択しているエンティティのメニューバーを表示する
	/// @param entity 選択中のエンティティのポインタ、nullptrでクラッシュ
	void ShowEntityMenuBar(ONEngine::GameEntity* entity);

	/// @brief エンティティの基本情報の表示を行う
	/// @param entity 表示対象のエンティティ
	void ShowEntityBasicInfo(ONEngine::GameEntity* entity);

	/// @brief エンティティのコンポーネントの表示を行う
	/// @param entity 表示対象のエンティティ
	void ShowEntityComponents(ONEngine::GameEntity* entity);

	/// @brief コンポーネントの追加用ポップアップ表示
	/// @param entity 対象のエンティティ
	void ShowAddComponentPopup(ONEngine::GameEntity* entity);

	void DrawComponentNode();



	///	--------------------------------------------------------------------------------------------------
	/// アセットのエディタ表示用関数群
	///	--------------------------------------------------------------------------------------------------

	/// @brief アセットInspector表示処理
	void AssetInspector();

	/// @brief テクスチャのInspector表示
	/// @param _texture 
	void TextureAssetInspector(ONEngine::Texture* tex);

private:
	/// ===================================================
	/// private : methods
	/// ===================================================

	/// ----- other class ----- ///
	ONEngine::EntityComponentSystem* pEcs_;
	EditorManager*                   pEditorManager_;
	ONEngine::AssetCollection*       pAssetCollection_;
	ONEngine::DxManager*             pDxManager_;



	std::string windowName_;
	ONEngine::IComponent* selectedComponent_ = nullptr;
	std::vector<std::function<void()>> inspectorFunctions_;
	std::unordered_map<size_t, ComponentUIBinding> componentUIBindings_;

};

template<typename T>
inline void InspectorWindow::RegisterComponent(ComponentType type, EditFunc function) {
	size_t hash = GetComponentHash<T>();
	//componentDebugFuncs_[hash] = _func;
	//componentNames_[hash] = GetComponentTypeName<T>();

	ComponentUIBinding binding = {
		.name = GetComponentTypeName<T>(),
		.type = type,
		.function = function
	};

	componentUIBindings_[hash] = binding;
}


} /// Editor

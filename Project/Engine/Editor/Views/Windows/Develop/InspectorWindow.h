#pragma once

/// std
#include <vector>
#include <functional>
#include <map>

/// externals
#include <imgui.h>

/// engine
#include "Engine/ECS/Entity/GameEntity/GameEntity.h"
#include "Engine/ECS/Component/Collection/ComponentHash.h"

/// editor
#include "../../EditorViewCollection.h"

namespace ONEngine {
class IComponent;
class GameEntity;
}

namespace ONEngine::Asset {
class Texture;
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
		Script,		/// Script
	};

	using EditFunc = std::function<void(const std::vector<ONEngine::IComponent*>&)>;

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
		ONEngine::Asset::AssetCollection* assetCollection,
		EditorManager* editorManager
	);
	~InspectorWindow() {}

	/// @brief imgui windowの描画処理
	void ShowImGui() override;


	/// @brief Componentのデバッグ関数を登録する
	/// @tparam T Componentの型
	/// @param type コンポーネントの分類
	/// @param func 単一コンポーネント用のデバッグ関数
	template<typename T>
	void RegisterComponent(ComponentType type, std::function<void(T*)> func);

	/// @brief Componentのデバッグ関数を登録する (複数選択対応)
	/// @tparam T Componentの型
	/// @param type コンポーネントの分類
	/// @param func 複数コンポーネント用のデバッグ関数
	template<typename T>
	void RegisterComponentMulti(ComponentType type, std::function<void(const std::vector<T*>&)> func);


	/// --------------------------------------------------------------------------------------------------
	/// エンティティのエディタ表示用関数群
	///	--------------------------------------------------------------------------------------------------

	/// @brief EntityのInspector表示処理
	void EntityInspector();

	/// @brief 複数選択時のInspector表示処理
	void MultiEntityInspector(const std::vector<ONEngine::GameEntity*>& entities);

	/// @brief 選択しているエンティティを検索
	/// @return 選択しているエンティティ、選択していなければnullptr
	ONEngine::GameEntity* GetSelectedEntity(const ONEngine::Guid& entityGuid);

	/// @brief 選択しているエンティティのリストを取得
	std::vector<ONEngine::GameEntity*> GetSelectedEntities();

	/// @brief 選択しているエンティティのメニューバーを表示する
	/// @param entity 選択中のエンティティのポインタ、nullptrでクラッシュ
	void ShowEntityMenuBar(ONEngine::GameEntity* entity);

	/// @brief エンティティの基本情報の表示を行う
	/// @param entity 表示対象のエンティティ
	void ShowEntityBasicInfo(ONEngine::GameEntity* entity);

	/// @brief 複数エンティティの基本情報の表示を行う
	void ShowMultiEntityBasicInfo(const std::vector<ONEngine::GameEntity*>& entities);

	/// @brief エンティティのコンポーネントの表示を行う
	/// @param entity 表示対象のエンティティ
	void ShowEntityComponents(ONEngine::GameEntity* entity);

	/// @brief 複数エンティティの共通コンポーネントを表示する
	void ShowMultiEntityComponents(const std::vector<ONEngine::GameEntity*>& entities);

	/// @brief コンポーネントの追加用ポップアップ表示
	/// @param entity 対象のエンティティ
	void ShowAddComponentPopup(ONEngine::GameEntity* entity);

	/// @brief 複数エンティティに対するコンポーネント追加用ポップアップ
	void ShowMultiAddComponentPopup(const std::vector<ONEngine::GameEntity*>& entities);

	/// @brief コンポーネントタイプごとの色を取得する
	/// @param type 対象のタイプ
	/// @return 色
	ImVec4 GetComponentBaseColor(ComponentType type) const;

	/// @brief コンポーネントのエディタ表示
	/// @param entity コンポーネントの親エンティティ
	/// @param itr 
	void DrawComponentNode(ONEngine::GameEntity* entity, auto& itr);

	/// @brief 複数コンポーネントのエディタ表示
	void DrawMultiComponentNode(const std::vector<ONEngine::GameEntity*>& entities, size_t hash, const std::vector<ONEngine::IComponent*>& comps);

	/// ヘッダー部分のUIを描画し、開いているかどうかを返す
	bool DrawComponentHeaderUI(ONEngine::IComponent* comp, const std::string& compName, ImVec4 baseColor);

	/// 複数コンポーネントのヘッダーUIを描画
	bool DrawMultiComponentHeaderUI(const std::vector<ONEngine::IComponent*>& comps, const std::string& compName, ImVec4 baseColor);

	/// ポップアップメニューの処理を行い、コンポーネントが削除されたら true を返す
	bool HandleComponentPopupMenu(ONEngine::GameEntity* entity, ONEngine::IComponent* comp, const std::string& compName, auto& itr);

	/// 複数コンポーネントのポップアップメニュー
	bool HandleMultiComponentPopupMenu(const std::vector<ONEngine::GameEntity*>& entities, size_t hash, const std::string& compName);

	/// コンポーネントの内部パラメータ群を描画する
	void DrawComponentInnerContent(ONEngine::IComponent* comp, size_t componentTypeId, bool enabled);

	/// 複数コンポーネントの内部パラメータを描画
	void DrawMultiComponentInnerContent(const std::vector<ONEngine::IComponent*>& comps, size_t componentTypeId, bool enabled);


	///	--------------------------------------------------------------------------------------------------
	/// アセットのエディタ表示用関数群
	///	--------------------------------------------------------------------------------------------------

	/// @brief アセットInspector表示処理
	void AssetInspector();

	/// @brief テクスチャのInspector表示
	/// @param _texture 
	void TextureAssetInspector(ONEngine::Asset::Texture* tex);

private:
	/// ===================================================
	/// private : methods
	/// ===================================================

	/// ----- other class ----- ///
	ONEngine::EntityComponentSystem* pEcs_;
	EditorManager* pEditorManager_;
	ONEngine::Asset::AssetCollection* pAssetCollection_;
	ONEngine::DxManager* pDxManager_;



	std::string windowName_;
	ONEngine::IComponent* selectedComponent_ = nullptr;
	std::vector<std::function<void()>> inspectorFunctions_;
	std::unordered_map<size_t, ComponentUIBinding> componentUIBindings_;

};

template<typename T>
inline void InspectorWindow::RegisterComponent(ComponentType type, std::function<void(T*)> func) {
	RegisterComponentMulti<T>(type, [func](const std::vector<T*>& comps) {
		if (!comps.empty()) func(comps[0]);
	});
}

template<typename T>
inline void InspectorWindow::RegisterComponentMulti(ComponentType type, std::function<void(const std::vector<T*>&)> func) {
	size_t hash = ONEngine::GetComponentHash<T>();

	ComponentUIBinding binding = {
		.name = ONEngine::GetComponentTypeName<T>(),
		.type = type,
		.function = [func](const std::vector<ONEngine::IComponent*>& comps) {
			std::vector<T*> typedComps;
			typedComps.reserve(comps.size());
			for (auto c : comps) typedComps.push_back(static_cast<T*>(c));
			func(typedComps);
		}
	};

	componentUIBindings_[hash] = binding;
}


} /// Editor

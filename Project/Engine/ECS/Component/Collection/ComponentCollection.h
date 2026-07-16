#pragma once

/// std
#include <unordered_map>
#include <functional>

#include "../Array/ComponentArray.h"
#include "ComponentHash.h"

/// //////////////////////////////////////////////
/// Componentのコレクションクラス
/// //////////////////////////////////////////////
namespace ONEngine {

/**
 * @class ComponentCollection
 * @brief すべてのコンポーネント配列（ComponentArray）を一括管理し、型あるいは型名文字列によるコンポーネントの生成・検索・一括削除を司るファクトリ＆マネージャクラス
 */
class ComponentCollection {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	/**
	 * @brief コンストラクタ
	 */
	ComponentCollection();

	/**
	 * @brief デストラクタ
	 */
	~ComponentCollection();


	/**
	 * @brief 特定のコンポーネント型（Comp）に対する生成ファクトリおよび配列を登録します。
	 * @tparam Comp 登録対象のコンポーネント型
	 */
	template<IsComponent Comp>
	void RegisterComponentFactory();

	/**
	 * @brief 指定されたコンポーネント型（Comp）を新規生成し、登録します（配列が存在しない場合は自動登録）。
	 * @tparam Comp 生成するコンポーネント型
	 * @return 追加されたコンポーネントの型付きポインタ
	 */
	template<IsComponent Comp>
	Comp* AddComponent();

	/**
	 * @brief コンポーネント名の文字列を指定して、新規コンポーネントを追加します。
	 * @param _name コンポーネントのクラス名（例: "Transform", "MeshRenderer" など）
	 * @return 生成されたコンポーネントの基底（IComponent）ポインタ
	 */
	IComponent* AddComponent(const std::string& _name);

	/**
	 * @brief 型と配列内インデックスを指定して、コンポーネントを取得します。
	 * @tparam Comp 取得したいコンポーネントの型
	 * @param _index 配列内インデックス
	 * @return 対象コンポーネントのポインタ
	 */
	template<IsComponent Comp>
	Comp* GetComponent(size_t _index);

	/**
	 * @brief 型付きで、指定されたインデックスのコンポーネントを削除します。
	 * @tparam Comp 削除対象のコンポーネント型
	 * @param _index 配列内インデックス
	 */
	template<IsComponent Comp>
	void RemoveComponent(size_t _index);

	/**
	 * @brief コンポーネントのハッシュとIDを指定して、コンポーネントを削除します。
	 * @param _hash コンポーネント型のハッシュ値
	 * @param _id 対象のコンポーネントID
	 */
	void RemoveComponent(size_t _hash, size_t _id);

	/**
	 * @brief 指定されたゲームエンティティ（GameEntity）にアタッチされているすべてのコンポーネントを一括削除します。
	 * @param _entity 対象のゲームエンティティポインタ
	 */
	void RemoveComponentAll(class GameEntity* _entity);


	/**
	 * @brief 指定されたコンポーネント型（Comp）に対応するコンポーネント配列オブジェクトを取得します。
	 * @tparam Comp 対象コンポーネント型
	 * @return ComponentArrayポインタ
	 */
	template <IsComponent Comp>
	ComponentArray<Comp>* GetComponentArray();

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	std::unordered_map<size_t, std::unique_ptr<IComponentArray>> arrayMap_;
	std::unordered_map<size_t, std::function<IComponent* ()>> factoryMap_;
};


/// //////////////////////////////////////////////
/// inline methods
/// //////////////////////////////////////////////

template<IsComponent Comp>
inline void ComponentCollection::RegisterComponentFactory() {
	size_t hash = GetComponentHash<Comp>();
	if (arrayMap_.find(hash) == arrayMap_.end()) {
		arrayMap_[hash] = std::make_unique<ComponentArray<Comp>>();
	}

	factoryMap_[hash] = [this, hash]() -> IComponent* {
		ComponentArray<Comp>* compArray = static_cast<ComponentArray<Comp>*>(arrayMap_[hash].get());
		return compArray->AddComponent();
		};
}

template<IsComponent Comp>
inline Comp* ComponentCollection::AddComponent() {
	size_t hash = GetComponentHash<Comp>();
	if (arrayMap_.find(hash) == arrayMap_.end()) {
		RegisterComponentFactory<Comp>();
	}

	Comp* comp = static_cast<Comp*>(factoryMap_[hash]());
	comp.id = arrayMap_[hash]->GetComponentIndex();

	return static_cast<Comp*>(factoryMap_[hash]());
}

template<IsComponent Comp>
inline Comp* ComponentCollection::GetComponent(size_t _index) {
	size_t hash = GetComponentHash<Comp>();
	ComponentArray<Comp>* componentArray = static_cast<ComponentArray<Comp>*>(arrayMap_[hash].get());

	return &componentArray->components_[_index];
}

template<IsComponent Comp>
inline void ComponentCollection::RemoveComponent(size_t _index) {
	size_t hash = GetComponentHash<Comp>();
	ComponentArray<Comp>* componentArray = static_cast<ComponentArray<Comp>*>(arrayMap_[hash].get());
	componentArray->usedIndices_.erase(std::remove(componentArray->usedIndices_.begin(), componentArray->usedIndices_.end(), _index), componentArray->usedIndices_.end());
	componentArray->removedIndices_.push_back(_index);

	componentArray->usedComponents_.erase(std::remove(
		componentArray->usedComponents_.begin(), componentArray->usedComponents_.end(),
		&componentArray->components_[_index]), componentArray->usedComponents_.end()
	);
}

template<IsComponent Comp>
inline ComponentArray<Comp>* ComponentCollection::GetComponentArray() {
	size_t hash = GetComponentHash<Comp>();
	if (arrayMap_.find(hash) != arrayMap_.end()) {
		return static_cast<ComponentArray<Comp>*>(arrayMap_[hash].get());
	}
	return nullptr;
}

} /// ONEngine

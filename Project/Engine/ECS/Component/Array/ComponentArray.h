#pragma once

/// std
#include <vector>
#include <unordered_map>

/// engine
#include "../Components/Interface/IComponent.h"
#include "Engine/Core/Utility/Utility.h"

/// ///////////////////////////////////////////////////
/// Componentの配列のinterfaceクラス
/// ///////////////////////////////////////////////////
namespace ONEngine {

static constexpr size_t kComponentCapacity = 2048;

/**
 * @class IComponentArray
 * @brief 特定のコンポーネント配列（ComponentArray）の型非依存な操作を提供する抽象クラス
 */
class IComponentArray {
	friend class ComponentCollection;
public:

	virtual ~IComponentArray() = default;

	/**
	 * @brief 型非依存のインターフェースとして新規コンポーネントを追加します。
	 * @return 追加されたコンポーネントの基底ポインタ
	 */
	virtual IComponent* AddComponentUntyped() = 0;

	/**
	 * @brief 指定されたインデックスのコンポーネントを削除し、再利用リストへ登録します。
	 * @param _index 削除対象のインデックス
	 */
	virtual void RemoveComponent(size_t _index) = 0;

	/**
	 * @brief 指定したコンポーネントの配列内インデックスを取得します。
	 * @param _component 検索対象のコンポーネント
	 * @return 配列のインデックス
	 */
	virtual size_t GetComponentIndex(IComponent* _component) = 0;

	/**
	 * @brief 新規に追加するコンポーネントのIDを発行します。
	 * @return 新規ID
	 */
	virtual size_t NewComponentId() = 0;

	/**
	 * @brief 現在アクティブなコンポーネントの総数を取得します。
	 * @return 使用中コンポーネント数
	 */
	virtual size_t GetUsedComponentCount() = 0;

protected:
	std::vector<size_t> usedIndices_;    ///< 使用中のインデックスのリスト
	std::vector<size_t> removedIndices_; ///< 削除されたインデックスのリスト
};


/**
 * @class ComponentArray
 * @brief 同一型のコンポーネント（Comp）をメモリ上に連続して管理し、IDとインデックスのマップ、削除・再利用の追跡を行うジェネリックコンテナ
 */
template <IsComponent Comp>
class ComponentArray final : public IComponentArray {
	friend class ComponentCollection;
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	/**
	 * @brief コンストラクタ。上限数をあらかじめ予約（reserve）します。
	 */
	ComponentArray();
	~ComponentArray() override = default;


	/**
	 * @brief 指定されたコンポーネント型（Comp）を配列に追加します。
	 * @return 追加されたコンポーネントへの型付きポインタ
	 */
	Comp* AddComponent();

	/**
	 * @brief 型非依存のインターフェースとして新規コンポーネントを追加します（再利用キューを考慮）。
	 * @return 基底型（IComponent）のポインタ
	 */
	IComponent* AddComponentUntyped() override;

	/**
	 * @brief 配列のインデックスから特定のコンポーネントを取得します。
	 * @param _index 配列内インデックス
	 * @return 対象コンポーネントの型付きポインタ。範囲外の場合は nullptr
	 */
	Comp* GetComponent(size_t _index);

	/**
	 * @brief 指定されたIDに対応するインデックス位置のコンポーネントを削除（無効化）します。
	 * @param _index 削除対象のID（内部インデックス）
	 */
	void RemoveComponent(size_t _index) override;

	/**
	 * @brief 指定されたコンポーネントポインタが属する配列内のインデックスを検索します。
	 * @param _component 検索対象ポインタ
	 * @return インデックス
	 */
	size_t GetComponentIndex(IComponent* _component) override;

	/**
	 * @brief 現在アクティブ（削除されていない）コンポーネントの総数を取得します。
	 * @return 使用中コンポーネント数
	 */
	size_t GetUsedComponentCount() override;

	/**
	 * @brief 新しいコンポーネントIDを発行します。
	 * @return 新規ID
	 */
	size_t NewComponentId() override;

	/**
	 * @brief 現在使用中（アクティブ）であるすべての型付きコンポーネントへのポインタの配列を取得します。
	 * @return 使用中コンポーネントポインタのリストの参照
	 */
	std::vector<Comp*>& GetUsedComponents();

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	/// first: コンポーネントのID, second: インデックス
	std::unordered_map<size_t, size_t> indexMap_; ///< コンポーネントのIDとインデックスのマップ
	std::vector<Comp> components_;
	std::vector<Comp*> usedComponents_; ///< 使用中のコンポーネントのリスト
};

template <IsComponent Comp>
inline ComponentArray<Comp>::ComponentArray() {
	/// n個のコンポーネントを格納できるように予約
	components_.reserve(kComponentCapacity);
}

template <IsComponent Comp>
inline Comp* ComponentArray<Comp>::AddComponent() {
	Comp* comp = static_cast<Comp*>(AddComponentUntyped());
	return comp;
}

template <IsComponent Comp>
inline IComponent* ComponentArray<Comp>::AddComponentUntyped() {
	///< 削除されたインデックスがある場合
	if(removedIndices_.size() > 0) {
		size_t index = removedIndices_.back();
		removedIndices_.pop_back();
		usedIndices_.push_back(index);

		components_[index] = Comp(); ///< 今までのデータを上書き
		components_[index].id = static_cast<uint32_t>(index); ///< IDを設定

		///< IDとインデックスのマップを更新
		indexMap_[components_[index].id] = index;

		usedComponents_.push_back(&components_[index]); ///< 使用中のコンポーネントリストに追加
		return &components_[index];
	}

	components_.emplace_back();
	size_t index = NewComponentId();
	usedIndices_.push_back(index);

	components_[index].id = static_cast<uint32_t>(index); ///< IDを設定

	///< IDとインデックスのマップを更新
	indexMap_[components_[index].id] = index;

	usedComponents_.push_back(&components_[index]); ///< 使用中のコンポーネントリストに追加
	return &components_[index];
}

template<IsComponent Comp>
inline Comp* ComponentArray<Comp>::GetComponent(size_t _index) {
	if(_index >= components_.size()) {
		Console::LogError("ComponentArray: GetComponent failed, index out of range.");
		return nullptr;
	}
	return &components_[_index];
}

template <IsComponent Comp>
inline void ComponentArray<Comp>::RemoveComponent(size_t _id) {
	if(_id >= components_.size()) {
		Console::LogError("ComponentArray: RemoveComponent failed, index out of range.");
		return;
	}

	usedIndices_.erase(std::remove(usedIndices_.begin(), usedIndices_.end(), _id), usedIndices_.end());
	removedIndices_.push_back(_id);

	components_[_id].SetOwner(nullptr); ///< コンポーネントのオーナーをnullptrに設定
	usedComponents_.erase(std::remove(usedComponents_.begin(), usedComponents_.end(), &components_[_id]), usedComponents_.end());
}

template <IsComponent Comp>
inline size_t ComponentArray<Comp>::GetComponentIndex(IComponent* _component) {

	for(size_t i = 0; i < components_.size(); i++) {
		const Comp& comp = components_[i];
		if(comp.id == _component->id) {
			return i;
		}
	}

	return 0;
}

template <IsComponent Comp>
inline size_t ComponentArray<Comp>::GetUsedComponentCount() {
	return usedComponents_.size();
}

template <IsComponent Comp>
inline size_t ComponentArray<Comp>::NewComponentId() {
	return static_cast<size_t>(components_.size() - 1);
}

template <IsComponent Comp>
inline std::vector<Comp*>& ComponentArray<Comp>::GetUsedComponents() {
	return usedComponents_;
}

} /// ONEngine

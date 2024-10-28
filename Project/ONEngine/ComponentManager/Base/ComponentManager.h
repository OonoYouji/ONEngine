#pragma once

/// std
#include <unordered_map>
#include <memory>

#include "ComponentArray.h"


class ComponentManager final {
	ComponentManager() {}
	~ComponentManager() {}
public:

	/// <summary>
	/// インスタンスの確保
	/// </summary>
	/// <returns> return : インスタンス </returns>
	static ComponentManager* GetInstance() {
		static ComponentManager instance;
		return &instance;
	}

	/// <summary>
	/// 終了処理
	/// </summary>
	void Finalize();


	/// <summary>
	/// すべてのコンポーネントの更新処理
	/// </summary>
	void Update();


	/// <summary>
	/// コンポーネントの追加
	/// </summary>
	/// <typeparam name="T"> BaseComponentを継承しているクラス</typeparam>
	/// <returns> return : 追加したコンポーネントのポインタ </returns>
	template <typename T, typename = typename std::enable_if<std::is_base_of<BaseComponent, T>::value>::type>
	T* AddComponent() {
		size_t id = ComponentArray<T>::GetId();
		if(componentArrays_.find(id) == componentArrays_.end()) {
			componentArrays_[id] = std::make_shared<ComponentArray<T>>();
		}
		return std::static_pointer_cast<ComponentArray<T>>(componentArrays_[id])->AddComponent();
	}

private:

	std::unordered_map<size_t, std::shared_ptr<BaseComponentArray>> componentArrays_;
};
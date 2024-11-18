#pragma once

/// std
#include <vector>
#include <memory>

class ECSManager final {

	/// ===================================================
	/// コンポーネントの配列クラスの基底クラス
	/// ===================================================
	class IComponentPool {};


	/// ===================================================
	/// コンポーネントの配列
	/// ===================================================
	template<typename T>
	class ComponentPool : IComponentPool{
		friend class ECSManager;
	public:
		ComponentPool(const size_t size) : components_(size) {}
		~ComponentPool() {}

		T* AddComponent(size_t _entity);
		T* GetComponent(size_t _entityComponent);

		static size_t GetId();

	private:
		std::vector<T> components_;
		static size_t compId_;
	};




	/// ===================================================
	/// エンティティのコンテナクラス
	/// ===================================================
	class EnemyContainer {
	public:
		EnemyContainer() {}
		~EnemyContainer() {}

		void Add(size_t _entity);

		void Remove(const size_t _entity);

		const std::vector<size_t>& GetEntities() const { return entities_; }
	private:
		std::vector<size_t> entities_;
		std::vector<size_t> entitiyToIndex_;
	};

public:

	ECSManager() {}
	~ECSManager() {}

	static ECSManager* GetInstance() {
		ECSManager instance;
		return &instance;
	}

	void Initialize();
	void Update();


	/// <summary>
	/// エンティティを作成する
	/// </summary>
	/// <returns></returns>
	size_t GenerateEntity();


	template<typename T>
	T* AddComponent(size_t _entity);


	template<typename T>
	void RemoveComponent(size_t _entity);

private:

	/// entity
	std::vector<size_t> entities_;
	size_t              nextEntityId_;


	/// component
	std::vector<std::unique_ptr<IComponentPool>> componentPools_;
	static size_t nextCompArrayId_;
};


size_t ECSManager::nextCompArrayId_ = 0;
template<typename T>
size_t ECSManager::ComponentPool<T>::compId_ = 0;


template<typename T>
inline T* ECSManager::ComponentPool<T>::AddComponent(size_t _entity) {
	if(components_.size() < _entity) {
		components_.resize(_entity, T());
	}
	components_[_entity] = T();
	return components_[_entity];
}

template<typename T>
inline size_t ECSManager::ComponentPool<T>::GetId() {
	// この関数を初めて読んだ時にIDを発行
	if(!ComponentPool<T>::GetId) {
		ComponentPool<T>::GetId = ++ECSManager::nextCompArrayId_;
	}
	return ComponentPool<T>::compId_;
	return size_t();
}

template<typename T>
inline T* ECSManager::AddComponent(size_t _entity) {
	size_t id = ComponentPool<T>::GetId();

	if(!componentPools_[_entity]) {
		componentPools_[_entity] = std::make_unique<ComponentPool<T>>();
	}

	ComponentPool<T>* compPool = componentPools_[_entity];
	T* resultComp = compPool->AddComponent(_entity);

	return resultComp;
}

template<typename T>
inline void ECSManager::RemoveComponent(size_t _entity) {
	size_t id = ComponentPool<T>::GetId();

}

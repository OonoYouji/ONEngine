#pragma once

/// std
#include <vector>
#include <memory>

#include "../System/ISystem/ISystem.h"

class ECSManager final {

	/// ===================================================
	/// コンポーネントの配列クラスの基底クラス
	/// ===================================================
	class IComponentPool {
	public:
		IComponentPool() {}
		virtual ~IComponentPool() {}
	};


	/// ===================================================
	/// コンポーネントの配列
	/// ===================================================
	template<typename T>
	class ComponentPool : IComponentPool {
		friend class ECSManager;
	public:
		ComponentPool(const size_t _size) : components_(_size) {}
		~ComponentPool() {}

		T* AddComponent(size_t _entity);
		T* GetComponent(size_t _entity);

		static size_t GetId();

	private:
		std::vector<T> components_;
		static size_t  compId_;
	};

public:

	ECSManager() {}
	~ECSManager() {}

	static ECSManager* GetInstance() {
		static ECSManager instance;
		return &instance;
	}

	void Initialize();
	void Update();


	/// <summary>
	/// エンティティを作成する
	/// </summary>
	/// <returns></returns>
	size_t GenerateEntity();

	/// <summary>
	/// エンティティを削除する
	/// </summary>
	/// <param name="_entity"></param>
	void RemoveEntity(size_t _entity);

	/// <summary>
	/// すべてのエンティティを得る
	/// </summary>
	/// <returns></returns>
	const std::vector<size_t>& GetEntities() const { return entities_; }

	/// <summary>
	/// Componentの追加
	/// </summary>
	/// <typeparam name="T">   : 追加するComponentの型      </typeparam>
	/// <param name="_entity"> : 追加さきのEntity           </param>
	/// <returns>              return : 追加されたComponent </returns>
	template<typename T>
	T* AddComponent(size_t _entity);

	/// <summary>
	/// Componentの削除
	/// </summary>
	/// <typeparam name="T">   : 削除するComponentの型 </typeparam>
	/// <param name="_entity"> : 削除する先のEntity    </param>
	template<typename T>
	void RemoveComponent(size_t _entity);

	/// <summary>
	/// Componentの取得
	/// </summary>
	/// <typeparam name="T">   : ゲットしたいComponentの型                      </typeparam>
	/// <param name="_entity"> : 取得先のEntity                                </param>
	/// <returns>       return : 取得したComponentのポインター、失敗したら nullptr </returns>
	template<typename T>
	T* GetComponent(size_t _entity);

	/// <summary>
	/// Systemの追加
	/// </summary>
	/// <param name="_system"> : 追加するSystemクラスのインスタンス </param>
	void AddSystem(ISystem* _system);

private:

	/// entity
	std::vector<size_t> entities_;
	std::vector<size_t> recycleEntities_;
	size_t              nextEntityId_;


	/// component
	std::vector<std::unique_ptr<IComponentPool>> componentPools_;
	static size_t nextCompArrayId_;


	/// system
	std::vector<std::unique_ptr<ISystem>> systems_;

};


template<typename T>
size_t ECSManager::ComponentPool<T>::compId_ = 0;



/// ===================================================
/// ComponentPool : methods
/// ===================================================

template<typename T>
inline T* ECSManager::ComponentPool<T>::AddComponent(size_t _entity) {
	if(components_.size() < _entity) {
		components_.resize(_entity, T());
	}
	components_[_entity] = T();
	return &components_[_entity];
}

template<typename T>
inline T* ECSManager::ComponentPool<T>::GetComponent(size_t _entity) {

	/// entitiyが有効なら
	if(components_.size() >= _entity) {
		return &components_[_entity];
	}
	return nullptr;
}


template<typename T>
inline size_t ECSManager::ComponentPool<T>::GetId() {
	// この関数を初めて読んだ時にIDを発行
	if(!ComponentPool<T>::compId_) {
		ComponentPool<T>::compId_ = ++ECSManager::nextCompArrayId_;
	}
	return ComponentPool<T>::compId_;
}



/// ===================================================
/// ECSManager : methods
/// ===================================================

template<typename T>
inline T* ECSManager::AddComponent(size_t _entity) {
	size_t id = ComponentPool<T>::GetId();

	if(id >= componentPools_.size()) {
		componentPools_.resize(id + 1);
	}

	/// poolがなければpoolを作る
	if(!componentPools_[id]) {
		componentPools_[id] = std::unique_ptr<IComponentPool>(new ComponentPool<T>(128));
	}

	ComponentPool<T>* compPool = static_cast<ComponentPool<T>*>(componentPools_[id].get());
	T* resultComp = compPool->AddComponent(_entity);

	return resultComp;
}

template<typename T>
inline void ECSManager::RemoveComponent(size_t _entity) {
	size_t id = ComponentPool<T>::GetId();


	recycleEntities_.push_back(_entity);
}

template<typename T>
inline T* ECSManager::GetComponent(size_t _entity) {
	size_t id = ComponentPool<T>::GetId();
	T* result = static_cast<ComponentPool<T>*>(componentPools_[id].get())->GetComponent(_entity);
	return result;
}
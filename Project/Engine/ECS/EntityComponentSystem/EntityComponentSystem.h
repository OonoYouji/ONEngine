#pragma once

/// std
#include <unordered_map>
#include <memory>
#include <string>

/// engine
#include "../Entity/Interface/IEntity.h"
#include "../Entity/Camera/Camera.h"
#include "../Component/ComponentArray/ComponentArray.h"


/// ///////////////////////////////////////////////////
/// ECSの基盤クラス
/// ///////////////////////////////////////////////////
class EntityComponentSystem final {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	EntityComponentSystem(class DxManager* _pDxManager);
	~EntityComponentSystem();

	void Initialize();
	void Update();


	/// ----- entity ----- ///

	template<typename T>
	T* GenerateEntity() requires std::is_base_of_v<IEntity, T>;

	void RemoveEntity(IEntity* _entity);

	const std::vector<std::unique_ptr<IEntity>>& GetEntities() const { return entities_; }

	/// @brief 新しい camera を生成する
	/// @return cameraへのポインタ
	Camera* GenerateCamera();

	/// @brief 新しい　2D camera を生成する
	/// @return 2D cameraへのポインタ
	Camera2D* GenerateCamera2D();



	/// ----- component ----- ///

	template<typename Comp>
	Comp* AddComponent() requires std::is_base_of_v<IComponent, Comp>;

	template<typename Comp>
	Comp* GetComponent(size_t _index) requires std::is_base_of_v<IComponent, Comp>;

	template<typename Comp>
	void RemoveComponent(size_t _index) requires std::is_base_of_v<IComponent, Comp>;

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	class DxManager* pDxManager_ = nullptr;

	/// ----- entity ----- ///
	std::vector<std::unique_ptr<IEntity>> entities_;
	std::vector<Camera*> cameras_;
	std::vector<Camera2D*> camera2ds_;
	Camera* mainCamera_ = nullptr;

	/// ----- component ----- ///
	std::unordered_map<std::string, std::unique_ptr<IComponentArray>> componentArrayMap_;


public:
	/// ===================================================
	/// public : accessor
	/// ====================================================

	/// @brief main cameraの設定
	/// @param _camera Cameraクラスへのポインタ
	void SetMainCamera(Camera* _camera) { mainCamera_ = _camera; }

	/// @brief main cameraの設定
	/// @param _index cameraのインデックス
	void SetMainCamera(size_t _index) { mainCamera_ = cameras_[_index]; }


	/// @brief entities の取得
	/// @return entities
	const std::vector<std::unique_ptr<IEntity>>& GetEntities() { return entities_; }

	/// @brief cameras の取得
	/// @return cameras
	const std::vector<Camera*>& GetCameras() { return cameras_; }

	/// @brief 2d cameras の取得
	/// @return camera2ds
	const std::vector<Camera2D*>& GetCamera2Ds() { return camera2ds_; }

	/// @brief main cameraの取得
	/// @return Cameraクラスへのポインタ
	const Camera* GetMainCamera() const { return mainCamera_; }
	Camera* GetMainCamera() { return mainCamera_; }
};



/// ===================================================
/// inline methods
/// ===================================================

template<typename T>
inline T* EntityComponentSystem::GenerateEntity() requires std::is_base_of_v<IEntity, T> {
	std::unique_ptr<T> entity = std::make_unique<T>();
	entity->pEntityComponentSystem_ = this;
	entity->Initialize();

	T* entityPtr = entity.get();
	entities_.push_back(std::move(entity));

	return entityPtr;
}

template<typename Comp>
inline Comp* EntityComponentSystem::AddComponent() requires std::is_base_of_v<IComponent, Comp> {
	size_t hash = typeid(Comp).hash_code();
	auto it = componentArrayMap_[hash]->AddComponent();
	return static_cast<Comp*>(componentArrayMap_[hash]->GetComponent(it));
}

template<typename Comp>
inline Comp* EntityComponentSystem::GetComponent(size_t _index) requires std::is_base_of_v<IComponent, Comp> {
	size_t hash = typeid(Comp).hash_code();
	auto it = componentArrayMap_[hash]->GetComponent(_index);
	return static_cast<Comp*>(it);
}

template<typename Comp>
inline void EntityComponentSystem::RemoveComponent(size_t _index) requires std::is_base_of_v<IComponent, Comp> {
	size_t hash = typeid(Comp).hash_code();
	componentArrayMap_[hash]->RemoveComponent(_index);
}

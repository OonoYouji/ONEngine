#pragma once

/// std
#include <unordered_map>
#include <vector>
#include <memory>
#include <string>
#include <deque>

#include "../Factory/EntityFactory.h"

class Camera;


class EntityCollection final {
public:

	EntityCollection(class EntityComponentSystem* _ecs, class DxManager* _dxManager);
	~EntityCollection();

	template<typename T>
	T* GenerateEntity() requires std::is_base_of_v<IEntity, T>;
	IEntity* GenerateEntity(const std::string& _name, bool _isInit);


	template<typename T>
	T* GenerateCamera() requires std::is_base_of_v<Camera, T>;
	Camera* GenerateCamera();


	void RemoveEntity(IEntity* _entity, bool _deleteChildren = true);

	void RemoveEntityAll();

	template <typename T>
	T* FindEntity() requires std::is_base_of_v<IEntity, T>;
	
	template <typename T>
	std::vector<T*> FindEntities() requires std::is_base_of_v<IEntity, T>;

	/// @brief 全エンティティを更新
	void UpdateEntities();

	/// @brief 引数のエンティティの更新(子も同時に
	/// @param _entity 更新したいエンティティ
	void UpdateEntity(IEntity* _entity);

	void AddDoNotDestroyEntity(IEntity* _entity);
	void RemoveDoNotDestroyEntity(IEntity* _entity);

	void SetFactoryRegisterFunc(std::function<void(EntityFactory*)> _func);

	size_t NewEntityID();

private:

	class EntityComponentSystem* pECS_;
	class DxManager* pDxManager_;
	class DxDevice* pDxDevice_;

	std::unique_ptr<EntityFactory> factory_;

	/// entityのIDを管理するためのdeque
	std::deque<size_t> usedEntityIDs_;
	std::deque<size_t> removedEntityIDs_;

	/// entityの本体を持つ配列
	std::vector<std::unique_ptr<IEntity>> entities_;
	std::vector<Camera*> cameras_;
	std::vector<IEntity*> doNotDestroyEntities_;

	Camera* mainCamera_ = nullptr;
	Camera* mainCamera2D_ = nullptr;
	Camera* debugCamera_ = nullptr;

	std::function<void(EntityFactory*)> factoryRegisterFunc_;

public:

	void SetMainCamera(Camera* _camera);
	void SetMainCamera(size_t _index);

	void SetMainCamera2D(Camera* _camera);
	void SetMainCamera2D(size_t _index);

	void SetDebugCamera(Camera* _camera);
	void SetDebugCamera(size_t _index);

	const std::vector<std::unique_ptr<IEntity>>& GetEntities() const;

	const std::vector<Camera*>& GetCameras();

	const Camera* GetMainCamera() const;
	Camera* GetMainCamera();

	const Camera* GetMainCamera2D() const;
	Camera* GetMainCamera2D();

	const Camera* GetDebugCamera() const;
	Camera* GetDebugCamera();



};


template<typename T>
inline T* EntityCollection::GenerateEntity() requires std::is_base_of_v<IEntity, T> {

	std::string name = typeid(T).name();
	if (name.find("class ") == 0) {
		name = name.substr(6); // Remove "class " prefix
	}

	IEntity* entity = GenerateEntity(name, true);
	if (!entity) {
		factory_->Register(name, []() { return std::make_unique<T>(); });
		entity = GenerateEntity(name, true);
		if (!entity) {
			Console::Log(std::format("Failed to generate entity of type: {}", name));
		}
	}

	return static_cast<T*>(entity);
}

template<typename T>
inline T* EntityCollection::GenerateCamera() requires std::is_base_of_v<Camera, T> {
	std::unique_ptr<T> camera = std::make_unique<T>(pDxDevice_);
	camera->pEntityComponentSystem_ = pECS_;
	camera->id_ = NewEntityID();
	camera->CommonInitialize();
	camera->Initialize();
	T* cameraPtr = camera.get();
	entities_.push_back(std::move(camera));
	cameras_.push_back(cameraPtr);
	return cameraPtr;
}

template<typename T>
inline T* EntityCollection::FindEntity() requires std::is_base_of_v<IEntity, T> {
	for (const auto& entity : entities_) {
		if (T* found = dynamic_cast<T*>(entity.get())) {
			return found;
		}
	}
	return nullptr;
}

template<typename T>
inline std::vector<T*> EntityCollection::FindEntities() requires std::is_base_of_v<IEntity, T> {
	std::vector<T*> foundEntities;
	for (const auto& entity : entities_) {
		if (T* found = dynamic_cast<T*>(entity.get())) {
			foundEntities.push_back(found);
		}
	}

	return foundEntities;
}

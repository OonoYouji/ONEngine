#include "EntityCollection.h"

/// engine
#include "Engine/DirectX12/Manager/DxManager.h"


EntityCollection::EntityCollection(DxManager* _dxManager)
	: pDxManager_(_dxManager) {}

EntityCollection::~EntityCollection() {}


void EntityCollection::Initialize() {
	entities_.reserve(128);
}

void EntityCollection::Update() {

	for (std::unique_ptr<IEntity>& entity : entities_) {
		entity->Update();
	}
}


Camera* EntityCollection::GenerateCamera() {
	std::unique_ptr<Camera> camera = std::make_unique<Camera>(pDxManager_->GetDxDevice());
	camera->Initialize();

	Camera* cameraPtr = camera.get();
	entities_.push_back(std::move(camera));

	return cameraPtr;
}

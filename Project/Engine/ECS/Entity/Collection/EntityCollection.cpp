#include "EntityCollection.h"

/// std
#include <numbers>

/// engine
#include "Engine/Core/DirectX12/Manager/DxManager.h"
#include "Engine/Core/ImGui/ImGuiMath.h"


EntityCollection::EntityCollection(DxManager* _dxManager)
	: pDxManager_(_dxManager) {}

EntityCollection::~EntityCollection() {}


void EntityCollection::Initialize() {
	entities_.reserve(128);
	cameras_.reserve(8);
	camera2ds_.reserve(8);

	Camera* mainCamera = GenerateCamera();
	mainCamera->SetPosition(Vector3(0.0f, 20.0f, -25.0f));
	mainCamera->SetRotate(Vector3(std::numbers::pi_v<float> / 5.0f, 0.0f, 0.0f));
	SetMainCamera(mainCamera);
}

void EntityCollection::Update() {

	//for (std::unique_ptr<IEntity>& entity : entities_) {
	//	entity->Update();
	//	entity->transform_->Update();
	//}
}


Camera* EntityCollection::GenerateCamera() {
	std::unique_ptr<Camera> camera = std::make_unique<Camera>(pDxManager_->GetDxDevice());
	//camera->pEntityComponentSystem_ = this;
	camera->Initialize();

	Camera* cameraPtr = camera.get();
	entities_.push_back(std::move(camera));
	cameras_.push_back(cameraPtr);

	return cameraPtr;
}

Camera2D* EntityCollection::GenerateCamera2D() {
	std::unique_ptr<Camera2D> camera = std::make_unique<Camera2D>(pDxManager_->GetDxDevice());
	//camera->pEntityComponentSystem_ = this;
	camera->Initialize();

	Camera2D* cameraPtr = camera.get();
	entities_.push_back(std::move(camera));
	camera2ds_.push_back(cameraPtr);

	return cameraPtr;
}

void EntityCollection::RemoveEntity(IEntity* _entity) {
	/// entityの削除
	auto itr = std::remove_if(entities_.begin(), entities_.end(), [_entity](const std::unique_ptr<IEntity>& entity) {
		return entity.get() == _entity;
	});

	entities_.erase(itr, entities_.end());
}

//void EntityCollection::ImGuiDebug() {
//	{	/// Hierarchy window 
//		ImGui::Begin("Hierarchy");
//
//
//		/*/// selectableからentityを選択
//		for (std::unique_ptr<IEntity>& entity : entities_) {
//			if (ImGui::Selectable(entity->GetName().c_str(), selectedEntity_ == entity.get())) {
//				selectedEntity_ = entity.get();
//			}
//		}*/
//
//
//		ImGui::End();
//	}
//
//
//	{	/// Inspector window
//		ImGui::Begin("Inspector");
//
//		/// 選択されていなければ何もしない
//		if (!selectedEntity_) {
//			return;
//		}
//
//		TransformDebug(selectedEntity_->transform_);
//
//		ImGui::End();
//	}
//}


#ifdef _DEBUG

void EntityCollection::ImGui() {

}

#endif // _DEBUG
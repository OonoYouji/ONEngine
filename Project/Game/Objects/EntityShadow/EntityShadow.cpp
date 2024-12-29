#include "EntityShadow.h"

#include "ComponentManager/MeshRenderer/MeshRenderer.h"

EntityShadow::EntityShadow(BaseGameObject* _entity, Model* _model)
	: pEntity_(_entity), pModel_(_model) {
	CreateTag(this);
}

EntityShadow::~EntityShadow() {}

void EntityShadow::Initialize() {

	meshRenderer_ = AddComponent<MeshRenderer>();
	meshRenderer_->SetModel(pModel_);
	meshRenderer_->SetMaterial("white2x2.png");
	meshRenderer_->SetIsLighting(false);
	meshRenderer_->SetColor(Vec4(0.0f, 0.0f, 0.0f, 1.0f));

	SetParent(pEntity_->GetTransform());
	pTransform_->scale.y = 0.1f;

}

void EntityShadow::Update() {

	pTransform_->position.y = -pEntity_->GetPosition().y;

}


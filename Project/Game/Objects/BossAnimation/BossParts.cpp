#include "BossParts.h"

/// std
#include <cassert>


BossParts::BossParts() {
	CreateTag(this);
}

BossParts::~BossParts() {}

void BossParts::Initialize() {

	assert(!modelName_.empty());

	meshRenderer_ = AddComponent<MeshRenderer>();
	meshRenderer_->SetModel(modelName_);
}

void BossParts::Update() {

}

void BossParts::SetModelName(const std::string& _modelName) {
	modelName_ = _modelName;
}

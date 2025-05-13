#include "Skybox.h"

/// engine
#include "Engine/ECS/Component/Component.h"

Skybox::Skybox() {}
Skybox::~Skybox() {}

void Skybox::Initialize() {
	SetTexturePath("Packages/Textures/rostock_laage_airport_4k.dds");
	transform_->scale = Vec3::kOne * 30.0f;
}

void Skybox::Update() {

}

void Skybox::SetTexturePath(const std::string& _path) {
	texturePath_ = _path;
}

const std::string& Skybox::GetTexturePath() const {
	return texturePath_;
}

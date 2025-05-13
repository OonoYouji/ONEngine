#include "Skybox.h"

/// engine
#include "Engine/ECS/Component/Component.h"

Skybox::Skybox() {}
Skybox::~Skybox() {}

void Skybox::Initialize() {
	SetTexturePath("Assets/Textures/circle.png");
}

void Skybox::Update() {

}

void Skybox::SetTexturePath(const std::string& _path) {
	texturePath_ = _path;
}

const std::string& Skybox::GetTexturePath() const {
	return texturePath_;
}

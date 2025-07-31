#include "Skybox.h"


Skybox::Skybox() {
	SetDDSTexturePath("./Packages/Textures/kloofendal_48d_partly_cloudy_puresky_2k.dds");
}
Skybox::~Skybox() {}


void Skybox::SetDDSTexturePath(const std::string& _texturePath) {
	texturePath_ = _texturePath;
}

const std::string& Skybox::GetDDSTexturePath() const {
	return texturePath_;
}
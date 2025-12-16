#include "Skybox.h"

/// externals
#include <imgui.h>

/// editor
#include "Engine/Editor/Math/ImGuiMath.h"

using namespace ONEngine;

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



void ComponentDebug::SkyboxDebug(const Skybox* _skybox) {
	if (!_skybox) {
		return;
	}

	std::string texturePath = _skybox->GetDDSTexturePath();
	Editor::ImMathf::InputText("Skybox Texture Path", &texturePath, ImGuiInputTextFlags_ReadOnly);

}


void ONEngine::from_json(const nlohmann::json& _j, Skybox& _s) {
	if (_j.contains("enable")) {
		_s.enable = _j.at("enable").get<int>();
	}

	if (_j.contains("texturePath")) {
		_s.SetDDSTexturePath(_j.at("texturePath").get<std::string>());
	} else {
		_s.SetDDSTexturePath("./Packages/Textures/kloofendal_48d_partly_cloudy_puresky_2k.dds");
	}
}

void ONEngine::to_json(nlohmann::json& _j, const Skybox& _s) {
	_j = nlohmann::json{
		{ "type", "Skybox" },
		{ "enable", _s.enable },
		{ "texturePath", _s.GetDDSTexturePath() },
	};
}


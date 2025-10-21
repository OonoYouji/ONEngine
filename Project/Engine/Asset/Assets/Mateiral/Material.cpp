#include "Material.h"

/// externals
#include <imgui.h>

/// engine
#include "Engine/Editor/Commands/ImGuiCommand/ImGuiCommand.h"


void ShowGuiMaterial(const std::string& _label, Material* _material) {
	if(!_material) {
		return;
	}

	/// Idを振る
	ImGui::PushID(_label.c_str());

	/// 色
	ImMath::DragFloat4("BaseColor", &_material->baseColor_);


	ImGui::PopID();
}


/// //////////////////////////////////////////////////////////
/// Material
/// //////////////////////////////////////////////////////////

Material::Material() = default;
Material::~Material() = default;
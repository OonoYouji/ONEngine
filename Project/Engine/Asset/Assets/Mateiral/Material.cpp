#include "Material.h"

/// std
#include <fstream>
#include <filesystem>

/// externals
#include <imgui.h>

/// engine
#include "Engine/Editor/Commands/ImGuiCommand/ImGuiCommand.h"


void ShowGuiMaterial(const std::string& _label, Material* _material) {
	if (!_material) {
		return;
	}

	/// Idを振る
	ImGui::PushID(_label.c_str());

	/// 色
	ImMath::DragFloat4("BaseColor", &_material->baseColor);


	ImGui::PopID();
}

Material GenerateMaterial() {
	Material material;

	material.guid = GenerateGuid();
	material.baseColor = Vector4::kWhite;
	material.postEffectFlags = 1;

	return material;
}

void GenerateMaterialFile(const std::string& _filepath, Material* _material) {
	/// _filepathにマテリアル情報を書き込む

	/// _filepathがないなら生成する
	if (std::filesystem::exists(_filepath) == false) {
		std::ofstream ofs(_filepath);
		ofs.close();
	}

	std::ofstream ofs(_filepath);
	if (!ofs) {
		return;
	}


	/// 引数のマテリアル情報を使用する
	/// nullptrならデフォルト値で生成する
	Material material;
	if (_material) {
		material = *_material;
	} else {
		material = GenerateMaterial();
	}

	ofs << "MaterialFileVersion: 1\n";
	ofs << "BaseColor: " << material.baseColor.x << " " << material.baseColor.y << " " << material.baseColor.z << " " << material.baseColor.w << "\n";
	ofs << "PostEffectFlags: " << material.postEffectFlags << "\n";

	ofs.close();
}


/// //////////////////////////////////////////////////////////
/// Material
/// //////////////////////////////////////////////////////////

Material::Material() = default;
Material::~Material() = default;
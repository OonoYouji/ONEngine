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
	ofs << "guid: " << material.guid.ToString() << "\n";
	ofs << "BaseColor: " << material.baseColor.x << " " << material.baseColor.y << " " << material.baseColor.z << " " << material.baseColor.w << "\n";
	ofs << "PostEffectFlags: " << material.postEffectFlags << "\n";

	ofs.close();
}

void from_json(const nlohmann::json& _j, Material& _material) {
	_material.guid = _j.value("guid", Guid{});
	_material.baseColor = _j.value("baseColor", Vector4::kWhite);
	_material.postEffectFlags = _j.value("postEffectFlags", 1u);
}

void to_json(nlohmann::json& _j, const Material& _material) {
	_j = {
		{ "guid", _material.guid },
		{ "baseColor", _material.baseColor },
		{ "postEffectFlags", _material.postEffectFlags }
	};
}


/// //////////////////////////////////////////////////////////
/// Material
/// //////////////////////////////////////////////////////////

Material::Material() = default;
Material::~Material() = default;



bool Material::HasBaseTexture() const {
	return baseTextureGuid_.has_value();
}

const Guid& Material::GetBaseTextureGuid() const {
	return baseTextureGuid_.value();
}

bool Material::HasNormalTexture() const {
	return normalTextureGuid_.has_value();
}

const Guid& Material::GetNormalTextureGuid() const {
	return normalTextureGuid_.value();
}

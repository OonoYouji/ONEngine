#include "Material.h"

/// std
#include <fstream>
#include <filesystem>

/// externals
#include <imgui.h>

/// engine
#include "Engine/Editor/Commands/ImGuiCommand/ImGuiCommand.h"

namespace ONEngine::Asset {

/**
 * @brief デフォルト値が設定された新しいマテリアルを生成します。
 * @return 生成されたMaterialオブジェクト
 */
Material GenerateMaterial() {
	/// ----- 新規のMaterialを作成して返す ----- ///
	Material material;

	material.guid = GenerateGuid();
	material.baseColor = Vector4::White;
	material.postEffectFlags = 1;

	return material;
}

/**
 * @brief 指定されたファイルパスにマテリアルファイルを新規生成します。
 * @param _filepath 生成するファイルパス
 * @param _material 参照するマテリアル（nullptrならデフォルト値を使用）
 */
void GenerateMaterialFile(const std::string& _filepath, Material* _material) {
	/// _filepathにマテリアル情報を書き込む

	/// _filepathがないなら生成する
	if(std::filesystem::exists(_filepath) == false) {
		std::ofstream ofs(_filepath);
		ofs.close();
	}

	std::ofstream ofs(_filepath);
	if(!ofs) {
		return;
	}


	/// 引数のマテリアル情報を使用する
	/// nullptrならデフォルト値で生成する
	Material material;
	if(_material) {
		material = *_material;
	} else {
		material = GenerateMaterial();
	}

	/// ファイルに情報を書きこむ
	ofs << "MaterialFileVersion: 1\n";
	ofs << "guid: " << material.guid.ToString() << "\n";
	ofs << "BaseColor: " << material.baseColor.x << " " << material.baseColor.y << " " << material.baseColor.z << " " << material.baseColor.w << "\n";
	ofs << "PostEffectFlags: " << material.postEffectFlags << "\n";
	ofs << "UVTransform_Position: " << material.uvTransform.position.x << " " << material.uvTransform.position.y << "\n";
	ofs << "UVTransform_Scale: " << material.uvTransform.scale.x << " " << material.uvTransform.scale.y << "\n";
	ofs << "UVTransform_Rotate: " << material.uvTransform.rotate << "\n";

	ofs.close();
}


/// ---------------------------------------------------
/// Json変換
/// ---------------------------------------------------

/**
 * @brief jsonオブジェクトからMaterialオブジェクトへデシリアライズを行います。
 * @param _j jsonオブジェクト
 * @param _material 復元先マテリアルの参照
 */
void from_json(const nlohmann::json& _j, Material& _material) {
	/// ----- JsonデータをMaterialに変換する ----- ///

	_j.at("baseColor").get<ONEngine::Vector4>();

	_material.guid = _j.value("guid", Guid{});
	_material.baseColor = _j.value("baseColor", Vector4::Red);
	_material.postEffectFlags = _j.value("postEffectFlags", 1u);
	_material.uvTransform = _j.value("uvTransform", UVTransform{});

	Guid baseTextureGuid = _j.value("baseTextureGuid", Guid::kInvalid);
	if(baseTextureGuid.CheckValid()) {
		_material.baseTextureGuid_ = baseTextureGuid;
	} else {
		_material.baseTextureGuid_ = std::nullopt;
	}

	Guid normalTextureGuid = _j.value("normalTextureGuid", Guid::kInvalid);
	if(normalTextureGuid.CheckValid()) {
		_material.normalTextureGuid_ = normalTextureGuid;
	} else {
		_material.normalTextureGuid_ = std::nullopt;
	}
}

/**
 * @brief Materialオブジェクトからjsonオブジェクトへシリアライズを行います。
 * @param _j jsonオブジェクト
 * @param _material 変換元マテリアル
 */
void to_json(nlohmann::json& _j, const Material& _material) {
	/// ----- MaterialデータをJsonに変換する ----- ///
	_j = {
		{ "guid", _material.guid },
		{ "baseColor", _material.baseColor },
		{ "postEffectFlags", _material.postEffectFlags },
		{ "uvTransform", _material.uvTransform },
		{ "baseTextureGuid", _material.baseTextureGuid_.has_value() ? _material.baseTextureGuid_.value() : Guid::kInvalid },
		{ "normalTextureGuid", _material.normalTextureGuid_.has_value() ? _material.normalTextureGuid_.value() : Guid::kInvalid },
	};
}


/// //////////////////////////////////////////////////////////
/// Material
/// //////////////////////////////////////////////////////////

/**
 * @brief コンストラクタ。デフォルト値を設定します。
 */
Material::Material() {
	baseColor = Vector4::White;
	postEffectFlags = 1;
	uvTransform = UVTransform();
};
/**
 * @brief デストラクタ。
 */
Material::~Material() = default;



/**
 * @brief アルベドテクスチャが設定されているか判定します。
 * @return 設定されている場合はtrue
 */
bool Material::HasBaseTexture() const {
	return baseTextureGuid_.has_value();
}

/**
 * @brief アルベドテクスチャのGUIDを取得します。
 * @return アルベドテクスチャのGUID
 */
const Guid& Material::GetBaseTextureGuid() const {
	return baseTextureGuid_.value();
}

/**
 * @brief アルベドテクスチャのGUIDを設定します。
 * @param _guid 設定するGUID
 */
void Material::SetBaseTextureGuid(const Guid& _guid) {
	/// ----- base texture guidの設定 ----- ///
	if(baseTextureGuid_.has_value()) {
		baseTextureGuid_.value() = _guid;
	} else {
		baseTextureGuid_ = std::make_optional<Guid>();
		baseTextureGuid_ = _guid;
	}
}

/**
 * @brief 法線テクスチャが設定されているか判定します。
 * @return 設定されている場合はtrue
 */
bool Material::HasNormalTexture() const {
	return normalTextureGuid_.has_value();
}

/**
 * @brief 法線テクスチャのGUIDを取得します。
 * @return 法線テクスチャのGUID
 */
const Guid& Material::GetNormalTextureGuid() const {
	return normalTextureGuid_.value();
}

/**
 * @brief 法線テクスチャのGUIDを設定します。
 * @param _guid 設定するGUID
 */
void Material::SetNormalTextureGuid(const Guid& _guid) {
	/// ----- 法線 texture の guid を登録 ----- ///
	if(normalTextureGuid_.has_value()) {
		normalTextureGuid_.value() = _guid;
	} else {
		normalTextureGuid_ = std::make_optional<Guid>();
		normalTextureGuid_ = _guid;
	}
}

} /// namespace ONEngine::Asset
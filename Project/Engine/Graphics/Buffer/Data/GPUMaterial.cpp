#include "GPUMaterial.h"

/// engine
#include "Engine/ECS/Entity/GameEntity/GameEntity.h"
#include "Engine/Editor/Commands/ComponentEditCommands/ComponentJsonConverter.h"
#include "Engine/Asset/Collection/AssetCollection.h"



/// ------------------------------------------------
/// GPUMaterial Json変換
/// ------------------------------------------------

void to_json(nlohmann::json& _j, const GPUMaterial& _material) {
	_j = nlohmann::json{
		{ "uvTransform", _material.uvTransform },
		{ "baseColor", _material.baseColor },
		{ "postEffectFlags", _material.postEffectFlags },
		{ "entityId", _material.entityId },
		{ "baseTextureId", _material.baseTextureId },
		{ "normalTextureId", _material.normalTextureId }
	};
}

void from_json(const nlohmann::json& _j, GPUMaterial& _material) {
	_material.uvTransform     = _j.value("uvTransform", UVTransform{});
	_material.baseColor       = _j.value("baseColor", Vector4::kWhite);
	_material.postEffectFlags = _j.value("postEffectFlags", PostEffectFlags_None);
	_material.entityId        = _j.value("entityId", 0);
	_material.baseTextureId   = _j.value("baseTextureId", -1);
	_material.normalTextureId = _j.value("normalTextureId", -1);
}

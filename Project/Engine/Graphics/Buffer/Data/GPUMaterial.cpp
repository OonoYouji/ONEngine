#include "GPUMaterial.h"

/// engine
#include "Engine/ECS/Entity/GameEntity/GameEntity.h"
#include "Engine/Editor/Commands/ComponentEditCommands/ComponentJsonConverter.h"
#include "Engine/Asset/Collection/AssetCollection.h"

void to_json(nlohmann::json& _j, const UVTransform& _uvTransform) {
	_j = nlohmann::json{
		{ "position", _uvTransform.position },
		{ "scale", _uvTransform.scale },
		{ "rotate", _uvTransform.rotate },
	};
}

void from_json(const nlohmann::json& _j, UVTransform& _uvTransform) {
	_uvTransform.position = _j.value("position", Vector2::kZero);
	_uvTransform.scale = _j.value("scale", Vector2::kOne);
	_uvTransform.rotate = _j.value("rotate", 0.0f);
}


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

void to_json(nlohmann::json& _j, const CPUMaterial& _material) {
	_j = nlohmann::json{
		{ "uvTransform", _material.uvTransform },
		{ "baseColor", _material.baseColor },
		{ "postEffectFlags", _material.postEffectFlags },
		{ "baseTextureGuid", _material.baseTextureIdPair.first },
		{ "normalTextureGuid", _material.normalTextureIdPair.first }
	};
}

void from_json(const nlohmann::json& _j, CPUMaterial& _material) {
}

CPUMaterial::CPUMaterial() = default;
CPUMaterial::~CPUMaterial() = default;

GPUMaterial CPUMaterial::ToGPUMaterial() {
	return GPUMaterial{
		.uvTransform     = UVTransform{},
		.baseColor       = Vector4::kWhite,
		.postEffectFlags = PostEffectFlags_None,
		.entityId        = pOwnerEntity_ ? pOwnerEntity_->GetId() : 0,
		.baseTextureId   = baseTextureIdPair.second,
		.normalTextureId = normalTextureIdPair.second,
	};
}

void CPUMaterial::SetOwnerEntity(GameEntity* _pEntity) {
	pOwnerEntity_ = _pEntity;
}

#include "SpriteRenderer.h"

/// external
#include <imgui.h>

/// engine
#include "Engine/Core/Utility/Tools/Log.h"
#include "Engine/Core/ImGui/Math/ImGuiMath.h"
#include "Engine/Editor/Commands/ComponentEditCommands/ComponentJsonConverter.h"
#include "Engine/Asset/Collection/AssetCollection.h"


/// /////////////////////////////////////////////////////////////
/// デバッグ用のSpriteRenderer
/// /////////////////////////////////////////////////////////////

void COMP_DEBUG::SpriteDebug(SpriteRenderer* _sr, AssetCollection* _assetCollection) {
	if (!_sr) {
		return;
	}

	float indentValue = 1.2f;
	ImGui::Indent(indentValue);
	/// colorの変更
	if (ImMathf::MaterialEdit("material", &_sr->gpuMaterial_, _assetCollection)) {}
	ImGui::Unindent(indentValue);
}


void to_json(nlohmann::json& _j, const SpriteRenderer& _sr) {
	_j = nlohmann::json{
		{ "type", "SpriteRenderer" },
		{ "enable", _sr.enable },
		{ "material", _sr.gpuMaterial_ }
	};
}

void from_json(const nlohmann::json& _j, SpriteRenderer& _sr) {
	_sr.enable = _j.value("enable", static_cast<int>(true));
	_sr.gpuMaterial_ = _j.value("material", GPUMaterial{});
}


/// /////////////////////////////////////////////////////////////
/// SpriteRenderer
/// /////////////////////////////////////////////////////////////

SpriteRenderer::SpriteRenderer() {
	gpuMaterial_.baseColor = Vector4::kWhite;
	gpuMaterial_.entityId = 0;
	gpuMaterial_.baseTextureId = 0;
	gpuMaterial_.uvTransform = UVTransform();
	gpuMaterial_.postEffectFlags = 0;
}
SpriteRenderer::~SpriteRenderer() {}


void SpriteRenderer::SetColor(const Vector4& _color) {
	gpuMaterial_.baseColor = _color;
}

const Vector4& SpriteRenderer::GetColor() const {
	return gpuMaterial_.baseColor;
}

const GPUMaterial& SpriteRenderer::GetMaterial() const {
	return gpuMaterial_;
}

GPUMaterial& SpriteRenderer::GetMaterial() {
	return gpuMaterial_;
}


/// ===================================================
/// csで使用するための関数群
/// ===================================================

Vector4 InternalSpriteMethods::InternalGetColor(uint64_t _nativeHandle) {
	SpriteRenderer* sr = reinterpret_cast<SpriteRenderer*>(_nativeHandle);
	if (sr) {
		return sr->GetColor();
	}

	Console::LogError("InternalSpriteMethods::InternalGetColor() | native handle is invalid");

	return Vector4();
}

void InternalSpriteMethods::InternalSetColor(uint64_t _nativeHandle, Vector4 _color) {
	SpriteRenderer* sr = reinterpret_cast<SpriteRenderer*>(_nativeHandle);
	if (sr) {
		sr->SetColor(_color);
		return;
	}

	Console::LogError("InternalSpriteMethods::InternalSetColor() | native handle is invalid");
}

#include "SpriteRenderer.h"

/// external
#include <imgui.h>

/// engine
#include "Engine/Core/Utility/Tools/Log.h"
#include "Engine/Core/ImGui/Math/ImGuiMath.h"
#include "Engine/Editor/Commands/ComponentEditCommands/ComponentJsonConverter.h"
#include "Engine/Graphics/Resource/GraphicsResourceCollection.h"

SpriteRenderer::SpriteRenderer() {
	material_.baseColor = Vector4::kWhite;
	material_.entityId = 0;
	material_.baseTextureId = -1;
	material_.uvTransform = UVTransform();
	material_.postEffectFlags = 0;
}
SpriteRenderer::~SpriteRenderer() {}


void SpriteRenderer::SetColor(const Vector4& _color) {
	material_.baseColor = _color;
}

const Vector4& SpriteRenderer::GetColor() const {
	return material_.baseColor;
}

const Material& SpriteRenderer::GetMaterial() const {
	return material_;
}

Material& SpriteRenderer::GetMaterial() {
	return material_;
}

/// /////////////////////////////////////////////////////////////
/// デバッグ用のSpriteRenderer
/// /////////////////////////////////////////////////////////////

void COMP_DEBUG::SpriteDebug(SpriteRenderer* _sr, GraphicsResourceCollection* _resourceCollection) {
	if (!_sr) {
		return;
	}
	Material& material = _sr->GetMaterial();
	ImGui::Indent(0.9f);
	/// colorの変更
	if (ImMathf::MaterialEdit("material", &material, _resourceCollection)) {
	}
	ImGui::Unindent(0.9f);
}


void to_json(nlohmann::json& _j, const SpriteRenderer& _sr) {
	_j = nlohmann::json{
		{ "type", "SpriteRenderer" },
		{ "enable", _sr.enable },
		{ "material", _sr.GetMaterial() }
	};
}

void from_json(const nlohmann::json& _j, SpriteRenderer& _sr) {
	_sr.enable = _j.value("enable", static_cast<int>(true));
	Material material = _j.value("material", Material());
	_sr.GetMaterial() = material;
}
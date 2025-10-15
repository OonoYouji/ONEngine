#include "SpriteRenderer.h"

/// external
#include <imgui.h>

/// engine
#include "Engine/Core/Utility/Tools/Log.h"
#include "Engine/Core/ImGui/Math/ImGuiMath.h"
#include "Engine/Editor/Commands/ComponentEditCommands/ComponentJsonConverter.h"
#include "Engine/Graphics/Resource/GraphicsResourceCollection.h"


/// /////////////////////////////////////////////////////////////
/// デバッグ用のSpriteRenderer
/// /////////////////////////////////////////////////////////////

void COMP_DEBUG::SpriteDebug(SpriteRenderer* _sr, GraphicsResourceCollection* _resourceCollection) {
	if (!_sr) {
		return;
	}

	float indentValue = 1.2f;
	ImGui::Indent(indentValue);
	/// colorの変更
	if (ImMathf::MaterialEdit("material", &_sr->material_, _resourceCollection)) {}
	ImGui::Unindent(indentValue);
}


void to_json(nlohmann::json& _j, const SpriteRenderer& _sr) {
	_j = nlohmann::json{
		{ "type", "SpriteRenderer" },
		{ "enable", _sr.enable },
		{ "material", _sr.material_ }
	};
}

void from_json(const nlohmann::json& _j, SpriteRenderer& _sr) {
	_sr.enable = _j.value("enable", static_cast<int>(true));
	_sr.material_ = _j.value("material", Material{});
}


/// /////////////////////////////////////////////////////////////
/// SpriteRenderer
/// /////////////////////////////////////////////////////////////

SpriteRenderer::SpriteRenderer() {
	material_.baseColor = Vector4::kWhite;
	material_.entityId = 0;
	material_.baseTextureId = 0;
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

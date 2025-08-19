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
	texturePath_ = "./Packages/Textures/white.png";
}
SpriteRenderer::~SpriteRenderer() {}


void SpriteRenderer::SetTexturePath(const std::string& _path) {
	texturePath_ = _path;
}

void SpriteRenderer::SetColor(const Vector4& _color) {
	material_.baseColor = _color;
}

const std::string& SpriteRenderer::GetTexturePath() const {
	return texturePath_;
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
	std::string texturePath = _sr->GetTexturePath();

	ImGui::Indent(0.9f);
	/// colorの変更
	if (ImMathf::MaterialEdit("material", &material, _resourceCollection)) {
	}
	ImGui::Unindent(0.9f);
}

void COMP_DEBUG::SpriteTextureDebug(SpriteRenderer* _sr, std::string& _texturePath) {
	/// textureの変更
	ImGui::Text("texture path");
	ImGui::InputText("##texture", _texturePath.data(), _texturePath.capacity(), ImGuiInputTextFlags_ReadOnly);
	if (ImGui::BeginDragDropTarget()) {
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("AssetData")) {

			/// ペイロードが存在する場合
			if (payload->Data) {
				const char* droppedPath = static_cast<const char*>(payload->Data);
				std::string path = std::string(droppedPath);

				/// テクスチャのパスが有効な形式か確認
				if (path.find(".png") != std::string::npos
					|| path.find(".jpg") != std::string::npos
					|| path.find(".jpeg") != std::string::npos) {
					_sr->SetTexturePath(path);

					Console::Log(std::format("Texture path set to: {}", path));
				} else {
					Console::LogError("Invalid texture format. Please use .png, .jpg, or .jpeg.");
				}
			}
		}

		ImGui::EndDragDropTarget();
	}
}


void to_json(nlohmann::json& _j, const SpriteRenderer& _sr) {
	_j = nlohmann::json{
		{ "type", "SpriteRenderer" },
		{ "enable", _sr.enable },
		{ "texturePath", _sr.GetTexturePath() },
		{ "material", _sr.GetMaterial() }
	};
}

void from_json(const nlohmann::json& _j, SpriteRenderer& _sr) {
	_sr.enable = _j.value("enable", static_cast<int>(true));
	_sr.SetTexturePath(_j.value("texturePath", "./Packages/Textures/white.png"));
	Material material = _j.value("material", Material());
}
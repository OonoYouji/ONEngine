#include "SpriteRenderer.h"

/// external
#include <imgui.h>

/// engine
#include "Engine/Core/Utility/Tools/Log.h"
#include "Engine/Core/ImGui/Math/ImGuiMath.h"


SpriteRenderer::SpriteRenderer() {
	texturePath_ = "Packages/Textures/white.png";
}
SpriteRenderer::~SpriteRenderer() {}


void SpriteRenderer::SetTexturePath(const std::string& _path) {
	texturePath_ = _path;
}

void SpriteRenderer::SetColor(const Vector4& _color) {
	color_ = _color;
}

const std::string& SpriteRenderer::GetTexturePath() const {
	return texturePath_;
}

const Vector4& SpriteRenderer::GetColor() const {
	return color_;
}

/// /////////////////////////////////////////////////////////////
/// デバッグ用のSpriteRenderer
/// /////////////////////////////////////////////////////////////

void COMP_DEBUG::SpriteDebug(SpriteRenderer* _sr) {
	if(!_sr) {
		return;
	}

	Vector4 color = _sr->GetColor();
	std::string texturePath = _sr->GetTexturePath();

	/// colorの変更
	if (ImMathf::ColorEdit("color", &color)) {
		_sr->SetColor(color);
	}

	/// textureの表示
	SpriteTextureDebug(_sr, texturePath);
	

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

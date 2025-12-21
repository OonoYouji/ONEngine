#include "Texture2DPreview.h"

/// externals
#include <imgui.h>

void Editor::ShowTexture2DPreview(const std::string& _name, ONEngine::Texture* _texture, const ONEngine::Vector2& _textureSize, float _previewFactor) {
	if(_name != "") {
		ImGui::Text("%s", _name.c_str());
	} else {
		ImGui::Text("Texture2D Preview");
	}

	if(_texture) {
		ONEngine::Vector2 aspectRatio = _texture->GetTextureSize();
		aspectRatio /= (std::max)(aspectRatio.x, aspectRatio.y);

		ImTextureID texId = reinterpret_cast<ImTextureID>(_texture->GetSRVGPUHandle().ptr);
		ImGui::Image(texId, ImVec2(_previewFactor * aspectRatio.x, _previewFactor * aspectRatio.y));
	} else {
		/// テクスチャがない場合はドラッグドロップ領域を表示する
		ImVec2 size = ImVec2(_previewFactor, _previewFactor);
		ImVec2 pos = ImGui::GetCursorScreenPos();
		ImDrawList* drawList = ImGui::GetWindowDrawList();

		/// InvisibleButton はクリック判定やDragDropのターゲット領域になる
		ImGui::InvisibleButton("DropArea", size);

		/// 視覚的な四角形を描く
		ImU32 imColor = IM_COL32(100, 100, 255, 100); // 半透明の青
		drawList->AddRectFilled(pos, ImVec2(pos.x + size.x, pos.y + size.y), imColor, 4.0f);

		/// 枠線
		drawList->AddRect(pos, ImVec2(pos.x + size.x, pos.y + size.y), IM_COL32(255, 255, 255, 200), 4.0f, 0, 2.0f);
	}
}

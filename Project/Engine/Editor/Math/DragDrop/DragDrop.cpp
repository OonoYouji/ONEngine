#include "DragDrop.h"

/// externals
#include <imgui.h>

/// engine
#include "Engine/Asset/Collection/AssetCollection.h"
#include "Engine/Asset/AssetType.h"

using namespace Editor;

void Editor::DragDrop::SetDragDropPayload(const std::string& _filepath, const ONEngine::Guid& _guid, ONEngine::AssetCollection* _ac) {
	if(ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
		static AssetPayload payload;
		payload.filePath = _filepath;
		payload.guid = _ac->GetAssetGuidFromPath(payload.filePath);

		/// 持っているアセットのタイプに応じてプレビューを表示
		ONEngine::AssetType type = _ac->GetAssetTypeFromGuid(payload.guid);
		if(type == ONEngine::AssetType::Texture) {
			ONEngine::Texture* tex = _ac->GetTexture(payload.filePath);
			if(tex) {
				const float previewSize = 64.0f;

				ONEngine::Vector2 aspectRatio = tex->GetTextureSize();
				aspectRatio /= (std::max)(aspectRatio.x, aspectRatio.y);
				ImTextureID texId = reinterpret_cast<ImTextureID>(tex->GetSRVGPUHandle().ptr);
				ImGui::Image(texId, ImVec2(previewSize * aspectRatio.x, previewSize * aspectRatio.y));
			}
		}

		const AssetPayload* assetPtr = &payload;
		ImGui::SetDragDropPayload("AssetData", &assetPtr, sizeof(AssetPayload*));
		ImGui::EndDragDropSource();
	}
}

AssetPayload* DragDrop::GetDragDropPayload() {
	AssetPayload* result = nullptr;

	if(ImGui::BeginDragDropTarget()) {
		if(const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("AssetData")) {
			/// ペイロードが存在する場合
			if(payload->Data) {
				result = *static_cast<Editor::AssetPayload**>(payload->Data);
			}
		}

		ImGui::EndDragDropTarget();
	}

	return result;
}
#include "AssetDebugger.h"

/// externals
#include <imgui.h>

/// engine
#include "Engine/Asset/Collection/AssetCollection.h"
#include "Engine/Core/ImGui/Math/ImGuiMath.h"
#include "Engine/Core/ImGui/Math/AssetPayload.h"
#include "Engine/Editor/Commands/ImGuiCommand/ImGuiCommand.h"



namespace {

	/// テクスチャのドロップスペースの表示
	void DrawTextureDropSpace(const std::string& _areaName = "DropArea") {
		/// テクスチャがない場合はドラッグドロップ領域を表示する
		ImVec2 size = ImVec2(64, 64);
		ImVec2 pos = ImGui::GetCursorScreenPos();
		ImDrawList* drawList = ImGui::GetWindowDrawList();
		// InvisibleButton はクリック判定やDragDropのターゲット領域になる
		ImGui::InvisibleButton(_areaName.c_str(), size);
		// 視覚的な四角形を描く
		ImU32 imColor = IM_COL32(100, 100, 255, 100); // 半透明の青
		drawList->AddRectFilled(pos, ImVec2(pos.x + size.x, pos.y + size.y), imColor, 4.0f);
		// 枠線
		drawList->AddRect(pos, ImVec2(pos.x + size.x, pos.y + size.y), IM_COL32(255, 255, 255, 200), 4.0f, 0, 2.0f);
	}


	/// テクスチャのプレビュー表示
	void DrawTexturePreview(const Texture* _texture) {
		if (!_texture) {
			return;
		}
		Vector2 aspectRatio = _texture->GetTextureSize();
		aspectRatio /= (std::max)(aspectRatio.x, aspectRatio.y);
		ImTextureID texId = reinterpret_cast<ImTextureID>(_texture->GetSRVGPUHandle().ptr);
		ImGui::Image(texId, ImVec2(64.0f * aspectRatio.x, 64.0f * aspectRatio.y));
	}


	/// テクスチャのドロップ処理
	bool HandleTextureDrop(Material* _material) {
		bool edit = false;
		if (ImGui::BeginDragDropTarget()) {
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("AssetData")) {
				if (payload->Data) {
					AssetPayload* assetPayload = *static_cast<AssetPayload**>(payload->Data);
					std::string path = assetPayload->filePath;
					AssetType type = GetAssetTypeFromExtension(Mathf::FileExtension(path));
					if (type == AssetType::Texture) {
						const Guid& guid = assetPayload->guid;
						_material->SetBaseTextureGuid(guid);
						edit = true;
					}
				}
			}
			ImGui::EndDragDropTarget();
		}
		return edit;
	}


	/// 法線テクスチャのドロップ処理
	bool HandleNormalTextureDrop(Material* _material) {
		bool edit = false;
		if (ImGui::BeginDragDropTarget()) {
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("AssetData")) {
				if (payload->Data) {
					AssetPayload* assetPayload = *static_cast<AssetPayload**>(payload->Data);
					std::string path = assetPayload->filePath;
					AssetType type = GetAssetTypeFromExtension(Mathf::FileExtension(path));
					if (type == AssetType::Texture) {
						const Guid& guid = assetPayload->guid;
						_material->SetNormalTextureGuid(guid);
						edit = true;
					}
				}
			}
			ImGui::EndDragDropTarget();
		}
		return edit;
	}

}	/// namespace




bool ImMathf::MaterialEdit(const std::string& _label, Material* _material, AssetCollection* _assetCollection, bool _isEditNormalTexture) {

	/// nullptr check
	if (!_material) {
		return false;
	}

	/// 折りたたみヘッダー
	if (!ImGui::CollapsingHeader(_label.c_str())) {
		return false;
	}

	bool edit = false;

	/// ---------------------------------------------------
	/// 色の編集
	/// ---------------------------------------------------
	edit |= ImMathf::DragFloat4("Base Color", &_material->baseColor, 0.01f, 0.0f, 1.0f);


	/// Indentを付けて見やすくする
	ImGui::Indent();

	/// ---------------------------------------------------
	/// uvTransform
	/// ---------------------------------------------------
	if (ImGui::CollapsingHeader("UV Transform")) {
		edit |= ImMathf::UVTransformEdit("UV Transform", &_material->uvTransform);
	}


	/// ---------------------------------------------------
	/// ポストエフェクトフラグの編集
	/// ---------------------------------------------------
	if (ImGui::CollapsingHeader("PostEffectFlags")) {
		/// ポストエフェクトのフラグ
		if (ImGui::CheckboxFlags("Lighting", &_material->postEffectFlags, PostEffectFlags_Lighting)) {
			edit = true;
		}

		if (ImGui::CheckboxFlags("Grayscale", &_material->postEffectFlags, PostEffectFlags_Grayscale)) {
			edit = true;
		}

		if (ImGui::CheckboxFlags("EnvironmentReflection", &_material->postEffectFlags, PostEffectFlags_EnvironmentReflection)) {
			edit = true;
		}
	}


	if (ImGui::CollapsingHeader("Texture")) {

		/// ---------------------------------------------------
		/// ベーステクスチャの編集
		/// ---------------------------------------------------
		bool hasTextureGuid = _material->HasBaseTexture();
		if (hasTextureGuid) {
			DrawTexturePreview(_assetCollection->GetTexture(_assetCollection->GetTexturePath(_material->GetBaseTextureGuid())));
		} else {
			DrawTextureDropSpace("BaseTex");
		}
		HandleTextureDrop(_material);


		/// ---------------------------------------------------
		/// 法線テクスチャの編集
		/// ---------------------------------------------------
		bool hasNormalTextureGuid = _material->HasNormalTexture();
		if (hasNormalTextureGuid) {
			DrawTexturePreview(_assetCollection->GetTexture(_assetCollection->GetTexturePath(_material->GetNormalTextureGuid())));
		} else {
			DrawTextureDropSpace("NormalTex");
		}
		HandleNormalTextureDrop(_material);

	}


	ImGui::Unindent();

	return edit;
}

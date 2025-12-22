#include "DissolveMeshRenderer.h"

/// externals
#include <magic_enum/magic_enum.hpp>

/// engine
#include "Engine/Asset/Collection/AssetCollection.h"
#include "Engine/ECS/Entity/GameEntity/GameEntity.h"
#include "Engine/Editor/EditorUtils.h"

using namespace ONEngine;

void ONEngine::ShowGUI(DissolveMeshRenderer* _dmr, AssetCollection* _ac) {
	if(!_dmr) {
		return;
	}

	Editor::AssetPayload* payload = nullptr;

	/// mesh
	std::string meshName = _ac->GetAssetPath<Model>(_dmr->meshGuid_);
	ImGui::Text("Mesh: ");
	ImGui::SameLine();
	ImGui::InputText("##mesh", meshName.data(), meshName.capacity(), ImGuiInputTextFlags_ReadOnly);
	payload = Editor::DragDrop::GetDragDropPayload();
	if(payload) {
		if(_ac->GetAssetTypeFromGuid(payload->guid) == AssetType::Mesh) {
			_dmr->meshGuid_ = payload->guid;
		}
	}

	/// dissolve texture
	const float texturePreviewSize = 64.0f;
	const std::string dissolveTexName = _ac->GetAssetPath<Texture>(_dmr->dissolveTexture_);
	const Texture* dissolveTex = _ac->GetTextureFromGuid(_dmr->dissolveTexture_);
	Editor::ShowTexture2DPreview(dissolveTexName, const_cast<Texture*>(dissolveTex), dissolveTex->GetTextureSize(), texturePreviewSize);
	payload = Editor::DragDrop::GetDragDropPayload();
	if(payload) {
		if(_ac->GetAssetTypeFromGuid(payload->guid) == AssetType::Texture) {
			_dmr->dissolveTexture_ = payload->guid;
		}
	}

	/// compare
	Editor::Combo<DissolveCompare>("Dissolve Compare", _dmr->dissolveCompare_);

	Editor::SliderFloat("Dissolve Threshold", _dmr->dissolveThreshold_, 0.0f, 1.0f);


	/// material
	Editor::ImMathf::MaterialEdit("Material##MeshRenderer", &_dmr->material_, _ac);


}

void ONEngine::from_json(const nlohmann::json& _j, DissolveMeshRenderer& _dmr) {
	_dmr.meshGuid_ = _j.value("meshGuid", Guid::kInvalid);
	_dmr.material_ = _j.value("material", Material());
	_dmr.dissolveTexture_ = _j.value("dissolveTexture", Guid::kInvalid);
}

void ONEngine::to_json(nlohmann::json& _j, const DissolveMeshRenderer& _dmr) {
	_j = {
		{ "type", "DissolveMeshRenderer" },
		{ "meshGuid", _dmr.meshGuid_ },
		{ "material", _dmr.material_ },
		{ "dissolveTexture", _dmr.dissolveTexture_ }
	};
}

/// ///////////////////////////////////////////////////
/// ここから DissolveMeshRenderer の定義
/// ///////////////////////////////////////////////////

DissolveMeshRenderer::DissolveMeshRenderer() {}
DissolveMeshRenderer::~DissolveMeshRenderer() {}


const Guid& DissolveMeshRenderer::GetMeshGuid() const {
	return meshGuid_;
}

const Guid& DissolveMeshRenderer::GetDissolveTextureGuid() const {
	return dissolveTexture_;
}

uint32_t DissolveMeshRenderer::GetDissolveTextureId(AssetCollection* _ac) const {
	const Texture* dissolveTex = _ac->GetTextureFromGuid(dissolveTexture_);
	if(dissolveTex) {
		return dissolveTex->GetSRVDescriptorIndex();
	}
	return 0;
}

float DissolveMeshRenderer::GetDissolveThreshold() const {
	return dissolveThreshold_;
}

GPUMaterial DissolveMeshRenderer::GetGPUMaterial(AssetCollection* _ac) const {
	GPUMaterial result;
	result.uvTransform = material_.uvTransform;
	result.baseColor = material_.baseColor;
	result.postEffectFlags = material_.postEffectFlags;
	result.entityId = GetOwner()->GetId();

	if(material_.HasBaseTexture()) {
		const Texture* baseTex = _ac->GetTextureFromGuid(material_.GetBaseTextureGuid());
		if(baseTex) {
			result.baseTextureId = baseTex->GetSRVDescriptorIndex();
		}
	}

	return result;
}

uint32_t ONEngine::DissolveMeshRenderer::GetDissolveCompare() const {
	return static_cast<uint32_t>(dissolveCompare_);
}



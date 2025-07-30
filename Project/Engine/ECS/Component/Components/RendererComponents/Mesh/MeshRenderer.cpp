#include "MeshRenderer.h"

/// std
#include <format>

/// engine
#include "Engine/Core/ImGui/Math/ImGuiMath.h"
#include "Engine/Graphics/Pipelines/Collection/RenderingPipelineCollection.h"
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"


MeshRenderer::MeshRenderer() {
	SetMeshPath("./Packages/Models/primitive/cube.obj");
	SetTexturePath("./Packages/Textures/white.png");
	material_.color = Vector4::kWhite;
	material_.postEffectFlags = PostEffectFlags_Lighting;
	if (GetOwner()) {
		material_.entityId = GetOwner()->GetId();
	}
}

MeshRenderer::~MeshRenderer() {}

void MeshRenderer::SetMeshPath(const std::string& _path) {
	meshPath_ = _path;
}

void MeshRenderer::SetTexturePath(const std::string& _path) {
	texturePath_ = _path;
}

void MeshRenderer::SetColor(const Vector4& _color) {
	material_.color = _color;
}

void MeshRenderer::SetPostEffectFlags(uint32_t _flags) {
	material_.postEffectFlags = _flags;
}

void MeshRenderer::SetMaterialEntityId() {
	IEntity* owner = GetOwner();
	if (owner) {
		material_.entityId = owner->GetId();
	}
}

const std::string& MeshRenderer::GetMeshPath() const {
	return meshPath_;
}

const std::string& MeshRenderer::GetTexturePath() const {
	return texturePath_;
}

const Vector4& MeshRenderer::GetColor() const {
	return material_.color;
}

const Material& MeshRenderer::GetMaterial() const {
	return material_;
}

uint32_t MeshRenderer::GetPostEffectFlags() const {
	return material_.postEffectFlags;
}


MonoString* InternalGetMeshName(uint64_t _nativeHandle) {
	/// ptrから MeshRenderer を取得
	MeshRenderer* renderer = reinterpret_cast<MeshRenderer*>(_nativeHandle);
	if (!renderer) {
		Console::Log("MeshRenderer pointer is null");
		return nullptr;
	}

	return mono_string_new(mono_domain_get(), renderer->GetMeshPath().c_str());
}

void InternalSetMeshName(uint64_t _nativeHandle, MonoString* _meshName) {
	/// ptrから MeshRenderer を取得
	MeshRenderer* renderer = reinterpret_cast<MeshRenderer*>(_nativeHandle);
	if (!renderer) {
		Console::Log("MeshRenderer pointer is null");
		return;
	}

	/// stringに変換&設定
	char* meshName = mono_string_to_utf8(_meshName);
	std::string meshNameStr(meshName);
	renderer->SetMeshPath(meshNameStr);
	mono_free(meshName);
}

Vector4 InternalGetMeshColor(uint64_t _nativeHandle) {
	/// ptrから MeshRenderer を取得
	MeshRenderer* renderer = reinterpret_cast<MeshRenderer*>(_nativeHandle);
	if (!renderer) {
		Console::Log("MeshRenderer pointer is null");
		return Vector4::kZero;
	}

	return renderer->GetColor();
}

void InternalSetMeshColor(uint64_t _nativeHandle, Vector4 _color) {
	/// ptrから MeshRenderer を取得
	MeshRenderer* renderer = reinterpret_cast<MeshRenderer*>(_nativeHandle);
	if (renderer) {
		renderer->SetColor(_color);
	} else {
		Console::Log("MeshRenderer pointer is null");
	}
}

uint32_t InternalGetPostEffectFlags(uint64_t _nativeHandle) {
	/// ptrから MeshRenderer を取得
	MeshRenderer* renderer = reinterpret_cast<MeshRenderer*>(_nativeHandle);
	if (!renderer) {
		Console::LogError("MeshRenderer pointer is null");
		return PostEffectFlags_None;
	}
	return renderer->GetPostEffectFlags();
}

void InternalSetPostEffectFlags(uint64_t _nativeHandle, uint32_t _flags) {
	/// ptrから MeshRenderer を取得
	MeshRenderer* renderer = reinterpret_cast<MeshRenderer*>(_nativeHandle);
	if (renderer) {
		renderer->SetPostEffectFlags(_flags);
	} else {
		Console::LogError("MeshRenderer pointer is null");
	}
}

void COMP_DEBUG::MeshRendererDebug(MeshRenderer* _mr) {
	if (!_mr) {
		return;
	}

	/// param get
	Vec4 color = _mr->GetColor();
	std::string meshPath = _mr->GetMeshPath();
	std::string texturePath = _mr->GetTexturePath();

	/// edit
	if (ImGuiColorEdit("color", &color)) {
		_mr->SetColor(color);
	}


	ImGui::Spacing();


	/// meshの変更
	ImGui::Text("mesh path");
	ImGui::InputText("##mesh", meshPath.data(), meshPath.capacity(), ImGuiInputTextFlags_ReadOnly);
	if (ImGui::BeginDragDropTarget()) {
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("AssetData")) {

			/// ペイロードが存在する場合
			if (payload->Data) {
				const char* droppedPath = static_cast<const char*>(payload->Data);
				std::string path = std::string(droppedPath);

				/// メッシュのパスが有効な形式か確認
				if (path.find(".obj") != std::string::npos
					|| path.find(".gltf") != std::string::npos) {
					_mr->SetMeshPath(path);

					Console::Log(std::format("Mesh path set to: {}", path));
				} else {
					Console::LogError("Invalid mesh format. Please use .obj or .gltf.");
				}
			}
		}
		ImGui::EndDragDropTarget();
	}


	/// textureの変更
	ImGui::Text("texture path");
	ImGui::InputText("##texture", texturePath.data(), texturePath.capacity(), ImGuiInputTextFlags_ReadOnly);
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
					_mr->SetTexturePath(path);

					Console::Log(std::format("Texture path set to: {}", path));
				} else {
					Console::LogError("Invalid texture format. Please use .png, .jpg, or .jpeg.");
				}
			}
		}

		ImGui::EndDragDropTarget();
	}


	/// materialの設定
	const std::string labels[2] = {
		"Lighting",
		"Grayscale",
	};

	for (size_t i = 0; i < 2; i++) {
		int bit = 1 << (i);
		bool isChecked = (_mr->GetPostEffectFlags() & bit) != 0;
		if (ImGui::Checkbox(labels[i].c_str(), &isChecked)) {
			if (isChecked) {
				_mr->SetPostEffectFlags(_mr->GetPostEffectFlags() | bit);
			} else {
				_mr->SetPostEffectFlags(_mr->GetPostEffectFlags() & ~bit);
			}
		}
	}


}

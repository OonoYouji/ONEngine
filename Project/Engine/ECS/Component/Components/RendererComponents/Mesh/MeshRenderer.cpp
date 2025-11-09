#include "MeshRenderer.h"

/// std
#include <format>

/// engine
#include "Engine/Asset/AssetType.h"
#include "Engine/Asset/Collection/AssetCollection.h"
#include "Engine/Core/ImGui/Math/ImGuiMath.h"
#include "Engine/Core/ImGui/Math/AssetDebugger.h"
#include "Engine/Core/ImGui/Math/AssetPayload.h"
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/Editor/Commands/ComponentEditCommands/ComponentJsonConverter.h"

MeshRenderer::MeshRenderer() {
	SetMeshPath("./Packages/Models/primitive/cube.obj");
	material_.baseColor = Vector4::kWhite;
	material_.postEffectFlags = PostEffectFlags_Lighting;
}

MeshRenderer::~MeshRenderer() = default;

void MeshRenderer::SetupRenderData(AssetCollection* _assetCollection) {
	gpuMaterial_.postEffectFlags = material_.postEffectFlags;
	gpuMaterial_.baseColor = material_.baseColor;
	gpuMaterial_.entityId = GetOwner() ? GetOwner()->GetId() : 0;

	if (material_.HasBaseTexture()) {
		gpuMaterial_.baseTextureId = _assetCollection->GetTextureIndexFromGuid(material_.GetBaseTextureGuid());
	} else {
		gpuMaterial_.baseTextureId = 0;
	}
}

void MeshRenderer::SetMeshPath(const std::string& _path) {
	meshPath_ = _path;
}

void MeshRenderer::SetColor(const Vector4& _color) {
	material_.baseColor = _color;
}

void MeshRenderer::SetPostEffectFlags(uint32_t _flags) {
	material_.postEffectFlags = _flags;
}

const std::string& MeshRenderer::GetMeshPath() const {
	return meshPath_;
}

const Vector4& MeshRenderer::GetColor() const {
	return material_.baseColor;
}

const GPUMaterial& MeshRenderer::GetGpuMaterial() const {
	return gpuMaterial_;
}

uint32_t MeshRenderer::GetPostEffectFlags() const {
	return material_.postEffectFlags;
}

const Guid& MeshRenderer::GetTextureGuid() const {
	return material_.guid;
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

void COMP_DEBUG::MeshRendererDebug(MeshRenderer* _mr, AssetCollection* _assetCollection) {
	if (!_mr) {
		return;
	}

	/// param get
	Vector4& color = _mr->material_.baseColor;
	std::string& meshPath = _mr->meshPath_;
	//std::string& texturePath = _mr->texturePath_;

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
			if (payload->Data) {
				AssetPayload* assetPayload = *static_cast<AssetPayload**>(payload->Data);
				std::string path = assetPayload->filePath;
				AssetType type = GetAssetTypeFromExtension(Mathf::FileExtension(path));

				/// メッシュのパスが有効な形式か確認
				if (type == AssetType::Mesh) {
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

	/// ----------------------------------------------
	/// テクスチャのプレビュー表示
	/// ----------------------------------------------

	bool hasTextureGuid = _mr->material_.HasBaseTexture();
	if (hasTextureGuid) {
		if (Texture* tex = _assetCollection->GetTexture(_assetCollection->GetTexturePath(_mr->material_.GetBaseTextureGuid()))) {
			Vector2 aspectRatio = tex->GetTextureSize();
			aspectRatio /= (std::max)(aspectRatio.x, aspectRatio.y);

			ImTextureID texId = reinterpret_cast<ImTextureID>(tex->GetSRVGPUHandle().ptr);
			ImGui::Image(texId, ImVec2(64.0f * aspectRatio.x, 64.0f * aspectRatio.y));
		}
	} else {
		/// テクスチャがない場合はドラッグドロップ領域を表示する
		ImVec2 size = ImVec2(64, 64);
		ImVec2 pos = ImGui::GetCursorScreenPos();
		ImDrawList* drawList = ImGui::GetWindowDrawList();

		// InvisibleButton はクリック判定やDragDropのターゲット領域になる
		ImGui::InvisibleButton("DropArea", size);

		// 視覚的な四角形を描く
		ImU32 imColor = IM_COL32(100, 100, 255, 100); // 半透明の青
		drawList->AddRectFilled(pos, ImVec2(pos.x + size.x, pos.y + size.y), imColor, 4.0f);

		// 枠線
		drawList->AddRect(pos, ImVec2(pos.x + size.x, pos.y + size.y), IM_COL32(255, 255, 255, 200), 4.0f, 0, 2.0f);
	}


	/// ----------------------------------------------
	/// ドラッグアンドドロップでテクスチャを設定
	/// ----------------------------------------------
	if (ImGui::BeginDragDropTarget()) {
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("AssetData")) {

			/// ペイロードが存在する場合
			if (payload->Data) {
				AssetPayload* assetPayload = *static_cast<AssetPayload**>(payload->Data);
				const std::string path = assetPayload->filePath;

				/// テクスチャのパスが有効な形式か確認
				const AssetType type = GetAssetTypeFromExtension(Mathf::FileExtension(path));
				if (type == AssetType::Texture) {
					_mr->material_.SetBaseTextureGuid(assetPayload->guid);

					Console::Log(std::format("Texture path set to: {}", path));
				} else {
					Console::LogError("Invalid texture format. Please use .png, .jpg, or .jpeg.");
				}
			}
		}

		ImGui::EndDragDropTarget();
	}


	/// ----------------------------------------------
	/// materialの設定
	/// ----------------------------------------------
	ImMathf::MaterialEdit("Material##MeshRenderer", &_mr->material_, _assetCollection);

}


void from_json(const nlohmann::json& _j, MeshRenderer& _m) {
	if (_j.contains("enable")) {
		_m.enable = _j.at("enable").get<int>();
	}

	_m.SetMeshPath(_j.at("meshPath").get<std::string>());


	/// デバッグのためにvalueではなくcontainsでチェック
	if (_j.contains("material")) {
		_m.material_ = _j.at("material").get<Material>();
	}

}

void to_json(nlohmann::json& _j, const MeshRenderer& _m) {
	_j = nlohmann::json{
		{ "type", "MeshRenderer" },
		{ "enable", _m.enable },
		{ "meshPath", _m.GetMeshPath() },
		{ "material", _m.material_ },
	};
}
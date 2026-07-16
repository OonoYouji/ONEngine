#include "MeshRenderer.h"
#include <nlohmann/json.hpp>

/// std
#include <format>

/// engine
#include "Engine/Asset/AssetType.h"
#include "Engine/Asset/Collection/AssetCollection.h"
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/Editor/Commands/ComponentEditCommands/ComponentJsonConverter.h"

/// editor
#include "Engine/Editor/Math/ImGuiMath.h"
#include "Engine/Editor/Math/AssetDebugger.h"
#include "Engine/Editor/Math/AssetPayload.h"

using namespace ONEngine;

MeshRenderer::MeshRenderer() {
	SetMeshPath("./Packages/Models/primitive/cube.obj");
	material_.baseColor = Vector4::White;
	material_.postEffectFlags = PostEffectFlags_Lighting;
}

MeshRenderer::~MeshRenderer() = default;

/**
 * @brief アセットコレクションから、指定されたパスのメッシュ・マテリアル・テクスチャデータを取得し、描画に必要なGPU用定数バッファのセットアップを行います。
 */
void MeshRenderer::SetupRenderData(Asset::AssetCollection* _assetCollection) {
	gpuMaterial_.postEffectFlags = material_.postEffectFlags;
	gpuMaterial_.baseColor = material_.baseColor;
	gpuMaterial_.uvTransform = material_.uvTransform;
	gpuMaterial_.entityId = GetOwner() ? GetOwner()->GetId() : 0;

	if (material_.HasBaseTexture()) {
		gpuMaterial_.baseTextureId = _assetCollection->GetTextureIndexFromGuid(material_.GetBaseTextureGuid());
	} else {
		gpuMaterial_.baseTextureId = 0;
	}
}

/**
 * @brief 描画に使用するメッシュアセットのファイルパス（.objや.fbx等）を設定します。
 */
void MeshRenderer::SetMeshPath(const std::string& _path) {
	meshPath_ = _path;
}

/**
 * @brief 描画カラー（RGBA）を設定します。
 */
void MeshRenderer::SetColor(const Vector4& _color) {
	material_.baseColor = _color;
}

/**
 * @brief ポストエフェクト描画制御フラグを設定します（アウトライン、Dissolve等）。
 */
void MeshRenderer::SetPostEffectFlags(uint32_t _flags) {
	material_.postEffectFlags = _flags;
}

/**
 * @brief UVトランスフォーム（テクスチャのタイリングやオフセットなど）を設定します。
 */
void MeshRenderer::SetUVTransform(const UVTransform& _uvTransform) {
	material_.uvTransform = _uvTransform;
}

/**
 * @brief 描画プライオリティ（RenderQueue）を設定します（Background, Telegraph, Default等）。
 */
void MeshRenderer::SetRenderQueue(RenderQueue _queue) {
	renderQueue_ = _queue;
}

/**
 * @brief 現在の描画プライオリティを取得します。
 */
RenderQueue MeshRenderer::GetRenderQueue() const {
	return renderQueue_;
}

/**
 * @brief 設定されているメッシュアセットのファイルパスを取得します。
 */
const std::string& MeshRenderer::GetMeshPath() const {
	return meshPath_;
}

/**
 * @brief 設定されているカラー値を取得します。
 */
const Vector4& MeshRenderer::GetColor() const {
	return material_.baseColor;
}

/**
 * @brief 定数バッファ送信用にパックされたGPUマテリアルデータ（GPUMaterial）を取得します。
 */
const GPUMaterial& MeshRenderer::GetGpuMaterial() const {
	return gpuMaterial_;
}

/**
 * @brief ポストエフェクト描画制御フラグを取得します。
 */
uint32_t MeshRenderer::GetPostEffectFlags() const {
	return material_.postEffectFlags;
}

/**
 * @brief UVトランスフォームパラメータを取得します。
 */
const UVTransform& MeshRenderer::GetUVTransform() const {
	return material_.uvTransform;
}

/**
 * @brief アタッチされているカラーマップテクスチャのGuidを取得します。
 */
const Guid& MeshRenderer::GetTextureGuid() const {
	return material_.guid;
}


/**
 * @brief C#（Mono）インターフェース用：メッシュ名を取得します。
 */
MonoString* ONEngine::InternalGetMeshName(uint64_t _nativeHandle) {
	/// ptrから MeshRenderer を取得
	MeshRenderer* renderer = reinterpret_cast<MeshRenderer*>(_nativeHandle);
	if (!renderer) {
		Console::Log("MeshRenderer pointer is null");
		return nullptr;
	}

	return mono_string_new(mono_domain_get(), renderer->GetMeshPath().c_str());
}

/**
 * @brief C#（Mono）インターフェース用：メッシュ名を設定します。
 */
void ONEngine::InternalSetMeshName(uint64_t _nativeHandle, MonoString* _meshName) {
	/// ptrから MeshRenderer を取得
	MeshRenderer* renderer = reinterpret_cast<MeshRenderer*>(_nativeHandle);
	if (!renderer) {
		Console::Log("MeshRenderer pointer is null");
		return;
	}

	/// stringに変換&設定
	char* meshName = mono_string_to_utf8(_meshName);
	if(meshName) {
		std::string meshNameStr(meshName);
		renderer->SetMeshPath(meshNameStr);
		mono_free(meshName);
	}
}

/**
 * @brief C#（Mono）インターフェース用：メッシュカラーを取得します。
 */
Vector4 ONEngine::InternalGetMeshColor(uint64_t _nativeHandle) {
	/// ptrから MeshRenderer を取得
	MeshRenderer* renderer = reinterpret_cast<MeshRenderer*>(_nativeHandle);
	if (!renderer) {
		Console::Log("MeshRenderer pointer is null");
		return Vector4::Zero;
	}

	return renderer->GetColor();
}

/**
 * @brief C#（Mono）インターフェース用：メッシュカラーを設定します。
 */
void ONEngine::InternalSetMeshColor(uint64_t _nativeHandle, Vector4 _color) {
	/// ptrから MeshRenderer を取得
	MeshRenderer* renderer = reinterpret_cast<MeshRenderer*>(_nativeHandle);
	if (renderer) {
		renderer->SetColor(_color);
	} else {
		Console::Log("MeshRenderer pointer is null");
	}
}

/**
 * @brief C#（Mono）インターフェース用：ポストエフェクトフラグを取得します。
 */
uint32_t ONEngine::InternalGetPostEffectFlags(uint64_t _nativeHandle) {
	/// ptrから MeshRenderer を取得
	MeshRenderer* renderer = reinterpret_cast<MeshRenderer*>(_nativeHandle);
	if (!renderer) {
		Console::LogError("MeshRenderer pointer is null");
		return PostEffectFlags_None;
	}
	return renderer->GetPostEffectFlags();
}

/**
 * @brief C#（Mono）インターフェース用：ポストエフェクトフラグを設定します。
 */
void ONEngine::InternalSetPostEffectFlags(uint64_t _nativeHandle, uint32_t _flags) {
	/// ptrから MeshRenderer を取得
	MeshRenderer* renderer = reinterpret_cast<MeshRenderer*>(_nativeHandle);
	if (renderer) {
		renderer->SetPostEffectFlags(_flags);
	} else {
		Console::LogError("MeshRenderer pointer is null");
	}
}

/**
 * @brief C#（Mono）インターフェース用：描画キュー優先度を取得します。
 */
uint32_t ONEngine::InternalGetRenderQueue(uint64_t _nativeHandle) {
	MeshRenderer* renderer = reinterpret_cast<MeshRenderer*>(_nativeHandle);
	return renderer ? static_cast<uint32_t>(renderer->GetRenderQueue()) : 2;
}

/**
 * @brief C#（Mono）インターフェース用：描画キュー優先度を設定します。
 */
void ONEngine::InternalSetRenderQueue(uint64_t _nativeHandle, uint32_t _queue) {
	MeshRenderer* renderer = reinterpret_cast<MeshRenderer*>(_nativeHandle);
	if (renderer) renderer->SetRenderQueue(static_cast<RenderQueue>(_queue));
}

/**
 * @brief エディタ用：MeshRendererコンポーネントのデバッグ表示（Gui描画等）処理を行います。
 */
void ComponentDebug::MeshRendererDebug(MeshRenderer* _mr, Asset::AssetCollection* _assetCollection) {
	if (!_mr) {
		return;
	}

	/// param get
	Vector4& color = _mr->material_.baseColor;
	std::string& meshPath = _mr->meshPath_;

	/// edit
	if (Editor::ImGuiColorEdit("color", &color)) {
		_mr->SetColor(color);
	}

	const char* queueNames[] = { "Background", "Telegraph", "Default" };
	int currentQueue = static_cast<int>(_mr->renderQueue_);
	if (ImGui::Combo("Render Queue", &currentQueue, queueNames, 3)) {
		_mr->renderQueue_ = static_cast<RenderQueue>(currentQueue);
	}

	ImGui::Spacing();


	/// meshの変更
	ImGui::Text("mesh path");
	ImGui::InputText("##mesh", meshPath.data(), meshPath.capacity(), ImGuiInputTextFlags_ReadOnly);
	if (ImGui::BeginDragDropTarget()) {
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("AssetData")) {
			if (payload->Data) {
				Editor::AssetPayload* assetPayload = *static_cast<Editor::AssetPayload**>(payload->Data);
				std::string path = assetPayload->filePath;
				Asset::AssetType type = Asset::GetAssetTypeFromExtension(FileSystem::FileExtension(path));

				/// メッシュのパスが有効な形式か確認
				if (type == Asset::AssetType::Mesh) {
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
		if (Asset::Texture* tex = _assetCollection->GetTexture(_assetCollection->GetTexturePath(_mr->material_.GetBaseTextureGuid()))) {
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
				Editor::AssetPayload* assetPayload = *static_cast<Editor::AssetPayload**>(payload->Data);
				const std::string path = assetPayload->filePath;

				/// テクスチャのパスが有効な形式か確認
				const Asset::AssetType type = Asset::GetAssetTypeFromExtension(FileSystem::FileExtension(path));
				if (type == Asset::AssetType::Texture) {
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
	Editor::ImMathf::MaterialEdit("Material##MeshRenderer", &_mr->material_, _assetCollection);

}


/**
 * @brief JSONからのデシリアライズ
 */
void ONEngine::from_json(const nlohmann::json& _j, MeshRenderer& _m) {
	if (_j.contains("enable")) {
		_m.enable = _j.at("enable").get<int>();
	}

	_m.SetMeshPath(_j.at("meshPath").get<std::string>());


	/// デバッグのためにvalueではなくcontainsでチェック
	if (_j.contains("material")) {
		_m.material_ = _j.at("material").get<Asset::Material>();
	}

	if (_j.contains("renderQueue")) {
		_m.renderQueue_ = static_cast<RenderQueue>(_j.at("renderQueue").get<uint32_t>());
	}

}

/**
 * @brief JSONへのシリアライズ
 */
void ONEngine::to_json(nlohmann::json& _j, const MeshRenderer& _m) {
	_j = nlohmann::json{
		{ "type", "MeshRenderer" },
		{ "enable", _m.enable },
		{ "meshPath", _m.GetMeshPath() },
		{ "material", _m.material_ },
		{ "renderQueue", static_cast<uint32_t>(_m.renderQueue_) },
	};
}

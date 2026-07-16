#include "SpriteRenderer.h"

/// external
#include <imgui.h>
#include <format>

/// engine
#include "Engine/Core/Utility/Tools/Log.h"
#include "Engine/ECS/Entity/GameEntity/GameEntity.h"
#include "Engine/Editor/Commands/ComponentEditCommands/ComponentJsonConverter.h"
#include "Engine/Asset/Collection/AssetCollection.h"

/// editor
#include "Engine/Editor/Math/ImGuiMath.h"
#include "Engine/Editor/Math/AssetDebugger.h"

#include "Engine/Core/DirectX12/Manager/DxManager.h"

using namespace ONEngine;

/// /////////////////////////////////////////////////////////////
/// デバッグ用のSpriteRenderer
/// /////////////////////////////////////////////////////////////

/**
 * @brief エディタ用：SpriteRendererコンポーネントのデバッグ表示（Gui描画等）処理を行います。
 */
void ComponentDebug::SpriteDebug(SpriteRenderer* _sr, Asset::AssetCollection* _assetCollection) {
	if (!_sr) {
		return;
	}

	float indentValue = 1.8f;
	ImGui::Indent(indentValue);

	Editor::ImMathf::MaterialEdit("Material", &_sr->material_, _assetCollection, false);

	ImGui::Unindent(indentValue);
}


/**
 * @brief JSONへのシリアライズ
 */
void ONEngine::to_json(nlohmann::json& _j, const SpriteRenderer& _sr) {
	_j = nlohmann::json{
		{ "type", "SpriteRenderer" },
		{ "enable", _sr.enable },
		{ "material", _sr.material_ }
	};
}

/**
 * @brief JSONからのデシリアライズ
 */
void ONEngine::from_json(const nlohmann::json& _j, SpriteRenderer& _sr) {
	_sr.enable = _j.value("enable", static_cast<int>(true));
	_sr.material_ = _j.value("material", Asset::Material{});
}


/// /////////////////////////////////////////////////////////////
/// SpriteRenderer
/// /////////////////////////////////////////////////////////////

SpriteRenderer::SpriteRenderer() {
	gpuMaterial_.baseColor = Vector4::White;
	gpuMaterial_.entityId = 0;
	gpuMaterial_.baseTextureId = 0;
	gpuMaterial_.uvTransform = UVTransform();
	gpuMaterial_.postEffectFlags = 0;

	material_.baseColor = Vector4::White;
	material_.postEffectFlags = 0;
}
SpriteRenderer::~SpriteRenderer() {}

/**
 * @brief アセットコレクションからテクスチャのバインディング情報を取得し、GPU描画に必要な定数バッファのセットアップを行います。
 */
void SpriteRenderer::RenderingSetup(Asset::AssetCollection* _assetCollection) {

	gpuMaterial_.baseColor = material_.baseColor;
	gpuMaterial_.postEffectFlags = material_.postEffectFlags;
	/// base texture
	if (material_.HasBaseTexture()) {
		int32_t textureIndex = _assetCollection->GetTextureIndexFromGuid(material_.GetBaseTextureGuid());
		if (textureIndex != -1) {
			gpuMaterial_.baseTextureId = textureIndex;
		} else {
			gpuMaterial_.baseTextureId = 0;
			Console::LogWarning(std::format("[SpriteRenderer] Texture not found in AssetCollection. GUID: {}. Falling back to default (Index 0).", material_.GetBaseTextureGuid().ToString()));
		}
	} else {
		gpuMaterial_.baseTextureId = 0;
	}

	gpuMaterial_.uvTransform = material_.uvTransform;
	gpuMaterial_.entityId = owner_ ? static_cast<int32_t>(owner_->GetId()) : 0;
}


/**
 * @brief スプライトのカラー（RGBA）を設定します。
 */
void SpriteRenderer::SetColor(const Vector4& _color) {
	material_.baseColor = _color;
}

/**
 * @brief スプライトのUV変形（オフセット、スケール等）を設定します。
 */
void SpriteRenderer::SetUVTransform(const UVTransform& _uvTransform) {
	material_.uvTransform = _uvTransform;
}

/**
 * @brief 現在のスプライトカラーを取得します。
 */
const Vector4& SpriteRenderer::GetColor() const {
	return gpuMaterial_.baseColor;
}

/**
 * @brief GPUへ転送するためのマテリアルデータを取得します。
 */
const GPUMaterial& SpriteRenderer::GetGpuMaterial() const {
	return gpuMaterial_;
}

/**
 * @brief 現在のスプライトUV変形設定を取得します。
 */
const UVTransform& SpriteRenderer::GetUVTransform() const {
	return material_.uvTransform;
}

/**
 * @brief スプライトが使用しているテクスチャの解像度（ピクセル幅・高さ）を取得します。
 */
Vector2 SpriteRenderer::GetTextureSize(Asset::AssetCollection* _assetCollection) const {
	if (material_.HasBaseTexture()) {
		Asset::Texture* texture = _assetCollection->GetTextureFromGuid(material_.GetBaseTextureGuid());
		if (texture) {
			return texture->GetTextureSize();
		}
	}
	return Vector2(0.0f, 0.0f);
}


/// ===================================================
/// csで使用するための関数群
/// ===================================================

/**
 * @brief C#（Mono）インターフェース用：スプライトカラーを取得します。
 */
Vector4 MonoInternalMethods::InternalGetColor(uint64_t _nativeHandle) {
	SpriteRenderer* sr = reinterpret_cast<SpriteRenderer*>(_nativeHandle);
	if (sr) {
		return sr->GetColor();
	}

	Console::LogError("MonoInternalMethods::InternalGetColor() | native handle is invalid");

	return Vector4();
}

/**
 * @brief C#（Mono）インターフェース用：スプライトカラーを設定します。
 */
void MonoInternalMethods::InternalSetColor(uint64_t _nativeHandle, Vector4 _color) {
	SpriteRenderer* sr = reinterpret_cast<SpriteRenderer*>(_nativeHandle);
	if (sr) {
		sr->SetColor(_color);
		return;
	}

	Console::LogError("MonoInternalMethods::InternalSetColor() | native handle is invalid");
}

/**
 * @brief C#（Mono）インターフェース用：テクスチャサイズを取得します。
 */
Vector2 MonoInternalMethods::InternalGetTextureSize(uint64_t _nativeHandle) {
	SpriteRenderer* sr = reinterpret_cast<SpriteRenderer*>(_nativeHandle);
	if (sr) {
		auto* assetCollection = Asset::AssetCollection::GetInstance();
		return sr->GetTextureSize(assetCollection);
	}

	Console::LogError("MonoInternalMethods::InternalGetTextureSize() | native handle is invalid");
	return Vector2(0.0f, 0.0f);
}
